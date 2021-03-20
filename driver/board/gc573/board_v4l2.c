/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * board_v4l2.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 
//#include <linux/kernel.h>
#include "board.h"
#include "cxt_mgr.h"
#include "framegrabber.h"
#include "mem_model.h"
#include "v4l2_model.h"
#include "board_v4l2.h"
#include "debug.h"
#include "pci_model.h"
#include "i2c_model.h"
#include "task_model.h"
#include "aver_xilinx.h"
#include "ite6805.h"
#include "board_alsa.h"
#include "board_i2c.h"

static int cnt_retry=0;

typedef struct
{
	BASIC_CXT_HANDLE_DECLARE;
        framegrabber_handle_t fg_handle;
        v4l2_model_handle_t v4l2_handle;
        i2c_model_handle_t i2c_model_handle;
        task_model_handle_t task_model_handle;
        handle_t aver_xilinx_handle;
        handle_t i2c_chip_handle[GC573_I2C_CHIP_COUNT];
        ite6805_frameinfo_t cur_fe_frameinfo;
        enum ite6805_audio_sample cur_fe_audioinfo;
        int cur_bchs_value;
        int cur_bchs_selection;
        task_handle_t check_signal_task_handle;
        int board_id;
}board_v4l2_context_t;

static board_chip_desc_t gc573_chip_desc[GC573_I2C_CHIP_COUNT]=
{
        [GC573_I2C_CHIP_ITE6805_0]=
        {
            .name=ITE6805_DRVNAME,
            .index=1,
        },
    
};

static framegrabber_setup_input_info_t gc573_input_info[] =
{
    [GC573_HDMI_INPUT] =
    {
        .name = "HDMI",
        .support_framesize_info =
        {
            [FRAMESIZE_640x480] = REFRESHRATE_60_BIT, 
            [FRAMESIZE_720x480] = REFRESHRATE_60_BIT, 
            [FRAMESIZE_720x576] = REFRESHRATE_50_BIT, 
            [FRAMESIZE_800x600] = REFRESHRATE_60_BIT, 
            [FRAMESIZE_1024x768] = REFRESHRATE_60_BIT,
            [FRAMESIZE_1280x720] = REFRESHRATE_50_BIT | REFRESHRATE_60_BIT,
            [FRAMESIZE_1280x768]  = REFRESHRATE_60_BIT,
            [FRAMESIZE_1280x800]  = REFRESHRATE_60_BIT,
            [FRAMESIZE_1280x1024] = REFRESHRATE_60_BIT,
            [FRAMESIZE_1360x768]  = REFRESHRATE_60_BIT,
            [FRAMESIZE_1440x900]  = REFRESHRATE_60_BIT,
            [FRAMESIZE_1680x1050] = REFRESHRATE_60_BIT,
            [FRAMESIZE_1920x1080] = REFRESHRATE_120_BIT | REFRESHRATE_60_BIT \
                                    | REFRESHRATE_50_BIT | REFRESHRATE_30_BIT | REFRESHRATE_25_BIT | REFRESHRATE_24_BIT,
            [FRAMESIZE_1920x1200] = REFRESHRATE_60_BIT  | REFRESHRATE_50_BIT | REFRESHRATE_30_BIT \
                                    | REFRESHRATE_25_BIT | REFRESHRATE_24_BIT,
            [FRAMESIZE_2560x1080] = REFRESHRATE_144_BIT | REFRESHRATE_120_BIT | REFRESHRATE_60_BIT, 
            [FRAMESIZE_2560x1440] = REFRESHRATE_144_BIT | REFRESHRATE_120_BIT | REFRESHRATE_60_BIT, 
            [FRAMESIZE_3840x2160] = REFRESHRATE_60_BIT  | REFRESHRATE_50_BIT | REFRESHRATE_30_BIT \
                                    | REFRESHRATE_25_BIT | REFRESHRATE_24_BIT, 
            [FRAMESIZE_4096x2160] = REFRESHRATE_60_BIT  | REFRESHRATE_50_BIT | REFRESHRATE_30_BIT \
                                    | REFRESHRATE_25_BIT | REFRESHRATE_24_BIT,                       
        },
    },
    [GC573_INPUT_COUNT] =
    {
		.name = NULL,
	},
};

static framegrabber_property_t  gc573_property={
		.name="GC573",
		.input_setup_info=gc573_input_info,
		.support_out_pixfmt_mask=FRAMEGRABBER_PIXFMT_BITMSK,//FRAMEGRABBER_PIXFMT_YUYV_BIT ,  //tt 0615
		//.max_supported_line_width=3840,
		//.max_supported_line_width=4096,
        .max_frame_size=4096 *2160,

};

static v4l2_model_device_setup_t device_info=
{
    .type = DEVICE_TYPE_GRABBER,
    .driver_name="GC573",
    .card_name="AVerMedia GC573",
    .capabilities= V4L2_MODEL_CAPS_CAPTURE_BIT | V4L2_MODEL_CAPS_READWRITE_BIT | V4L2_MODEL_CAPS_STREAMING_BIT,
    .buffer_type=V4L2_MODEL_BUF_TYPE_DMA_SG,
};

static void *board_v4l2_alloc(void);
static void board_v4l2_release(void *cxt);
static void gc573_v4l2_stream_on(v4l2_model_callback_parameter_t *cb_info);
static void gc573_v4l2_stream_off(v4l2_model_callback_parameter_t *cb_info);
static void gc573_stream_on(framegrabber_handle_t handle);
static void gc573_stream_off(framegrabber_handle_t handle);
//static void gc573_bchs_read(framegrabber_handle_t handle);
static void gc573_bchs_write(framegrabber_handle_t handle);
static void gc573_brightness_read(framegrabber_handle_t handle,int *brightness);
static void gc573_contrast_read(framegrabber_handle_t handle,int *contrast);
static void gc573_hue_read(framegrabber_handle_t handle,int *hue);
static void gc573_saturation_read(framegrabber_handle_t handle,int *saturation);
static void gc573_hdcp_state_read(framegrabber_handle_t handle,int *hdcp_state);
static void gc573_hdcp_state_set(framegrabber_handle_t handle,int hdcp_state);
static int gc573_i2c_read(framegrabber_handle_t handle, unsigned char channel, unsigned int slave, unsigned int sub, unsigned char sublen, unsigned char *data, unsigned int datalen, unsigned int is_10bit);
static int gc573_i2c_write(framegrabber_handle_t handle, unsigned char channel, unsigned int slave, unsigned int sub, unsigned char sublen, unsigned char *data, unsigned int datalen, unsigned int is_10bit);
static int gc573_reg_read(framegrabber_handle_t handle, unsigned int offset, unsigned char n_bytes, unsigned int *data);
static int gc573_reg_write(framegrabber_handle_t handle, unsigned int offset, unsigned char n_bytes, unsigned int data);

static int gc573_flash_dump(framegrabber_handle_t handle,int start_block, int blocks, U8_T *flash_dump); //
static int gc573_flash_update(framegrabber_handle_t handle,int start_block, int blocks, U8_T *flash_dump); //

static framegrabber_interface_t gc573_ops={
    .stream_on      = gc573_stream_on,
    .stream_off     = gc573_stream_off,
    //.bchs_get     = gc573_bchs_read,
    .brightness_get = gc573_brightness_read,
    .contrast_get   = gc573_contrast_read,
    .hue_get        = gc573_hue_read,
    .saturation_get = gc573_saturation_read,
    .bchs_set       = gc573_bchs_write,
    .flash_read     = gc573_flash_dump,
    .flash_update   = gc573_flash_update,
    .hdcp_state_get = gc573_hdcp_state_read,
    .hdcp_state_set  = gc573_hdcp_state_set,
    .i2c_read = gc573_i2c_read,
    .i2c_write = gc573_i2c_write,
    .reg_read = gc573_reg_read,
    .reg_write = gc573_reg_write,
};

static void *board_v4l2_alloc()
{
	board_v4l2_context_t *cxt;
	cxt=mem_model_alloc_buffer(sizeof(*cxt));

	return cxt;
}

static void board_v4l2_release(void *cxt)
{
	board_v4l2_context_t *board_v4l2_cxt=cxt;

	if(board_v4l2_cxt)
	{
            if(board_v4l2_cxt->check_signal_task_handle)
            {
                task_model_release_task(board_v4l2_cxt->task_model_handle,board_v4l2_cxt->check_signal_task_handle);
                board_v4l2_cxt->check_signal_task_handle=NULL;
            }
            cxt_manager_unref_context(board_v4l2_cxt->task_model_handle);
            cxt_manager_unref_context(board_v4l2_cxt->aver_xilinx_handle);
            mem_model_free_buffer(board_v4l2_cxt);
	}
}

static void gc573_v4l2_stream_on(v4l2_model_callback_parameter_t *cb_info)
{
    board_v4l2_context_t *board_v4l2_cxt=cb_info->asso_data;
    
    debug_msg("%s %p\n",__func__,board_v4l2_cxt);

}

static void gc573_v4l2_stream_off(v4l2_model_callback_parameter_t *cb_info)
{
    board_v4l2_context_t *board_v4l2_cxt=cb_info->asso_data;
    debug_msg("%s %p\n",__func__,board_v4l2_cxt);
    
}
static void gc573_stream_on(framegrabber_handle_t handle)
{
    board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
    aver_xilinx_video_process_cfg_t vip_cfg;
    int width,height;
    int out_width,out_height;
    int frameinterval=0;
    int audio_rate=0;
    int dual_pixel=0;
    //int hdcp_state = 0;
    framegrabber_framemode_e framemode;
    const framegrabber_pixfmt_t *pixfmt=framegrabber_g_out_pixelfmt(handle);
    
    handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];

    //ite6805_get_hdcp_state(ite6805_handle, &hdcp_state);
    //ite6805_set_hdcp_state(ite6805_handle, hdcp_state);

//    enum ite6805_audio_sample fe_audioinfo=0;
    ite6805_frameinfo_t *fe_frameinfo=&board_v4l2_cxt->cur_fe_frameinfo; 
    
    //aver_xilinx_frame_info_t detected_frameinfo;
    mesg("%s\n",__func__);
    //aver_xilinx_get_frameinfo(board_v4l2_cxt->aver_xilinx_handle,&detected_frameinfo);
    framemode=framegrabber_g_input_interlace(board_v4l2_cxt->fg_handle);
    framegrabber_g_input_framesize(board_v4l2_cxt->fg_handle,&width,&height);

    //frameinterval=framegrabber_g_input_framerate(board_v4l2_cxt->fg_handle);
    frameinterval=framegrabber_g_out_framerate(board_v4l2_cxt->fg_handle);
    if (frameinterval ==0)
    {
		frameinterval=framegrabber_g_input_framerate(board_v4l2_cxt->fg_handle);
	}
    //framegrabber_s_out_framerate(board_v4l2_cxt->fg_handle,0);
    
    framegrabber_g_out_framesize(board_v4l2_cxt->fg_handle,&out_width,&out_height);
    debug_msg("%s in %dx%d out %dx%d\n",__func__,width,height,out_width,out_height);
    
    if ((out_width ==0) || (out_height ==0))
    {
        debug_msg("debug...\n");
        out_width = 1920;
        out_height = 1080;
        framegrabber_s_out_framesize(board_v4l2_cxt->fg_handle,1920,1080);
    }

    if(framemode==FRAMEGRABBER_FRAMEMODE_INTERLACED)
        vip_cfg.in_videoformat.is_interlace=TRUE;
    else
        vip_cfg.in_videoformat.is_interlace=FALSE;
        
    //audio_rate = framegrabber_g_input_audioinfo(board_v4l2_cxt->fg_handle);
    
    dual_pixel = framegrabber_g_input_dualmode(board_v4l2_cxt->fg_handle);
    
    //aver_xilinx_dual_pixel(board_v4l2_cxt->aver_xilinx_handle,dual_pixel);
    if (dual_pixel ==1)
        vip_cfg.dual_pixel = 1;
    else
        vip_cfg.dual_pixel = 0;   
    
    audio_rate = aver_xilinx_get_audioinfo(board_v4l2_cxt->aver_xilinx_handle);
    vip_cfg.audio_rate = audio_rate;
    
    framegrabber_s_input_audioinfo(board_v4l2_cxt->fg_handle,audio_rate);

    mesg("%s...vip_cfg.audio_rate_index=%d\n",__func__,vip_cfg.audio_rate);
    ite6805_get_frameinfo(ite6805_handle,fe_frameinfo);
    
    debug_msg("%s========fe_frameinfo->packet_colorspace=%d\n",__func__,fe_frameinfo->packet_colorspace);
    
    vip_cfg.in_videoformat.fps = framegrabber_g_input_framerate(board_v4l2_cxt->fg_handle);
    //if(vip_cfg.is_interlace==TRUE)
        //vip_cfg.in_framerate = vip_cfg.in_framerate * 2;
    vip_cfg.packet_colorspace = fe_frameinfo->packet_colorspace;    
    vip_cfg.in_videoformat.vactive = width;
    vip_cfg.in_videoformat.hactive = height;
    //if (vip_cfg.in_videoformat.is_interlace==TRUE)
        //vip_cfg.in_videoformat.hactive *= 2; 
    vip_cfg.out_videoformat.width=out_width;
    vip_cfg.out_videoformat.height=out_height;
    vip_cfg.clip_start.x=0;
    vip_cfg.clip_start.y=0;
    vip_cfg.clip_size.width=width;
    vip_cfg.clip_size.height=height;
//  vip_cfg.valid_mask=CLIP_CFG_VALID_MASK|SCALER_CFG_VALID_MASK;
//  vip_cfg.valid_mask=SCALER_CFG_VALID_MASK|SCALER_CFG_SHRINK_MASK;
    vip_cfg.valid_mask=SCALER_CFG_VALID_MASK|FRAMERATE_CFG_VALID_MASK;
    vip_cfg.pixel_clock=board_v4l2_cxt->cur_fe_frameinfo.pixel_clock;
    vip_cfg.in_ddrmode = fe_frameinfo->ddr_mode;
    //vip_cfg.out_framerate= 0; //for test
    vip_cfg.out_videoformat.fps=frameinterval;
    mesg("%s..in_framerate=%d  out_framerate=%d pixel_clock=%d\n",__func__,vip_cfg.in_videoformat.fps,vip_cfg.out_videoformat.fps,vip_cfg.pixel_clock);

    ite6805_get_workingmode(ite6805_handle, &(vip_cfg.in_workingmode)) ;
    vip_cfg.in_colorspacemode = 0; // 0-yuv, 1-rgb limit, 2-rgb full
    ite6805_get_colorspace(ite6805_handle, &(vip_cfg.in_colorspacemode));
    vip_cfg.in_videoformat.colorspace = VIDEO_RGB_MODE;
    vip_cfg.in_packetsamplingmode = 1; // 0-rgb, 1-422, 2-444	
    vip_cfg.in_packetcsc_bt = COLORMETRY_ITU709;
    ite6805_get_sampingmode(ite6805_handle, &(vip_cfg.in_packetsamplingmode));
    vip_cfg.currentCSC = CAPTURE_BT709_COMPUTER;
	
    //enable video bypass
    if (((vip_cfg.in_videoformat.vactive == 4096) && (vip_cfg.out_videoformat.width == 4096)
       && (vip_cfg.out_videoformat.height == 2160) && (vip_cfg.out_videoformat.height == 2160)) || //4096x2160
       ((vip_cfg.in_videoformat.vactive == 3840) && (vip_cfg.out_videoformat.width == 3840)
       && (vip_cfg.out_videoformat.height == 2160) && (vip_cfg.out_videoformat.height == 2160)) || //3840x2160
       ((vip_cfg.in_videoformat.vactive == 2560) && (vip_cfg.out_videoformat.width == 2560)  
       && (vip_cfg.in_videoformat.hactive == 1600) && (vip_cfg.out_videoformat.height == 1600)) || //2560x1600
       ((vip_cfg.in_videoformat.vactive == 2560) && (vip_cfg.out_videoformat.width == 2560) 
       && (vip_cfg.in_videoformat.hactive == 1440) && (vip_cfg.out_videoformat.height == 1440)) || //2560x1440
       ((vip_cfg.in_videoformat.vactive == 2560) && (vip_cfg.out_videoformat.width == 2560) 
       && (vip_cfg.in_videoformat.hactive == 1080) && (vip_cfg.out_videoformat.height == 1080)) ||  //2560x1080
       ((vip_cfg.in_videoformat.vactive == 1920) && (vip_cfg.out_videoformat.width == 1920) 
       && (vip_cfg.in_videoformat.hactive == 1440) && (vip_cfg.out_videoformat.height == 1440)) || //1920x1440
       ((vip_cfg.in_videoformat.vactive == 1856) && (vip_cfg.out_videoformat.width == 1856)
       && (vip_cfg.in_videoformat.hactive == 1392) && (vip_cfg.out_videoformat.height == 1392)) || //1856x1392
       ((vip_cfg.in_videoformat.vactive == 1792) && (vip_cfg.out_videoformat.width == 1792)
       && (vip_cfg.in_videoformat.hactive == 1344) && (vip_cfg.out_videoformat.height == 1344)) || //1792x1344
       ((vip_cfg.in_videoformat.vactive == 2048) && (vip_cfg.out_videoformat.width == 2048)
       && (vip_cfg.in_videoformat.hactive == 1152) && (vip_cfg.out_videoformat.height == 1152)) || //2048x1152
       ((vip_cfg.in_videoformat.vactive == 1920) && (vip_cfg.out_videoformat.width == 1920)
       && (vip_cfg.in_videoformat.hactive == 1200) && (vip_cfg.out_videoformat.height == 1200)) || //1920x1200
       ((vip_cfg.in_videoformat.vactive == 1920) && (vip_cfg.out_videoformat.width == 1920)
       && (vip_cfg.in_videoformat.hactive == 1080) && (vip_cfg.out_videoformat.height == 1080)
       && (vip_cfg.in_videoformat.fps == 120)))
    {
		mesg("%s bypass = 1.. in %dx%d out %dx%d\n",__func__,vip_cfg.in_videoformat.vactive, vip_cfg.in_videoformat.hactive,vip_cfg.out_videoformat.width,vip_cfg.out_videoformat.height);
        //aver_xilinx_video_bypass(board_v4l2_cxt->aver_xilinx_handle,1); //enable video bypass
        vip_cfg.video_bypass = 1;
	}
	else
	{
		mesg("%s bypass = 0.. in %dx%d out %dx%d\n",__func__,vip_cfg.in_videoformat.vactive,vip_cfg.in_videoformat.hactive,vip_cfg.out_videoformat.width,vip_cfg.out_videoformat.height);
		//aver_xilinx_video_bypass(board_v4l2_cxt->aver_xilinx_handle,0); //disable video bypass
		vip_cfg.video_bypass = 0;
	}
	
    switch(pixfmt->pixfmt_out)
    {
        case AVER_XILINX_FMT_YUYV:
        case AVER_XILINX_FMT_UYVY:
        case AVER_XILINX_FMT_YVYU:
        case AVER_XILINX_FMT_VYUY:
            vip_cfg.pixel_format=AVER_XILINX_FMT_YUYV;
            mesg("%s...vip_cfg.pixel_format_output=AVER_XILINX_FMT_YUYV\n",__func__);
            break;
        case AVER_XILINX_FMT_RGBP: //RGB565
        case AVER_XILINX_FMT_RGBR: //RGB565X
        case AVER_XILINX_FMT_RGBO:
        case AVER_XILINX_FMT_RGBQ:
        case AVER_XILINX_FMT_RGB3: //RGB24 4:4:4
        case AVER_XILINX_FMT_BGR3: 
        case AVER_XILINX_FMT_RGB4:
        case AVER_XILINX_FMT_BGR4:
            vip_cfg.pixel_format=AVER_XILINX_FMT_RGB3;
            mesg("%s...vip_cfg.pixel_format=AVER_XILINX_FMT_RGB3\n",__func__);
            break;
        default:
            vip_cfg.pixel_format=AVER_XILINX_CS_YUV422;
            break;
    }       
    aver_xilinx_config_video_process(board_v4l2_cxt->aver_xilinx_handle,&vip_cfg);
    
    //sys_msleep(200);
    aver_xilinx_enable_video_streaming(board_v4l2_cxt->aver_xilinx_handle,TRUE);
   
    //ite6805_set_freerun_screen(ite6805_handle,FALSE);    
}

static void gc573_stream_off(framegrabber_handle_t handle)
{
	board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
	//handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];
	
	//debug_msg(">>>>>>>>>>>%s...\n",__func__);
	//ite6805_set_freerun_screen(ite6805_handle,TRUE);
	
    aver_xilinx_enable_video_streaming(board_v4l2_cxt->aver_xilinx_handle,FALSE);
}

static int gc573_flash_dump(framegrabber_handle_t handle,int start_block, int blocks, U8_T *flash_dump) //
{
//	board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
	//char version[10];
	int ret=0;
		
    //ret = aver_xilinx_flash_dump(board_v4l2_cxt->aver_xilinx_handle,start_block,blocks,flash_dump);
    
    return ret;
}

static int gc573_flash_update(framegrabber_handle_t handle,int start_block, int blocks, U8_T *flash_update)
{
//	board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
	//char version[10];
	int ret=0;
	
    //ret = aver_xilinx_flash_update(board_v4l2_cxt->aver_xilinx_handle,start_block,blocks,flash_update);
    
    return ret;
}

static void gc573_brightness_read(framegrabber_handle_t handle,int *brightness)
{
	board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);//
    //handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];
   
	//ite6805_get_brightness(ite6805_handle,&board_v4l2_cxt->cur_bchs_value); 
    //board_v4l2_cxt->cur_bchs_value = aver_xilinx_get_bright(board_v4l2_cxt->aver_xilinx_handle);
	
	//framegrabber_set_data(handle,board_v4l2_cxt);
	*brightness = board_v4l2_cxt->cur_bchs_value;
	//handle->fg_bchs_value = *brightness;
	debug_msg("%s get brightness=%x\n",__func__,board_v4l2_cxt->cur_bchs_value);
}
static void gc573_contrast_read(framegrabber_handle_t handle,int *contrast)
{
	board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
    //handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];
   
	//ite6805_get_contrast(ite6805_handle,&board_v4l2_cxt->cur_bchs_value); 
    //board_v4l2_cxt->cur_bchs_value = aver_xilinx_get_contrast(board_v4l2_cxt->aver_xilinx_handle);
	
	//framegrabber_set_data(handle,board_v4l2_cxt);
	*contrast = board_v4l2_cxt->cur_bchs_value;
	//handle->fg_bchs_value = *contrast;
	debug_msg("%s get contrast=%x\n",__func__,board_v4l2_cxt->cur_bchs_value);
}

static void gc573_hue_read(framegrabber_handle_t handle,int *hue)
{
	board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
    //handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];
   
	//ite6805_get_hue(ite6805_handle,&board_v4l2_cxt->cur_bchs_value); 
    //board_v4l2_cxt->cur_bchs_value = aver_xilinx_get_hue(board_v4l2_cxt->aver_xilinx_handle);	

	//framegrabber_set_data(handle,board_v4l2_cxt);
	*hue = board_v4l2_cxt->cur_bchs_value;
	//handle->fg_bchs_value = *hue;
	debug_msg("%s get hue=%x\n",__func__,board_v4l2_cxt->cur_bchs_value);
}

static void gc573_saturation_read(framegrabber_handle_t handle,int *saturation)
{
	board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
    //handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];
   
	//ite6805_get_saturation(ite6805_handle,&board_v4l2_cxt->cur_bchs_value); 
    //board_v4l2_cxt->cur_bchs_value = aver_xilinx_get_saturation(board_v4l2_cxt->aver_xilinx_handle);
	
	//framegrabber_set_data(handle,board_v4l2_cxt);
	*saturation = board_v4l2_cxt->cur_bchs_value;
	//handle->fg_bchs_value = *saturation;
	debug_msg("%s get saturation=%x\n",__func__,board_v4l2_cxt->cur_bchs_value);
}

static void gc573_bchs_write(framegrabber_handle_t handle)
{
//	board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
	//handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];
   
	int bchs_value = handle->fg_bchs_value;
	int bchs_select = handle->fg_bchs_selection;	
		
	
	//debug_msg("%s...bchs_select=%d\n",__func__,bchs_select);
		
	{
		//framegrabber_s_input_bchs(board_v4l2_cxt->fg_handle,board_v4l2_cxt->cur_bchs_value,board_v4l2_cxt->cur_bchs_selection);
		//ite6805_set_bchs(ite6805_handle,&bchs_value,&bchs_select); 
        //aver_xilinx_set_bchs(board_v4l2_cxt->aver_xilinx_handle,bchs_value,bchs_select); 
		if (bchs_select ==0) debug_msg("set brightness=%d\n",bchs_value);
		if (bchs_select ==1) debug_msg("set contrast=%d\n",bchs_value);
		if (bchs_select ==2) debug_msg("set hue=%d\n",bchs_value);
		if (bchs_select ==3) debug_msg("set saturation=%d\n",bchs_value);
	}
}

static void gc573_hdcp_state_read(framegrabber_handle_t handle,int *hdcp_state)
{
	board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);

    aver_xilinx_get_hdcp_state(board_v4l2_cxt->aver_xilinx_handle,hdcp_state);
}

static void gc573_hdcp_state_set(framegrabber_handle_t handle,int hdcp_state)
{
	board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);

    aver_xilinx_set_hdcp_state(board_v4l2_cxt->aver_xilinx_handle, hdcp_state);
}

static int gc573_i2c_read(framegrabber_handle_t handle, unsigned char channel, unsigned int slave, unsigned int sub, unsigned char sublen, unsigned char *data, unsigned int datalen, unsigned int is_10bit)
{
    board_v4l2_context_t *board_v4l2_cxt = framegrabber_get_data(handle);
    i2c_model_handle_t i2c_mgr = board_v4l2_cxt->i2c_model_handle;
    cxt_mgr_handle_t cxt_mgr = cxt_mgr=get_cxt_manager_from_context(i2c_mgr);

    return board_i2c_read(cxt_mgr, channel, slave, sub, data, datalen);
}

static int gc573_i2c_write(framegrabber_handle_t handle, unsigned char channel, unsigned int slave, unsigned int sub, unsigned char sublen, unsigned char *data, unsigned int datalen, unsigned int is_10bit)
{
    board_v4l2_context_t *board_v4l2_cxt = framegrabber_get_data(handle);
    i2c_model_handle_t i2c_mgr = board_v4l2_cxt->i2c_model_handle;
    cxt_mgr_handle_t cxt_mgr = cxt_mgr=get_cxt_manager_from_context(i2c_mgr);

    return board_i2c_write(cxt_mgr, channel, slave, sub, data, datalen);
}

static int gc573_reg_read(framegrabber_handle_t handle, unsigned int offset, unsigned char n_bytes, unsigned int *data)
{
    board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
    int ret = 0;

    ret = aver_xilinx_read_register(board_v4l2_cxt->aver_xilinx_handle, offset, n_bytes, data);

    return ret;
}

static int gc573_reg_write(framegrabber_handle_t handle, unsigned int offset, unsigned char n_bytes, unsigned int data)
{
    board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
    int ret = 0;

    mesg("%s +", __func__);
#if 1
    if (offset == 0 && data == 0)
    {
        //stop task
        board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
        handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];

        ite6805_power_off(ite6805_handle);

        return 0;
    }
    else if (offset == 0 && data == 1)
    {
        //start task
        board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
        handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];

        ite6805_power_on(ite6805_handle);

        return 0;
    }
#endif

    ret = aver_xilinx_write_register(board_v4l2_cxt->aver_xilinx_handle, offset, n_bytes, data);

    return ret;
}

static void gc573_video_buffer_done(void *data)
{
    board_v4l2_context_t *board_v4l2_cxt=data;
    //mesg("%s board_v4l2_cxt %p\n",__func__,board_v4l2_cxt);
    if(board_v4l2_cxt)
    {
        v4l2_model_buffer_done(board_v4l2_cxt->v4l2_handle);
    }
}

static void gc573_v4l2_buffer_prepare(v4l2_model_callback_parameter_t *cb_info)
{
    board_v4l2_context_t *board_v4l2_cxt=cb_info->asso_data;
    v4l2_model_buffer_info_t *buffer_info=cb_info->u.buffer_prepare_info.buffer_info;
    //mesg("%s %p buffer_info %p\n",__func__,board_v4l2_cxt,buffer_info);
    if(buffer_info)
    {
        int i;
        v4l2_model_buf_desc_t *desc;
        unsigned framebufsize,remain;
        int width,height;
        unsigned bytesperline;
        
        framegrabber_g_out_framesize(board_v4l2_cxt->fg_handle,&width,&height);
        bytesperline=framegrabber_g_out_bytesperline(board_v4l2_cxt->fg_handle);
        framebufsize=bytesperline*height; 
        //debug_msg("%s %dx%d framesize %u\n",__func__,width,height,framebufsize);
      //  mesg("buf type %d count %d\n",buffer_info->buf_type,buffer_info->buf_count[0]);
        for(i=0,desc=buffer_info->buf_info[0],remain=framebufsize;i<buffer_info->buf_count[0];i++)
        {
            if(remain >= desc[i].size)
            {
                aver_xilinx_add_to_cur_desclist(board_v4l2_cxt->aver_xilinx_handle,desc[i].addr,desc[i].size);
                remain -= desc[i].size;
            }else
            {
                aver_xilinx_add_to_cur_desclist(board_v4l2_cxt->aver_xilinx_handle,desc[i].addr,remain);
                remain =0;
                break;
            }
                
            //mesg("addr %08x size %x\n",desc[i].addr,desc[i].size);
        }

        aver_xilinx_active_current_desclist(board_v4l2_cxt->aver_xilinx_handle,gc573_video_buffer_done,board_v4l2_cxt);
    }  
}

static void gc573_v4l2_buffer_init(v4l2_model_callback_parameter_t *cb_info)
{
//    board_v4l2_context_t *board_v4l2_cxt=cb_info->asso_data;
    v4l2_model_buffer_info_t *buffer_info=cb_info->u.buffer_prepare_info.buffer_info;
    //mesg("%s %p buffer_info %p\n",__func__,board_v4l2_cxt,buffer_info);
    if(buffer_info)
    {
        int i;
        v4l2_model_buf_desc_t *desc;
        //mesg("buf type %d count %d\n",buffer_info->buf_type,buffer_info->buf_count[0]);
        for(i=0,desc=buffer_info->buf_info[0];i<buffer_info->buf_count[0];i++)
        {
            //mesg("addr %08x size %x\n",desc[i].addr,desc[i].size);
        } 
    }  
}
/*
static void gc573_v4l2_hardware_init(framegrabber_handle_t handle)
{
    board_v4l2_context_t *board_v4l2_cxt=framegrabber_get_data(handle);
    handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];

    iTE6805_Hardware_Init(ite6805_handle);
}
*/
static void check_signal_stable_task(void *data)
{
    board_v4l2_context_t *board_v4l2_cxt=data;
    //ite6805_frameinfo_t *fe_frameinfo=&board_v4l2_cxt->cur_fe_frameinfo; 
    //handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0]; //check 20170511
    int width,height;
    int dual_pixel;
    int dual_pixel_set;
    framegrabber_framemode_e framemode;
    aver_xilinx_frame_info_t detected_frameinfo;
    ite6805_frameinfo_t *fe_frameinfo=&board_v4l2_cxt->cur_fe_frameinfo; 
    BOOL_T is_interlace;
    
    debug_msg("%s\n",__func__);
    
    //ite6805_get_frameinfo(ite6805_handle,fe_frameinfo);
    //debug_msg("%s ite6805 detected size %dx%d;framerate %d \n",__func__,fe_frameinfo->width,fe_frameinfo->height,fe_frameinfo->framerate);
        
    aver_xilinx_get_frameinfo(board_v4l2_cxt->aver_xilinx_handle,&detected_frameinfo,fe_frameinfo->pixel_clock/*0*/); 
    framemode=framegrabber_g_input_framemode(board_v4l2_cxt->fg_handle);
    is_interlace=(framemode==FRAMEGRABBER_FRAMEMODE_INTERLACED) ? TRUE : FALSE;
    framegrabber_g_input_framesize(board_v4l2_cxt->fg_handle,&width,&height);
    dual_pixel = framegrabber_g_input_dualmode(board_v4l2_cxt->fg_handle);
    
    if (dual_pixel ==1)
        dual_pixel_set = detected_frameinfo.width*2;
    else
        dual_pixel_set = detected_frameinfo.width; 
    
    debug_msg("%s framegrabber size %dx%d\n",__func__,width,height);
    debug_msg("%s fpga detected size %dx%d\n",__func__,detected_frameinfo.width,detected_frameinfo.height);
    if((((detected_frameinfo.width<320 || detected_frameinfo.height<240)) || (width !=/*detected_frameinfo.width*/dual_pixel_set)|| (height !=detected_frameinfo.height)) && (cnt_retry<3))
    {
		cnt_retry++;
		//debug_msg("%s retry...in %dx%d  get %dx%d\n",__func__,width,height,dual_pixel_set,detected_frameinfo.height); //test
        task_model_run_task_after(board_v4l2_cxt->task_model_handle,board_v4l2_cxt->check_signal_task_handle,100000);
        return;
    }
    
    //debug_msg("%s fpga detected size %dx%d\n",__func__,detected_frameinfo.width,detected_frameinfo.height);
   
    if (fe_frameinfo->is_interlace)
    {
		detected_frameinfo.width = fe_frameinfo->width;
		detected_frameinfo.height = fe_frameinfo->height/2;
	}
	else
	{
		detected_frameinfo.width = fe_frameinfo->width;
		detected_frameinfo.height = fe_frameinfo->height;
	}
      
    
    if(width==detected_frameinfo.width && height==detected_frameinfo.height && (is_interlace==detected_frameinfo.is_interlace))
    {
		debug_msg("%s fix input %dx%d to fpga %dx%d\n",__func__,width,height,detected_frameinfo.width,detected_frameinfo.height);
        framegrabber_mask_s_status(board_v4l2_cxt->fg_handle,FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT,FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT);
        cnt_retry=0;
    }else
    {
        debug_msg("%s adjust input %dx%d to fpga %dx%d\n",__func__,width,height,detected_frameinfo.width,detected_frameinfo.height);
      
        width=detected_frameinfo.width;
        height=detected_frameinfo.height;
        
        framegrabber_s_input_framesize(board_v4l2_cxt->fg_handle,width,height);
        if(is_interlace!=detected_frameinfo.is_interlace)
        {
            framemode=(detected_frameinfo.is_interlace) ? FRAMEGRABBER_FRAMEMODE_INTERLACED:FRAMEGRABBER_FRAMEMODE_PROGRESS;
            framegrabber_s_input_framemode(board_v4l2_cxt->fg_handle,framemode);
        }
        task_model_run_task_after(board_v4l2_cxt->task_model_handle,board_v4l2_cxt->check_signal_task_handle,50000); // run again to comfirm
    }   
}

static void gc573_ite6805_event(void *cxt,ite6805_event_e event)
{
    board_v4l2_context_t *board_v4l2_cxt=cxt;
    ite6805_frameinfo_t *fe_frameinfo=&board_v4l2_cxt->cur_fe_frameinfo; 
    enum ite6805_audio_sample *fe_audioinfo=&board_v4l2_cxt->cur_fe_audioinfo; 
    handle_t ite6805_handle=board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];
    ite6805_out_format_e out_format;
    unsigned int hdcp_flag;
    
    debug_msg("%s event %d\n",__func__,event);
    
    switch(event)
    {
    case ITE6805_LOCK:
    {
        //debug_msg("ITE6805_LOCK\n");
        //aver_xilinx_get_frameinfo(aver_xilinx_handle,&detected_frameinfo,fe_frameinfo->pixel_clock); 
        
        {
            //aver_xilinx_color_adjust_control(board_v4l2_cxt->aver_xilinx_handle);
            ite6805_get_frameinfo(ite6805_handle,fe_frameinfo);
            
//            aver_xilinx_hdmi_hotplug(board_v4l2_cxt->aver_xilinx_handle);
        
            mesg("%s locked fe %dx%d%c\n",__func__,fe_frameinfo->width,fe_frameinfo->height,(fe_frameinfo->is_interlace)?'i':'p'/*,fe_frameinfo->framerate*/);
            //debug_msg("pixclock %d colorspace %d\n",fe_frameinfo->pixel_clock,fe_frameinfo->colorspace);
   
//work around for ITE6805 detect issue      
            if ((fe_frameinfo->is_interlace==0) && ((fe_frameinfo->height == 240) || (fe_frameinfo->height == 288)))
            {
                //debug_msg("ITE6805 height adapter\n");
                fe_frameinfo->is_interlace =1;
                fe_frameinfo->height += fe_frameinfo->height;	
                fe_frameinfo->framerate /= 2;
            } 
            if ((fe_frameinfo->is_interlace==1) && ((fe_frameinfo->height == 480) || (fe_frameinfo->height == 576)))
            {
                //debug_msg("ITE6805 framerate adapter\n");	
                fe_frameinfo->framerate /= 2;
            } 
            ite6805_get_hdcp_level(ite6805_handle, &hdcp_flag);
            *fe_audioinfo = aver_xilinx_get_audioinfo(board_v4l2_cxt->aver_xilinx_handle);		
            framegrabber_s_input_audioinfo(board_v4l2_cxt->fg_handle,*fe_audioinfo);
			 
            framegrabber_s_input_framerate(board_v4l2_cxt->fg_handle,fe_frameinfo->framerate,fe_frameinfo->denominator);
            
            framegrabber_s_input_interlace(board_v4l2_cxt->fg_handle,fe_frameinfo->is_interlace);
            
            framegrabber_s_input_framesize(board_v4l2_cxt->fg_handle,fe_frameinfo->width,fe_frameinfo->height); //tt 0602
            
            framegrabber_s_input_dualmode(board_v4l2_cxt->fg_handle,fe_frameinfo->dual_pixel); //tt 1003
            
            framegrabber_s_hdcp_flag(board_v4l2_cxt->fg_handle,hdcp_flag);
            
            //aver_xilinx_dual_pixel(board_v4l2_cxt->aver_xilinx_handle,fe_frameinfo->dual_pixel);
     
            if((fe_frameinfo->pixel_clock>170000000/*150000*/) || (fe_frameinfo->dual_pixel_like ==1))//rr1012
            {
                //ite6805_out_format_e out_format;
                if(fe_frameinfo->packet_colorspace==CS_YUV)
                    out_format=ITE6805_OUT_FORMAT_SDR_422_2X24_INTERLEAVE_MODE0;//ITE6805_OUT_FORMAT_SDR_422_24_MODE4;//ADV7619_OUT_FORMAT_SDR_422_2X24_INTERLEAVE_MODE0; //1003
                else
                    out_format=ITE6805_OUT_FORMAT_SDR_444_2X24_INTERLEAVE_MODE0;
                ite6805_set_out_format(ite6805_handle,out_format);
                //ite6805_set_out_format(board_v4l2_cxt->i2c_chip_handle[CX511H_I2C_CHIP_ITE6805_1],out_format);
            }
            else
            {
                out_format=ITE6805_OUT_FORMAT_SDR_ITU656_24_MODE0;
                ite6805_set_out_format(ite6805_handle,out_format);
                //ite6805_set_out_format(board_v4l2_cxt->i2c_chip_handle[CX511H_I2C_CHIP_ITE6805_1],ITE6805_OUT_FORMAT_SDR_ITU656_24_MODE0);
            }
            //debug_msg("=========== ite6805_set_out_format=%02x\n",out_format);
        }
        //sys_msleep(100);
        //debug_msg("pixelclock %d detected %dx%d%c\n",fe_frameinfo->pixel_clock,detected_frameinfo.width,detected_frameinfo.height,(detected_frameinfo.is_interlace) ?'i':'p');
        framegrabber_s_input_status(board_v4l2_cxt->fg_handle,FRAMEGRABBER_INPUT_STATUS_OK);
        framegrabber_mask_s_status(board_v4l2_cxt->fg_handle,FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT,FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT);

        //task_model_run_task_after(board_v4l2_cxt->task_model_handle,board_v4l2_cxt->check_signal_task_handle,100000);
    }
        break;
    case ITE6805_UNLOCK:
        //debug_msg("ITE6805_UNLOCK\n");
        framegrabber_s_hdcp_flag(board_v4l2_cxt->fg_handle,0);
        framegrabber_s_input_framesize(board_v4l2_cxt->fg_handle,0,0); //tt 0615
        framegrabber_mask_s_status(board_v4l2_cxt->fg_handle,FRAMEGRABBER_STATUS_SIGNAL_LOCKED_BIT,0);
        framegrabber_s_input_status(board_v4l2_cxt->fg_handle,FRAMEGRABBER_INPUT_STATUS_NO_SIGNAL);
        break;
        
    case ITE6805_HDCP:
        //debug_msg("ITE6805_HDCP\n");
        ite6805_get_hdcp_level(ite6805_handle, &hdcp_flag);
		framegrabber_s_hdcp_flag(board_v4l2_cxt->fg_handle,hdcp_flag);
		break;    
    case ITE6805_HPD_LOW:
        aver_xilinx_set_gpio_output(board_v4l2_cxt->aver_xilinx_handle, 2, 0);
        break;
    case ITE6805_HPD_HIGH:
        aver_xilinx_set_gpio_output(board_v4l2_cxt->aver_xilinx_handle, 2, 1);
        break;
    } 
   
}

void board_v4l2_init(cxt_mgr_handle_t cxt_mgr, int board_id)
{
    framegrabber_handle_t framegrabber_handle;
    board_v4l2_context_t *board_v4l2_cxt=NULL;
    v4l2_model_handle_t v4l2_handle;
    task_model_handle_t task_model_handle=NULL;
    i2c_model_handle_t  i2c_mgr=NULL;
    handle_t aver_xilinx_handle=NULL;
    task_handle_t task_handle=NULL;
    int i;
    
    enum
    {
        BOARD_V4L2_OK = 0,
        BOARD_V4L2_ERROR_CXT_MGR,
        BOARD_V4L2_ERROR_FRAMEGRABBER_INIT,
        BOARD_V4L2_ERROR_V4L2_MODEL_INIT,
        BOARD_V4L2_ERROR_ALLOC_CXT,
        BOARD_V4L2_ERROR_GET_MAIN_CHIP_HANDLE,
        BOARD_V4L2_ERROR_GET_I2C_MGR,
        BOARD_V4L2_ERROR_GET_TASK_MGR,
        BOARD_V4L2_ERROR_ALLOC_TASK,
    } err = BOARD_V4L2_OK;

    do
    {
        if (!cxt_mgr)
        {
            err = BOARD_V4L2_ERROR_CXT_MGR;
            break;
        }

        framegrabber_handle = framegrabber_init(cxt_mgr, &gc573_property, &gc573_ops);
        if (framegrabber_handle == NULL)
        {
            err = BOARD_V4L2_ERROR_FRAMEGRABBER_INIT;
            break;
        }
        board_v4l2_cxt = cxt_manager_add_cxt(cxt_mgr, BOARD_V4L2_CXT_ID, board_v4l2_alloc, board_v4l2_release);
        if (!board_v4l2_cxt)
        {
            err = BOARD_V4L2_ERROR_ALLOC_CXT;
            break;
        }
        board_v4l2_cxt->fg_handle = framegrabber_handle;
        framegrabber_set_data(framegrabber_handle,board_v4l2_cxt);
        
        v4l2_handle = v4l2_model_init(cxt_mgr, &device_info,framegrabber_handle);
        if(v4l2_handle==NULL)
	    {
            err=BOARD_V4L2_ERROR_V4L2_MODEL_INIT;
		    break;
	    }
	    board_v4l2_cxt->v4l2_handle=v4l2_handle;
        aver_xilinx_handle=cxt_manager_get_context(cxt_mgr,AVER_XILINX_CXT_ID,0);
	    if(aver_xilinx_handle==NULL)
	    {
            err=BOARD_V4L2_ERROR_GET_MAIN_CHIP_HANDLE;
            break;
	    }
            
        board_v4l2_cxt->aver_xilinx_handle=aver_xilinx_handle;
        i2c_mgr=cxt_manager_get_context(cxt_mgr,I2C_CXT_ID,0);
	    if(i2c_mgr==NULL)
	    {
            err=BOARD_V4L2_ERROR_GET_I2C_MGR;
            break;
	    }
	    board_v4l2_cxt->i2c_model_handle = i2c_mgr;
        for(i=0;i<GC573_I2C_CHIP_COUNT;i++)
	    {
            board_v4l2_cxt->i2c_chip_handle[i]=i2c_model_get_nth_driver_handle(i2c_mgr,gc573_chip_desc[i].name,gc573_chip_desc[i].index);
            debug_msg("board_v4l2 i2c_chip_handle[%d] %p\n",i,board_v4l2_cxt->i2c_chip_handle[i]);
	    }
        task_model_handle=cxt_manager_get_context(cxt_mgr,TASK_MODEL_CXT_ID,0);
        if(task_model_handle==NULL)
        {
            err=BOARD_V4L2_ERROR_GET_TASK_MGR;
            break;
        }
        board_v4l2_cxt->task_model_handle=task_model_handle;
        cxt_manager_ref_context(task_model_handle);
        task_handle=task_model_create_task(task_model_handle,check_signal_stable_task,board_v4l2_cxt,"check_signal_task");
        if(task_handle==NULL)
        {
            err=BOARD_V4L2_ERROR_ALLOC_TASK;
            break;
        }
        
        board_v4l2_cxt->check_signal_task_handle=task_handle;
        
        aver_xilinx_hdmi_hotplug(board_v4l2_cxt->aver_xilinx_handle);
        
        v4l2_model_register_callback(v4l2_handle,V4L2_MODEL_CALLBACK_STREAMON ,&gc573_v4l2_stream_on, board_v4l2_cxt);
        v4l2_model_register_callback(v4l2_handle,V4L2_MODEL_CALLBACK_STREAMOFF,&gc573_v4l2_stream_off, board_v4l2_cxt);
        v4l2_model_register_callback(v4l2_handle,V4L2_MODEL_CALLBACK_BUFFER_PREPARE,&gc573_v4l2_buffer_prepare, board_v4l2_cxt);
        v4l2_model_register_callback(v4l2_handle,V4L2_MODEL_CALLBACK_BUFFER_INIT,&gc573_v4l2_buffer_init, board_v4l2_cxt);
      
        ite6805_register_callback(board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0],gc573_ite6805_event,board_v4l2_cxt);
        framegrabber_start(framegrabber_handle);
        cxt_manager_ref_context(aver_xilinx_handle);
        
        board_v4l2_cxt->board_id = board_id;
        //ite6805_get_board_id(board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0],&board_v4l2_cxt->board_id);
        
    }
    while (0);
    if (err != BOARD_V4L2_OK)
    {
        switch (err)
        {
        case BOARD_V4L2_ERROR_ALLOC_TASK:
            cxt_manager_unref_context(task_model_handle);
        case BOARD_V4L2_ERROR_GET_TASK_MGR:
        case BOARD_V4L2_ERROR_GET_I2C_MGR:
        case BOARD_V4L2_ERROR_V4L2_MODEL_INIT:
        case BOARD_V4L2_ERROR_ALLOC_CXT:
        case BOARD_V4L2_ERROR_FRAMEGRABBER_INIT:
        case BOARD_V4L2_ERROR_CXT_MGR:
        default:
            break;
        }
        debug_msg("%s err %d\n", __func__, err);
    }
}

void board_v4l2_suspend(cxt_mgr_handle_t cxt_mgr)
{
    board_v4l2_context_t *board_v4l2_cxt;
    handle_t ite6805_handle;

    board_v4l2_cxt = cxt_manager_get_context(cxt_mgr,BOARD_V4L2_CXT_ID,0);
    if (!board_v4l2_cxt)
    {
        debug_msg("Error: cannot get board_v4l2_cxt");
        return;
    }

    ite6805_handle = board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];
    if (!ite6805_handle)
    {
        debug_msg("Error: cannot get ite6805_handle");
        return;
    }

    ite6805_power_off(ite6805_handle);
}

void board_v4l2_resume(cxt_mgr_handle_t cxt_mgr)
{
    board_v4l2_context_t *board_v4l2_cxt;
    handle_t ite6805_handle;

    board_v4l2_cxt = cxt_manager_get_context(cxt_mgr,BOARD_V4L2_CXT_ID,0);
    if (!board_v4l2_cxt)
    {
        debug_msg("Error: cannot get board_v4l2_cxt");
        return;
    }

    ite6805_handle = board_v4l2_cxt->i2c_chip_handle[GC573_I2C_CHIP_ITE6805_0];
    if (!ite6805_handle)
    {
        debug_msg("Error: cannot get ite6805_handle");
        return;
    }

    ite6805_power_on(ite6805_handle);

    aver_xilinx_hdmi_hotplug(board_v4l2_cxt->aver_xilinx_handle);
}
