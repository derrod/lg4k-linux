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
#define SaturationDefault  0x80
#define HueDefault         0x00
//static U32_T SharpnessDefault  = 0x00;

//To report color control range, define default color range here for 
//all video decoder. 

#define MIN_VAMP_BRIGHTNESS_UNITS   0
#define MAX_VAMP_BRIGHTNESS_UNITS   0x3ff

#define MIN_VAMP_CONTRAST_UNITS     0
#define MAX_VAMP_CONTRAST_UNITS     0x1ff

#define MIN_VAMP_SATURATION_UNITS   0
#define MAX_VAMP_SATURATION_UNITS   0x1ff

#define MIN_VAMP_HUE_UNITS          0
#define MAX_VAMP_HUE_UNITS          360


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
	


	if(v4l2m_context)
	{
		strncpy(cap->driver, v4l2m_context->device_info.driver_name,sizeof(cap->driver));
		
		//if (subsystem_id == 0x5113)
		    //strncpy(cap->card, v4l2m_context->device_info.card_name_1,sizeof(cap->card));
		//else
		    strncpy(cap->card, v4l2m_context->device_info.card_name,sizeof(cap->card));    
        
        sprintf(cap->bus_info, "PCIe:%s", dev_name(v4l2m_context->dev));
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4,18,0)
		if(v4l2m_context->device_info.capabilities)
		{
			cap->device_caps = v4l2_model_to_v4l2_caps(v4l2m_context->device_info.capabilities) ;	
		}
#else
//		cap->device_caps = V4L2_CAP_VIDEO_CAPTURE_MPLANE | V4L2_CAP_STREAMING;
        cap->device_caps = v4l2m_context->vdev.device_caps;
#endif
		cap->capabilities = cap->device_caps | V4L2_CAP_DEVICE_CAPS;
	}
	return 0;
}

int v4l2_model_ioctl_enum_fmt_vid_cap(struct file *file, void *fh, struct v4l2_fmtdesc *f)
{
    v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    U32_T index = f->index;

    if(f->type != V4L2_BUF_TYPE_VIDEO_CAPTURE && f->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE)
    {
        pr_err("ENUM_FMT with invalid type: %d\n", f->type);
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

        pixfmt=framegrabber_g_support_pixelfmt_by_index(v4l2m_context->framegrabber_handle,f->index);

        if(pixfmt==NULL)
            return -EINVAL;

        if ((pixfmt->num_planes > 1) && (f->type == V4L2_BUF_TYPE_VIDEO_CAPTURE)) {
            return -EINVAL;
        }

        pr_info("%s pixfmt=%d\n", pixfmt->name, f->index);
        f->index = index;

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

    if (f->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        pr_err("G_FMT with invalid type: %d\n", f->type);
        return -EINVAL;
    }

    interlace_mode = framegrabber_g_input_interlace(v4l2m_context->framegrabber_handle);
    pixfmt=framegrabber_g_out_pixelfmt(v4l2m_context->framegrabber_handle);

    if (!pixfmt || pixfmt->num_planes > 1) {
        pr_err("numplanes > 1 while type is single planar\n");
        pixfmt = framegrabber_g_support_pixelfmt_by_index(v4l2m_context->framegrabber_handle,0);
        framegrabber_s_out_pixelfmt(v4l2m_context->framegrabber_handle, pixfmt->fourcc);
    }

    if(pixfmt)
    {
        int width,height;
        unsigned bytesperline;
        unsigned sizeimage;

        framegrabber_g_out_framesize(v4l2m_context->framegrabber_handle, &width, &height);

        if(framegrabber_g_support_framesize(v4l2m_context->framegrabber_handle, width, height) != FRAMEGRABBER_OK) {

            framegrabber_g_input_framesize(v4l2m_context->framegrabber_handle, &width, &height);

            if (width == 0 || height == 0) {
                width = 1920;
                height = 1080;
            }
            pr_warn("no correct output framesize defined, using %dx%d\n", width, height);
            framegrabber_s_out_framesize(v4l2m_context->framegrabber_handle, width, height);
        }

        bytesperline = framegrabber_g_out_bytesperline(v4l2m_context->framegrabber_handle, 1);
        sizeimage = framegrabber_g_out_planarbuffersize(v4l2m_context->framegrabber_handle, 1);

        pr_info("pixelformat = %s bytesperline = %d and sizeimage = %d\n", pixfmt->name, bytesperline, sizeimage);

        f->fmt.pix.width = width;
        f->fmt.pix.height = height;
        f->fmt.pix.field = V4L2_FIELD_NONE; //Field
        f->fmt.pix.pixelformat = pixfmt->fourcc;

        f->fmt.pix.bytesperline = bytesperline;
        f->fmt.pix.sizeimage = sizeimage;
        if (pixfmt->is_yuv) {
            f->fmt.pix.colorspace = V4L2_COLORSPACE_REC709;
            f->fmt.pix.xfer_func = V4L2_XFER_FUNC_DEFAULT;
            f->fmt.pix.ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
            f->fmt.pix.quantization = V4L2_QUANTIZATION_DEFAULT;
        } else {
            f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
            f->fmt.pix.xfer_func = V4L2_XFER_FUNC_DEFAULT;
            f->fmt.pix.ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
            f->fmt.pix.quantization = V4L2_QUANTIZATION_DEFAULT;
        }

        if (interlace_mode) {
            //f->fmt.pix.height = height*2;
            f->fmt.pix.field = V4L2_FIELD_INTERLACED;
        }

        //pr_info("%s....f->fmt.pix.width=%d.f->fmt.pix.height=%d.\n",__func__,f->fmt.pix.width,f->fmt.pix.height);
        return 0;
    }

    return -EINVAL;
}

int v4l2_model_ioctl_g_fmt_vid_cap_mplane(struct file *file, void *fh,struct v4l2_format *f) {
    v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    const framegrabber_pixfmt_t *pixfmt;
    BOOL_T interlace_mode;

    if (f->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
        pr_err("G_FMT with invalid type: %d\n", f->type);
        return -EINVAL;
    }

    interlace_mode = framegrabber_g_input_interlace(v4l2m_context->framegrabber_handle);

    pixfmt=framegrabber_g_out_pixelfmt(v4l2m_context->framegrabber_handle);
    if (!pixfmt) {
        pr_warn("no output pixelformat defined\n");
        pixfmt = framegrabber_g_support_pixelfmt_by_index(v4l2m_context->framegrabber_handle,0);
        framegrabber_s_out_pixelfmt(v4l2m_context->framegrabber_handle, pixfmt->fourcc);
    }

    if(pixfmt) {
        int width, height;
        int i;

        framegrabber_g_out_framesize(v4l2m_context->framegrabber_handle, &width, &height);

        if(framegrabber_g_support_framesize(v4l2m_context->framegrabber_handle, width, height) != FRAMEGRABBER_OK) {

            framegrabber_g_input_framesize(v4l2m_context->framegrabber_handle, &width, &height);

            if (width == 0 || height == 0) {
                width = 1920;
                height = 1080;
            }
            pr_warn("no correct output framesize defined, using %dx%d\n", width, height);
            framegrabber_s_out_framesize(v4l2m_context->framegrabber_handle, width, height);
        }

        if (pixfmt->num_planes == 1) {
            f->fmt.pix.bytesperline = framegrabber_g_out_bytesperline(v4l2m_context->framegrabber_handle, 1);
            f->fmt.pix.sizeimage = framegrabber_g_out_planarbuffersize(v4l2m_context->framegrabber_handle, 1);

            if (pixfmt->is_yuv) {
                f->fmt.pix.colorspace = V4L2_COLORSPACE_REC709;
                f->fmt.pix.xfer_func = V4L2_XFER_FUNC_DEFAULT;
                f->fmt.pix.ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
                f->fmt.pix.quantization = V4L2_QUANTIZATION_DEFAULT;
            } else {
                f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
                f->fmt.pix.xfer_func = V4L2_XFER_FUNC_DEFAULT;
                f->fmt.pix.ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
                f->fmt.pix.quantization = V4L2_QUANTIZATION_DEFAULT;
            }

            if (interlace_mode) {
                //f->fmt.pix.height = height*2;
                f->fmt.pix.field = V4L2_FIELD_INTERLACED;
            } else {
                f->fmt.pix.field = V4L2_FIELD_NONE;
            }

            f->fmt.pix.priv = 0;
        }

        pr_info("pixelformat %s on %d planes with size %dx%d\n", pixfmt->name, pixfmt->num_planes, width, height);

        f->fmt.pix_mp.num_planes = pixfmt->num_planes;
        f->fmt.pix_mp.width = width;
        f->fmt.pix_mp.height = height;
        f->fmt.pix_mp.field = V4L2_FIELD_NONE; //Field
        f->fmt.pix_mp.pixelformat = pixfmt->fourcc;

        if (pixfmt->is_yuv) {
            f->fmt.pix_mp.colorspace = V4L2_COLORSPACE_REC709;
            f->fmt.pix_mp.xfer_func = V4L2_XFER_FUNC_709;
            f->fmt.pix_mp.ycbcr_enc = V4L2_YCBCR_ENC_709;
        } else {
            f->fmt.pix_mp.colorspace = V4L2_COLORSPACE_SRGB;
            f->fmt.pix_mp.xfer_func = V4L2_XFER_FUNC_SRGB;
            f->fmt.pix_mp.ycbcr_enc = V4L2_YCBCR_ENC_BT2020;
        }

        if (interlace_mode)
        {
            //f->fmt.pix.height = height*2;
            f->fmt.pix_mp.field=V4L2_FIELD_INTERLACED;
        }

        for (i=0; i < pixfmt->num_planes; i++) {
            f->fmt.pix_mp.plane_fmt[i].bytesperline = framegrabber_g_out_bytesperline(v4l2m_context->framegrabber_handle, i+1);
            f->fmt.pix_mp.plane_fmt[i].sizeimage = framegrabber_g_out_planarbuffersize(v4l2m_context->framegrabber_handle, i+1);

            pr_info("plane[%d]: set bytesperline %d and sizeimage %d\n", i, f->fmt.pix_mp.plane_fmt[i].bytesperline, f->fmt.pix_mp.plane_fmt[i].sizeimage);
        }

        return 0;
    }

    return -EINVAL;
}


int v4l2_model_ioctl_try_fmt_vid_cap(struct file *file, void *fh, struct v4l2_format *f)
{
    v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    const framegrabber_pixfmt_t *pixfmt;
    BOOL_T interlace_mode;

    if (f->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
        pr_err("TRY_FMT with invalid type: %d\n", f->type);
        return -EINVAL;
    }

    //pr_info("%s>>f->fmt.pix.width=%d.f->fmt.pix.height=%d.f->fmt.pix.pixelformat=%d\n",__func__,f->fmt.pix.width,f->fmt.pix.height,f->fmt.pix.pixelformat);
    pixfmt = framegrabber_g_support_pixelfmt_by_fourcc(v4l2m_context->framegrabber_handle, f->fmt.pix.pixelformat);
    if(!pixfmt || pixfmt->num_planes > 1)
    {
        pr_warn("fmt is not valid\n");
        pixfmt = framegrabber_g_support_pixelfmt_by_index(v4l2m_context->framegrabber_handle,0);

        f->fmt.pix.pixelformat = pixfmt->fourcc;
    }

    interlace_mode = framegrabber_g_input_interlace(v4l2m_context->framegrabber_handle);

    if(framegrabber_g_support_framesize(v4l2m_context->framegrabber_handle, f->fmt.pix.width, f->fmt.pix.height) != FRAMEGRABBER_OK) {
        int width, height;

        pr_warn("framesize %dx%d is not supported\n", f->fmt.pix.width, f->fmt.pix.height);

        framegrabber_g_input_framesize(v4l2m_context->framegrabber_handle, &width, &height);

        if (width == 0 || height == 0) {
            width = 1920;
            height = 1080;
        }

        f->fmt.pix.width = width;
        f->fmt.pix.height = height;
    }

    unsigned bytesperline;
    unsigned sizeimage;

    bytesperline = framegrabber_g_out_bytesperline(v4l2m_context->framegrabber_handle, 1);
    sizeimage = framegrabber_g_out_planarbuffersize(v4l2m_context->framegrabber_handle, 1);

    f->fmt.pix.bytesperline = bytesperline;
    f->fmt.pix.sizeimage = sizeimage;

    if (pixfmt->is_yuv) {
        f->fmt.pix.colorspace = V4L2_COLORSPACE_REC709;
        f->fmt.pix.xfer_func = V4L2_XFER_FUNC_DEFAULT;
        f->fmt.pix.ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
        f->fmt.pix.quantization = V4L2_QUANTIZATION_DEFAULT;
    } else {
        f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
        f->fmt.pix.xfer_func = V4L2_XFER_FUNC_DEFAULT;
        f->fmt.pix.ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
        f->fmt.pix.quantization = V4L2_QUANTIZATION_DEFAULT;
    }

    if (interlace_mode)
    {
        //f->fmt.pix.height = height*2;
        f->fmt.pix.field=V4L2_FIELD_INTERLACED;
    } else {
        f->fmt.pix.field = V4L2_FIELD_NONE;
    }

    f->fmt.pix.priv = 0;

    //pr_info("%s<<f->fmt.pix.width=%d.f->fmt.pix.height=%d.\n",__func__,f->fmt.pix.width,f->fmt.pix.height);
    return 0;
}

int v4l2_model_ioctl_try_fmt_vid_cap_mplane(struct file *file, void *fh, struct v4l2_format *f)
{
    v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    const framegrabber_pixfmt_t *pixfmt;
    BOOL_T interlace_mode;
    int i;

    if (f->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
        pr_err("TRY_FMT with invalid type: %d\n", f->type);
        return -EINVAL;
    }

    pixfmt = framegrabber_g_support_pixelfmt_by_fourcc(v4l2m_context->framegrabber_handle, f->fmt.pix_mp.pixelformat);
    if(!pixfmt)
    {
        pr_warn("fmt is not valid\n");
        pixfmt = framegrabber_g_out_pixelfmt(v4l2m_context->framegrabber_handle);

        f->fmt.pix_mp.pixelformat = pixfmt->fourcc;
    }

    if (f->fmt.pix_mp.num_planes != pixfmt->num_planes) {
        pr_warn("wrong number of planes %d\n", f->fmt.pix_mp.num_planes);
        f->fmt.pix_mp.num_planes = pixfmt->num_planes;
    }

    interlace_mode = framegrabber_g_input_interlace(v4l2m_context->framegrabber_handle);

    if(framegrabber_g_support_framesize(v4l2m_context->framegrabber_handle, f->fmt.pix_mp.width, f->fmt.pix_mp.height) != FRAMEGRABBER_OK) {
        int width,height;

        pr_warn("framesize %dx%d is not supported\n",  f->fmt.pix_mp.width, f->fmt.pix_mp.height);

        framegrabber_g_input_framesize(v4l2m_context->framegrabber_handle,&width,&height);

        if (width == 0 || height == 0) {
            width = 1920;
            height = 1080;
        }

        if (pixfmt->num_planes == 1) {
            f->fmt.pix.width=width;
            f->fmt.pix.height=height;
        }

        f->fmt.pix_mp.width=width;
        f->fmt.pix_mp.height=height;
    }

    if (pixfmt->num_planes == 1) {
        f->fmt.pix.bytesperline = framegrabber_g_out_bytesperline(v4l2m_context->framegrabber_handle, 1);
        f->fmt.pix.sizeimage = framegrabber_g_out_planarbuffersize(v4l2m_context->framegrabber_handle, 1);

        if (pixfmt->is_yuv) {
            f->fmt.pix.colorspace = V4L2_COLORSPACE_REC709;
            f->fmt.pix.xfer_func = V4L2_XFER_FUNC_DEFAULT;
            f->fmt.pix.ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
            f->fmt.pix.quantization = V4L2_QUANTIZATION_DEFAULT;
        } else {
            f->fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;
            f->fmt.pix.xfer_func = V4L2_XFER_FUNC_DEFAULT;
            f->fmt.pix.ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
            f->fmt.pix.quantization = V4L2_QUANTIZATION_DEFAULT;
        }

        if (interlace_mode) {
            //f->fmt.pix.height = height*2;
            f->fmt.pix.field = V4L2_FIELD_INTERLACED;
        } else {
            f->fmt.pix.field = V4L2_FIELD_NONE;
        }

        f->fmt.pix.priv = 0;
    }

    f->fmt.pix_mp.num_planes = pixfmt->num_planes;
    f->fmt.pix_mp.field = V4L2_FIELD_NONE; //Field
    f->fmt.pix_mp.pixelformat = pixfmt->fourcc;

    if (pixfmt->is_yuv) {
        f->fmt.pix_mp.colorspace = V4L2_COLORSPACE_REC709;
        f->fmt.pix_mp.xfer_func = V4L2_XFER_FUNC_DEFAULT;
        f->fmt.pix_mp.ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
        f->fmt.pix_mp.quantization = V4L2_QUANTIZATION_DEFAULT;
    } else {
        f->fmt.pix_mp.colorspace = V4L2_COLORSPACE_SRGB;
        f->fmt.pix_mp.xfer_func = V4L2_XFER_FUNC_DEFAULT;
        f->fmt.pix_mp.ycbcr_enc = V4L2_YCBCR_ENC_DEFAULT;
        f->fmt.pix_mp.quantization = V4L2_QUANTIZATION_DEFAULT;
    }

    if (interlace_mode)
    {
        //f->fmt.pix.height = height*2;
        f->fmt.pix_mp.field=V4L2_FIELD_INTERLACED;
    }

    for (i=0; i < pixfmt->num_planes; i++) {
        f->fmt.pix_mp.plane_fmt[i].bytesperline = framegrabber_g_out_bytesperline(v4l2m_context->framegrabber_handle, i+1);
        f->fmt.pix_mp.plane_fmt[i].sizeimage = framegrabber_g_out_planarbuffersize(v4l2m_context->framegrabber_handle, i+1);
    }

    //pr_info("%s<<f->fmt.pix.width=%d.f->fmt.pix.height=%d.\n",__func__,f->fmt.pix.width,f->fmt.pix.height);
    return 0;
}

int v4l2_model_ioctl_s_fmt_vid_cap(struct file *file, void *fh,struct v4l2_format *f)
{

	v4l2_model_context_t *v4l2m_context = video_drvdata(file);

	int ret = v4l2_model_ioctl_try_fmt_vid_cap(file, fh, f);
	if (ret < 0)
	{
		pr_err("try_vid_cap failed\n");
		return ret;
	}

	if (vb2_is_busy(&v4l2m_context->queue)) {
		pr_warn("vb2 is busy\n");
	    return -EBUSY;
	}

   pr_info("f->fmt.pix.width=%d..f->fmt.pix.height=%d..f->fmt.pix.pixelformat=%d\n",f->fmt.pix.width,f->fmt.pix.height,f->fmt.pix.pixelformat);
   framegrabber_s_out_framesize(v4l2m_context->framegrabber_handle,f->fmt.pix.width,f->fmt.pix.height); 
   framegrabber_s_out_pixelfmt(v4l2m_context->framegrabber_handle,f->fmt.pix.pixelformat);

    unsigned bytesperline = framegrabber_g_out_bytesperline(v4l2m_context->framegrabber_handle, 1);
    unsigned sizeimage = framegrabber_g_out_planarbuffersize(v4l2m_context->framegrabber_handle, 1);

    f->fmt.pix.bytesperline = bytesperline;
    f->fmt.pix.sizeimage = sizeimage;

    pr_info("bytesperline = %d and sizeimage = %d\n", bytesperline, sizeimage);

    return 0;
}

int v4l2_model_ioctl_s_fmt_vid_cap_mplane(struct file *file, void *fh,struct v4l2_format *f)
{

    v4l2_model_context_t *v4l2m_context = video_drvdata(file);

    int ret = v4l2_model_ioctl_try_fmt_vid_cap_mplane(file, fh, f);
    if (ret < 0)
    {
        pr_err("try_vid_cap failed\n");
        return ret;
    }

    if (vb2_is_busy(&v4l2m_context->queue)) {
        pr_info("%s.\n",__func__);
        return -EBUSY;
    }

    pr_info("f->fmt.pix_mp.width=%d..f->fmt.pix_mp.height=%d pixelformat=%d\n",f->fmt.pix.width,f->fmt.pix.height, f->fmt.pix_mp.pixelformat);
    framegrabber_s_out_framesize(v4l2m_context->framegrabber_handle,f->fmt.pix_mp.width,f->fmt.pix_mp.height);
    framegrabber_s_out_pixelfmt(v4l2m_context->framegrabber_handle,f->fmt.pix_mp.pixelformat);

    return 0;
}

int v4l2_model_ioctl_enum_framesizes(struct file *file, void *fh, struct v4l2_frmsizeenum *fsize)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	const framegrabber_pixfmt_t *pixfmt;
	int width=0,height=0;


	pixfmt=framegrabber_g_support_pixelfmt_by_fourcc(v4l2m_context->framegrabber_handle,fsize->pixel_format);
	//pr_info("%s %08x %p\n",__func__,fsize->pixel_format,pixfmt);
	if(pixfmt==NULL)
	{
		pr_info("%s..\n",__func__);
		return -EINVAL;
	}

    //pr_info("%s %08x %x\n",__func__,fsize->pixel_format,pixfmt->fourcc);

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
    int fivaldenominator=0;
    int fivalnumerator = 1;
    v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    BOOL_T interlace_mode;


    interlace_mode = framegrabber_g_input_interlace(v4l2m_context->framegrabber_handle);


    fivaldenominator=framegrabber_g_framesize_supportrefreshrate(v4l2m_context->framegrabber_handle, fival->width, fival->height, fival->index);

    if(fivaldenominator)
    {
        pr_info("fivaldenominator = %d\n", fivaldenominator);
        fival->type = V4L2_FRMIVAL_TYPE_DISCRETE;
        fival->discrete.numerator=fivalnumerator;
        fival->discrete.denominator=fivaldenominator;
    }else{
        return -EINVAL;
    }

    if (interlace_mode) fival->discrete.denominator /=2;


    //pr_info("%s.fivaldenominator =%d.fival->width=%d.fival->height=%d\n",__func__,fivaldenominator,fival->width,fival->height);
    return 0;
}

int v4l2_model_ioctl_g_parm(struct file *file, void *fh,struct v4l2_streamparm *a)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
    U32_T io_frame_rate;
    U32_T in_frame_rate;

	if ((a->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) && (a->type != V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE))
	{
		pr_err("unsupported video buffer type");
	    return -EINVAL;
	}

    io_frame_rate = framegrabber_g_out_framerate(v4l2m_context->framegrabber_handle);
    in_frame_rate = framegrabber_g_input_framerate(v4l2m_context->framegrabber_handle);

	a->parm.capture.capability   = V4L2_CAP_TIMEPERFRAME;

	a->parm.capture.readbuffers  = 1;
    a->parm.capture.timeperframe.numerator = 1;
    a->parm.capture.timeperframe.denominator = io_frame_rate;

	if ((a->parm.capture.timeperframe.denominator ==0) || (a->parm.capture.timeperframe.denominator >144))
	{
        if (in_frame_rate != 0) {
            a->parm.capture.timeperframe.denominator = in_frame_rate;
        } else {
            //If there is no input framerate and no output framerate is set, default to 60
            a->parm.capture.timeperframe.denominator = REFRESHRATE_60;
        }

        //If output framerate is not valid, set it to the read value
        framegrabber_s_out_framerate(v4l2m_context->framegrabber_handle,a->parm.capture.timeperframe.denominator);
    }

    pr_info("a->parm.capture.timeperframe.denominator=%d a->parm.capture.timeperframe.numerator=%d\n",a->parm.capture.timeperframe.denominator, a->parm.capture.timeperframe.numerator);

    return 0;
}

int v4l2_model_ioctl_s_parm(struct file *file, void *fh,struct v4l2_streamparm *a)
{
	v4l2_model_context_t *v4l2m_context = video_drvdata(file);
	U32_T io_frame_rate = 0;
	U32_T in_frame_rate = 0;
	pr_info("a->parm.capture.timeperframe.denominator=%d a->parm.capture.timeperframe.numerator=%d\n",a->parm.capture.timeperframe.denominator, a->parm.capture.timeperframe.numerator);

    if (a->parm.capture.timeperframe.numerator > 0)
	    io_frame_rate = a->parm.capture.timeperframe.denominator/a->parm.capture.timeperframe.numerator;
    in_frame_rate = framegrabber_g_input_framerate(v4l2m_context->framegrabber_handle);
    if ((io_frame_rate  > 0) && (io_frame_rate <= 144))
    {
        framegrabber_s_out_framerate(v4l2m_context->framegrabber_handle,io_frame_rate);
        //a->parm.capture.timeperframe.denominator = io_frame_rate;
        pr_info("set framerate to io_framerate= %u\n", io_frame_rate);
	}
	else if (in_frame_rate != 0)
	{
		framegrabber_s_out_framerate(v4l2m_context->framegrabber_handle,in_frame_rate);
		pr_info("set framerate to in_framerate= %u\n", in_frame_rate);
	} else {
        framegrabber_s_out_framerate(v4l2m_context->framegrabber_handle,REFRESHRATE_60);
        pr_info("set framerate to 60 Hz\n");
    }

    a->parm.capture.capability   = V4L2_CAP_TIMEPERFRAME;

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
            pr_info("brightness(%d)\n",ctrl->value);
			ret = 0;
			break;

		case V4L2_CID_CONTRAST:
	
			bchs_select = V4L2_BCHS_TYPE_CONTRAST;
		    pr_info("contrast(%d)\n",bchs_select);
            ctrl->value = framegrabber_g_input_bchs(v4l2m_context->framegrabber_handle,bchs_select);
			ret = 0;
			break;

		case V4L2_CID_SATURATION:
			
			bchs_select = V4L2_BCHS_TYPE_SATURATION;
		    pr_info("saturation(%d)\n",bchs_select);
            ctrl->value = framegrabber_g_input_bchs(v4l2m_context->framegrabber_handle,bchs_select);
			ret = 0;
			break;

		case V4L2_CID_HUE:
		
			bchs_select = V4L2_BCHS_TYPE_HUE;
		    pr_info("hue(%d)\n",bchs_select);
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

    return -EINVAL;
}

int v4l2_model_ioctl_s_std(struct file *file, void *fd,
            v4l2_std_id std)
{

    return 0;
}

int v4l2_model_ioctl_g_std(struct file *file, void *fd,
            v4l2_std_id *std)
{

    return 0;
}

int v4l2_model_ioctl_querystd(struct file *file, void *fd,
            v4l2_std_id *std)
{

    return 0;
}

int v4l2_model_g_tuner(struct file *file, void *fd,
            struct v4l2_tuner *tuner)
{

    return 0;
}

int v4l2_model_s_tuner(struct file *file, void *fd,
            const struct v4l2_tuner *tuner)
{

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

