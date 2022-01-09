/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * v4l2_model_videobuf2.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " "%s, %d: " fmt, __func__, __LINE__

#include "typedef.h"
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <media/v4l2-device.h>
#include <media/videobuf2-vmalloc.h>
#include <media/videobuf2-dma-sg.h>
#include <media/videobuf2-dma-contig.h>
#include <media/v4l2-common.h>
#include <linux/videodev2.h>

#include "queue.h"
#include "sys.h"
#include "v4l2_model.h"
#include "v4l2_model_device.h"
#include "v4l2_model_videobuf2.h"
#include "pic_bmp.h"

//static void v4l2_model_setup_buffer_cb_context(struct vb2_buffer *vb,v4l2_model_buffer_info_t *buffer_cb_context);

typedef enum _v4l2_model_vb2_image_type_e
{
    V4L2_MODEL_VB2_IMAGE_NOT_LOADED = 0,
    V4L2_MODEL_VB2_IMAGE_NO_SIGNAL,
    V4L2_MODEL_VB2_IMAGE_OUT_OF_RANGE,
    V4L2_MODEL_VB2_IMAGE_COPY_PROTECTION,
} v4l2_model_vb2_image_type_e;

typedef struct {
	struct mutex qlock;
	struct list_head buffer_list;
	u32 frames;
	v4l2_model_buffer_type_e queue_type;
	void *alloc_ctx;
	spinlock_t queuelock;
	u8 *image_data;
	v4l2_model_vb2_image_type_e image_type;
} v4l2_model_vb2_context_t;

typedef struct {
	/* common v4l buffer stuff -- must be first */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
	struct vb2_v4l2_buffer vb;
#else
	struct vb2_buffer vb;
#endif
	struct list_head list;

	unsigned long length;
	unsigned long bytesused;

	u32 pts;

	v4l2_model_buffer_info_t buffer_info;
	v4l2_model_buf_desc_t *buf_descs[VIDEO_MAX_PLANES];

} v4l2_model_vb2_buffer_t;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,8,0)
static int v4l2_model_qops_queue_setup(struct vb2_queue *q,
                                       unsigned int *num_buffers, unsigned int *num_planes,
                                       unsigned int sizes[], struct device *alloc_ctxs[])
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
static int v4l2_model_qops_queue_setup(struct vb2_queue *q,
				       unsigned int *num_buffers,
				       unsigned int *num_planes,
				       unsigned int sizes[], void *alloc_ctxs[])
#elif LINUX_VERSION_CODE>=KERNEL_VERSION(4,4,0)
static int v4l2_model_qops_queue_setup(struct vb2_queue *q, const void *parg,
				       unsigned int *num_buffers,
				       unsigned int *num_planes,
				       unsigned int sizes[], void *alloc_ctxs[])
#else

static int v4l2_model_qops_queue_setup(struct vb2_queue *q,
				       const struct v4l2_format *fmt,
				       unsigned int *num_buffers,
				       unsigned int *num_planes,
				       unsigned int sizes[], void *alloc_ctxs[])
#endif
{
	v4l2_model_context_t *v4l2m_context = vb2q_to_v4l2context(q);
	v4l2_model_vb2_context_t *vb2_context =
	    (v4l2_model_vb2_context_t *) v4l2m_context->vb2_context;
	v4l2_model_callback_item_t *callback_item =
	    &v4l2m_context->callbacks[V4L2_MODEL_CALLBACK_QUEUE_SETUP];
	int width, height;
	const framegrabber_pixfmt_t *pixfmt;
	framegrabber_pixfmt_enum_t pixfmt_out;

    framegrabber_g_out_framesize(v4l2m_context->framegrabber_handle, &width, &height);	//test

	pixfmt = framegrabber_g_out_pixelfmt(v4l2m_context->framegrabber_handle);
	pixfmt_out = pixfmt->pixfmt_out;
    pr_info("pixelformat %s\n", pixfmt->name);

    *num_planes = pixfmt->num_planes;

    switch (*num_planes) {
        case 3:
            if (vb2_context->alloc_ctx)
                alloc_ctxs[2] = vb2_context->alloc_ctx;
            sizes[2] = framegrabber_g_out_planarbuffersize(v4l2m_context->framegrabber_handle, 3);
            pr_info("size of plane 3 %d\n", sizes[2]);
            // fall through
        case 2:
            if (vb2_context->alloc_ctx)
                alloc_ctxs[1] = vb2_context->alloc_ctx;
            sizes[1] = framegrabber_g_out_planarbuffersize(v4l2m_context->framegrabber_handle, 2);
            pr_info("size of plane 2 %d\n", sizes[1]);
            // fall through
        case 1:
            if (vb2_context->alloc_ctx)
                alloc_ctxs[0] = vb2_context->alloc_ctx;
            sizes[0] = framegrabber_g_out_planarbuffersize(v4l2m_context->framegrabber_handle, 1);
            pr_info("size of plane 1 %d\n", sizes[0]);
    }
    pr_info("number of requested planes %d\n", *num_planes);
    pr_info("number of requested buffers %d\n", *num_buffers);

    if (v4l2m_context->pic_bmp_handle)
    {
        if (vb2_context->image_data)
        {
            vfree(vb2_context->image_data);
            vb2_context->image_data = NULL;
        }
        /*allocate full framebuffer size (i.e. size of all planes) even in multiplanar architecture */
        vb2_context->image_data = vmalloc(framegrabber_g_out_framebuffersize(v4l2m_context->framegrabber_handle));

        pic_bmp_prepare(v4l2m_context->pic_bmp_handle, 640, 480, width, height);
        load_no_signal_image(v4l2m_context->pic_bmp_handle, vb2_context->image_data, width, height
                , (pixfmt_out == YUYV) ? PIC_BMP_PIXFMT_YUYV
                          : (pixfmt_out == YVU420 || pixfmt_out == YVU420M) ? PIC_BMP_PIXFMT_YV12
                          : (pixfmt_out == NV12 || pixfmt_out == NV12M) ? PIC_BMP_PIXFMT_NV12
                          : (pixfmt_out == BGR3) ? PIC_BMP_PIXFMT_RGB24
                          : PIC_BMP_PIXFMT_OTHERS);
        vb2_context->image_type = V4L2_MODEL_VB2_IMAGE_NO_SIGNAL;
    }


    v4l2m_context->inibuffer_create = 0;
    v4l2m_context->inibuffer_index = 0;

	if (callback_item->callback) {
		callback_item->data.u.queue_setup_info.buffer_num =
		    *num_buffers;
		callback_item->callback(&callback_item->data);
	}

	return 0;
}

static void v4l2_model_qops_wait_prepare(struct vb2_queue *q)
{
	return vb2_ops_wait_prepare(q);
}

static void v4l2_model_qops_wait_finish(struct vb2_queue *q)
{
	return vb2_ops_wait_finish(q);
}

//invoked when each new buffer after it has been allocated

static int v4l2_model_qops_buf_init(struct vb2_buffer *vb)
{
    int i;
	v4l2_model_context_t *v4l2m_context = vb2b_to_v4l2context(vb);
	v4l2_model_callback_item_t *callback_item =
	    &v4l2m_context->callbacks[V4L2_MODEL_CALLBACK_BUFFER_INIT];

#if LINUX_VERSION_CODE >=KERNEL_VERSION(4,4,0)
    v4l2_model_vb2_buffer_t *v4l2_buf =
            container_of(to_vb2_v4l2_buffer(vb), v4l2_model_vb2_buffer_t, vb);
#else
    v4l2_model_vb2_buffer_t *v4l2_buf =
	    container_of(vb, v4l2_model_vb2_buffer_t, vb);
#endif
    v4l2_model_buffer_info_t *buffer_info = &v4l2_buf->buffer_info;

	v4l2m_context->inibuffer_create++;

    for (i=0; i < vb->num_planes; i++) {
        struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, i);

        buffer_info->buf_count[i] = sgt->nents;

        v4l2_buf->buf_descs[i] =
                kzalloc(sizeof(v4l2_model_buf_desc_t) * buffer_info->buf_count[i], GFP_KERNEL);
        pr_info("v4l2_buf %p, v4l2_buf->buffer_info %p plane %d buf_descs[%d] %p\n", v4l2_buf, &v4l2_buf->buffer_info, i, i, v4l2_buf->buf_descs[i]);
    }


	if (callback_item->callback) {
		v4l2_model_callback_parameter_t *parm = &callback_item->data;
		callback_item->callback(parm);
	}
        v4l2m_context->inibuffer_index =v4l2m_context->inibuffer_create;
	return 0;
}

//invoked when user space queues the buffer

static int v4l2_model_qops_buf_prepare(struct vb2_buffer *vb)
{
	int i;
	v4l2_model_context_t *v4l2m_context = vb2b_to_v4l2context(vb);
	v4l2_model_vb2_context_t *vb2_cxt = v4l2m_context->vb2_context;
	v4l2_model_callback_item_t *callback_item =
	    &v4l2m_context->callbacks[V4L2_MODEL_CALLBACK_BUFFER_PREPARE];
#if LINUX_VERSION_CODE >=KERNEL_VERSION(4,4,0)
	v4l2_model_vb2_buffer_t *v4l2_buf =
	    container_of(to_vb2_v4l2_buffer(vb), v4l2_model_vb2_buffer_t, vb);
#else
	v4l2_model_vb2_buffer_t *v4l2_buf =
	    container_of(vb, v4l2_model_vb2_buffer_t, vb);
#endif

	const framegrabber_pixfmt_t *pixfmt;
	int width, height;
	int buffer_size = 0;

//	pr_info("%s >>>>>\n",__func__);
	framegrabber_g_out_framesize(v4l2m_context->framegrabber_handle, &width, &height);
	pixfmt = framegrabber_g_out_pixelfmt(v4l2m_context->framegrabber_handle);

    pr_debug("framesize %dx%d %dbit\n", width, height, pixfmt->depth);
    pr_debug("numberofplanes = %d\n", vb->num_planes);


    v4l2_buf->buffer_info.planes = vb->num_planes;
	v4l2_buf->buffer_info.buf_type = vb2_cxt->queue_type;
	for (i = 0; i < vb->num_planes; i++) {
		v4l2_model_buffer_info_t *buffer_info = &v4l2_buf->buffer_info;
		switch (vb2_cxt->queue_type) {
		case V4L2_MODEL_BUF_TYPE_VMALLOC:
			buffer_info->buf_count[i] = 1;
			v4l2_buf->buf_descs[i] =
			    kzalloc(sizeof(v4l2_model_buf_desc_t) * buffer_info->buf_count[i], GFP_KERNEL);
			if (v4l2_buf->buf_descs[i]) {
				v4l2_model_buf_desc_t *buf_descs = v4l2_buf->buf_descs[i];
				buf_descs[i].addr = (unsigned long)vb2_plane_vaddr(vb, i);
				buf_descs[i].size = vb2_plane_size(vb, i);
			    buffer_size = buf_descs[i].size;
            }
			buffer_info->buf_info[i] = v4l2_buf->buf_descs[i];
			break;
		case V4L2_MODEL_BUF_TYPE_DMA_SG:
			{

				struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, i);
				struct scatterlist *sg;
				int j;

				buffer_info->buf_count[i] = sgt->nents;

				if (v4l2_buf->buf_descs[i]) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,19,0)
					if (!dma_map_sg
					    (v4l2m_context->dev, sgt->sgl,
					     sgt->nents, DMA_FROM_DEVICE))
						pr_err("error dma_map_sg\n");
#endif
                    pr_debug("plane[%d]: number of sg buffers %d\n", i, sgt->nents);
					for_each_sg(sgt->sgl, sg, sgt->nents, j) {
						v4l2_model_buf_desc_t *buf_descs = v4l2_buf->buf_descs[i];

						buf_descs[j].addr = sg_dma_address(sg);
						buf_descs[j].size = sg_dma_len(sg);
		                buffer_size += buf_descs[j].size;

                        pr_debug("plane[%d]: sg_buffer %lx sg_buffer_size %ld\n", i, buf_descs[j].addr, buf_descs[j].size);
					}
					buffer_info->buf_info[i] = v4l2_buf->buf_descs[i];
				}
			}
			break;
		case V4L2_MODEL_BUF_TYPE_DMA_CONT:
        {
            int j;
            v4l2_buf->buffer_info.buf_count[i] = 1;
            v4l2_buf->buf_descs[i] = kzalloc(sizeof(v4l2_model_buf_desc_t) *
                                             buffer_info->buf_count[i], GFP_KERNEL);
            if (v4l2_buf->buf_descs[i]) {
                v4l2_model_buf_desc_t *buf_descs = v4l2_buf->buf_descs[i];
                for (j = 0; j < buffer_info->buf_count[i]; j++) {
                    buf_descs[j].addr = vb2_dma_contig_plane_dma_addr(vb, i);
                    buf_descs[j].size = vb2_plane_size(vb, i);
                    buffer_size += buf_descs[j].size;
                }
            }
            buffer_info->buf_info[i] = v4l2_buf->buf_descs[i];
        }
                break;
        }
		/* set vb2 plane bytesused size */
        vb2_set_plane_payload(vb, i,vb2_plane_size(vb, i));

		v4l2_buf->length += vb2_plane_size(vb, i);
        pr_debug("plane[%d]: buffer_size = %d plane_size = %ld\n", i, buffer_size, vb2_plane_size(vb, i));
	}

	if (callback_item->callback) {
		v4l2_model_callback_parameter_t *parm = &callback_item->data;
		parm->u.buffer_prepare_info.buffer_info =
		    &v4l2_buf->buffer_info;
		callback_item->callback(parm);
	}

	return 0;
}

static void v4l2_model_qops_buf_cleanup(struct vb2_buffer *vb)
{
	v4l2_model_context_t *v4l2m_context = vb2b_to_v4l2context(vb);
	v4l2_model_callback_item_t *callback_item =
	    &v4l2m_context->callbacks[V4L2_MODEL_CALLBACK_BUFFER_CLEANUP];
#if LINUX_VERSION_CODE >=KERNEL_VERSION(4,4,0)
	v4l2_model_vb2_buffer_t *v4l2_buf =
	    container_of(to_vb2_v4l2_buffer(vb), v4l2_model_vb2_buffer_t, vb);
#else
	v4l2_model_vb2_buffer_t *v4l2_buf =
	    container_of(vb, v4l2_model_vb2_buffer_t, vb);
#endif
	int i;
	pr_info("===================== vb %p \n", vb);

	//return; //rr3

	for (i = 0; i < vb->num_planes; i++) {
		switch (v4l2_buf->buffer_info.buf_type) {
		case V4L2_MODEL_BUF_TYPE_VMALLOC:
			break;
		case V4L2_MODEL_BUF_TYPE_DMA_SG:
			{
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,19,0)
				struct sg_table *sgt =
				    vb2_dma_sg_plane_desc(vb, i);

				dma_unmap_sg(v4l2m_context->dev, sgt->sgl,
					     sgt->nents, DMA_FROM_DEVICE);

#endif
			}
			break;
		case V4L2_MODEL_BUF_TYPE_DMA_CONT:
			break;
		}
		kfree(v4l2_buf->buf_descs[i]);
		v4l2_buf->buf_descs[i] = NULL;
	}

	if (callback_item->callback) {
		callback_item->callback(&callback_item->data);
	}
}

// invoked just before the buffer is passed back to user space (process context)
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,15,0)
static int v4l2_model_qops_buf_finish(struct vb2_buffer *vb)
#else

static void v4l2_model_qops_buf_finish(struct vb2_buffer *vb)
#endif
{
	v4l2_model_context_t *v4l2m_context = vb2b_to_v4l2context(vb);
	//v4l2_model_vb2_context_t *vb2_context=(v4l2_model_vb2_context_t *)v4l2m_context->vb2_context;
	v4l2_model_callback_item_t *callback_item =
	    &v4l2m_context->callbacks[V4L2_MODEL_CALLBACK_BUFFER_FINISH];
	//pr_info("%s vb %p \n", __func__, vb);

	if (callback_item->callback) {
		callback_item->callback(&callback_item->data);
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,15,0)
	return 0;
#else
	return;
#endif
}

static int v4l2_model_qops_start_streaming(struct vb2_queue *q,
					   unsigned int count)
{
	v4l2_model_context_t *v4l2m_context = vb2q_to_v4l2context(q);
	v4l2_model_vb2_context_t *vb2_context =
	    (v4l2_model_vb2_context_t *) v4l2m_context->vb2_context;
	v4l2_model_callback_item_t *callback_item =
	    &v4l2m_context->callbacks[V4L2_MODEL_CALLBACK_STREAMON];
	pr_info("%s...\n", __func__);
	if (callback_item->callback) {
		callback_item->callback(&callback_item->data);
	}
	vb2_context->frames = 0;
	framegrabber_start_streaming(v4l2m_context->framegrabber_handle, true);
	//v4l2m_context->current_framebuf_info.rcv_size=0;
	v4l2_model_reset_video_data(v4l2m_context);

    v4l2m_context->inibuffer_index =v4l2m_context->inibuffer_create;

	return 0;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,16,0)
static int v4l2_model_qops_stop_streaming(struct vb2_queue *q)
#else

static void v4l2_model_qops_stop_streaming(struct vb2_queue *q)
#endif
{
	v4l2_model_context_t *v4l2m_context = vb2q_to_v4l2context(q);
	v4l2_model_vb2_context_t *vb2_context =
	    (v4l2_model_vb2_context_t *) v4l2m_context->vb2_context;
	v4l2_model_callback_item_t *callback_item =
	    &v4l2m_context->callbacks[V4L2_MODEL_CALLBACK_STREAMOFF];
	//v4l2_model_vb2_buffer_t *buf,*next_buf;

	if (callback_item->callback) {
		callback_item->callback(&callback_item->data);
	}
	if (list_empty(&vb2_context->buffer_list)) {
		pr_info("%s vb2_context->buffer_list empty\n", __func__);
	} else {
		v4l2_model_queue_cancel(v4l2m_context);
		
		pr_info("%s...\n", __func__);
	}
	//  v4l2m_context->current_framebuf_info.vbuf_info=NULL;
	//  v4l2m_context->current_framebuf_info.rcv_size=0;

	v4l2_model_reset_video_data(v4l2m_context);	

	framegrabber_start_streaming(v4l2m_context->framegrabber_handle, false);
	//framegrabber_s_out_framesize(v4l2m_context->framegrabber_handle, 0, 0);
        v4l2m_context->inibuffer_index=0;
	v4l2m_context->inibuffer_create=0;
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,16,0)
	return 0;
#endif
}

//invoked when pass actual ownership of the buffer to the driver,

static void v4l2_model_qops_buf_queue(struct vb2_buffer *vb)
{
	unsigned long flags = 0;

	v4l2_model_context_t *v4l2m_context = vb2b_to_v4l2context(vb);
	v4l2_model_callback_item_t *callback_item =
	    &v4l2m_context->callbacks[V4L2_MODEL_CALLBACK_BUFFER_QUEUE];
#if LINUX_VERSION_CODE >=KERNEL_VERSION(4,4,0)
	v4l2_model_vb2_buffer_t *v4l2_model_buffer =
	    container_of(to_vb2_v4l2_buffer(vb), v4l2_model_vb2_buffer_t, vb);
#else
	v4l2_model_vb2_buffer_t *v4l2_model_buffer =
	    container_of(vb, v4l2_model_vb2_buffer_t, vb);
#endif
	v4l2_model_vb2_context_t *vb2_context = v4l2m_context->vb2_context;

	//pr_info("%s vb %p\n",__func__,vb);
	spin_lock_irqsave(&vb2_context->queuelock, flags);
	list_add_tail(&v4l2_model_buffer->list, &vb2_context->buffer_list);
	spin_unlock_irqrestore(&vb2_context->queuelock, flags);

	if (callback_item->callback) {
		callback_item->callback(&callback_item->data);
	}
}

const struct vb2_ops v4l2_model_qops = {
	.queue_setup = v4l2_model_qops_queue_setup,

	/* do nothing */
	.wait_prepare = v4l2_model_qops_wait_prepare,
	.wait_finish = v4l2_model_qops_wait_finish,
	.buf_init = v4l2_model_qops_buf_init,

	.buf_prepare = v4l2_model_qops_buf_prepare,
	.buf_cleanup = v4l2_model_qops_buf_cleanup,
	.buf_finish = v4l2_model_qops_buf_finish,
	.start_streaming = v4l2_model_qops_start_streaming,
	.stop_streaming = v4l2_model_qops_stop_streaming,
	.buf_queue = v4l2_model_qops_buf_queue,
};

// static void v4l2_model_setup_buffer_cb_context(struct vb2_buffer *vb,v4l2_model_buffer_info_t *buffer_cb_context)
// {
//      int i;
//      if(buffer_cb_context)
//      {
//              buffer_cb_context->planes=vb->num_planes;
//              for(i=0;i<vb->num_planes;i++)
//              {
//                      buffer_cb_context->plane_vaddr[i]=vb2_plane_vaddr(vb,i);
//                      buffer_cb_context->plane_size[i]=vb2_plane_size(vb,i);
//              }
//      }

// }

void *v4l2_model_vb2_init(struct vb2_queue *q, v4l2_model_devicetype_t dev_type,
			  v4l2_model_buffer_type_e queue_type,
			  struct device *dev, void *priv)
{
	int rc;
	v4l2_model_vb2_context_t *vb2_context = NULL;

	if (dev_type == DEVICE_TYPE_VBI)
		q->type = V4L2_BUF_TYPE_VBI_CAPTURE;
	else
		q->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	vb2_context = kzalloc(sizeof(v4l2_model_vb2_context_t), GFP_KERNEL);
	if (!vb2_context)
		return NULL;

	INIT_LIST_HEAD(&vb2_context->buffer_list);
	mutex_init(&vb2_context->qlock);
	spin_lock_init(&vb2_context->queuelock);
	vb2_context->queue_type = queue_type;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)
	q->timestamp_flags = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
	q->timestamp_type = V4L2_BUF_FLAG_TIMESTAMP_MONOTONIC;
#endif
	q->drv_priv = priv;
	q->buf_struct_size = sizeof(v4l2_model_vb2_buffer_t);
	q->ops = &v4l2_model_qops;
	q->lock = &vb2_context->qlock;
	switch (queue_type) {
	case V4L2_MODEL_BUF_TYPE_VMALLOC:
		q->mem_ops = &vb2_vmalloc_memops;
		q->io_modes = VB2_READ | VB2_MMAP | VB2_USERPTR;
		break;
	case V4L2_MODEL_BUF_TYPE_DMA_CONT:
		q->mem_ops = &vb2_dma_contig_memops;
		q->io_modes = VB2_READ | VB2_MMAP | VB2_DMABUF;
		q->gfp_flags = GFP_DMA32;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,8,0)
        vb2_context->alloc_ctx = vb2_dma_contig_init_ctx(dev);
        if (!vb2_context->alloc_ctx)
        {
            pr_err("vb2_dma_contig_init_ctx fail\n");
            kfree(vb2_context);
            return NULL;
        }
#else
        vb2_context->alloc_ctx = dev;
#endif
		break;
	case V4L2_MODEL_BUF_TYPE_DMA_SG:
		q->mem_ops = &vb2_dma_sg_memops;
		q->io_modes = VB2_READ | VB2_USERPTR | VB2_MMAP | VB2_DMABUF;
		q->gfp_flags = GFP_DMA32;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,8,0)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)
        vb2_context->alloc_ctx = vb2_dma_sg_init_ctx(dev);
        if (!vb2_context->alloc_ctx)
        { 
            pr_err("vb2_dma_sg_init_ctx fail\n");
            kfree(vb2_context);
            return NULL;
        }
#endif
#else
        vb2_context->alloc_ctx = dev;
#endif
		break;
	}


    vb2_context->image_data = NULL;
    vb2_context->image_type = V4L2_MODEL_VB2_IMAGE_NOT_LOADED;

	rc = vb2_queue_init(q);
	if (rc) {
		pr_err("%s vb2 queue init failed\n", __func__);
		kfree(vb2_context);
		return NULL;
	}

	return (void *)vb2_context;
}

void v4l2_model_vb2_release(void *context)
{
    v4l2_model_vb2_context_t *vb2_context = (v4l2_model_vb2_context_t *) context;

    if (vb2_context)
    {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,8,0)
        if (vb2_context->alloc_ctx)
        {
            switch (vb2_context->queue_type)
            {
            case V4L2_MODEL_BUF_TYPE_VMALLOC:
                pr_info("%s vmalloc should not have alloc_cxt \n", __func__);
                break;
            case V4L2_MODEL_BUF_TYPE_DMA_CONT:
                vb2_dma_contig_cleanup_ctx(vb2_context->alloc_ctx);
                break;
            case V4L2_MODEL_BUF_TYPE_DMA_SG:
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)                                
                vb2_dma_sg_cleanup_ctx(vb2_context->alloc_ctx);
#endif                    
                break;
            }

        }
#endif

        if (vb2_context->image_data) vfree(vb2_context->image_data);
        kfree(vb2_context);
    }
}

void v4l2_model_queue_cancel(void *context)
{
	v4l2_model_context_t *v4l2m_context = (v4l2_model_context_t *) context;
	v4l2_model_vb2_context_t *vb2_context =
	    (v4l2_model_vb2_context_t *) v4l2m_context->vb2_context;
	v4l2_model_vb2_buffer_t *buf;
	framegrabber_status_bitmask_e framegrabber_status;
	pr_info("%s\n", __func__);
	//spin_lock_irqsave(&vb2_context->queuelock, flags);
	while (!list_empty(&vb2_context->buffer_list)) {
		struct vb2_buffer *vb;

		buf =
		    list_first_entry(&vb2_context->buffer_list,
				     v4l2_model_vb2_buffer_t, list);
#if LINUX_VERSION_CODE >=KERNEL_VERSION(4,4,0)
		vb = &buf->vb.vb2_buf;
#else
		vb = &buf->vb;
#endif
		//pr_info("%s vb %p done\n",__func__,vb);
		list_del_init(&buf->list);
		framegrabber_status =
		    framegrabber_g_status(v4l2m_context->framegrabber_handle);

		if ((framegrabber_status &
		     (FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT |
		      FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT)) ==
		    (FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT |
		     FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT)) {

			vb2_buffer_done(vb, VB2_BUF_STATE_DONE);
		} else {
			pr_info("%s framegrabber_status %08x\n", __func__,
			       framegrabber_status);
			vb2_buffer_done(vb, VB2_BUF_STATE_ERROR);
		}

	}
	//spin_unlock_irqrestore(&vb2_context->queuelock, flags);
	pr_info("%s done\n", __func__);
}

void v4l2_model_next_buffer(v4l2_model_handle_t context,
			    v4l2_model_buffer_info_t ** buffer_info)
{
	//struct vb2_buffer *vb;
	v4l2_model_vb2_buffer_t *next_buffer;
	v4l2_model_context_t *v4l2m_context = (v4l2_model_context_t *) context;
	v4l2_model_vb2_context_t *vb2_context =
	    (v4l2_model_vb2_context_t *) v4l2m_context->vb2_context;
	unsigned long flags;

	spin_lock_irqsave(&vb2_context->queuelock, flags);
	if (list_empty(&vb2_context->buffer_list)) {
		pr_info("vb2 queue is empty\n");
		*buffer_info = NULL;
		spin_unlock_irqrestore(&vb2_context->queuelock, flags);
		return;
	}

	next_buffer =
	    list_first_entry(&vb2_context->buffer_list, v4l2_model_vb2_buffer_t,
			     list);
	spin_unlock_irqrestore(&vb2_context->queuelock, flags);

	//vb= &next_buffer->vb;
	//pr_info("%s vb %p\n",__func__,vb);
	*buffer_info = &next_buffer->buffer_info;
}

void v4l2_model_buffer_done(v4l2_model_handle_t context)
{
	unsigned long flags;
	struct vb2_buffer *vb;

	v4l2_model_context_t *v4l2m_context = (v4l2_model_context_t *) context;

	v4l2_model_vb2_context_t *vb2_context =
	    (v4l2_model_vb2_context_t *) v4l2m_context->vb2_context;
	v4l2_model_vb2_buffer_t *buf;
	framegrabber_status_bitmask_e framegrabber_status;

	/* get next v4l2_model_buffer from vb2 context buffer_list */
	spin_lock_irqsave(&vb2_context->queuelock, flags);
	if (!list_empty(&vb2_context->buffer_list)) {
		buf = list_first_entry(&vb2_context->buffer_list,
				       v4l2_model_vb2_buffer_t, list);
		list_del_init(&buf->list);
	} else {
		buf = NULL;
	}
	spin_unlock_irqrestore(&vb2_context->queuelock, flags);

	if (!buf) {
		pr_err("no buffer to serve\n");
		return;
	}
#if   LINUX_VERSION_CODE >= KERNEL_VERSION(4,5,0)
	vb = &buf->vb.vb2_buf;
	buf->vb.field = V4L2_FIELD_NONE;
	buf->vb.sequence = vb2_context->frames++;
	vb->timestamp = ktime_get_ns();
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
	vb = &buf->vb.vb2_buf;
	buf->vb.field = V4L2_FIELD_NONE;
	buf->vb.sequence = vb2_context->frames++;
	v4l2_get_timestamp(&buf->vb.timestamp);
#else
	vb = &buf->vb;
	vb->v4l2_buf.field = V4L2_FIELD_NONE;
	vb->v4l2_buf.sequence = vb2_context->frames++;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,9,0)
	v4l2_get_timestamp(&vb->v4l2_buf.timestamp);
#else
	{
		struct timespec ts;
		ktime_get_ts(&ts);
		vb->v4l2_buf.timestamp.tv_sec = ts.tv_sec;
		vb->v4l2_buf.timestamp.tv_usec = ts.tv_nsec / NSEC_PER_USEC;
	}
#endif
#endif
    if (v4l2m_context->pic_bmp_handle)
    {
        u32 input_status, hdcp_flag;
        u8 *ptr = NULL;
        const framegrabber_pixfmt_t *pixfmt = framegrabber_g_out_pixelfmt(v4l2m_context->framegrabber_handle);
        framegrabber_pixfmt_enum_t pixfmt_out = pixfmt->pixfmt_out;
        input_status = framegrabber_g_input_status(v4l2m_context->framegrabber_handle);
        hdcp_flag = framegrabber_g_hdcp_flag(v4l2m_context->framegrabber_handle);

        if (input_status != FRAMEGRABBER_INPUT_STATUS_OK || hdcp_flag)
        {
            pr_debug("framegrabber input status not ok");
            v4l2_model_vb2_image_type_e type = V4L2_MODEL_VB2_IMAGE_NO_SIGNAL;
            if (input_status == FRAMEGRABBER_INPUT_STATUS_NO_SIGNAL || input_status == FRAMEGRABBER_INPUT_STATUS_NO_POWER) {
                type = V4L2_MODEL_VB2_IMAGE_NO_SIGNAL;
            }
            else if (input_status == FRAMEGRABBER_INPUT_STATUS_NO_SYNC) {
                type = V4L2_MODEL_VB2_IMAGE_OUT_OF_RANGE;
            } else {
                if (hdcp_flag) {
                    type = V4L2_MODEL_VB2_IMAGE_COPY_PROTECTION;
                }
            }

            if (type != vb2_context->image_type)
            {
                if (type == V4L2_MODEL_VB2_IMAGE_NO_SIGNAL)
                {
                    int width, height;
                    framegrabber_g_out_framesize(v4l2m_context->framegrabber_handle, &width, &height);
                    load_no_signal_image(v4l2m_context->pic_bmp_handle, vb2_context->image_data, width, height
                            , (pixfmt_out == YUYV) ? PIC_BMP_PIXFMT_YUYV
                            : (pixfmt_out == YVU420 || pixfmt_out == YVU420M) ? PIC_BMP_PIXFMT_YV12
                            : (pixfmt_out == NV12 || pixfmt_out == NV12M) ? PIC_BMP_PIXFMT_NV12
                            : (pixfmt_out == BGR3) ? PIC_BMP_PIXFMT_RGB24
                            : PIC_BMP_PIXFMT_OTHERS);
                    pr_debug("load no signal done\n");
                }
                else if (type == V4L2_MODEL_VB2_IMAGE_COPY_PROTECTION)
                {
                    int width, height;
                    framegrabber_g_out_framesize(v4l2m_context->framegrabber_handle, &width, &height);
                    load_copy_protection_image(v4l2m_context->pic_bmp_handle, vb2_context->image_data, width, height
                            , (pixfmt_out == YUYV) ? PIC_BMP_PIXFMT_YUYV
                            : (pixfmt_out == YVU420 || pixfmt_out == YVU420M) ? PIC_BMP_PIXFMT_YV12
                            : (pixfmt_out == NV12 || pixfmt_out == NV12M) ? PIC_BMP_PIXFMT_NV12
                            : (pixfmt_out == BGR3) ? PIC_BMP_PIXFMT_RGB24
                            : PIC_BMP_PIXFMT_OTHERS);
                    pr_debug("load copy protection done\n");
                }
                else if (type == V4L2_MODEL_VB2_IMAGE_OUT_OF_RANGE)
                {
                    int width, height;
                    framegrabber_g_out_framesize(v4l2m_context->framegrabber_handle, &width, &height);
                    load_out_of_range_image(v4l2m_context->pic_bmp_handle, vb2_context->image_data, width, height
                            , (pixfmt_out == YUYV) ? PIC_BMP_PIXFMT_YUYV
                            : (pixfmt_out == YVU420 || pixfmt_out == YVU420M) ? PIC_BMP_PIXFMT_YV12
                            : (pixfmt_out == NV12 || pixfmt_out == NV12M) ? PIC_BMP_PIXFMT_NV12
                            : (pixfmt_out == BGR3) ? PIC_BMP_PIXFMT_RGB24
                            : PIC_BMP_PIXFMT_OTHERS);
                    pr_debug("load out of range done\n");
                }
                vb2_context->image_type = type;
            }

            ptr = vb2_context->image_data;
        }

        if (ptr)
        {
            switch (vb2_context->queue_type)
            {
                case V4L2_MODEL_BUF_TYPE_VMALLOC:
                    break;
                case V4L2_MODEL_BUF_TYPE_DMA_SG:
                {
                    int i;
                    u8 *srcBuffer = ptr;
                    for (i =0; i < vb->num_planes; i++) {
                        struct sg_table *sgt = vb2_dma_sg_plane_desc(vb, i);
                        unsigned planesize = vb2_plane_size(vb, i);

                        size_t bytes_copied = sg_copy_from_buffer(sgt->sgl, sg_nents_for_len(sgt->sgl, planesize), srcBuffer, planesize);
                        pr_debug("copied %zd bytes from %p to sg_buffer in plane %d\n", bytes_copied, srcBuffer, i);

                        srcBuffer+=planesize;
                    }
                }
                    break;
                case V4L2_MODEL_BUF_TYPE_DMA_CONT:
                    break;
            }
        }
    }

	framegrabber_status =
	    framegrabber_g_status(v4l2m_context->framegrabber_handle);

	if ((framegrabber_status &
         (FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT |
          FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT)) != (FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT |
                                                      FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT)) {
        pr_debug("no input detected\n");
    }

    vb2_buffer_done(vb, VB2_BUF_STATE_DONE);
//    vb2_buffer_done(vb, VB2_BUF_STATE_ERROR);
}

void v4l2_model_reset_video_data(v4l2_model_handle_t context)
{
	v4l2_model_context_t *v4l2m_cxt = (v4l2_model_context_t *) context;
	if (v4l2m_cxt->current_framebuf_info.vbuf_info)
		v4l2_model_buffer_done(v4l2m_cxt);
	v4l2m_cxt->current_framebuf_info.vbuf_info = NULL;
	v4l2m_cxt->current_framebuf_info.rcv_size = 0;
}

void v4l2_model_feed_video_data(v4l2_model_handle_t context, void *buffer, SIZE_T size)
{
	v4l2_model_context_t *v4l2m_cxt = (v4l2_model_context_t *) context;
	framebuffer_info_t *pframebuf_info;
	//unsigned line;
	int width, height;
	//pr_info("%s %p %p size %ld\n",__func__,context,buffer,size);

	if (v4l2m_cxt) {
		pframebuf_info = &v4l2m_cxt->current_framebuf_info;
		if (!pframebuf_info->vbuf_info) {
			v4l2_model_next_buffer(v4l2m_cxt,
					       &pframebuf_info->vbuf_info);
			//pr_info("framebuf change to %p\n",pframebuf_info->vbuf_info->plane_vaddr[0]);
		}

		framegrabber_g_input_framesize(v4l2m_cxt->framegrabber_handle,
					       &width, &height);
		if ((width == 0) || (height == 0)) {

			v4l2_model_buffer_done(v4l2m_cxt);
			pframebuf_info->vbuf_info = NULL;
		} else {
			if (pframebuf_info->vbuf_info) {
				framegrabber_framemode_e framemode;
				unsigned bytesperline;

				const framegrabber_pixfmt_t *pixfmt =
				    framegrabber_g_out_pixelfmt(v4l2m_cxt->
								framegrabber_handle);
				u8 *framebuf;
				u8 *linebuf = NULL;
				unsigned line;
				unsigned offset;
				unsigned remain_size = size;
				unsigned pix_size = (pixfmt->depth >> 3);
				unsigned copy_size = 0;
				unsigned char *buf = buffer;
				unsigned frame_remain_size;
				unsigned line_stride = 0;
				unsigned framebuf_size;
#define copy_to_framebuffer(lb,b,s) \
                                { \
                                        if(lb-framebuf+s<=framebuf_size) \
                                        { \
                                                memcpy(lb,b,s);\
					}else \
                                        { \
                                                pr_warn("copy may out of framebuf size %u offset %lu size %lu !\n",framebuf_size,(unsigned long)(lb-framebuf),(unsigned long)s) ;\
					}\
				}

				framemode =
				    framegrabber_g_input_framemode(v4l2m_cxt->
								   framegrabber_handle);
				bytesperline = framegrabber_g_out_bytesperline(v4l2m_cxt->framegrabber_handle, 1);
				//bytesperline = width*pix_size;
				framegrabber_g_out_framesize(v4l2m_cxt->framegrabber_handle, &width, &height);	

				framebuf =
				    (u8 *) pframebuf_info->vbuf_info->
				    buf_info[0]->addr;
				framebuf_size =
				    pframebuf_info->vbuf_info->buf_info[0]->
				    size;
				//line = pframebuf_info->rcv_size / (width * pix_size);
				//offset = pframebuf_info->rcv_size % (width * pix_size);
				line = pframebuf_info->rcv_size / bytesperline;
				offset = pframebuf_info->rcv_size % bytesperline;
				if (framemode ==
				    FRAMEGRABBER_FRAMEMODE_PROGRESS)
					line_stride = bytesperline;
				else if (framemode ==
					 FRAMEGRABBER_FRAMEMODE_INTERLACED)
					line_stride = 2 * bytesperline;

				if (framemode ==
				    FRAMEGRABBER_FRAMEMODE_PROGRESS) {
					//pr_info("Progress line %d offset %d\n",line,line_stride*line);
					linebuf = &framebuf[line_stride * line];
				} else if (framemode ==
					   FRAMEGRABBER_FRAMEMODE_INTERLACED) {
					if (line >= (height / 2)) {
						line -= (height / 2);
						linebuf =
						    &framebuf[bytesperline +
							      line_stride *
							      line];
					} else {
						linebuf =
						    &framebuf[line_stride *
							      line];
					}

					//pr_info("Interlace  line %d offset %d\n",line,bytesperline+line_stride*line);

				}
				//pr_info("%dx%dx%d rcv_size %d line %d offset %d line_stride %d bytesperline %d\n",width,height,pix_size,pframebuf_info->rcv_size,line,offset,line_stride,bytesperline);
				frame_remain_size =
				    (width * height * pix_size) -
				    pframebuf_info->rcv_size;
				if (offset) {
					copy_size = (width * pix_size) - offset;
					if (remain_size < copy_size)
						copy_size = remain_size;
					copy_to_framebuffer(&linebuf[offset],
							    buf, copy_size);
					//pr_info("memcpy offset %d size %d\n",offset,copy_size);
					buf += copy_size;
					frame_remain_size -= copy_size;
					remain_size -= copy_size;
					pframebuf_info->rcv_size += copy_size;
					linebuf += line_stride;
				}
				do {
					//pr_info("remain_size %d frame_remain_size %d\n",remain_size,frame_remain_size);
					if (frame_remain_size > remain_size) {
						copy_size = (width * pix_size);

						while (remain_size >= copy_size) {
							//pr_info("%d memcpy %ld(%d) size %d\n",__LINE__,linebuf-framebuf,(linebuf-framebuf)/bytesperline,copy_size);
							copy_to_framebuffer
							    (linebuf, buf,
							     copy_size);
							buf += copy_size;
							remain_size -=
							    copy_size;
							pframebuf_info->
							    rcv_size +=
							    copy_size;
							if (framemode ==
							    FRAMEGRABBER_FRAMEMODE_PROGRESS)
							{
								linebuf +=
								    line_stride;
							} else {
								if (pframebuf_info->rcv_size == (width * pix_size * height / 2)) {
									linebuf
									    =
									    &framebuf
									    [bytesperline];
								} else {
									linebuf
									    +=
									    line_stride;
								}
							}

						}
						if (remain_size) {
							//pr_info("%d memcpy  %ld(%d) size %d\n",__LINE__,linebuf-framebuf,(linebuf-framebuf)/bytesperline,remain_size);
							copy_to_framebuffer
							    (linebuf, buf,
							     remain_size);
							pframebuf_info->
							    rcv_size +=
							    remain_size;
							remain_size = 0;
						}
					} else {

						copy_size = (width * pix_size);
						remain_size -=
						    frame_remain_size;
						while (frame_remain_size >=
						       copy_size) {
							//pr_info("%d memcpy %ld(%d) size %d\n",__LINE__,linebuf-framebuf,(linebuf-framebuf)/bytesperline,copy_size);
							copy_to_framebuffer
							    (linebuf, buf,
							     copy_size);
							buf += copy_size;
							frame_remain_size -=
							    copy_size;
							pframebuf_info->
							    rcv_size +=
							    copy_size;
							if (framemode ==
							    FRAMEGRABBER_FRAMEMODE_PROGRESS)
							{
								linebuf +=
								    line_stride;
							} else {
								if (pframebuf_info->rcv_size == (width * pix_size * height / 2)) {
									linebuf
									    =
									    &framebuf
									    [bytesperline];
								} else {
									linebuf
									    +=
									    line_stride;
								}
							}

						}
						if (frame_remain_size) {
							//pr_info("%d memcpy %ld(%d) size %d\n",__LINE__,linebuf-framebuf,(linebuf-framebuf)/bytesperline,remain_size);
							copy_to_framebuffer
							    (linebuf, buf,
							     remain_size);
							pframebuf_info->
							    rcv_size +=
							    frame_remain_size;
							frame_remain_size = 0;
						}

						v4l2_model_buffer_done
						    (v4l2m_cxt);
						pframebuf_info->vbuf_info =
						    NULL;
						pframebuf_info->rcv_size = 0;
						if (remain_size) {
							v4l2_model_next_buffer
							    (v4l2m_cxt,
							     &pframebuf_info->
							     vbuf_info);
							if (pframebuf_info->
							    vbuf_info == NULL) {
								pr_info
								    ("No video buffer to receive\n");
								break;
							} else {
								pframebuf_info->
								    rcv_size =
								    0;
								framebuf =
								    (u8 *)
								    pframebuf_info->
								    vbuf_info->
								    buf_info
								    [0]->addr;
								//pr_info("framebuf change to %p\n",framebuf);
								linebuf =
								    framebuf;
								frame_remain_size
								    =
								    (width *
								     height *
								     pix_size);
							}

						}

					}
				}
				while (remain_size);

			}
		}
	}

}

void v4l2_model_streamoff(v4l2_model_handle_t context)
{
	v4l2_model_context_t *v4l2m_context = (v4l2_model_context_t *) context;

	pr_info("%s\n", __func__);

	vb2_streamoff(&v4l2m_context->queue, V4L2_BUF_TYPE_VIDEO_CAPTURE);

}
