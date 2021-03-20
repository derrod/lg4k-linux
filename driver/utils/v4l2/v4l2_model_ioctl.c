/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * v4l2_model_ioctl.c
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
#include <linux/videodev2.h>
#include <linux/v4l2-dv-timings.h>
#include <linux/gpio.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>
#include <media/videobuf2-vmalloc.h>

#include "queue.h"
#include "sys.h"
#include "v4l2_model.h"
#include "v4l2_model_device.h"
#include "v4l2_model_table.h"

#include "mem_model.h"
//#include "tool/include/tool_ctrl.h"

// defaults for ADV7619
#define BrightnessDefault  0x200
#define ContrastDefault    0x100
#define SaturationDefault  0x100
#define HueDefault         0x00
//static U32_T SharpnessDefault  = 0x00;

//To report color control range, define default color range here for 
//all video decoder. 

#define MIN_VAMP_BRIGHTNESS_UNITS   0
#define MAX_VAMP_BRIGHTNESS_UNITS   0x3ff

#define MIN_VAMP_CONTRAST_UNITS     0
#define MAX_VAMP_CONTRAST_UNITS     0x1ff

#define MIN_VAMP_SATURATION_UNITS   0
#define MAX_VAMP_SATURATION_UNITS   360

#define MIN_VAMP_HUE_UNITS          0
#define MAX_VAMP_HUE_UNITS          0x1ff


int brightness_tmp=0;
int contrast_tmp=0;
int hue_tmp=0;
int saturation_tmp=0;
int mute_tmp=0;

struct v4l2_model_framesize 
{
	int width;
	int height;
};


enum v4l2_bchs_type {
	V4L2_BCHS_TYPE_BRIGHTNESS,
	V4L2_BCHS_TYPE_CONTRAST,
	V4L2_BCHS_TYPE_HUE,
	V4L2_BCHS_TYPE_SATURATION,
};

static struct v4l2_queryctrl g_gc573_ctrls[] =
{
	#if 1
	{
		V4L2_CID_BRIGHTNESS,           //id
		V4L2_CTRL_TYPE_INTEGER,        //type
		"Brightness",                  //name[32]
		MIN_VAMP_BRIGHTNESS_UNITS,     //minimum
		MAX_VAMP_BRIGHTNESS_UNITS,     //maximum
        1,                             //step
		BrightnessDefault,             //default_value
		0,                             //flags
	    { 0, 0 },                      //reserved[2]
	},
	{
		V4L2_CID_CONTRAST,             //id
		V4L2_CTRL_TYPE_INTEGER,        //type
		"Contrast",                    //name[32]
		MIN_VAMP_CONTRAST_UNITS,       //minimum
		MAX_VAMP_CONTRAST_UNITS,       //maximum
        1,                             //step
		ContrastDefault,               //default_value
		0,                             //flags
	    { 0, 0 },                      //reserved[2]
	},
	{
		V4L2_CID_SATURATION,           //id
		V4L2_CTRL_TYPE_INTEGER,        //type
		"Saturation",                  //name[32]
		MIN_VAMP_SATURATION_UNITS,     //minimum
		MAX_VAMP_SATURATION_UNITS,     //maximum
        1,                             //step
		SaturationDefault,             //default_value
		0,                             //flags
	    { 0, 0 },                      //reserved[2]
	},
	{
		V4L2_CID_HUE,                  //id
		V4L2_CTRL_TYPE_INTEGER,        //type
		"Hue",                         //name[32]
		MIN_VAMP_HUE_UNITS,            //minimum
		MAX_VAMP_HUE_UNITS,            //maximum
        1,                             //step
		HueDefault,                    //default_value
		0,                             //flags
	    { 0, 0 },                      //reserved[2]
	},
	#endif
	#if 0
	{
		V4L2_CID_AUTOGAIN,           //id
		V4L2_CTRL_TYPE_INTEGER,        //type
		"Hdcp enable",                 //name[32]
		0,                             //minimum
		1,                             //maximum
		1,                             //step
		0,                             //default_value
		0,                             //flags
		{ 0, 0 },                      //reserved[2]
	},
	#endif
	
	#if 1
	{
		V4L2_CID_GAIN,           //id
		V4L2_CTRL_TYPE_INTEGER,        //type
		"Sample rate",                        //name[32]
		32000,                             //minimum
		48000,                             //maximum
		1,                             //step
		48000,                             //default_value
		0,                             //flags
		{ 0, 0 },                      //reserved[2]
	}
	#endif
};

#define ARRAY_SIZE_OF_CTRL		(sizeof(g_gc573_ctrls)/sizeof(g_gc573_ctrls[0]))

static struct v4l2_queryctrl *find_ctrl(unsigned int id)
{
	int i;
	//scan supported queryctrl table
	for( i=0; i<ARRAY_SIZE_OF_CTRL; ++i )
		if( g_gc573_ctrls[i].id==id )
			return &g_gc573_ctrls[i];

	return 0;
}

u32 v4l2_model_to_v4l2_caps(u32 v4l2m_caps)
{
	u32 caps=0;
	int i;

	for(i=0;i<V4L2_MODEL_CAPS_MAX && v4l2m_caps;i++)
	{
		if(v4l2m_caps & (1 << i))
		{
			switch(i)
			{
				case V4L2_MODEL_CAPS_CAPTURE:
					v4l2m_caps ^=(1 << i);
					caps |= V4L2_CAP_VIDEO_CAPTURE;
					break;
				case V4L2_MODEL_CAPS_CAPTURE_MPLANE:
					v4l2m_caps ^=(1 << i);
					caps |= V4L2_CAP_VIDEO_CAPTURE_MPLANE;
					break;
				case V4L2_MODEL_CAPS_VBI_CAPTURE:
					v4l2m_caps ^=(1 << i);
					caps |= V4L2_CAP_VBI_CAPTURE;
					break;
				case V4L2_MODEL_CAPS_TUNER:
					v4l2m_caps ^=(1 << i);
					caps |= V4L2_CAP_TUNER;
					break;
				case V4L2_MODEL_CAPS_AUDIO:
					v4l2m_caps ^=(1 << i);
					caps |= V4L2_CAP_AUDIO;
					break;
				case V4L2_MODEL_CAPS_RADIO:
					v4l2m_caps ^=(1 << i);
					caps |= V4L2_CAP_RADIO;
					break;
				case V4L2_MODEL_CAPS_READWRITE:
					v4l2m_caps ^=(1 << i);
					caps |= V4L2_CAP_READWRITE;
					break;
				case V4L2_MODEL_CAPS_STREAMING:
					v4l2m_caps ^=(1 << i);
					caps |= V4L2_CAP_STREAMING;
					break;
			}
		}
	}
	if(v4l2m_caps)
	{
		//V4L2_MODEL_DEBUG("Unsupport capabilities!!!\n");	
		pr_info("Unsupport capabilities!!!\n");
	}
	

	return caps;
}


int v4l2_model_ioctl_querycap(struct file *file, void *fh, struct v4l2_capability *cap)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	
    //pr_info("%s...\n",__func__);

	if(v4l2m_context)
	{
		strncpy(cap->driver, v4l2m_context->device_info.driver_name,sizeof(cap->driver));
		
		//if (subsystem_id == 0x5113)
		    //strncpy(cap->card, v4l2m_context->device_info.card_name_1,sizeof(cap->card));
		//else
		    strncpy(cap->card, v4l2m_context->device_info.card_name,sizeof(cap->card));    
        
        sprintf(cap->bus_info, "%s", dev_name(v4l2m_context->dev));
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4,18,0)
		if(v4l2m_context->device_info.capabilities)
		{
			cap->device_caps = v4l2_model_to_v4l2_caps(v4l2m_context->device_info.capabilities) ;	
		}
#else
		cap->device_caps = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
#endif
		cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;
	}
	return 0;
}

int v4l2_model_ioctl_enum_fmt_vid_cap(struct file *file, void *fh, struct v4l2_fmtdesc *f)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	U32_T index = f->index;

    //pr_info("%s...\n",__func__);
	if(f->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
	{
		pr_info("%s.\n",__func__);
		return -EINVAL;
	}

	if(v4l2m_context)
	{
		const framegrabber_pixfmt_t *pixfmt;
		if(f->index <0)
		{
			return -EINVAL;
		}
		if(f->index >= FRAMEGRABBER_PIXFMT_MAX)
		{
			return -EINVAL;
		}
		else
		{
		    pixfmt=framegrabber_g_support_pixelfmt_by_index(v4l2m_context->framegrabber_handle,f->index);
		    //pr_info("%s..pixfmt=%d.\n",__func__,f->index);
		    f->index = index;
		    f->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		    strlcpy(f->description, pixfmt->name, sizeof(f->description));
		    f->pixelformat=pixfmt->fourcc;
		}
		
		
		if(pixfmt==NULL)
			return -EINVAL;
        //pr_info("%s....pixfmt=%d.\n",__func__,f->index);
		strlcpy(f->description, pixfmt->name, sizeof(f->description));
		f->pixelformat=pixfmt->fourcc;
	}

	return 0;
	
}


int v4l2_model_ioctl_g_fmt_vid_cap(struct file *file, void *fh,struct v4l2_format *f)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	const framegrabber_pixfmt_t *pixfmt;
	BOOL_T interlace_mode;

	pixfmt=framegrabber_g_out_pixelfmt(v4l2m_context->framegrabber_handle);
	interlace_mode = framegrabber_g_input_interlace(v4l2m_context->framegrabber_handle); 
	if(pixfmt)
	{
		int width,height;
		//unsigned bytesperline;
        //pr_info("%s...\n",__func__);
		framegrabber_g_input_framesize(v4l2m_context->framegrabber_handle,&width,&height);
		//bytesperline=framegrabber_g_out_bytesperline(v4l2m_context->framegrabber_handle);
        //pr_info("%s..f->fmt.pix.width=%d.f->fmt.pix.height=%d.\n",__func__,f->fmt.pix.width,f->fmt.pix.height);

		f->fmt.pix.width=width;
		f->fmt.pix.height=height;
		f->fmt.pix.field=V4L2_FIELD_NONE; //Field
		f->fmt.pix.pixelformat  = pixfmt->fourcc;

		//f->fmt.pix.bytesperline = bytesperline;
		f->fmt.pix.bytesperline = (f->fmt.pix.width * pixfmt->depth) >> 3;
		f->fmt.pix.sizeimage =	f->fmt.pix.height * f->fmt.pix.bytesperline;
		if (pixfmt->is_yuv)
			f->fmt.pix.colorspace = V4L2_COLORSPACE_REC709;
		else
			f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

        
        if (interlace_mode)  
        {
			//f->fmt.pix.height = height*2;
			f->fmt.pix.field=V4L2_FIELD_INTERLACED;
		}
		//pr_info("%s....f->fmt.pix.width=%d.f->fmt.pix.height=%d.\n",__func__,f->fmt.pix.width,f->fmt.pix.height);
		return 0;
	} 

	return -EINVAL;
}

int v4l2_model_ioctl_try_fmt_vid_cap(struct file *file, void *fh, struct v4l2_format *f)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	const framegrabber_pixfmt_t *fmt;
	int width,height;
	BOOL_T interlace_mode;
    //pr_info("%s>>f->fmt.pix.width=%d.f->fmt.pix.height=%d.f->fmt.pix.pixelformat=%d\n",__func__,f->fmt.pix.width,f->fmt.pix.height,f->fmt.pix.pixelformat);
	fmt = framegrabber_g_support_pixelfmt_by_fourcc(v4l2m_context->framegrabber_handle, f->fmt.pix.pixelformat);
        
	framegrabber_g_input_framesize(v4l2m_context->framegrabber_handle,&width,&height);
	
    interlace_mode = framegrabber_g_input_interlace(v4l2m_context->framegrabber_handle); 
        
    if(!fmt)
    {
		pr_info("%s..\n",__func__);
        return 0;
	}

	//if(width!=0 && height!=0) 
	{
		if ((f->fmt.pix.width <320) || (f->fmt.pix.width >4096) || (f->fmt.pix.height <240) || (f->fmt.pix.height >2160))
		{
		    if (interlace_mode)
		    {
		        f->fmt.pix.width=width;
		        //f->fmt.pix.height=height*2;
		        f->fmt.pix.field =V4L2_FIELD_INTERLACED; //field order
		        //pr_info("%s>f->fmt.pix.width=%d.f->fmt.pix.height=%d.\n",__func__,f->fmt.pix.width,f->fmt.pix.height);
		    }
		    else
		    {
			    f->fmt.pix.width=width;
		        f->fmt.pix.height=height;
		        f->fmt.pix.field = V4L2_FIELD_NONE;
		    }
		}
		else
		{
			if ((f->fmt.pix.height == height) && interlace_mode)
			{
			    //f->fmt.pix.height=height*2;  
			    f->fmt.pix.field =V4L2_FIELD_INTERLACED;
			    //pr_info("%s f->fmt.pix.width=%d.f->fmt.pix.height=%d.\n",__func__,f->fmt.pix.width,f->fmt.pix.height);
			}
			else
			{
				f->fmt.pix.field = V4L2_FIELD_NONE;
			}
		}
	}
	if ((f->fmt.pix.width ==0) || (f->fmt.pix.height ==0))
	{
		f->fmt.pix.width = 1920;
		f->fmt.pix.height = 1080;
	}
	
	
	f->fmt.pix.bytesperline = (f->fmt.pix.width * fmt->depth) >> 3;
	f->fmt.pix.sizeimage =f->fmt.pix.height * f->fmt.pix.bytesperline;
        
	if (fmt!=NULL && fmt->is_yuv)
		f->fmt.pix.colorspace = V4L2_COLORSPACE_REC709;//V4L2_COLORSPACE_SMPTE170M;
	else
		f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

	f->fmt.pix.priv = 0;
        
    //pr_info("%s<<f->fmt.pix.width=%d.f->fmt.pix.height=%d.\n",__func__,f->fmt.pix.width,f->fmt.pix.height);
	return 0;

}

int v4l2_model_ioctl_s_fmt_vid_cap(struct file *file, void *fh,struct v4l2_format *f)
{

	v4l2_model_context_t *v4l2m_context = video_drvdata(file);

	int ret = v4l2_model_ioctl_try_fmt_vid_cap(file, fh, f);
	if (ret < 0)
	{
		pr_info("%s..\n",__func__);
		return ret;
	}

	if (vb2_is_busy(&v4l2m_context->queue)) {
		pr_info("%s.\n",__func__);
	    return -EBUSY;
	}

	/* TODO check setting format is supported */

   //pr_info("%s...\n",__func__);

   if(framegrabber_g_support_pixelfmt_by_fourcc(v4l2m_context->framegrabber_handle,f->fmt.pix.pixelformat)==NULL)
   {
	   pr_info("..%s..\n",__func__);
	   return -EINVAL;
   }
   if(f->fmt.pix.width==0 || f->fmt.pix.height==0)
   {
	   pr_info("..%s.\n",__func__);
	   return -EINVAL;
   }
   //pr_info("%s.f->fmt.pix.width=%d..f->fmt.pix.height=%d\n",__func__,f->fmt.pix.width,f->fmt.pix.height);
   framegrabber_s_out_framesize(v4l2m_context->framegrabber_handle,f->fmt.pix.width,f->fmt.pix.height); 
   framegrabber_s_out_pixelfmt(v4l2m_context->framegrabber_handle,f->fmt.pix.pixelformat); 

    return 0;
}

int v4l2_model_ioctl_enum_framesizes(struct file *file, void *fh, struct v4l2_frmsizeenum *fsize)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	const framegrabber_pixfmt_t *pixfmt;
	int width=0,height=0;

    //pr_info("%s...\n",__func__);
	pixfmt=framegrabber_g_support_pixelfmt_by_fourcc(v4l2m_context->framegrabber_handle,fsize->pixel_format);
	//pr_info("%s %08x %p\n",__func__,fsize->pixel_format,pixfmt);
	if(pixfmt==NULL)
	{
		pr_info("%s..\n",__func__);
		return -EINVAL;
	}
	else
	{
		//pr_info("%s %08x %x\n",__func__,fsize->pixel_format,pixfmt->fourcc);
	}

	if(framegrabber_g_supportframesize(v4l2m_context->framegrabber_handle,fsize->index,&width,&height)!=FRAMEGRABBER_OK)
	{
		//pr_info("%s. invalid framesize\n",__func__);
		return -EINVAL;
	}

	//pr_info("%s...supportframesize width=%d height=%d..\n",__func__,width,height); //12
	fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
	fsize->pixel_format=pixfmt->fourcc;
	fsize->discrete.width=width;
	fsize->discrete.height=height;

	return 0;
}

int v4l2_model_ioctl_enum_input(struct file *file, void *fh, struct v4l2_input *inp)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    pr_info("%s...\n",__func__);
	if (inp->index >= framegrabber_g_input_num(v4l2m_context->framegrabber_handle))
		return -EINVAL;
	inp->type = V4L2_INPUT_TYPE_CAMERA;
	sprintf(inp->name, "%s", framegrabber_g_input_name(v4l2m_context->framegrabber_handle,inp->index));
	inp->status=0;

	if(inp->index==framegrabber_g_input(v4l2m_context->framegrabber_handle))
	{
		u32 input_status=framegrabber_g_input_status(v4l2m_context->framegrabber_handle);
        //pr_info("%s..\n",__func__);
		if(input_status == FRAMEGRABBER_INPUT_STATUS_OK)
		{
			//pr_info("%s.FRAMEGRABBER_INPUT_STATUS_OK\n",__func__);
			inp->status=0;
		}else
		{
			//pr_info("..%s..\n",__func__);
			if(input_status & FRAMEGRABBER_INPUT_STATUS_NO_POWER)
				inp->status |= V4L2_IN_ST_NO_POWER;

			if(input_status & FRAMEGRABBER_INPUT_STATUS_NO_SIGNAL)
				inp->status |= V4L2_IN_ST_NO_SIGNAL;

			if(input_status & FRAMEGRABBER_INPUT_STATUS_NO_SYNC)
				inp->status |= V4L2_IN_ST_NO_SYNC;
		}
	}else
	{
		//pr_info("..%s.\n",__func__);
		inp->status |= V4L2_IN_ST_NO_POWER;
	}
	
	return 0;
}

int v4l2_model_ioctl_g_input(struct file *file, void *fh, unsigned int *i)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	*i=framegrabber_g_input(v4l2m_context->framegrabber_handle);
	//pr_info("%s..input=%d.\n",__func__,*i);
	return 0;
}

int v4l2_model_ioctl_s_input(struct file *file, void *fh, unsigned int i)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	//pr_info("%s...\n",__func__);
	if(i < 0 || i > framegrabber_g_input_num(v4l2m_context->framegrabber_handle))
	{
		pr_info("..%s..\n",__func__);
		return -EINVAL;
	}

	framegrabber_s_input(v4l2m_context->framegrabber_handle,i);

	return 0;
}

int v4l2_model_ioctl_enum_frameintervals(struct file *file, void *fh, struct v4l2_frmivalenum *fival)
{
	int frameinterval=0;
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    BOOL_T interlace_mode;

	
	interlace_mode = framegrabber_g_input_interlace(v4l2m_context->framegrabber_handle); 
    //pr_info("%s...\n",__func__);

	frameinterval=framegrabber_g_framesize_supportrefreshrate(v4l2m_context->framegrabber_handle,fival->width,fival->height,fival->index);
	if(frameinterval)
	{
		//pr_info("%s..\n",__func__);
		fival->type = V4L2_FRMIVAL_TYPE_DISCRETE;
		fival->discrete.numerator=1;
		fival->discrete.denominator=frameinterval;
	}else
	{
		//pr_info("%s.\n",__func__);
        if(fival->index==0)
        {
			//frameinterval = framegrabber_g_out_framerate(v4l2m_context->framegrabber_handle);
			//if (frameinterval ==0)
			{
                frameinterval=framegrabber_g_input_framerate(v4l2m_context->framegrabber_handle);
			}
            fival->type = V4L2_FRMIVAL_TYPE_DISCRETE;
		    fival->discrete.numerator=1;
		    //fival->discrete.numerator=1000;
		    fival->discrete.denominator=frameinterval;
                
        }else{
		    return -EINVAL;
        }
	}
	
	if (interlace_mode) fival->discrete.denominator /=2;
	
	
    //pr_info("%s.frameinterval =%d.fival->width=%d.fival->height=%d\n",__func__,frameinterval,fival->width,fival->height);
	return 0;
}

int v4l2_model_ioctl_g_parm(struct file *file, void *fh,struct v4l2_streamparm *a)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	//int current_denominator;
	//pr_info("%s...\n",__func__);
	if (a->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
	{
		pr_info("%s..\n",__func__);
	    return -EINVAL;
	}

	a->parm.capture.capability   = V4L2_CAP_TIMEPERFRAME;
	a->parm.capture.capturemode  = V4L2_MODE_HIGHQUALITY;
	a->parm.capture.timeperframe.numerator = 1;
	//pr_info("%s>>>>>>>>>denominator=%d\n",__func__,a->parm.capture.timeperframe.denominator);
	a->parm.capture.readbuffers  = 1;
	//a->parm.capture.timeperframe.denominator = framegrabber_g_out_framerate(v4l2m_context->framegrabber_handle);
	//pr_info("%s>>>>>>>>>denominator=%d\n",__func__,a->parm.capture.timeperframe.denominator);
	if ((a->parm.capture.timeperframe.denominator ==0) || (a->parm.capture.timeperframe.denominator >60)) 
	{
		a->parm.capture.timeperframe.denominator = framegrabber_g_input_framerate(v4l2m_context->framegrabber_handle);
		//pr_info("%s..io_framerate=%d\n",__func__,a->parm.capture.timeperframe.denominator/a->parm.capture.timeperframe.numerator);
	}else
	{//
		//framegrabbe_s_out_framerate(v4l2m_context->framegrabber_handle,a->parm.capture.timeperframe.denominator/a->parm.capture.timeperframe.numerator);
		
		//pr_info("%s..in_framerate=%d\n",__func__,a->parm.capture.timeperframe.denominator/a->parm.capture.timeperframe.numerator);
	}
    
	return 0;
}

int v4l2_model_ioctl_s_parm(struct file *file, void *fh,struct v4l2_streamparm *a)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	U32_T io_frame_rate;
	U32_T in_frame_rate;
	//pr_info("%s...a->parm.capture.timeperframe.denominator=%d\n",__func__,a->parm.capture.timeperframe.denominator);
	io_frame_rate = a->parm.capture.timeperframe.denominator/a->parm.capture.timeperframe.numerator;
	in_frame_rate = framegrabber_g_input_framerate(v4l2m_context->framegrabber_handle);
    if ((io_frame_rate  !=0) /*&& (io_frame_rate <=85)*/)
    {
        framegrabber_s_out_framerate(v4l2m_context->framegrabber_handle,io_frame_rate);
        //a->parm.capture.timeperframe.denominator = io_frame_rate;
        pr_info("%s set io_framerate= %u\n", __func__, io_frame_rate);
	}
	else
	{
		framegrabber_s_out_framerate(v4l2m_context->framegrabber_handle,in_frame_rate);
		pr_info("%s set in_framerate= %u\n", __func__, in_frame_rate);
	}
   
    
    //framegrabber_s_input_framerate(v4l2m_context->framegrabber_handle,a->parm.capture.timeperframe.denominator/a->parm.capture.timeperframe.numerator,a->parm.capture.timeperframe.denominator); //for test 
	//pr_info("%s..%d  %d\n",__func__,a->parm.capture.timeperframe.denominator,a->parm.capture.timeperframe.numerator);
	return 0;
}

int v4l2_model_ioctl_g_ctrl(struct file *file, void *fh,struct v4l2_control *a)//
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	
    struct v4l2_control *ctrl = a;
	//struct v4l2_queryctrl *found_ctrl = find_ctrl(ctrl->id);
	int ret = -EINVAL;
	//int BCHSinfo;
	int bchs_select=0;
	 
    //pr_info("%s...ctrl->id=%x\n",__func__,ctrl->id);
   
	//bchs_select = ADV7619_BCHS_DISABLE;
	
	switch( ctrl->id ) {
		case V4L2_CID_BRIGHTNESS: //0x00980900
			
		    bchs_select = V4L2_BCHS_TYPE_BRIGHTNESS;
		    //adv7619_get_bchs(v4l2m_context->adv7619_handle,&BCHSinfo,bchs_select);
            ctrl->value = framegrabber_g_input_bchs(v4l2m_context->framegrabber_handle,bchs_select);
            pr_info("%s...brightness(%d)\n",__func__,ctrl->value);
			ret = 0;
			break;

		case V4L2_CID_CONTRAST:
	
			bchs_select = V4L2_BCHS_TYPE_CONTRAST;
		    pr_info("%s...contrast(%d)\n",__func__,bchs_select);
            ctrl->value = framegrabber_g_input_bchs(v4l2m_context->framegrabber_handle,bchs_select);
			ret = 0;
			break;

		case V4L2_CID_SATURATION:
			
			bchs_select = V4L2_BCHS_TYPE_SATURATION;
		    pr_info("%s...saturation(%d)\n",__func__,bchs_select);
            ctrl->value = framegrabber_g_input_bchs(v4l2m_context->framegrabber_handle,bchs_select);
			ret = 0;
			break;

		case V4L2_CID_HUE:
		
			bchs_select = V4L2_BCHS_TYPE_HUE;
		    pr_info("%s...hue(%d)\n",__func__,bchs_select);
            ctrl->value = framegrabber_g_input_bchs(v4l2m_context->framegrabber_handle,bchs_select);
			ret = 0;
			break; // 
#if 1			
		case V4L2_CID_GAIN:
			//SetBoolCtrl(ctrl);
			//ctrl->value = mute_tmp;
			ret = 0;
			ctrl->value =framegrabber_g_input_audioinfo(v4l2m_context->framegrabber_handle);
		    if (ctrl->value ==0)
		        ctrl->value = 32000;
		    else if (ctrl->value ==1)
		        ctrl->value = 44100;
		    else if (ctrl->value ==2)
		        ctrl->value = 48000; 
		    else  
		        ctrl->value = 96000;  
		
		    pr_info("%s...audio(%d)\n",__func__,ctrl->value);
			break;	
#endif
#if 0			
		case V4L2_CID_AUTOGAIN:
			//SetBoolCtrl(ctrl);
			//ctrl->value = mute_tmp;
			ret = 0;
			ctrl->value =framegrabber_g_hdcp_state(v4l2m_context->framegrabber_handle);
		    
		
		    pr_info("%s...hdcp flag(%d)\n",__func__,ctrl->value);
			break;	
#endif
		default:
		    ctrl->value =0;
			pr_info("control id %d not handled\n", ctrl->id);
		    break;	
		
	}
	#if 0
	if (ret ==0)
	{
		ctrl->value = framegrabber_g_input_bchs(v4l2m_context->framegrabber_handle,&bchs_select);	
	}
	#endif
	//pr_info("%s...ctrl->value(%d)=%x\n",__func__,bchs_select,ctrl->value);
	return ret;
}

int v4l2_model_ioctl_s_ctrl(struct file *file, void *fh,struct v4l2_control *a)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	
	//handle_t adv7619_handle =board_v4l2_cxt->i2c_chip_handle[0];
    struct v4l2_control *ctrl = a;
	struct v4l2_queryctrl *found_ctrl = find_ctrl(ctrl->id);
	int ret = -EINVAL;
//	int io_bchs_value=0;
//	int io_bchs_select=0;

    //pr_info("%s...\n",__func__);
	
	if( found_ctrl ) {
		switch( found_ctrl->type ) {
		case V4L2_CTRL_TYPE_INTEGER:
			if( ctrl->value >= found_ctrl->minimum 
				|| ctrl->value <= found_ctrl->maximum ) {
				//SetIntegerCtrl(ctrl);
				{
					switch( ctrl->id ) {
	                case V4L2_CID_BRIGHTNESS:
                    framegrabber_s_input_bchs(v4l2m_context->framegrabber_handle,ctrl->value,V4L2_BCHS_TYPE_BRIGHTNESS);
		            break;

	                case V4L2_CID_CONTRAST:
                    framegrabber_s_input_bchs(v4l2m_context->framegrabber_handle,ctrl->value,V4L2_BCHS_TYPE_CONTRAST);
		            break;

	                case V4L2_CID_HUE:
                    framegrabber_s_input_bchs(v4l2m_context->framegrabber_handle,ctrl->value,V4L2_BCHS_TYPE_HUE);
		            break;

	                case V4L2_CID_SATURATION:
                    framegrabber_s_input_bchs(v4l2m_context->framegrabber_handle,ctrl->value,V4L2_BCHS_TYPE_SATURATION);
		            break;
                    case V4L2_CID_AUTOGAIN:
                    //framegrabber_s_hdcp_state(v4l2m_context->framegrabber_handle, ctrl->value);
                    pr_info("%s...hdcp state(%d)\n",__func__,ctrl->value);
                    break;	

	                default:
		            break;
	            }
								
			}
				
				//adv7619_set_bchs(v4l2m_context->adv7619_handle,&io_bchs_value,&io_bchs_select);
				ret = 0;
			} else {
				//error
				pr_info("control %s out of range\n", found_ctrl->name);
			}
			break;
#if 0
		case V4L2_CTRL_TYPE_BOOLEAN:
			//SetBoolCtrl(ctrl);
			mute_tmp = ctrl->value;
			ret = 0;
			break;
#endif
		default:
			//error
			pr_info("control type %d not handled\n", found_ctrl->type);
			
		}
	}

	return ret;
}

int v4l2_model_ioctl_queryctrl(struct file *file, void *fh,struct v4l2_queryctrl *a)
{
	struct v4l2_queryctrl *ctrl = a;
	struct v4l2_queryctrl *found_ctrl = find_ctrl(ctrl->id);
	int ret = -EINVAL;

	if( 0==found_ctrl ) {
		unsigned int id = ctrl->id;
        mem_model_memset(ctrl, 0, sizeof(*ctrl));
        ctrl->id = id;
		ctrl->flags = V4L2_CTRL_FLAG_DISABLED;
	} else {
		*ctrl = *found_ctrl;
		pr_info("control %s\n", found_ctrl->name);
		ret = 0;
	}

	return ret;
	
}

int v4l2_model_ioctl_cropcap(struct file *file, void *fh,struct v4l2_cropcap *a)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	int width,height;

	if (a->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
	{
		pr_info("%s..\n",__func__);
		return -EINVAL;
	}
  
	framegrabber_g_input_framesize(v4l2m_context->framegrabber_handle,&width,&height);
	
	a->bounds.left = 0;
	a->bounds.top = 0;
	a->bounds.width = width;
	a->bounds.height = height;

	a->defrect = a->bounds;
	a->pixelaspect.numerator = 16;	/* 4:3 FIXME: remove magic numbers */
	a->pixelaspect.denominator = 9;


	return 0;
}


int v4l2_model_ioctl_g_hdcp_state(struct file *file, unsigned int *arg)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    int ret=0;

	*arg =framegrabber_g_hdcp_state(v4l2m_context->framegrabber_handle);

	return ret;
}

int v4l2_model_ioctl_s_hdcp_state(struct file *file,unsigned int *arg)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    int ret=0;
    pr_info("ioctl set hdcp state:%d \n", *arg);
    framegrabber_s_hdcp_state(v4l2m_context->framegrabber_handle, *arg);
	return ret;
}


int v4l2_model_ioctl_g_flash(struct file *file,struct v4l2_dump_flash *flash_dump)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    int ret=0;

    pr_info("%s pad=%d, sb=%d, bs=%d\n", __func__,
		flash_dump->pad, flash_dump->start_block, flash_dump->blocks);

	ret = framegrabber_g_flash(v4l2m_context->framegrabber_handle, flash_dump->start_block, flash_dump->blocks, flash_dump->flash);

	return ret;
}

int v4l2_model_ioctl_s_flash(struct file *file,struct v4l2_dump_flash *flash_update)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    int ret=0;

    pr_info("%s pad=%d, sb=%d, bs=%d\n", __func__,
		flash_update->pad, flash_update->start_block, flash_update->blocks);
#if 1
	ret = framegrabber_s_flash(v4l2m_context->framegrabber_handle, flash_update->start_block, 
	                           flash_update->blocks, flash_update->flash);
#endif
	return ret;
}


int v4l2_model_ioctl_s_dv_timings(struct file *file, void *fd,
                                struct v4l2_dv_timings *timings)
{
    pr_info("%s...\n", __func__);
    return 0;
}


int v4l2_model_ioctl_g_dv_timings(struct file *file, void *fd,
                                struct v4l2_dv_timings *timings)
{
    pr_info("%s...\n", __func__);
    return 0;
}

int v4l2_model_ioctl_enum_std(struct file *file, void *fd,
                                struct v4l2_standard *std)
{
	pr_info("%s...\n",__func__);
    return -EINVAL;
}

int v4l2_model_ioctl_s_std(struct file *file, void *fd,
            v4l2_std_id std)
{
	pr_info("%s...\n",__func__);
    return 0;
}

int v4l2_model_ioctl_g_std(struct file *file, void *fd,
            v4l2_std_id *std)
{
	pr_info("%s...\n",__func__);
    return 0;
}

int v4l2_model_ioctl_querystd(struct file *file, void *fd,
            v4l2_std_id *std)
{
	pr_info("%s...\n",__func__);
    return 0;
}

int v4l2_model_g_tuner(struct file *file, void *fd,
            struct v4l2_tuner *tuner)
{
	pr_info("%s...\n",__func__);
    return 0;
}

int v4l2_model_s_tuner(struct file *file, void *fd,
            const struct v4l2_tuner *tuner)
{
	pr_info("%s...\n",__func__);
    return 0;
}

int v4l2_model_ioctl_g_i2c(struct file *file, struct i2c_t* arg)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	struct i2c_t i2c;
	int ret = 0;

	if((NULL == v4l2m_context) || (NULL == arg))
	{
		return -1;
	}

	ret = copy_from_user(&i2c, arg, sizeof(struct i2c_t));
	if (ret) return ret;

	ret = framegrabber_g_i2c(v4l2m_context->framegrabber_handle, i2c.channel, i2c.slave_address, i2c.sub_address, i2c.sub_address_length, i2c.data, i2c.data_length, i2c.is_10bit_address);
	if (ret) return ret;


	ret = copy_to_user(arg, &i2c, sizeof(struct i2c_t));
	if (ret) return ret;

	return ret;
}

int v4l2_model_ioctl_s_i2c(struct file *file, struct i2c_t* arg)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	struct i2c_t i2c;
	int ret = 0;

	if((NULL == v4l2m_context) || (NULL == arg))
	{
		return -1;
	}

	ret = copy_from_user(&i2c, arg, sizeof(struct i2c_t));
	if (ret) return ret;

	ret = framegrabber_s_i2c(v4l2m_context->framegrabber_handle, i2c.channel, i2c.slave_address, i2c.sub_address, i2c.sub_address_length, i2c.data, i2c.data_length, i2c.is_10bit_address);
	if (ret) return ret;

	return ret;
}

int v4l2_model_ioctl_g_reg(struct file *file, struct reg_t* arg)
{
    v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    struct reg_t reg;
    int ret = 0;

    if((NULL == v4l2m_context) || (NULL == arg))
    {
        return -1;
    }

    ret = copy_from_user(&reg, arg, sizeof(struct reg_t));
    if (ret) return ret;

    ret = framegrabber_g_reg(v4l2m_context->framegrabber_handle, reg.offset, reg.n_bytes, &reg.data);
    if (ret) return ret;

    ret = copy_to_user(arg, &reg, sizeof(struct reg_t));
    if (ret) return ret;

    return ret;
}

int v4l2_model_ioctl_s_reg(struct file *file, struct reg_t* arg)
{
    v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    struct reg_t reg;
    int ret = 0;

    if((NULL == v4l2m_context) || (NULL == arg))
    {
        return -1;
    }

    ret = copy_from_user(&reg, arg, sizeof(struct reg_t));
    if (ret) return ret;

    ret = framegrabber_s_reg(v4l2m_context->framegrabber_handle, reg.offset, reg.n_bytes, reg.data);
    if (ret) return ret;

    return ret;
}

