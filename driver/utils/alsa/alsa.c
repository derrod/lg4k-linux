/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * alsa.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/version.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/control.h>
#include <sound/initval.h>

#include "sagitta.h"
#include "alsa.h"
#include "trace.h"

/* sagitta_module instance */
static struct sagitta_module *instance;
struct timer_list filldata_timer;

#define TEST_SND_SIZE 7

u64 nsecs_to_jiffies64(u64 n)
{
#if (NSEC_PER_SEC % HZ) == 0
        /* Common case, HZ = 100, 128, 200, 250, 256, 500, 512, 1000 etc. */
        return div_u64(n, NSEC_PER_SEC / HZ);
#elif (HZ % 512) == 0
        /* overflow after 292 years if HZ = 1024 */
        return div_u64(n * HZ / 512, NSEC_PER_SEC / 512);
#else
        /*
         * Generic case - optimized for cases where HZ is a multiple of 3.
         * overflow after 64.99 years, exact for HZ = 60, 72, 90, 120 etc.
         */
        return div_u64(n * 9, (9ull * NSEC_PER_SEC + HZ / 2) / HZ);
#endif
}

unsigned char buffer[1024] = { [0 ... 1023] = 13 };


static void filldata(unsigned long data)
{
//    struct sagitta_virt_dev *svdev = (struct sagitta_virt_dev *) data;
//    struct sagitta_snd_dev *ssdev =
//        (struct sagitta_snd_dev *) sagitta_virtdev_getdata(svdev);
	struct sagitta_snd_dev *ssdev =(struct sagitta_snd_dev *)data;

    ssdev->buf_deliver(ssdev, buffer, 1024);
    mod_timer(&filldata_timer, msecs_to_jiffies(100));
}

#if 0
static int sagitta_snd_test_stream(void *this)
{
    struct sagitta_snd_dev *ssdev =
        (struct sagitta_snd_dev *) this;

    unsigned char *audio_head = &_binary_audio_dump_start;
    int i;

    int blob_size = &_binary_audio_dump_size;
    int deliver_size;
    int done_size;
    bool reset;

    printk("%s blob_size = %d\n", __func__, blob_size);

    for(;;)
    {
        if(kthread_should_stop())
            goto stop;

        for(;;)
        {
            if(kthread_should_stop())
                goto stop;

            if((audio_head + TEST_SND_SIZE) < &_binary_audio_dump_end) {
                deliver_size = TEST_SND_SIZE;
                reset = true;
            } else {
                deliver_size = &_binary_audio_dump_end - audio_head;
                reset = false;
            }
            printk("%s deliver\n", __func__);
#if 1
            for(i = 0; i < deliver_size; i++) {
                if(*(audio_head + i)) {
                }
            }
#endif
//            ssdev->buf_deliver(ssdev->sdev.parent, audio_head, deliver_size);
            audio_head += deliver_size;
            if(reset)
                audio_head = &_binary_audio_dump_start;

            set_current_state(TASK_INTERRUPTIBLE);
//            schedule_timeout(nsecs_to_jiffies64(500));
            schedule_timeout(HZ);
        }
//        schedule_work(&ssdev->trigger_deliver);

    }
stop:
    printk("%s end\n", __func__);

    __set_current_state(TASK_RUNNING);

    return 0;
}
#endif

static void snd_sagitta_fillbuf(struct sagitta_snd_dev *ssdev, u8 *buf, size_t length)
{
    struct snd_pcm_runtime *runtime;
	struct snd_pcm_substream *substream;

    unsigned int oldptr;
    unsigned int stride;
    unsigned int cnt;

    bool period_elapsed = false;

    if(!ssdev->substream)
        return;

	if(!length)
        return;

	substream = ssdev->substream;
    runtime = ssdev->substream->runtime;
    stride = runtime->frame_bits >> 3;
    oldptr = ssdev->hwptr;

    /* bytes to frames */
    length /= stride;

    
	/* buffer address */
    if(oldptr + length >= runtime->buffer_size)
    {
        cnt = runtime->buffer_size - oldptr;
        memcpy(runtime->dma_area + oldptr * stride,
                buf, cnt * stride);
        memcpy(runtime->dma_area, buf + cnt * stride,
                length * stride - cnt * stride);
    }
    else
    {
        memcpy(runtime->dma_area + oldptr * stride,
                buf, length * stride);
    }

    snd_pcm_stream_lock(substream);
    ssdev->hwptr += length;
    if(ssdev->hwptr >= runtime->buffer_size)
        ssdev->hwptr -= runtime->buffer_size;

    ssdev->capture_transfer_done += length;
    if(ssdev->capture_transfer_done >= runtime->period_size)
    {
        ssdev->capture_transfer_done -= runtime->period_size;
        period_elapsed = true;
    }
    snd_pcm_stream_unlock(substream);

    if(period_elapsed)
        snd_pcm_period_elapsed(substream);
}

static struct snd_pcm_hardware snd_sagitta_hw = {
    .info = SNDRV_PCM_INFO_MMAP |
        SNDRV_PCM_INFO_INTERLEAVED |
        SNDRV_PCM_INFO_BLOCK_TRANSFER |
        SNDRV_PCM_INFO_MMAP_VALID,
    .formats = SNDRV_PCM_FMTBIT_S16_LE,

    .rates =        SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |
                    SNDRV_PCM_RATE_48000 | SNDRV_PCM_RATE_88200 |
                    SNDRV_PCM_RATE_96000 | SNDRV_PCM_RATE_176400 |
                    SNDRV_PCM_RATE_192000,
    .rate_max =		192000,
    .rate_min =		32000,
    .channels_max =	2,
    .channels_min =	2,
    .period_bytes_min = (4*2048),
    .period_bytes_max = (16*2048),
    .periods_min = 128,
    .periods_max = 512,
    .buffer_bytes_max = (512*16*2048),
};

static int snd_sagitta_pcm_open(struct snd_pcm_substream *substream)
{
    struct sagitta_snd_dev *sdev = snd_pcm_substream_chip(substream);
    struct snd_pcm_runtime *runtime = substream->runtime;

    printk("%s\n", __func__);

    sdev->substream = substream;
    runtime->hw = snd_sagitta_hw;
    snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);

    sdev->buf_deliver = &snd_sagitta_fillbuf;

	return 0;
}

static int snd_sagitta_pcm_close(struct snd_pcm_substream *substream)
{
    struct sagitta_snd_dev *sdev = snd_pcm_substream_chip(substream);

    printk("%s\n", __func__);

    sdev->substream = NULL;

	return 0;
}

static int snd_sagitta_hw_params(struct snd_pcm_substream *substream,
        struct snd_pcm_hw_params *hw_params)
{
	printk("%s() alloc buffer size = %u"
			" periods = %u period_bytes = %u\n",
			__func__,
			params_buffer_bytes(hw_params),
			params_periods(hw_params),
			params_period_bytes(hw_params));

    if(substream->runtime->dma_area)
    {
        substream->runtime->dma_area = NULL;
    }

    return snd_pcm_lib_alloc_vmalloc_buffer(substream,
            params_buffer_bytes(hw_params));
}

static int snd_sagitta_hw_free(struct snd_pcm_substream *substream)
{
    printk("%s\n", __func__);

    if(substream->runtime->dma_area)
    {
        substream->runtime->dma_area = NULL;
        substream->runtime->dma_addr = 0;
        substream->runtime->dma_bytes = 0;
    }

    return snd_pcm_lib_free_vmalloc_buffer(substream);
}

static int snd_sagitta_prepare(struct snd_pcm_substream *substream)
{
	struct sagitta_snd_dev *sdev = snd_pcm_substream_chip(substream);

    printk("%s\n", __func__);

    sdev->capture_transfer_done = 0;
    sdev->hwptr = 0;

    return 0;
}

static void sagitta_audio_trigger(struct work_struct *work)
{
    struct sagitta_snd_dev *ssdev =
        container_of(work, struct sagitta_snd_dev, wq_trigger);

    if(atomic_read(&ssdev->stream_started))
    {

        printk("%s start\n", __func__);
#if 0
        add_timer(&filldata_timer);
#endif
    }
    else {
        printk("%s stop\n", __func__);
#if 0
        del_timer_sync(&filldata_timer);
#endif
    }
}

static int snd_sagitta_card_trigger(struct snd_pcm_substream *substream,
			int cmd)
{
	struct sagitta_snd_dev *sdev = snd_pcm_substream_chip(substream);

    printk("%s\n", __func__);

    switch(cmd)
    {
    case SNDRV_PCM_TRIGGER_START:
        atomic_set(&sdev->stream_started, 1);
        break;
    case SNDRV_PCM_TRIGGER_STOP:
        atomic_set(&sdev->stream_started, 0);
        break;
    default:
        return -EINVAL;
    }

    schedule_work(&sdev->wq_trigger);

    return 0;
}

static snd_pcm_uframes_t snd_sagitta_pointer(
		struct snd_pcm_substream *substream)
{
    struct sagitta_snd_dev *sdev = snd_pcm_substream_chip(substream);
	unsigned long flags;
	snd_pcm_uframes_t hwptr;

	spin_lock_irqsave(&sdev->slock, flags);
	hwptr = sdev->hwptr;
	spin_unlock_irqrestore(&sdev->slock, flags);

    return hwptr;
}

static struct snd_pcm_ops snd_sagitta_pcm_ops = {
    .open = snd_sagitta_pcm_open,
    .close = snd_sagitta_pcm_close,
    .ioctl = snd_pcm_lib_ioctl,
    .hw_params = snd_sagitta_hw_params,
    .hw_free = snd_sagitta_hw_free,
    .prepare = snd_sagitta_prepare,
    .trigger = snd_sagitta_card_trigger,
    .pointer = snd_sagitta_pointer,
    .page = snd_pcm_lib_get_vmalloc_page,
//	.page = snd_pl330_page,
};

static int snd_sagitta_pcm(struct sagitta_snd_dev *dev,
        int device_idx, const char *name)
{
    struct snd_pcm *pcm;
    int err;

    printk("%s\n", __func__);

    err = snd_pcm_new(dev->card, name, device_idx, 0, 1, &pcm);
    if(err < 0)
        return err;

    pcm->private_data = dev;
    strcpy(pcm->name, name);
    snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE,
            &snd_sagitta_pcm_ops);

    return 0;
}

static struct sagitta_snd_dev *audio_dev_register(struct sagitta_dev *sdev)
{
    struct sagitta_snd_dev *snd_dev;
    const struct sagitta_board *board = sdev->board;
    struct snd_card *card;

    int err;

    printk("%s\n", __func__);
#if 0
    snd_dev = kmalloc(sizeof(struct sagitta_snd_dev), GFP_KERNEL);
    if(!snd_dev) {
        sagitta_trace(instance, "%s no memory to allocate snd_dev\n", __func__);
        return NULL;
    }
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)
    err = snd_card_new(NULL, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1,
#else
    err = snd_card_create(SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1,
#endif
            THIS_MODULE, sizeof(struct sagitta_snd_dev), &card);
    if(err < 0)
        goto err;

    /* init sagitta_snd_dev */
    snd_dev = (struct sagitta_snd_dev *) card->private_data;
    /* assign parent dev */
    snd_dev->sdev = sdev;
    snd_dev->card = card;
    INIT_LIST_HEAD(&snd_dev->list);

    err = snd_sagitta_pcm(snd_dev, 0, "Sagitta PCM");
    if(err < 0)
        goto err;

    /* init lock */
    mutex_init(&snd_dev->mlock);
    spin_lock_init(&snd_dev->slock);

    INIT_WORK(&snd_dev->wq_trigger, sagitta_audio_trigger);

    /* snd_card->driver name size = 16 */
    strncpy(card->driver, "Sagitta", 16);
    strncpy(card->shortname, board->name, 32);
    sprintf(card->longname, "%s", board->name);
    
    /* set device dev */
    snd_card_set_dev(card, sdev->dev);

    err = snd_card_register(card);
    if(err < 0) {
        snd_card_free(card);
        goto err;
    }

    return snd_dev;
err:
    return NULL;
}

void audio_dev_deregister(struct sagitta_snd_dev *ssdev)
{
    if(ssdev->card) {
        printk("%s snd_card_free\n", __func__);
        snd_card_free(ssdev->card);
        ssdev->card = NULL;
    }
}

static void alsa_probe(struct sagitta_module *module, struct sagitta_dev *sdev)
{
    struct sagitta_snd_dev *ssdev;
    struct alsa_module *alsa_mod=(struct alsa_module *)module;
//    struct sagitta_virt_dev *svdev =
//        (struct sagitta_virt_dev *) sagitta_get_alsa_dev(sdev);
//
//    struct sagitta_virt_container *container =
//        (struct sagitta_virt_container *) sdev->priv;





    ssdev = audio_dev_register(sdev);

    //sagitta_virtdev_setdata(svdev, ssdev);
    //container->ssdev = ssdev;

    init_timer(&filldata_timer);
    filldata_timer.expires = msecs_to_jiffies(100);
    filldata_timer.function = filldata;
    filldata_timer.data = (unsigned long) ssdev;
    alsa_mod->context=ssdev;

#if 0
    if(ssdev)
        list_add_tail(&ssdev->list, &_this->list);
#endif
}

static void alsa_disconnect(struct sagitta_module *module, struct sagitta_dev *sdev)
{
	struct alsa_module *alsa_mod=(struct alsa_module *)module;
    //struct sagitta_virt_dev *svdev =       (struct sagitta_virt_dev *) sagitta_get_alsa_dev(sdev);
    //struct sagitta_snd_dev *ssdev =   (struct sagitta_snd_dev *) sagitta_virtdev_getdata(svdev);
	struct sagitta_snd_dev *ssdev =   (struct sagitta_snd_dev *)alsa_mod->context;

    printk("%s\n", __func__);

    //sagitta_trace(module, "%s\n", __func__);


    audio_dev_deregister(ssdev);
}

void alsa_module_init(struct sagitta_module *mod)
{
    //sagitta_trace(mod, "%s\n", __func__);

    //INIT_LIST_HEAD(&mod->list);

    instance = mod;
    mod->probe = &alsa_probe;
    mod->disconnect = &alsa_disconnect;
}

void alsa_module_exit(struct sagitta_module *module)
{
    //sagitta_trace(module, "%s\n", __func__);
}

void *alsa_module_get_context(struct sagitta_dev *sdev)
{
	struct sagitta_module *mod,*tmp;

	list_for_each_entry_safe(mod,tmp,&sdev->root->modules,list)
	{
		if(strncmp(mod->name,ALSA_MODULE_ID_STR,strlen(mod->name))==0)
		{
			struct alsa_module *this=(struct alsa_module *)mod;
			//printk("%s found context %p\n",__func__,this->context);
			return this->context;
		}
	}

	return NULL;

}

