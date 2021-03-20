/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * v4l2_model_device.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
#include "v4l2_model.h"
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-vmalloc.h>
#include <linux/v4l2-dv-timings.h>
#include <linux/videodev2.h>

#include "v4l2_model_videobuf2.h"
#include "v4l2_model_ioctl.h"
#include "v4l2_model_device.h"
#include "framegrabber.h"


typedef struct v4l2_model_fh_s
{
    struct v4l2_fh fh;
}v4l2_model_fh_t;

#if 0
struct v4l2_dump_flash {
	__u32 pad;
	__u32 start_block;
	__u32 blocks;
	__u32 reserved[5];
	__u8  *flash;
};
#endif

static int v4l2_model_fops_open(struct file *);
static int v4l2_model_fops_release(struct file *);
static int v4l2_model_fops_mmap (struct file *, struct vm_area_struct *);
static unsigned int v4l2_model_fops_poll (struct file *, struct poll_table_struct *);
static ssize_t v4l2_model_fops_read (struct file *, char __user *, size_t, loff_t *);
static ssize_t v4l2_model_fops_write (struct file *, const char __user *, size_t, loff_t *);
static long v4l2_model_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static void v4l2_model_notify(struct v4l2_subdev *sd,unsigned int notification, void *arg);
static void *v4l2_model_alloc(void);
static void v4l2_model_release(void *context);

//void v4l2_board_register_subdev(struct sagitta_dev *sdev, struct v4l2_device *v4l2_dev);

static struct v4l2_file_operations v4l2_model_fops = {
	.owner			= THIS_MODULE,
	.open			= v4l2_model_fops_open,
	.release		= v4l2_model_fops_release,
	.unlocked_ioctl	= v4l2_model_ioctl,
	.mmap			= v4l2_model_fops_mmap,
	.poll			= v4l2_model_fops_poll,
	.read			= v4l2_model_fops_read,
	.write			= v4l2_model_fops_write,
};


static struct v4l2_ioctl_ops v4l2_model_ioctl_ops = {
	.vidioc_reqbufs             = vb2_ioctl_reqbufs,
	.vidioc_create_bufs         = vb2_ioctl_create_bufs,
	.vidioc_prepare_buf         = vb2_ioctl_prepare_buf,
	.vidioc_querybuf            = vb2_ioctl_querybuf,
	.vidioc_qbuf                = vb2_ioctl_qbuf,
	.vidioc_dqbuf               = vb2_ioctl_dqbuf,
	.vidioc_streamon 			= vb2_ioctl_streamon,
	.vidioc_streamoff 			= vb2_ioctl_streamoff,

	.vidioc_querycap      		= v4l2_model_ioctl_querycap,
	.vidioc_enum_fmt_vid_cap	= v4l2_model_ioctl_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap		= v4l2_model_ioctl_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap  	= v4l2_model_ioctl_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap     	= v4l2_model_ioctl_s_fmt_vid_cap,
	.vidioc_enum_framesizes   	= v4l2_model_ioctl_enum_framesizes,
	.vidioc_enum_input    		= v4l2_model_ioctl_enum_input,
	.vidioc_g_input       		= v4l2_model_ioctl_g_input,
	.vidioc_s_input       		= v4l2_model_ioctl_s_input,
	.vidioc_enum_frameintervals = v4l2_model_ioctl_enum_frameintervals,
	.vidioc_g_parm        		= v4l2_model_ioctl_g_parm,
	.vidioc_s_parm        		= v4l2_model_ioctl_s_parm,
	.vidioc_g_ctrl        		= v4l2_model_ioctl_g_ctrl,
	.vidioc_s_ctrl        		= v4l2_model_ioctl_s_ctrl,
	.vidioc_queryctrl           = v4l2_model_ioctl_queryctrl,
	//.vidioc_g_edid				= v4l2_model_ioctl_g_edid,
	//.vidioc_s_edid				= v4l2_model_ioctl_s_edid,
	
	//.vidioc_cropcap				= v4l2_model_ioctl_cropcap,
    //.vidioc_s_dv_timings        = v4l2_model_ioctl_s_dv_timings,
    //.vidioc_g_dv_timings        = v4l2_model_ioctl_g_dv_timings,
#if 0
    .vidioc_s_std               = v4l2_model_ioctl_s_std,
    .vidioc_g_std               = v4l2_model_ioctl_g_std,
    .vidioc_querystd            = v4l2_model_ioctl_querystd,
    .vidioc_g_tuner             = v4l2_model_g_tuner,
    .vidioc_s_tuner             = v4l2_model_s_tuner,
#endif
};


v4l2_model_handle_t v4l2_model_init(cxt_mgr_handle_t cxt_mgr,v4l2_model_device_setup_t *device_info,framegrabber_handle_t framegrabber_handle)
{
	v4l2_model_context_t *context=NULL;
	if(cxt_mgr)
	{
		struct video_device *vdev;
		int ret;
		int devicetype = 0;
		struct device *dev;
		v4l2_model_error_e err=V4L2_MODEL_OK;

		do
		{
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,18,0)
			int i;
#endif
			dev=cxt_manager_get_dev(cxt_mgr);
			if(!dev)
			{
				err=V4L2_MODEL_ERROR_NO_DEV;
				break;
			}
			context=cxt_manager_add_cxt(cxt_mgr,V4L2_CXT_ID,v4l2_model_alloc,v4l2_model_release);
			if(!context)
			{
				err=V4L2_MODEL_ERROR_ALLOC;
				break;
			}
                        context->dev=dev;
			mutex_init(&context->lock);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,18,0)
			for(i=0;i<HOOK_IOCTL_TYPE_NUM;i++)
				INIT_LIST_HEAD(&context->ioctl_hook_list[i]);
#endif
		    memcpy(&context->device_info,device_info, sizeof(v4l2_model_device_setup_t));
		    switch(context->device_info.type)
		    {
		    	case DEVICE_TYPE_GRABBER:
		            devicetype = VFL_TYPE_VIDEO;
		    		break;
		    	case DEVICE_TYPE_VBI:
		    		devicetype = VFL_TYPE_VBI;
		    		break;
		    	case DEVICE_TYPE_RADIO:
		            devicetype = VFL_TYPE_RADIO;
		    		break;
		        case DEVICE_TYPE_SUBDEV:
		            devicetype = VFL_TYPE_SUBDEV;
		    		break;
		        default:
		            printk("%s no supported device type 0x%x\n",__func__, context->device_info.type);
		            break;
		    }
		    ret = v4l2_device_register(dev,&context->v4l2_dev);
		    if(ret)
		    {
		    	err=V4L2_MODEL_ERROR_REGISTER_V4L2;
		    	break;
		    }
		    context->framegrabber_handle=framegrabber_handle;
		   	//framegrabber_associate_v4l2dev(framegrabber_handle,&context->v4l2_dev);
		    context->v4l2_dev.notify=v4l2_model_notify;
		    /* initialize video buffer 2 and get vb2 context */
		    context->vb2_context = v4l2_model_vb2_init(&context->queue, devicetype,device_info->buffer_type,dev, context);
		    if(!context->vb2_context)
		    {
		    	err=V4L2_MODEL_ERROR_VIDEO_BUF;
		    	break;
		    }
		    context->pic_bmp_handle = cxt_manager_get_context(cxt_mgr, PCI_BMP_CXT_ID, 0);

		    vdev = &context->vdev;
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,18,0)
			vdev->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
#endif
	        vdev->release = video_device_release_empty;
	    	vdev->v4l2_dev = &context->v4l2_dev;
	        vdev->vfl_dir = VFL_DIR_RX;
	    	vdev->fops = &v4l2_model_fops;
	    	vdev->ioctl_ops = &v4l2_model_ioctl_ops;
	    	vdev->lock = &context->lock;
	        vdev->queue = &context->queue;
	        /* debug message level */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,0,0)
	        vdev->dev_debug=0;
#else
	        vdev->debug = 0;
#endif
	        video_set_drvdata(vdev, context);

	        ret = video_register_device(vdev, devicetype, -1);
	        if(ret)
	        {
	        	err=V4L2_MODEL_ERROR_REGISTER_VIDEO;
	        	break;
	        }

		}while(0);
		if(err!=V4L2_MODEL_OK)
		{
			switch(err)
			{
				case V4L2_MODEL_ERROR_REGISTER_VIDEO:
					v4l2_model_vb2_release(context->vb2_context);
				case V4L2_MODEL_ERROR_VIDEO_BUF:
					v4l2_device_unregister(&context->v4l2_dev);
				case V4L2_MODEL_ERROR_REGISTER_V4L2:
					kfree(context);
				case V4L2_MODEL_ERROR_ALLOC:
				case V4L2_MODEL_ERROR_NO_DEV:
					break;

				default:
					break;
			}
		}
	}

	return (v4l2_model_handle_t )context;

}

static void *v4l2_model_alloc()
{
	v4l2_model_context_t *context;

	context = kzalloc(sizeof(*context), GFP_KERNEL);

	return context;
}

static void v4l2_model_release(v4l2_model_handle_t context)
{
	v4l2_model_context_t *v4l2m_context = (v4l2_model_context_t *) context;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,18,0)
	int i;
	struct list_head *hook_list_head=NULL;
#endif
	

	if(!v4l2m_context) {
        return;
    }
    printk("%s\n", __func__);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,18,0)
    for(i=0;i<HOOK_IOCTL_TYPE_NUM;i++)
    {
    	v4l2_model_ioctl_hook_item_t *item;
    	hook_list_head=&v4l2m_context->ioctl_hook_list[i];
    	
    	do
    	{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
    		item =list_first_entry_or_null(hook_list_head,v4l2_model_ioctl_hook_item_t,list);
#else
		item=(!list_empty(hook_list_head) ? list_first_entry(hook_list_head,v4l2_model_ioctl_hook_item_t,list) : NULL);
#endif

    		if(item)
    		{
    			list_del(&item->list);
    			kfree(item);
    		}
	   	}while(item!=NULL);	
     }
#endif

    video_unregister_device(&v4l2m_context->vdev);
    v4l2_model_vb2_release(v4l2m_context->vb2_context);
    v4l2_device_unregister(&v4l2m_context->v4l2_dev);
    kfree(v4l2m_context);
}

void v4l2_model_disconnect(v4l2_model_handle_t context)
{
	v4l2_model_context_t *v4l2m_context=(v4l2_model_context_t *)context;
	if(v4l2m_context)
	{
		mutex_lock(v4l2m_context->queue.lock);
		mutex_lock(&v4l2m_context->lock);

		v4l2_device_disconnect(&v4l2m_context->v4l2_dev);
		mutex_unlock(&v4l2m_context->lock);
		mutex_unlock(v4l2m_context->queue.lock);
	}
}

void v4l2_model_reference(v4l2_model_handle_t context)
{
	v4l2_model_context_t *v4l2m_context=(v4l2_model_context_t *)context;
	if(v4l2m_context)
	{
		v4l2_device_get(&v4l2m_context->v4l2_dev);
	}
}

void v4l2_model_unreference(v4l2_model_handle_t context)
{
	v4l2_model_context_t *v4l2m_context=(v4l2_model_context_t *)context;
	if(v4l2m_context)
	{
		v4l2_device_put(&v4l2m_context->v4l2_dev);
	}
}

int v4l2_model_register_callback(v4l2_model_handle_t context,
        v4l2_model_callback_e callback_no,
        v4l2_model_callback_t callback,
        void *asso_data)
{
	v4l2_model_context_t *v4l2m_context=(v4l2_model_context_t *)context;
	int ret=V4L2_MODEL_OK;
	int done=0;
	do
	{
		v4l2_model_callback_item_t *callback_item;
		if(v4l2m_context==NULL)
		{
			ret=V4L2_MODEL_ERROR_INVALID_CONTEXT;
			break;
		}
		if(callback_no <0 || callback_no >=V4L2_MODEL_CALLBACK_NUM)
		{
			ret=V4L2_MODEL_ERROR_INVALID_CALLBACK_NO;
			break;
		}
		callback_item=&v4l2m_context->callbacks[callback_no];
		if(callback_item->callback)
		{
			ret=V4L2_MODEL_ERROR_DUPLICATE_CALLBACK;
			break;
		}
		callback_item->callback=callback;
		callback_item->data.asso_data=asso_data;
                switch(callback_no)
                {
                    case V4L2_MODEL_CALLBACK_STREAMON:
                    case V4L2_MODEL_CALLBACK_STREAMOFF:
                    case V4L2_MODEL_CALLBACK_BUFFER_INIT:
                    case V4L2_MODEL_CALLBACK_BUFFER_QUEUE:
                    case V4L2_MODEL_CALLBACK_BUFFER_FINISH:
                    case V4L2_MODEL_CALLBACK_BUFFER_CLEANUP:
                    default:
                        callback_item->data.type=BASIC_TYPE;
                        break;
                    case V4L2_MODEL_CALLBACK_BUFFER_PREPARE:
                        callback_item->data.type=BUFFER_PREPARE_TYPE;
                        break;
                    case V4L2_MODEL_CALLBACK_QUEUE_SETUP:
                        callback_item->data.type=QUEUE_SETUP_TYPE;
                        break;
                }
		done=1;
	}while(0);
	if(!done)
		return ret;
	return ret;
}

int  v4l2_model_unregister_callback(v4l2_model_handle_t context,v4l2_model_callback_e callback_no)
{
	v4l2_model_context_t *v4l2m_context=(v4l2_model_context_t *)context;
	int ret=V4L2_MODEL_OK;
	int done=0;
	v4l2_model_callback_item_t *callback_item;
	do
	{
		if(v4l2m_context==NULL)
                {
                    ret=V4L2_MODEL_ERROR_INVALID_CONTEXT;
                    break;
                }
		if(callback_no <0 || callback_no >=V4L2_MODEL_CALLBACK_NUM)
		{
			ret=V4L2_MODEL_ERROR_INVALID_CALLBACK_NO;
			break;
		}
		callback_item=&v4l2m_context->callbacks[callback_no];
		callback_item->callback=NULL;
		callback_item->data.asso_data=NULL;
                callback_item->data.type=BASIC_TYPE;
		done=1;
	}while(0);
	if(!done)
		return ret;

	return ret;
}

static int v4l2_model_fops_open(struct file *fp)
{
    v4l2_model_fh_t *v4l2m_fh;
    v4l2_model_context_t *v4l2m_context =
            (v4l2_model_context_t *) video_drvdata(fp);

    v4l2m_fh = kzalloc(sizeof (*v4l2m_fh), GFP_KERNEL);

    //    printk("%s v4l2m_fh  %p\n", __func__, v4l2m_fh);

    if (v4l2m_fh)
    {
        v4l2_fh_init(&v4l2m_fh->fh, &v4l2m_context->vdev);
        fp->private_data = &v4l2m_fh->fh;
        v4l2_fh_add(&v4l2m_fh->fh);
    }

    return 0;
}

static int v4l2_model_fops_release(struct file *fp)
{
	struct v4l2_fh *fh = fp->private_data;
	v4l2_model_fh_t *v4l2m_fh = container_of(fh,v4l2_model_fh_t,fh);
        //v4l2_model_context_t *v4l2m_context =  (v4l2_model_context_t *) video_drvdata(fp);


	struct video_device *vdev = video_devdata(fp);

	if (fp->private_data == vdev->queue->owner) {
		vb2_queue_release(vdev->queue);
		vdev->queue->owner = NULL;
	}
        
	v4l2_fh_del(&v4l2m_fh->fh);
	v4l2_fh_exit(&v4l2m_fh->fh);
	kfree(v4l2m_fh);

	return 0;
}

static int v4l2_model_fops_mmap (struct file *fp, struct vm_area_struct *vm)
{
	return vb2_fop_mmap(fp,vm);
}

static unsigned int v4l2_model_fops_poll (struct file *fp, struct poll_table_struct *poll)
{
	return vb2_fop_poll(fp,poll);
}

static ssize_t v4l2_model_fops_write (struct file *file, const char __user *buf,size_t count, loff_t *ppos)
{
	return vb2_fop_write(file,(char __user *)buf,count,ppos);
}

static ssize_t v4l2_model_fops_read (struct file *file, char __user *buf,size_t count, loff_t *ppos)
{
	return vb2_fop_read(file,buf,count,ppos);
}

#define AVER_FLASH_IOCTL_FW_DUMP  _IOR('F', 210, struct v4l2_dump_flash)
#define AVER_FLASH_IOCTL_FW_UPDATE _IOWR('F', 211, struct v4l2_dump_flash)

#define AVER_HDCP_IOCTL_GET_STATE  _IOR('H', 220, unsigned int)
#define AVER_HDCP_IOCTL_SET_STATE _IOWR('H', 221, unsigned int)

#define AVER_GET_I2C  _IOR('A', 0, struct i2c_t)
#define AVER_SET_I2C  _IOWR('A', 1, struct i2c_t)
#define AVER_GET_REG  _IOR('A', 2, struct reg_t)
#define AVER_SET_REG  _IOWR('A', 3, struct reg_t)

static long v4l2_model_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	long ret;
	int hdcp_state=0;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,18,0)
	long try;
	v4l2_model_context_t *v4l2m_context = (v4l2_model_context_t *) video_drvdata(file);
	struct list_head *hook_list_head=NULL;
	struct list_head *pos;

	hook_list_head=&v4l2m_context->ioctl_hook_list[HOOK_PREV_IOCTL];
	if(!list_empty(hook_list_head))
	{	
		list_for_each(pos,hook_list_head)
		{
			v4l2_model_ioctl_hook_item_t *ioctl_hook_item;
			ioctl_hook_item=list_entry(pos,v4l2_model_ioctl_hook_item_t,list);
			try=video_usercopy(ioctl_hook_item->data,cmd,arg,(v4l2_kioctl)ioctl_hook_item->ioctl_hook_func);
			if(try!=-EINVAL)
			{
				return try;
				
			}
		}
	}
#endif

	//printk("%s..cmd=%d\n",__func__,cmd);
	switch (cmd)
	{
		case AVER_FLASH_IOCTL_FW_DUMP:
		    printk("AVER_FLASH_IOCTL_FW_DUMP\n");
			ret = v4l2_model_ioctl_g_flash(file,(struct v4l2_dump_flash *)arg);
			break;

		case AVER_FLASH_IOCTL_FW_UPDATE:
		    printk("----->AVER_FLASH_IOCTL_FW_UPDATE\n");
			ret = v4l2_model_ioctl_s_flash(file,(struct v4l2_dump_flash *)arg);
			break;

		case AVER_HDCP_IOCTL_GET_STATE:
		    printk("AVER_HDCP_IOCTL_GET_STATE\n");
			ret = v4l2_model_ioctl_g_hdcp_state(file, &hdcp_state);
			copy_to_user ((unsigned int *)arg, &hdcp_state, sizeof(hdcp_state));
			break;

		case AVER_HDCP_IOCTL_SET_STATE:
            //printk("hdcp:%x \n", arg);
			//copy_from_user (&hdcp_state,(unsigned int *) arg, sizeof(hdcp_state));
            hdcp_state = arg;
            printk("----->AVER_HDCP_IOCTL_SET_STATE:%ld \n", arg);
			ret = v4l2_model_ioctl_s_hdcp_state(file,&hdcp_state);
			break;

		case AVER_GET_I2C:
			//printk("----->AVER_GET_I2C\n");
			ret = v4l2_model_ioctl_g_i2c(file, (struct i2c_t *) arg);
			if (ret) {
				printk("AVER_GET_I2C failed...\n");
			}
			break;
		case AVER_SET_I2C:
			//printk("----->AVER_SET_I2C\n");
			ret = v4l2_model_ioctl_s_i2c(file, (struct i2c_t *) arg);
			if (ret) {
				printk("AVER_SET_I2C failed...\n");
			}
			break;
		case AVER_GET_REG:
			//printk("----->AVER_GET_REG\n");
			ret = v4l2_model_ioctl_g_reg(file, (struct reg_t *) arg);
			if (ret) {
				printk("AVER_GET_REG failed...\n");
			}
			break;
		case AVER_SET_REG:
			//printk("----->AVER_SET_REG\n");
			ret = v4l2_model_ioctl_s_reg(file, (struct reg_t *) arg);
			if (ret) {
				printk("AVER_SET_REG failed...\n");
			}
			break;

		default:
			ret = video_ioctl2(file, cmd, arg);
			break;
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,18,0)
	//ret = video_ioctl2(file, cmd, arg);
	hook_list_head=&v4l2m_context->ioctl_hook_list[HOOK_POST_IOCTL];
	if(!list_empty(hook_list_head))
	{	
		list_for_each(pos,hook_list_head)
		{
			v4l2_model_ioctl_hook_item_t *ioctl_hook_item;
			ioctl_hook_item=list_entry(pos,v4l2_model_ioctl_hook_item_t,list);
			try=video_usercopy(ioctl_hook_item->data,cmd,arg,(v4l2_kioctl)ioctl_hook_item->ioctl_hook_func);
			if(try!=-EINVAL)
			{
				return try;
			}	
			
		}
	}
#endif
	return ret;

}

int v4l2_model_hook_ioctl(v4l2_model_handle_t v4l2_context,v4l2_model_ioctl_hook_type_e hooktype,v4l2_model_ioctl_hook_func_t ioctl_hook,void *data)
{
	v4l2_model_context_t *v4l2m_context=(v4l2_model_context_t *)v4l2_context;
	int ret=0;

	if(v4l2m_context)
	{
		struct list_head *hook_list_head=NULL;
		v4l2_model_ioctl_hook_item_t *ioctl_hook_item;
		do
		{
			if(hooktype <0 || hooktype >HOOK_IOCTL_TYPE_NUM)
			{
				ret=-1;
				break;
			}	
			ioctl_hook_item=(v4l2_model_ioctl_hook_item_t *)kzalloc(sizeof(*ioctl_hook_item), GFP_KERNEL);
			if(ioctl_hook_item==NULL)
			{
				ret=-1;
				break;
			}
			INIT_LIST_HEAD(&ioctl_hook_item->list);
			ioctl_hook_item->ioctl_hook_func=ioctl_hook;
			ioctl_hook_item->data=data;	
			hook_list_head=&v4l2m_context->ioctl_hook_list[hooktype];
			list_add_tail(&ioctl_hook_item->list,hook_list_head);

		}while(0);

	}
	return ret;

}
struct v4l2_device *v4l2_model_get_v4l2_device(v4l2_model_handle_t context)
{
	v4l2_model_context_t *v4l2m_context=(v4l2_model_context_t *)context;
	return &v4l2m_context->v4l2_dev;
}

static void v4l2_model_notify(struct v4l2_subdev *sd,unsigned int notification, void *arg)
{
	v4l2_model_context_t *v4l2m_context=container_of(sd->v4l2_dev,v4l2_model_context_t,v4l2_dev);

	printk("%s sd name %s owner %s\n",__func__,sd->name,sd->owner->name);
	framegrabber_notify(v4l2m_context->framegrabber_handle,sd->name,notification, arg);
//	printk("%s %s notification %d \n",__func__,sd->name,notification);
//	if(notification==2)
//	{
//		struct v4l2_dv_timings detected_timings;
//		v4l2_model_output_fmt_t *input_fmt;
//		int status;
//		v4l2_subdev_call(sd,video, query_dv_timings, &detected_timings);
//		v4l2_subdev_call(sd,video, g_input_status, &status);
//		printk("%s input status %x\n",__func__,status);
//		printk("%s detected %dx%d%c\n", __func__,detected_timings.bt.width,	detected_timings.bt.height,detected_timings.bt.interlaced == V4L2_DV_INTERLACED ? 'i' : 'p');
//		v4l2_subdev_call(sd,video, s_dv_timings, &detected_timings);
//
//
//		input_fmt=&v4l2m_context->input_fmt;
//		input_fmt->width = detected_timings.bt.width;
//		input_fmt->height = detected_timings.bt.height;
//		input_fmt->is_interlace = detected_timings.bt.interlaced == V4L2_DV_INTERLACED;
//
//
//	}

}

int v4l2_model_get_current_frame_size(v4l2_model_handle_t context,struct frame_size *frame_size)
{
	v4l2_model_context_t *v4l2m_context=(v4l2_model_context_t *)context;
	framegrabber_g_input_framesize(v4l2m_context->framegrabber_handle,&frame_size->width,&frame_size->height);

	return 0;
}

//void *v4l2_model_get_context(struct sagitta_dev *sdev)
//{
//	struct sagitta_module *mod,*tmp;
//
//	list_for_each_entry_safe(mod,tmp,&sdev->root->modules,list)
//	{
//		if(strncmp(mod->name,V4L2_MODULE_ID_STR,strlen(mod->name))==0)
//		{
//			struct v4l2_module *this=(struct v4l2_module *)mod;
//
//			return this->v4l2_context;
//		}
//	}
//
//	return NULL;
//
//}

