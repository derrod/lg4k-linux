/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * alsa_model.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " "%s, %d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/workqueue.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/control.h>
#include <sound/initval.h>
#include "cxt_mgr.h"
#include "alsa_model.h"

typedef struct
{
    alsa_model_callback_fun_t cb_func;
    void *cb_cxt;
} alsa_cb_info_t;

typedef struct
{
    BASIC_CXT_HANDLE_DECLARE;
    struct list_head list;
    struct snd_card *card;
    struct snd_pcm_substream *substream;
    struct snd_pcm_hardware hw_parm;

    /* trigger stream start/stop */
    struct work_struct wq_trigger;
    atomic_t stream_started;

    snd_pcm_uframes_t capture_transfer_done;
    snd_pcm_uframes_t hwptr;

    struct mutex mlock;
    spinlock_t slock;
    
    struct snd_pcm *pcm;
    struct task_struct *test_stream;
    struct work_struct trigger_deliver;

    alsa_cb_info_t cb_info[ALSA_MODEL_MAX_CB_NO];


} alsa_model_t;

static void *alsa_model_alloc(void);
static void alsa_model_release(void *context);
static int alsa_model_pcm_open(struct snd_pcm_substream *substream);
static int alsa_model_pcm_close(struct snd_pcm_substream *substream);
static int alsa_model_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *hw_params);
static int alsa_model_hw_free(struct snd_pcm_substream *substream);
static int alsa_model_prepare(struct snd_pcm_substream *substream);
static int alsa_model_card_trigger(struct snd_pcm_substream *substream, int cmd);
static snd_pcm_uframes_t alsa_model_pointer(struct snd_pcm_substream *substream);

static struct snd_pcm_ops alsa_model_pcm_ops = {
    .open = alsa_model_pcm_open,
    .close = alsa_model_pcm_close,
    .ioctl = snd_pcm_lib_ioctl,
    .hw_params = alsa_model_hw_params,
    .hw_free = alsa_model_hw_free,
    .prepare = alsa_model_prepare,
    .trigger = alsa_model_card_trigger,
    .pointer = alsa_model_pointer,
    .page = snd_pcm_lib_get_vmalloc_page,
};

static struct snd_pcm_hardware alsa_model_hw = {
    .info = SNDRV_PCM_INFO_MMAP |
    SNDRV_PCM_INFO_INTERLEAVED |
    SNDRV_PCM_INFO_BLOCK_TRANSFER |
    SNDRV_PCM_INFO_MMAP_VALID,
    .formats = SNDRV_PCM_FMTBIT_S16_LE,
    .rates = SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_44100 |
    SNDRV_PCM_RATE_48000,
    .rate_max = 48000,//192000
    .rate_min = 32000,
    .channels_max = 2,
    .channels_min = 2,
    .period_bytes_min = ALSA_MODEL_DEFAULT_PERIOD_SIZE,
    .period_bytes_max = ALSA_MODEL_DEFAULT_MAX_PERIOD_NUM*ALSA_MODEL_DEFAULT_PERIOD_SIZE,
    .periods_min = 8, //1
    .periods_max = ALSA_MODEL_DEFAULT_MAX_PERIOD_NUM,
    .buffer_bytes_max = ALSA_MODEL_DEFAULT_MAX_PERIOD_NUM*ALSA_MODEL_DEFAULT_PERIOD_SIZE,
};

static void *alsa_model_alloc()
{
    alsa_model_t *alsa_cxt = (alsa_model_t *) kzalloc(sizeof (alsa_model_t), GFP_KERNEL);


    if (alsa_cxt)
    {

    }
    return alsa_cxt;
}

static void alsa_model_release(void *context)
{
    alsa_model_t *alsa_cxt = context;

    pr_info("%s\n", __func__);
    if (alsa_cxt)
    {
        if (alsa_cxt->card)
        {
            snd_card_free(alsa_cxt->card);
            pr_info("%s snd_card_free \n", __func__);
        }
        kfree(alsa_cxt);
    }
    pr_info("%s done\n", __func__);

}
#if 1
static int alsa_model_new_pcm(alsa_model_t *alsa_cxt, int device_idx, alsa_model_pcm_info_t *pcm_info)
{
    //struct snd_pcm *pcm;
    int err;


    err = snd_pcm_new(alsa_cxt->card, pcm_info->name, device_idx, 0, pcm_info->capture_count, &alsa_cxt->pcm);
    if (err < 0)
        return err;

    alsa_cxt->pcm->private_data = alsa_cxt;
    strcpy(alsa_cxt->pcm->name, pcm_info->name);
    snd_pcm_set_ops(alsa_cxt->pcm, SNDRV_PCM_STREAM_CAPTURE, &alsa_model_pcm_ops);

    return 0;
}
#endif
#if 0
static int alsa_model_new_pcm(alsa_model_t *alsa_cxt, int device_idx, alsa_model_pcm_info_t *pcm_info)
{
    struct snd_pcm *pcm;
    int err;


    err = snd_pcm_new(alsa_cxt->card, pcm_info->name, device_idx, 0, pcm_info->capture_count, &pcm);
    if (err < 0)
        return err;

    pcm->private_data = alsa_cxt;
    strcpy(pcm->name, pcm_info->name);
    snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &alsa_model_pcm_ops);

    return 0;
}
#endif
static int alsa_model_pcm_open(struct snd_pcm_substream *substream)
{
    alsa_model_t *alsa_cxt = snd_pcm_substream_chip(substream);
    struct snd_pcm_runtime *runtime = substream->runtime;
    alsa_cb_info_t *cb_info = NULL;

    pr_info("%s\n", __func__);

    alsa_cxt->substream = substream;
    runtime->hw = alsa_cxt->hw_parm;
    snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS);
    cb_info = &alsa_cxt->cb_info[ALSA_MODEL_OPEN_CB];
    if (cb_info->cb_func)
    {
        cb_info->cb_func(cb_info->cb_cxt);
    }



    return 0;
}

static int alsa_model_pcm_close(struct snd_pcm_substream *substream)
{
    alsa_model_t *alsa_cxt = snd_pcm_substream_chip(substream);
    alsa_cb_info_t *cb_info = NULL;


    alsa_cxt->substream = NULL;
    cb_info = &alsa_cxt->cb_info[ALSA_MODEL_CLOSE_CB];
    if (cb_info->cb_func)
    {
        cb_info->cb_func(cb_info->cb_cxt);
    }

    return 0;
}

static int alsa_model_hw_params(struct snd_pcm_substream *substream, struct snd_pcm_hw_params *hw_params)
{
    pr_info("%s() alloc buffer size = %u"" periods = %u period_bytes = %u\n", __func__, params_buffer_bytes(hw_params), params_periods(hw_params), params_period_bytes(hw_params));

    if (substream->runtime->dma_area)
    {
        substream->runtime->dma_area = NULL;
    }

    return snd_pcm_lib_alloc_vmalloc_buffer(substream, params_buffer_bytes(hw_params));
}

static int alsa_model_hw_free(struct snd_pcm_substream *substream)
{
    pr_info("%s\n", __func__);

    if (substream->runtime->dma_area)
    {
        substream->runtime->dma_area = NULL;
        substream->runtime->dma_addr = 0;
        substream->runtime->dma_bytes = 0;
    }

    return snd_pcm_lib_free_vmalloc_buffer(substream);
}

static int alsa_model_prepare(struct snd_pcm_substream *substream)
{
    alsa_model_t *alsa_cxt = snd_pcm_substream_chip(substream);

    pr_info("%s\n", __func__);

    alsa_cxt->capture_transfer_done = 0;
    alsa_cxt->hwptr = 0;

    return 0;
}

static int alsa_model_card_trigger(struct snd_pcm_substream *substream, int cmd)
{
    alsa_model_t *alsa_cxt = snd_pcm_substream_chip(substream);
    alsa_cb_info_t *cb_info = NULL;

    pr_info("%s %p\n", __func__,substream);

    switch (cmd)
    {
    case SNDRV_PCM_TRIGGER_START:
        cb_info = &alsa_cxt->cb_info[ALSA_MODEL_CAPTURE_START_CB];
        if (cb_info->cb_func)
        {
            cb_info->cb_func(cb_info->cb_cxt);
        }
        atomic_set(&alsa_cxt->stream_started, 1);
        break;
    case SNDRV_PCM_TRIGGER_STOP:
        cb_info = &alsa_cxt->cb_info[ALSA_MODEL_CAPTURE_STOP_CB];
        if (cb_info->cb_func)
        {
            cb_info->cb_func(cb_info->cb_cxt);
        }
        atomic_set(&alsa_cxt->stream_started, 0);
        break;
    default:
        return -EINVAL;
    }

    //schedule_work(&alsa_cxt->wq_trigger);

    return 0;
}

static snd_pcm_uframes_t alsa_model_pointer(struct snd_pcm_substream *substream)
{
    alsa_model_t *alsa_cxt = snd_pcm_substream_chip(substream);
    unsigned long flags;
    snd_pcm_uframes_t hwptr;

    spin_lock_irqsave(&alsa_cxt ->slock, flags);
    hwptr = alsa_cxt ->hwptr;
    spin_unlock_irqrestore(&alsa_cxt ->slock, flags);
//    pr_info("%s %d\n",__func__,hwptr);

    return hwptr;
}

void alsa_model_feed_data(alsa_model_handle_t handle, U8_T *buf, SIZE_T length)
{
    alsa_model_t *alsa_cxt = handle;
    struct snd_pcm_runtime *runtime;
    struct snd_pcm_substream *substream;

    unsigned int oldptr;
    unsigned int stride;
    unsigned int cnt;
    size_t copy_frames;
    size_t remain_frames;
    //u8 *buffer=buf;


    bool period_elapsed = false;
    //  int i;
    // pr_info("%s %d: ",__func__,length);
    // for(i=0;i<16;i++)
    // {
    // 	pr_info("%02x ",buf[i]);
    // }
    // pr_info("\n");

    if (!alsa_cxt->substream)
    {
        //    	pr_info("NO substream\n");
        return;
    }

    if (!length)
    {
        pr_info("NO length\n");
        return;
    }
    if (!alsa_cxt->substream->runtime)
    {
        pr_info("NO runtime\n");
        return;

    }
    if (!alsa_cxt->substream->runtime->frame_bits)
    {
        pr_info("frame_bits 0\n");
        return;
    }

    substream = alsa_cxt->substream;
    runtime = alsa_cxt->substream->runtime;
    stride = runtime->frame_bits >> 3;
    oldptr = alsa_cxt->hwptr;

    /* bytes to copy_frames */
    remain_frames = length / stride;
    copy_frames = remain_frames;
    //pr_info("alsa stride:%d remain:%d copy:%d \n", stride, remain_frames, copy_frames);
    /*    
        if(oldptr)
        {
            copy_frames=runtime->buffer_size-oldptr;
            if(remain_frames<copy_frames)
                    copy_frames=remain_frames;
            memcpy(runtime->dma_area + oldptr * stride, buffer, copy_frames * stride);
            buffer+=copy_frames * stride;
            remain_frames-=copy_frames;
            alsa_cxt->hwptr+=copy_frames;
        }

        while(remain_frames)
        {
            if(remain_frames>=runtime->buffer_size)
            {
                    copy_frames=runtime->buffer_size;
            }else
            {
                    copy_frames=remain_frames;
            }
            memcpy(runtime->dma_area, buffer, copy_frames * stride);
            buffer+=copy_frames * stride;
            remain_frames-=copy_frames;

        }
     */
    /* buffer address */
    if (oldptr + copy_frames >= runtime->buffer_size)
    {
        cnt = runtime->buffer_size - oldptr;
        memcpy(runtime->dma_area + oldptr * stride, buf, cnt * stride);
        memcpy(runtime->dma_area, buf + cnt * stride, copy_frames * stride - cnt * stride);
    }
    else
    {
        memcpy(runtime->dma_area + oldptr * stride, buf, copy_frames * stride);
    }

    snd_pcm_stream_lock(substream);
    alsa_cxt->hwptr += copy_frames;
    if (alsa_cxt->hwptr >= runtime->buffer_size)
        alsa_cxt->hwptr -= runtime->buffer_size;

    alsa_cxt->capture_transfer_done += copy_frames;
    if (alsa_cxt->capture_transfer_done >= runtime->period_size)
    {
        alsa_cxt->capture_transfer_done -= runtime->period_size;
        period_elapsed = true;
    }
    snd_pcm_stream_unlock(substream);

    if (period_elapsed)
        snd_pcm_period_elapsed(substream);
}

void alsa_model_suspend(alsa_model_handle_t handle)
{
    alsa_model_t *alsa_cxt = handle;
    snd_power_change_state(alsa_cxt->card, SNDRV_CTL_POWER_D3hot);
    snd_pcm_suspend_all(alsa_cxt->pcm);
}

void alsa_model_resume(alsa_model_handle_t handle)
{
    alsa_model_t *alsa_cxt = handle;
    snd_power_change_state(alsa_cxt->card, SNDRV_CTL_POWER_D0);
}

void alsa_model_register_callback(alsa_model_handle_t handle, alsa_model_callback_e no, alsa_model_callback_fun_t cb_func, void *cxt)
{
    alsa_model_t *alsa_cxt = handle;

    if (alsa_cxt)
    {
        do
        {
            if (!cb_func)
                break;
            if (no < 0 || no >= ALSA_MODEL_MAX_CB_NO)
                break;
            alsa_cxt->cb_info[no].cb_func = cb_func;
            alsa_cxt->cb_info[no].cb_cxt = cxt;
        }
        while (0);

    }

}

alsa_model_handle_t alsa_model_init(cxt_mgr_handle_t cxt_mgr, alsa_model_setup_t *alsa_setup_info)
{
    alsa_model_t *alsa_cxt = NULL;
    int i, min, max;
    //	int ret;

    if (cxt_mgr)
    {
        struct snd_card *card;
        int ret;
        struct device *dev;

        enum
        {
            ALSA_MODEL_NO_ERROR = 0,
            ALSA_MODEL_ERROR_ALLOC,
            ALSA_MODEL_NO_SETUP_INFO,
            ALSA_MODEL_CREATE_SND_CARD_ERROR,
            ALSA_MODEL_CREATE_PCM_ERR,
            ALSA_MODEL_REGISTER_SND_CARD_ERROR,
        } err = ALSA_MODEL_NO_ERROR;

        do
        {
            if (!alsa_setup_info)
            {
                err = ALSA_MODEL_NO_SETUP_INFO;
                break;
            }
            alsa_cxt = cxt_manager_add_cxt(cxt_mgr, ALSA_CXT_ID, alsa_model_alloc, alsa_model_release);
            if (!alsa_cxt)
            {
                err = ALSA_MODEL_ERROR_ALLOC;
                break;
            }
            dev = cxt_manager_get_dev(cxt_mgr);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)
            ret = snd_card_new(dev, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1, THIS_MODULE, 0, &card);
#else
            ret = snd_card_create(SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1, THIS_MODULE, 0, &card);
#endif
            if (ret < 0)
            {
                err = ALSA_MODEL_CREATE_SND_CARD_ERROR;
                break;
            }
            alsa_cxt->card = card;
            /* init lock */
            mutex_init(&alsa_cxt->mlock);
            spin_lock_init(&alsa_cxt->slock);

            INIT_LIST_HEAD(&alsa_cxt->list);
            for (i = 0; i < alsa_setup_info->pcm_count; i++)
            {
                ret = alsa_model_new_pcm(alsa_cxt, i, &alsa_setup_info->pcm_info[i]);
                if (ret != 0)
                {
                    break;
                }
            }
            if (ret < 0)
            {
                err = ALSA_MODEL_CREATE_PCM_ERR;
                break;
            }
            alsa_cxt->hw_parm = alsa_model_hw;
            alsa_cxt->hw_parm.formats = 0;
            alsa_cxt->hw_parm.rates = 0;
            for (i = 0; i < ALSA_MODEL_FMT_COUNT; i++)
            {
                if (alsa_setup_info->support_fmt_mask & BIT(i))
                {
                    switch (i)
                    {
                    case _ALSA_MODEL_FMT_S16_LE:
                        alsa_cxt->hw_parm.formats |= SNDRV_PCM_FMTBIT_S16_LE;
                        break;
                    case _ALSA_MODEL_FMT_S24_LE:
                        alsa_cxt->hw_parm.formats |= SNDRV_PCM_FMTBIT_S24_LE;
                        break;
                    }
                }
            }
            for (i = 0, min = ALSA_MODEL_RATE_COUNT, max = -1; i < ALSA_MODEL_RATE_COUNT; i++)
            {
                if (alsa_setup_info->support_rate_mask & BIT(i))
                {
                    if (min > i)
                        min = i;
                    if (max < i)
                        max = i;
                    switch (i)
                    {
                    case _ALSA_MODEL_RATE_32K:
                        alsa_cxt->hw_parm.rates |= SNDRV_PCM_RATE_32000;
                        break;
                    case _ALSA_MODEL_RATE_44_1K:
                        alsa_cxt->hw_parm.rates |= SNDRV_PCM_RATE_44100;
                        break;
                    case _ALSA_MODEL_RATE_48K:
                        alsa_cxt->hw_parm.rates |= SNDRV_PCM_RATE_48000;
                        break;
                    case _ALSA_MODEL_RATE_96K:
                        alsa_cxt->hw_parm.rates |= SNDRV_PCM_RATE_96000;
                        break;
                    case _ALSA_MODEL_RATE_192K:
                        alsa_cxt->hw_parm.rates |= SNDRV_PCM_RATE_192000;
                        break;

                    default:
                        break;
                    }
                }
            }
            switch (min)
            {
            case _ALSA_MODEL_RATE_32K:
                alsa_cxt->hw_parm.rate_min = 32000;
                break;
            case _ALSA_MODEL_RATE_44_1K:
                alsa_cxt->hw_parm.rate_min = 44100;
                break;
            case _ALSA_MODEL_RATE_48K:
                alsa_cxt->hw_parm.rate_min = 48000;
                break;
            case _ALSA_MODEL_RATE_96K:
                alsa_cxt->hw_parm.rate_min = 96000;
                break;
            case _ALSA_MODEL_RATE_192K:
                alsa_cxt->hw_parm.rate_min = 192000;
                break;
            default:
                break;
            }
            switch (max)
            {
            case _ALSA_MODEL_RATE_32K:
                alsa_cxt->hw_parm.rate_max = 32000;
                break;
            case _ALSA_MODEL_RATE_44_1K:
                alsa_cxt->hw_parm.rate_max = 44100;
                break;
            case _ALSA_MODEL_RATE_48K:
                alsa_cxt->hw_parm.rate_max = 48000;
                break;
            case _ALSA_MODEL_RATE_96K:
                alsa_cxt->hw_parm.rate_max = 96000;
                break;
            case _ALSA_MODEL_RATE_192K:
                alsa_cxt->hw_parm.rate_max = 192000;
                break;
            default:
                break;
            }
            if (alsa_setup_info->period_size)
                alsa_cxt->hw_parm.period_bytes_min = alsa_setup_info->period_size;
            if (alsa_setup_info->max_period_num)
                alsa_cxt->hw_parm.periods_max = alsa_setup_info->max_period_num;
            alsa_cxt->hw_parm.period_bytes_max = alsa_cxt->hw_parm.period_bytes_min * alsa_cxt->hw_parm.periods_max;
            alsa_cxt->hw_parm.buffer_bytes_max = alsa_cxt->hw_parm.period_bytes_max;
            /* snd_card->driver name size = 16 */
            strncpy(card->driver, alsa_setup_info->name, 16);
            strncpy(card->shortname, alsa_setup_info->name, 32);
            sprintf(card->longname, "%s", alsa_setup_info->name);

            /* set device dev */

            snd_card_set_dev(card, dev);

            ret = snd_card_register(card);
            if (ret < 0)
            {
                err = ALSA_MODEL_REGISTER_SND_CARD_ERROR;
                break;
            }





        }
        while (0);
        if (err != ALSA_MODEL_NO_ERROR)
        {
            switch (err)
            {
            case ALSA_MODEL_REGISTER_SND_CARD_ERROR:
            case ALSA_MODEL_CREATE_PCM_ERR:
                snd_card_free(card);
                // fall through
            case ALSA_MODEL_CREATE_SND_CARD_ERROR:
                cxt_manager_unref_context(alsa_cxt);
                // fall through
            case ALSA_MODEL_ERROR_ALLOC:
            case ALSA_MODEL_NO_SETUP_INFO:
                break;
            default:
                break;
            }
        }

    }
    return (alsa_model_handle_t) alsa_cxt;
}
