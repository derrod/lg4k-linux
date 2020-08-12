/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * framegrabber.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
#include "typedef.h"
#include <stdarg.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <media/v4l2-device.h>
#include "cxt_mgr.h"
#include "framegrabber.h"
#include "framegrabber_priv.h"
#include <media/videobuf2-vmalloc.h>

#include "v4l2_model.h"
#include "v4l2_model_device.h"

static void *framegrabber_alloc(void);
static void framegrabber_release(void *);


static const framegrabber_pixfmt_t support_pixfmts[] = {
	
	[FRAMEGRABBER_PIXFMT_YUYV]={ //YUYV index=0
		.name     = "4:2:2, packed, YUYV",
		.fourcc   = V4L2_PIX_FMT_YUYV,
		.depth    = 16,
		.is_yuv   = true,
		.pixfmt_out = YUYV,
	},
	#if 0
	[FRAMEGRABBER_PIXFMT_UYVY]={ //UYVY
		.name     = "4:2:2, packed, UYVY",
		.fourcc   = V4L2_PIX_FMT_UYVY,
		.depth    = 16,
		.is_yuv   = true,
		.pixfmt_out = UYVY,
	},
	[FRAMEGRABBER_PIXFMT_YVYU]={ //YVYU
		.name     = "4:2:2, packed, YVYU",
		.fourcc   = V4L2_PIX_FMT_YVYU,
		.depth    = 16,
		.is_yuv   = true,
		.pixfmt_out = YVYU,
	},
	
	[FRAMEGRABBER_PIXFMT_VYUY]={ //VYUY
		.name     = "4:2:2, packed, VYUY",
		.fourcc   = V4L2_PIX_FMT_VYUY,
		.depth    = 16,
		.is_yuv   = true,
		.pixfmt_out = VYUY,
	},

	[FRAMEGRABBER_PIXFMT_RGB565]={ //RGBP
		.name     = "RGB565 (LE)",
		.fourcc   = V4L2_PIX_FMT_RGB565, /* gggbbbbb rrrrrggg */
		.depth    = 16,
		.is_yuv   = false,
		.pixfmt_out = RGBP,
	},
	[FRAMEGRABBER_PIXFMT_RGB565X]={ //RGBR
		.name     = "RGB565 (BE)",
		.fourcc   = V4L2_PIX_FMT_RGB565X, /* rrrrrggg gggbbbbb */
		.depth    = 16,
		.is_yuv   = false,
		.pixfmt_out = RGBR,
	},
	[FRAMEGRABBER_PIXFMT_RGB555]={ //RGBO
		.name     = "RGB555 (LE)",
		.fourcc   = V4L2_PIX_FMT_RGB555, /* gggbbbbb arrrrrgg */
		.depth    = 16,
		.is_yuv   = false,
		.pixfmt_out = RGBO,
	},
	[FRAMEGRABBER_PIXFMT_RGB555X]={ //RGBQ
		.name     = "RGB555 (BE)",
		.fourcc   = V4L2_PIX_FMT_RGB555X, /* arrrrrgg gggbbbbb */
		.depth    = 16,
		.is_yuv   = false,
		.pixfmt_out = RGBQ,
	},
	#endif
	[FRAMEGRABBER_PIXFMT_RGB24]={ //RGB3 index=8
		.name     = "RGB24 (LE)",
		.fourcc   = V4L2_PIX_FMT_RGB24, /* rgb */
		.depth    = 24,
		.is_yuv   = false,
		.pixfmt_out = RGB3,
	},
	#if 0
	[FRAMEGRABBER_PIXFMT_BGR24]={ //BGR3
		.name     = "RGB24 (BE)",
		.fourcc   = V4L2_PIX_FMT_BGR24, /* bgr */
		.depth    = 24,
		.is_yuv   = false,
		.pixfmt_out = BGR3,
	},
	[FRAMEGRABBER_PIXFMT_RGB32]={ //RGB4
		.name     = "RGB32 (LE)",
		.fourcc   = V4L2_PIX_FMT_RGB32, /* argb */
		.depth    = 32,
		.is_yuv   = false,
		.pixfmt_out = RGB4,
	},
	[FRAMEGRABBER_PIXFMT_BGR32]={ //BGR4
		.name     = "RGB32 (BE)",
		.fourcc   = V4L2_PIX_FMT_BGR32, /* bgra */
		.depth    = 32,
		.is_yuv   = false,
		.pixfmt_out = BGR4,
	},
	#endif
};

static const framegrabber_frame_size_t framegrabber_support_frame_size[FRAMEGRABBER_SUPPORT_FRAMESIZE_NUM] = {
	[FRAMESIZE_640x480]=	{ .width = 640, 	.height = 480 },
	[FRAMESIZE_720x480]= 	{ .width = 720, 	.height = 480 },
	[FRAMESIZE_720x576]= 	{ .width = 720, 	.height = 576 },
	[FRAMESIZE_800x600]= 	{ .width = 800, 	.height = 600 },
	[FRAMESIZE_1024x768]= 	{ .width = 1024, 	.height = 768 },
	[FRAMESIZE_1280x720]=	{ .width = 1280, 	.height = 720 },
	[FRAMESIZE_1280x768]=	{ .width = 1280, 	.height = 768 },
	[FRAMESIZE_1280x800]=	{ .width = 1280, 	.height = 800 },
	[FRAMESIZE_1280x1024]=	{ .width = 1280, 	.height = 1024},
	[FRAMESIZE_1360x768]=	{ .width = 1360, 	.height = 768 },
	[FRAMESIZE_1440x900]=	{ .width = 1440, 	.height = 900 },
	[FRAMESIZE_1680x1050]=	{ .width = 1680, 	.height = 1050},
	[FRAMESIZE_1920x1080]=	{ .width = 1920, 	.height = 1080},
	[FRAMESIZE_1920x1200]=	{ .width = 1920, 	.height = 1200},
	[FRAMESIZE_2560x1080]=	{ .width = 2560, 	.height = 1080},
	[FRAMESIZE_2560x1440]=	{ .width = 2560, 	.height = 1440},
	[FRAMESIZE_3840x2160]=	{ .width = 3840, 	.height = 2160},
	[FRAMESIZE_4096x2160]=	{ .width = 4096, 	.height = 2160},
};


static const int framegrabber_support_refreshrate[]= {
    [REFRESHRATE_15]=15,
    [REFRESHRATE_24]=24,
    [REFRESHRATE_25]=25,
    [REFRESHRATE_30]=30,
    [REFRESHRATE_50]=50,
    [REFRESHRATE_60]=60,
    [REFRESHRATE_100]=100,
    [REFRESHRATE_120]=120,
    [REFRESHRATE_144]=144,
    [REFRESHRATE_240]=240,
};

framegrabber_handle_t framegrabber_init(cxt_mgr_handle_t cxt_mgr,framegrabber_property_t *prop,framegrabber_interface_t *operation)
{
	framegrabber_context_t *fg_cxt=NULL;
	framegrabber_error_e err=FRAMEGRABBER_OK;

	do
	{
		int i,j;
		BOOL_T end_of_inputinfo;

		if(!cxt_mgr)
			break;
		fg_cxt=cxt_manager_add_cxt(cxt_mgr,FG_CXT_ID,framegrabber_alloc,framegrabber_release);
		if(!fg_cxt)
		{
			err=FRAMEGRABBER_ERROR_ALLOC_FAIL;
			break;
		}
		if(prop->input_setup_info==NULL)
		{
			err=FRAMEGRABBER_ERROR_INPUT_NULL;
			break;
		}
		for(i=0,end_of_inputinfo=false;i<FRAMEGRABBER_MAX_INPUT_NUM;i++)
		{
			if(prop->input_setup_info[i].name)
			{
				fg_cxt->input_info[i].name=prop->input_setup_info[i].name;
				fg_cxt->input_info[i].framesize_cap=0;
				for(j=0;j<FRAMEGRABBER_SUPPORT_FRAMESIZE_NUM;j++)
				{
					if(prop->input_setup_info[i].support_framesize_info[j]!=0)
					{
						fg_cxt->input_info[i].framesize_cap |= (1 << j);
						fg_cxt->input_info[i].framesize_info[j]=prop->input_setup_info[i].support_framesize_info[j];
					}
				}
			}else
			{
				fg_cxt->input_no=i;
				end_of_inputinfo=true;
				break;
			}
		}
		if(!end_of_inputinfo)
		{
			err=FRAMEGRABBER_ERROR_INPUT_OVERFLOW;
			break;
		}
		fg_cxt->current_input=0;
		fg_cxt->current_input_status=FRAMEGRABBER_INPUT_STATUS_NO_POWER;
		if(prop->support_out_pixfmt_mask==0)
		{
			err=FRAMEGRABBER_ERROR_NO_PIXFMT_SETUP;
			break;
		}
		fg_cxt->pixfmt_cap=prop->support_out_pixfmt_mask;
		fg_cxt->max_supported_width=prop->max_supported_width;
                fg_cxt->max_frame_size=prop->max_frame_size;
		for(i=0;i<FRAMEGRABBER_PIXFMT_MAX;i++)
		{
			if(fg_cxt->pixfmt_cap & (1<<i) )
			{
				//printk("%s fg_cxt->current_out_pixfmt=%d\n",__func__,i);
				fg_cxt->current_out_pixfmt=i;
				break;
			}
		}
		memcpy(&fg_cxt->interface,operation,sizeof(*operation));



	}while(0);
	if(err!=FRAMEGRABBER_OK)
	{
		switch(err)
		{
			case FRAMEGRABBER_ERROR_INPUT_NULL:
			case FRAMEGRABBER_ERROR_ALLOC_FAIL:
				break;
			default:
				break;
		}

	}

	return (fg_cxt) ? &fg_cxt->interface : NULL;

}


static framegrabber_context_t *framegrabber_getcontext(framegrabber_handle_t handle)
{
	return container_of(handle,framegrabber_context_t,interface);
}

static void *framegrabber_alloc()
{
	framegrabber_context_t *context=NULL;

	context = kzalloc(sizeof(*context), GFP_KERNEL);

	return context;
}

static void framegrabber_release(void *context)
{
	framegrabber_context_t *fg_cxt=context;
	if(fg_cxt)
		kfree(fg_cxt);
}


void framegrabber_set_data(framegrabber_handle_t handle,void *data)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	context->data=data;
}
void *framegrabber_get_data(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	return context->data;
}


void framegrabber_start(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	if(context->interface.s_input)
	{
		context->interface.s_input(handle,context->current_input);
	}
}

int framegrabber_g_input_num(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	return context->input_no;
}

const char *framegrabber_g_input_name(framegrabber_handle_t handle,int input)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	if(input<0 || input > context->input_no)
	{
		return NULL;
	}
	return context->input_info[input].name;
}

int framegrabber_g_input(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	return context->current_input;
}

int framegrabber_s_input(framegrabber_handle_t handle,int input)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	if(input <0 && input >= context->input_no)
	{
		return FRAMEGRABBER_ERROR_INPUT_INVALID;
	}
	if(context->current_input!=input)
	{
		if(context->interface.s_input)
			context->interface.s_input(handle,input);
		context->current_input=input;
	}

	return FRAMEGRABBER_OK;
}

framegrabber_input_status_e framegrabber_g_input_status(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	return context->current_input_status;
}

void framegrabber_s_input_status(framegrabber_handle_t handle,framegrabber_input_status_e input_status)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	context->current_input_status=input_status;

}

void framegrabber_mask_s_status(framegrabber_handle_t handle,framegrabber_status_bitmask_e mask,framegrabber_status_bitmask_e status)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	framegrabber_status_bitmask_e current_status=context->current_status;
	framegrabber_status_bitmask_e diff;

	current_status &= ~mask;
	current_status |= (status & mask);
	diff=current_status ^ context->current_status;
	//printk("%s diff %08x status %08x=>%08x\n",__func__,diff,context->current_status,current_status);
	context->current_status=current_status;


#if 1
    if(diff & (FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT) )
    {
        if((current_status & (FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT))==(FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT))
        {
            // streaming status changed from STOP to START, so we have to set streaming on
            if(handle->stream_on)
            {
                printk("%s stream_on\n",__func__);
                handle->stream_on(handle);
            }
        }else
        {
            // streaming status changed from START to STOP, so we have to set streaming off
            if(handle->stream_off)
            {
                printk("%s stream_off\n",__func__);
                handle->stream_off(handle);
            }
        }
    }
    else
    if(diff & (FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT) )
    {
        if((current_status & (FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT | FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT))==(FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT | FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT))
        {
            // signal status changed from UNLOCK to LOCK and the current streaming status is START, so we have to set streaming on
            if(handle->stream_on)
            {
                printk("%s stream_on\n",__func__);
                handle->stream_on(handle);
            }
        }
    }
#else
	if(diff & (FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT|FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT) )
	{
		if((current_status & (FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT|FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT))==(FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT|FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT))
		{
			if(handle->stream_on)
			{
				printk("%s stream_on\n",__func__);
				handle->stream_on(handle);
			}
		}else
		{
			if(handle->stream_off)
			{
				printk("%s stream_off\n",__func__);
				handle->stream_off(handle);
			}
		}
	}
#endif


	return;
}

framegrabber_status_bitmask_e framegrabber_g_status(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	return context->current_status;
}


const framegrabber_pixfmt_t *framegrabber_g_out_pixelfmt(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	return &support_pixfmts[context->current_out_pixfmt];
}

const framegrabber_pixfmt_t *framegrabber_g_support_pixelfmt_by_index(framegrabber_handle_t handle,int index) //
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	int i,idx=-1;
	u32 mask=context->pixfmt_cap;

    
    if (index >= FRAMEGRABBER_PIXFMT_MAX)
	    return NULL;
	    
	for(i=0;i<FRAMEGRABBER_PIXFMT_MAX && mask;i++) //12
	{
		if( mask & (1<<i))
		{
			idx++;
			mask ^= (1<<i);
			if(idx==index)
			{
				return &support_pixfmts[i];
			}
		}
	}

	return NULL;
}

const framegrabber_pixfmt_t *framegrabber_g_support_pixelfmt_by_fourcc(framegrabber_handle_t handle,u32 fourcc)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	int i;
	int pixfmt_index=-1;

	for(i=0;i<FRAMEGRABBER_PIXFMT_MAX;i++)
	{
		if(support_pixfmts[i].fourcc==fourcc)
		{
			if(context->pixfmt_cap & (1<<i))
				pixfmt_index=i;
			break;
		}
	}
	if(pixfmt_index==-1)
			return NULL;

	return &support_pixfmts[pixfmt_index];
}


int framegrabber_s_out_pixelfmt(framegrabber_handle_t handle,u32 fourcc)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	int i;
	int pixfmt_index=-1;

	for(i=0;i<FRAMEGRABBER_PIXFMT_MAX;i++)
	{
		if(support_pixfmts[i].fourcc==fourcc)
		{
			if(context->pixfmt_cap & (1<<i))
				pixfmt_index=i;
			break;
		}
	}
	if(pixfmt_index==-1)
		return FRAMEGRABBER_ERROR_NO_SUPPORT_PIXFMT;
	context->current_out_pixfmt=pixfmt_index;
	if(context->interface.s_out_pixfmt)
		context->interface.s_out_pixfmt(handle,fourcc);

	return FRAMEGRABBER_OK;
}

void framegrabber_s_out_framesize(framegrabber_handle_t handle,int width,int height)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	if((context->current_out_framesize.width==width) && (context->current_out_framesize.height==height))
		return;
	context->current_out_framesize.width=width;
	context->current_out_framesize.height=height;
	if(context->interface.s_framesize)
		context->interface.s_framesize(handle,width,height);
}

unsigned framegrabber_g_max_framebuffersize(framegrabber_handle_t handle)
{
    framegrabber_context_t *context=framegrabber_getcontext(handle);
    const framegrabber_pixfmt_t *pixfmt=framegrabber_g_out_pixelfmt(handle);
    unsigned max_framebufsize=0;
        
    max_framebufsize=(context->max_frame_size * pixfmt->depth) >> 3;
                
    return max_framebufsize;
}

unsigned framegrabber_g_out_bytesperline(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	const framegrabber_pixfmt_t *pixfmt=framegrabber_g_out_pixelfmt(handle);
	unsigned bytesperline;

//        if(context->max_supported_width)
//            bytesperline=(context->max_supported_width * pixfmt->depth) >> 3;
//        else
//        {
            if(context->current_out_framesize.width)
                bytesperline=(context->current_out_framesize.width * pixfmt->depth) >> 3;
            else
                bytesperline=(context->current_input_framesize.width * pixfmt->depth) >> 3;
        //}


	return bytesperline;
}

void framegrabber_g_out_framesize(framegrabber_handle_t handle,int *width,int *height)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	*width=context->current_out_framesize.width;
	*height=context->current_out_framesize.height;

}

void framegrabber_g_input_framesize(framegrabber_handle_t handle,int *width,int *height)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	*width=context->current_input_framesize.width;
	*height=context->current_input_framesize.height;
}


framegrabber_framemode_e framegrabber_g_input_framemode(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	return context->framemode;
}

void framegrabber_s_input_framemode(framegrabber_handle_t handle,framegrabber_framemode_e framemode)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	if((framemode <0) || (framemode > FRAMEGRABBER_FRAMEMODE_COUNT))
		return;
	context->framemode=framemode;
}


void framegrabber_s_input_framesize(framegrabber_handle_t handle,int width,int height)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	context->current_input_framesize.width=width;
	context->current_input_framesize.height=height;
}

void framegrabber_s_input_dualmode(framegrabber_handle_t handle,int dual_pixel)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	context->current_dual_pixel=dual_pixel;
}

int framegrabber_g_input_dualmode(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
    printk(">>>>>%s ..%d\n",__func__,context->current_dual_pixel);
	return context->current_dual_pixel;
}

int framegrabber_g_input_framerate(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
    //printk(">>>>>%s ..%d\n",__func__,context->current_framerate);
	return context->current_framerate;
}

int framegrabber_g_input_denominator(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	
	return context->current_denominator;
}

void framegrabber_s_input_framerate(framegrabber_handle_t handle,int framerate,int denominator)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	context->current_framerate=framerate;
    context->current_denominator=denominator;
    //printk(">>>>>%s ..%d\n",__func__,context->current_framerate);
}

void framegrabber_s_out_framerate(framegrabber_handle_t handle,int framerate)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	context->current_out_framerate=framerate;
    
    //printk(">>>>>>%s ..%d\n",__func__,context->current_out_framerate);
}

int framegrabber_g_out_framerate(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	
    //printk(">>>>>>>%s ..%d\n",__func__,context->current_out_framerate);
    
	return context->current_out_framerate;
}

void framegrabber_s_input_audioinfo(framegrabber_handle_t handle,enum framegrabber_audio_sample audioinfo)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	context->current_audioinfo=audioinfo;
  
    //printk(">>>>>%s ..%d\n",__func__,context->current_audioinfo);
}

int framegrabber_g_input_audioinfo(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	
    //printk(">>>>>>>%s ..%d\n",__func__,context->current_audioinfo);
    
	return context->current_audioinfo;
}

int framegrabber_g_input_bchs(framegrabber_handle_t handle,int bchs_select)
{
//	framegrabber_context_t *context=framegrabber_getcontext(handle);
	//framegrabber_context_t *context;
	int bchs_value=0;
    
    //context->current_bchs_selection=*bchs_select;
    
	//printk("%s current_bchs_value(%d) = %x\n",__func__,context->current_bchs_selection,context->current_bchs_value);
	
	switch (bchs_select)
	{
		case 0: //brightness
		{
			handle->brightness_get(handle,&bchs_value); 
			//bchs_value = &handle->fg_bchs_value;
			//printk("%s brightness=%d\n",__func__,*bchs_value);  
		    break;
		}
		case 1: //contrast
		{
			handle->contrast_get(handle,&bchs_value); 
			//bchs_value = &handle->fg_bchs_value;
			//printk("%s contrast=%d\n",__func__,*bchs_value);   
		    break;
		}
		case 2: //hue
		{
			handle->hue_get(handle,&bchs_value); 
			//bchs_value = &handle->fg_bchs_value;
			//printk("%s hue=%d\n",__func__,*bchs_value);  
		    break;
		}
		case 3: //saturation
		{
			handle->saturation_get(handle,&bchs_value); 
			//bchs_value = &handle->fg_bchs_value;
			//printk("%s saturation=%d\n",__func__,bchs_value);  
		    break;
		}
		
	}
	
	//context->current_bchs_value = *bchs_value;
	//printk("%s bchs_value(%d) = %d\n",__func__,context->current_bchs_selection,context->current_bchs_value);
	return bchs_value;
	
}

void framegrabber_s_input_bchs(framegrabber_handle_t handle,int bchs_value,int bchs_select)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	//int fg_bchs_value=0;
	//int fg_bchs_select=0;
    
    //fg_bchs_value = bchs_value;
    //fg_bchs_select = bchs_select;
    handle->fg_bchs_value = bchs_value;
    handle->fg_bchs_selection = bchs_select;
    context->current_bchs_selection=bchs_select;
    context->current_bchs_value=bchs_value;
    
    if(handle->bchs_set)
		handle->bchs_set(handle);   
	
	printk("%s current_bchs_value(%d) = %d\n",__func__,bchs_select,context->current_bchs_value);
	
    
}

int framegrabber_g_hdcp_state(framegrabber_handle_t handle)
{
//	framegrabber_context_t *context=framegrabber_getcontext(handle);
	//framegrabber_context_t *context;
	int state_value=0;
    
    handle->hdcp_state_get(handle,&state_value);
    printk("%s hdcp state value=%d\n",__func__,state_value);  

	return state_value;
	
}

void framegrabber_s_hdcp_state(framegrabber_handle_t handle,int hdcp_state)
{
//	framegrabber_context_t *context=framegrabber_getcontext(handle);

	handle->hdcp_state_set(handle, hdcp_state);   
}

int framegrabber_g_input_interlace(framegrabber_handle_t handle)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	return context->current_input_interlace;
}

void framegrabber_s_input_interlace(framegrabber_handle_t handle,BOOL_T interlace)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);

	context->current_input_interlace=interlace;

}

int framegrabber_g_supportframesize(framegrabber_handle_t handle,int index,int *width,int *height)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	int i,idx=-1;
	int in_width=0,in_height=0;
	framegrabber_g_input_framesize(handle,&in_width,&in_height);

	//if(in_width==0 && in_height==0)
	{
		u32 framesize_mask=context->input_info[context->current_input].framesize_cap;
		for(i=0;i<FRAMEGRABBER_SUPPORT_FRAMESIZE_NUM &&  framesize_mask;i++)
		{
			if(framesize_mask & (1<<i))
			{
				framesize_mask ^= (1<<i);
				idx++;
				if(idx==index)
				{
					*width=framegrabber_support_frame_size[i].width;
					*height=framegrabber_support_frame_size[i].height;
					return FRAMEGRABBER_OK;
				}
			}

		}
		//*width=0;
		//*height=0;
	}
	#if 0
	else
	{
		if(index==0)
		{
			*width=in_width;
			*height=in_height;
			return FRAMEGRABBER_OK;
		}
	}
	#endif
	return FRAMEGRABBER_ERROR_INVALID_FRAMESIZE;
}

int framegrabber_g_framesize_supportrefreshrate(framegrabber_handle_t handle,int width,int height,int index)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	int i,idx=-1;
	u32 refreshrate_mask=0;
        
	for(i=0;i<FRAMEGRABBER_SUPPORT_FRAMESIZE_NUM ;i++)
	{
        
		if(framegrabber_support_frame_size[i].width==width && framegrabber_support_frame_size[i].height==height)
		{
        
                    if(context->input_info[context->current_input].framesize_cap & (1<<i) )
                    {
        
                        if(context->input_info[context->current_input].framesize_info[i])
                        {
                            refreshrate_mask=context->input_info[context->current_input].framesize_info[i];
                            break;
                        }
                    }
                }
	}

	for(i=0;i<FRAMEGRABBER_SUPPORT_REFERSHRATE_NUM && refreshrate_mask;i++)
	{
		if(refreshrate_mask & (1<<i))
		{
			refreshrate_mask ^= (1<<i);
			idx++;
			if(idx==index)
			{
				return framegrabber_support_refreshrate[i];
			}

		}
	}

	return 0;
}

void framegrabber_start_streaming(framegrabber_handle_t handle,BOOL_T on)
{
	if(on)
	{
		framegrabber_mask_s_status(handle,FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT ,FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT);

	}else
	{
		framegrabber_mask_s_status(handle,FRAMEGRABBER_STATUS_V4L_START_STREAMING_BIT,0);
	}
}

void framegrabber_notify(framegrabber_handle_t handle,char *fromname,int notification,void *arg)
{
	framegrabber_context_t *context=framegrabber_getcontext(handle);
	if(context->interface.notify!=NULL)
	{
		context->interface.notify(handle,fromname,notification,arg);
	}
}

int framegrabber_s_flash(framegrabber_handle_t handle, int start_block, int blocks, U8_T *flash_update)
{
	int ret=0;
	framegrabber_context_t *context = framegrabber_getcontext(handle);

	if (context->interface.flash_update)
	{
		ret = context->interface.flash_update(handle, start_block, blocks, flash_update);
	}
	return 0;
}

int framegrabber_g_flash(framegrabber_handle_t handle, int start_block, int blocks, U8_T *flash_dump)
{
	int ret=0;
	framegrabber_context_t *context = framegrabber_getcontext(handle);

	if (context->interface.flash_read)
	{
		ret = context->interface.flash_read(handle, start_block, blocks, flash_dump);
	}
	return 0;
}

int framegrabber_g_hdcp_flag(framegrabber_handle_t handle)
{
	framegrabber_context_t *context = framegrabber_getcontext(handle);
    int hdcp_flag;
	hdcp_flag = context->hdcp_flag;
	
	//printk("%s hdcp_flag =%u\n",__func__,hdcp_flag);
	return hdcp_flag;
}

void framegrabber_s_hdcp_flag(framegrabber_handle_t handle, unsigned int hdcp_flag)
{
	framegrabber_context_t *context = framegrabber_getcontext(handle);
	
	context->hdcp_flag = hdcp_flag;
	
	//printk("%s hdcp_flag =%u\n",__func__,hdcp_flag);
}

int framegrabber_g_i2c(framegrabber_handle_t handle, unsigned char channel, unsigned int slave, unsigned int sub, unsigned char sublen, unsigned char *data, unsigned int datalen, unsigned int is_10bit)
{
	framegrabber_context_t *context = framegrabber_getcontext(handle);
	if(context == NULL)
	{
		printk("param is null!\n");
		return -1;
	}

	if(!context->interface.i2c_read)
	{
		printk("function pointer is null\n");
		return -1;
	}

	return context->interface.i2c_read(handle, channel, slave, sub, sublen, data, datalen, is_10bit);
}

int framegrabber_s_i2c(framegrabber_handle_t handle, unsigned char channel, unsigned int slave, unsigned int sub, unsigned char sublen, unsigned char *data, unsigned int datalen, unsigned int is_10bit)
{
	framegrabber_context_t *context = framegrabber_getcontext(handle);
	if(context == NULL)
	{
		printk("param is null!\n");
		return -1;
	}

	if(!context->interface.i2c_write)
	{
		printk("function pointer is null\n");
		return -1;
	}

	return context->interface.i2c_write(handle, channel, slave, sub, sublen, data, datalen, is_10bit);
}

int framegrabber_g_reg(framegrabber_handle_t handle, unsigned int offset, unsigned char n_bytes, unsigned int *data)
{
    framegrabber_context_t *context = framegrabber_getcontext(handle);
    if(context == NULL)
    {
        printk("param is null!\n");
        return -1;
    }

    if(!context->interface.reg_read)
    {
        printk("function pointer is null\n");
        return -1;
    }

    return context->interface.reg_read(handle, offset, n_bytes, data);
}

int framegrabber_s_reg(framegrabber_handle_t handle, unsigned int offset, unsigned char n_bytes, unsigned int data)
{
    framegrabber_context_t *context = framegrabber_getcontext(handle);
    if(context == NULL)
    {
        printk("param is null!\n");
        return -1;
    }

    if(!context->interface.reg_write)
    {
        printk("function pointer is null\n");
        return -1;
    }

    return context->interface.reg_write(handle, offset, n_bytes, data);
}

//static int framegrabber_call_subdev_internal(framegrabber_context_t *context,struct v4l2_subdev *sd,framegrabber_subdev_op_e op,va_list args)
//{
//	int ret;
//
//	switch(op)
//	{
//			case SUBDEV_NOP:
//				break;
//			case SUBDEV_LOG_STATUS:
//				ret=v4l2_subdev_call(sd,core,log_status);
//				break;
//			case SUBDEV_IO_PIN_CONFIG:
//				{
//					size_t n=va_arg(args,size_t);
//					struct v4l2_subdev_io_pin_config *pincfg=va_arg(args,struct v4l2_subdev_io_pin_config *);
//					ret=v4l2_subdev_call(sd,core,s_io_pin_config,n,pincfg);
//				}
//				break;
//
//			case SUBDEV_INIT:
//				{
//					u32 val=va_arg(args,u32);
//					ret=v4l2_subdev_call(sd,core,init,val);
//				}
//				break;
//			case SUBDEV_LOAD_FW:
//				ret=v4l2_subdev_call(sd,core,load_fw);
//				break;
//			case 	SUBDEV_RESET:
//				{
//					u32 val=va_arg(args,u32);
//					ret=v4l2_subdev_call(sd,core,reset,val);
//				}
//				break;
//			case SUBDEV_S_GPIO:
//				{
//					u32 val=va_arg(args,u32);
//					ret=v4l2_subdev_call(sd,core,s_gpio,val);
//				}
//				break;
//			case SUBDEV_QUERYCTL:
//				{
//					struct v4l2_queryctrl *qc=va_arg(args,struct v4l2_queryctrl *);
//						ret=v4l2_subdev_call(sd,core,queryctrl,qc);
//				}
//				break;
//			case SUBDEV_G_CTRL:
//				{
//					struct v4l2_control *ctrl=va_arg(args,struct v4l2_control *);
//					ret=v4l2_subdev_call(sd,core,g_ctrl,ctrl);
//				}
//				break;
//			case SUBDEV_S_CTRL:
//				{
//					struct v4l2_control *ctrl=va_arg(args,struct v4l2_control *);
//					v4l2_subdev_call(sd,core,s_ctrl,ctrl);
//				}
//				break;
//			case SUBDEV_G_EXT_CTRL:
//				{
//					struct v4l2_ext_controls *ctrls=va_arg(args,struct v4l2_ext_controls *);
//					ret=v4l2_subdev_call(sd,core,g_ext_ctrls,ctrls);
//				}
//				break;
//			case SUBDEV_S_EXT_CTRL:
//				{
//					struct v4l2_ext_controls *ctrls=va_arg(args,struct v4l2_ext_controls *);
//					ret=v4l2_subdev_call(sd,core,s_ext_ctrls,ctrls);
//				}
//				break;
//			case SUBDEV_TRY_EXT_CTRL:
//				{
//					struct v4l2_ext_controls *ctrls=va_arg(args,struct v4l2_ext_controls *);
//					ret=v4l2_subdev_call(sd,core,try_ext_ctrls,ctrls);
//				}
//				break;
//			case SUBDEV_QUERYMENU:
//				{
//					struct v4l2_querymenu *qm=va_arg(args,struct v4l2_querymenu *);
//					ret=v4l2_subdev_call(sd,core,querymenu,qm);
//				}
//				break;
//			case SUBDEV_IOCTL:
//				{
//					unsigned int cmd= va_arg(args,int);
//					void *arg=va_arg(args,void *);
//					ret=v4l2_subdev_call(sd,core,ioctl,cmd,arg);
//				}
//				break;
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)
//			case SUBDEV_COMPAT_IOCTL32:
//#ifdef CONFIG_COMPAT
//				{
//					unsigned int cmd= va_arg(args,int);
//					unsigned long arg=va_arg(args,unsigned long);
//					ret=v4l2_subdev_call(sd,core,compat_ioctl32,cmd,arg);
//				}
//#else
//						return FRAMEGRABBER_ERROR_NOSUPPORT_SUBDEV_OP;
//#endif
//						break;
//#endif
//			case SUBDEV_G_REGISTER:
//#ifdef CONFIG_VIDEO_ADV_DEBUG
//				{
//					struct v4l2_dbg_register *reg=va_arg(args,struct v4l2_dbg_register *);
//					ret=v4l2_subdev_call(sd,core,g_register,reg);
//				}
//					break;
//
//#else
//				return FRAMEGRABBER_ERROR_NOSUPPORT_SUBDEV_OP;
//#endif
//			case SUBDEV_S_REGISTER:
//#ifdef CONFIG_VIDEO_ADV_DEBUG
//				{
//					struct v4l2_dbg_register *reg=va_arg(args,struct v4l2_dbg_register *);
//					ret=v4l2_subdev_call(sd,core,s_register,reg);
//				}
//				break;
//
//#else
//				return FRAMEGRABBER_ERROR_NOSUPPORT_SUBDEV_OP;
//#endif
//			case SUBDEV_S_POWER:
//				{
//					int on=va_arg(args,int);
//					ret=v4l2_subdev_call(sd,core,s_power,on);
//				}
//				break;
//			case SUBDEV_S_INTERRUPT_SERVICE_ROUTINE:
//				{
//					u32 status=va_arg(args,u32);
//					BOOL_T *handled=va_arg(args,BOOL_T *);
//					ret=v4l2_subdev_call(sd,core,interrupt_service_routine,status,handled);
//				}
//				break;
//			case SUBDEV_SUBSCRIBE_EVENT:
//				{
//					struct v4l2_fh *fh=va_arg(args,struct v4l2_fh *);
//					struct v4l2_event_subscription *sub=va_arg(args,struct v4l2_event_subscription *);
//					ret=v4l2_subdev_call(sd,core,subscribe_event,fh,sub);
//				}
//				break;
//
//			case SUBDEV_UNSUBSCRIBE_EVENT:
//				{
//					struct v4l2_fh *fh=va_arg(args,struct v4l2_fh *);
//					struct v4l2_event_subscription *sub=va_arg(args,struct v4l2_event_subscription *);
//
//					ret=v4l2_subdev_call(sd,core,unsubscribe_event,fh,sub);
//				}
//				break;
//
//			case SUBDEV_S_CLOCK_FREQ:
//				{
//					u32 freq=va_arg(args,u32);
//					ret=v4l2_subdev_call(sd,audio,s_clock_freq,freq);
//				}
//				break;
//
//			case SUBDEV_S_I2S_CLOCK_FREQ:
//				{
//					u32 freq=va_arg(args,u32);
//					ret=v4l2_subdev_call(sd,audio,s_i2s_clock_freq,freq);
//				}
//				break;
//
//			case SUBDEV_S_A_ROUTING:
//				{
//					u32 input=va_arg(args,u32);
//					u32 output=va_arg(args,u32);
//					u32 config=va_arg(args,u32);
//					ret=v4l2_subdev_call(sd,audio,s_routing,input,output,config);
//				}
//				break;
//
//			case SUBDEV_S_A_STREAM:
//				{
//					int enable=va_arg(args,int);
//					ret=v4l2_subdev_call(sd,audio,s_stream,enable);
//				}
//				break;
//
//			case SUBDEV_S_V_ROUTING:
//				{
//						u32 input,output,config;
//						input=va_arg(args,u32);
//						output=va_arg(args,u32);
//						config=va_arg(args,u32);
//						ret=v4l2_subdev_call(sd,video,s_routing,input,output,config);
//				}
//				break;
//			case SUBDEV_S_CRYSTAL_FREQ:
//				{
//					u32 freq=va_arg(args,u32);
//					u32 flags=va_arg(args,u32);
//					ret=v4l2_subdev_call(sd,video,s_crystal_freq,freq,flags);
//				}
//				break;
//			case SUBDEV_G_STD:
//				{
//					v4l2_std_id *norm=va_arg(args,v4l2_std_id *);
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)
//					ret=v4l2_subdev_call(sd,video,g_std,norm);
//#else
//					ret=v4l2_subdev_call(sd,core,g_std,norm);
//#endif
//				}
//				break;
//			case SUBDEV_S_STD:
//				{
//					v4l2_std_id norm=va_arg(args,v4l2_std_id);
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)
//                                        ret=v4l2_subdev_call(sd,video,s_std,norm);
//#else
//                                        ret=v4l2_subdev_call(sd,core,s_std,norm);
//#endif
//
//				}
//				break;
//			case SUBDEV_G_STD_OUTPUT:
//				{
//					v4l2_std_id *std=va_arg(args,v4l2_std_id *);
//					ret=v4l2_subdev_call(sd,video,g_std_output,std);
//				}
//				break;
//			case SUBDEV_S_STD_OUTPUT:
//				{
//					v4l2_std_id std=va_arg(args,v4l2_std_id);
//					v4l2_subdev_call(sd,video,s_std_output,std);
//				}
//				break;
//
//			case SUBDEV_QUERYSTD:
//				{
//					v4l2_std_id *std=va_arg(args,v4l2_std_id *);
//					ret=v4l2_subdev_call(sd,video,querystd,std);
//				}
//				break;
//
//			case SUBDEV_G_TVNORMS:
//				{
//					v4l2_std_id *std=va_arg(args,v4l2_std_id *);
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,16,0)
//					ret=v4l2_subdev_call(sd,video,g_tvnorms,std);
//#else
//					ret=v4l2_subdev_call(sd,video,g_tvnorms_output,std);
//#endif
//				}
//				break;
//
//			case SUBDEV_G_TVNORMS_OUTPUT:
//				{
//					v4l2_std_id *std=va_arg(args,v4l2_std_id *);
//					ret=v4l2_subdev_call(sd,video,g_tvnorms_output,std);
//				}
//				break;
//
//			case SUBDEV_G_INPUT_STATUS:
//				{
//					u32 *status=va_arg(args,u32 *);
//					ret=v4l2_subdev_call(sd,video,g_input_status,status);
//				}
//				break;
//
//			case SUBDEV_S_V_STREAM:
//				{
//					int enable=va_arg(args,int);
//					ret=v4l2_subdev_call(sd,video,s_stream,enable);
//				}
//				break;
//
//			case SUBDEV_CROPCAP:
//				{
//					struct v4l2_cropcap *cc=va_arg(args,struct v4l2_cropcap *);
//					ret=v4l2_subdev_call(sd,video,cropcap,cc);
//				}
//				break;
//
//			case SUBDEV_G_CROP:
//				{
//					struct v4l2_crop *crop=va_arg(args,struct v4l2_crop *);
//					ret=v4l2_subdev_call(sd,video,g_crop,crop);
//				}
//				break;
//
//			case SUBDEV_S_CROP:
//				{
//					struct v4l2_crop *crop=va_arg(args,struct v4l2_crop *);
//					ret=v4l2_subdev_call(sd,video,s_crop,crop);
//				}
//				break;
//
//			case SUBDEV_G_PARM:
//				{
//					struct v4l2_streamparm *param=va_arg(args,struct v4l2_streamparm *);
//					ret=v4l2_subdev_call(sd,video,g_parm,param);
//				}
//				break;
//
//			case SUBDEV_S_PARM:
//				{
//					struct v4l2_streamparm *param=va_arg(args,struct v4l2_streamparm *);
//					ret=v4l2_subdev_call(sd,video,s_parm,param);
//				}
//				break;
//
//			case SUBDEV_G_FRAME_INTERVAL:
//				{
//					struct v4l2_subdev_frame_interval *interval=va_arg(args,struct v4l2_subdev_frame_interval *);
//					ret=v4l2_subdev_call(sd,video,g_frame_interval,interval);
//				}
//				break;
//
//			case SUBDEV_S_FRAME_INTERVAL:
//				{
//					struct v4l2_subdev_frame_interval *interval=va_arg(args,struct v4l2_subdev_frame_interval *);
//					ret=v4l2_subdev_call(sd,video,s_frame_interval,interval);
//				}
//				break;
//
//			case SUBDEV_ENUM_FRAMESIZES:
//				{
//#if LINUX_VERSION_CODE < KERNEL_VERSION(4,1,0)
//					struct v4l2_frmsizeenum *fsize=va_arg(args,struct v4l2_frmsizeenum *);
//
//					ret=v4l2_subdev_call(sd,video,enum_framesizes,fsize);
//#endif
//				}
//				break;
//
//			case SUBDEV_ENUM_FRAMEINTERVALS:
//				{
//#if LINUX_VERSION_CODE < KERNEL_VERSION(4,1,0)
//					struct v4l2_frmivalenum *fival=va_arg(args,struct v4l2_frmivalenum *);
//
//					ret=v4l2_subdev_call(sd,video,enum_frameintervals,fival);
//#endif
//				}
//				break;
//
//			case SUBDEV_S_DV_TIMINGS:
//				{
//					struct v4l2_dv_timings *timings=va_arg(args,struct v4l2_dv_timings *);
//					ret=v4l2_subdev_call(sd,video,s_dv_timings,timings);
//				}
//				break;
//
//			case SUBDEV_G_DV_TIMINGS:
//				{
//					struct v4l2_dv_timings *timings=va_arg(args,struct v4l2_dv_timings *);
//					ret=v4l2_subdev_call(sd,video,g_dv_timings,timings);
//				}
//				break;
//			case SUBDEV_QUERY_DV_TIMINGS:
//				{
//					struct v4l2_dv_timings *timings=va_arg(args,struct v4l2_dv_timings *);
//					ret=v4l2_subdev_call(sd,video,query_dv_timings,timings);
//				}
//				break;
//
//			case SUBDEV_S_RX_BUFFER:
//				{
//					void *buf=va_arg(args,void *);
//					unsigned int *size=va_arg(args,unsigned int *);
//					ret=v4l2_subdev_call(sd,video,s_rx_buffer,buf,size);
//				}
//				break;
//			default:
//				return FRAMEGRABBER_ERROR_NOSUPPORT_SUBDEV_OP;
//				break;
//
//
//	}
//	return ret;
//}

//int framegrabber_call_subdev(framegrabber_handle_t handle,char *name,framegrabber_subdev_op_e op, ...)
//{
//	va_list args;
//	framegrabber_context_t *context=framegrabber_getcontext(handle);
//	struct v4l2_subdev *sd;
//	BOOL_T found=false;
//	int ret;
//
//	va_start(args, op);
//
//	v4l2_device_for_each_subdev(sd,context->v4l2_dev)
//	{
//		if(strncmp(sd->name,name,strlen(name))==0)
//		{
//				found=true;
//				ret=framegrabber_call_subdev_internal(context,sd,op,args);
//		}
//
// 	}
//	va_end(args);
//
//	if(!found)
//	{
//			return FRAMEGRABBER_ERROR_NO_MATCH_SUBDEV;
//	}
//	if(ret==-ENOIOCTLCMD)
//	{
//		return FRAMEGRABBER_ERROR_NOIMPLEMENT_SUBDEV_OP;
//	}
//
//
//
//	return FRAMEGRABBER_OK;
//}

//void *framegrabber_get_v4l2_context(framegrabber_handle_t handle)
//{
//	framegrabber_context_t *context=framegrabber_getcontext(handle);
//	void *v4l2_context;
//
//	v4l2_context=container_of(context->v4l2_dev,v4l2_model_context_t,v4l2_dev);
//	return v4l2_context;
//}





