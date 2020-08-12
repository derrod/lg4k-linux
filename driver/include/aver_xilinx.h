/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * aver_xilinx.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef AVER750_H
#define AVER750_H

#include "ite6805.h"

#ifdef __cplusplus
extern "C" {
#endif
#define AVER_XILINX_CXT_ID fourcc_id('A','V','E','R')
#define AVER_XILINX_GPIODRV_NAME "AVER_XILINX_GPIO"    

#define NA 0xff

#define RGBF_to_BT709	0
#define RGBL_to_BT709	1
#define RGBF_to_BT601	2
#define RGBL_to_BT601	3
#define RGBF_to_BT2020	4	
#define RGBL_to_BT2020	5
#define RGBL_to_RGBF	6

#define BT709_to_RGBF	0
#define BT709_to_RGBL	1
#define BT601_to_RGBF	2
#define BT601_to_RGBL	3

#define BT601_to_BT709	4
#define BT709_to_BT601	5

#define BT601_to_BT2020	6	
#define BT709_to_BT2020	7
#define BT2020_to_RGBF	8
#define BT2020_to_RGBL	9
#define BT2020_to_BT709	0xa
#define BT2020_to_BT601	0xb
#define BT_BYPASS_MODE	0xc

#define XV_VSCALER_MAX_V_TAPS           (12)
#define XV_VSCALER_MAX_V_PHASES         (64)
#define XV_HSCALER_MAX_H_TAPS           (12)
#define XV_HSCALER_MAX_H_PHASES         (64)
#define XV_HSCALER_MAX_H_PHASES_EXPO    (6)
#define XV_HSCALER_MAX_LINE_WIDTH       (4096)	

#define STEP_PRECISION         (65536)  // 2^16

/* Mask definitions for Low and high 16 bits in a 32 bit number */
#define XVSC_MASK_LOW_16BITS       (0x0000FFFF)
#define XVSC_MASK_HIGH_16BITS      (0xFFFF0000)
#define XHSC_MASK_LOW_16BITS       (0x0000FFFF)
#define XHSC_MASK_HIGH_16BITS      (0xFFFF0000)


#define XV_HSCALER_CTRL_ADDR_AP_CTRL              0x0000
#define XV_HSCALER_CTRL_ADDR_GIE                  0x0004
#define XV_HSCALER_CTRL_ADDR_IER                  0x0008
#define XV_HSCALER_CTRL_ADDR_ISR                  0x000c
#define XV_HSCALER_CTRL_ADDR_HWREG_HEIGHT_DATA    0x0010
#define XV_HSCALER_CTRL_BITS_HWREG_HEIGHT_DATA    16
#define XV_HSCALER_CTRL_ADDR_HWREG_WIDTHIN_DATA   0x0018
#define XV_HSCALER_CTRL_BITS_HWREG_WIDTHIN_DATA   16
#define XV_HSCALER_CTRL_ADDR_HWREG_WIDTHOUT_DATA  0x0020
#define XV_HSCALER_CTRL_BITS_HWREG_WIDTHOUT_DATA  16
#define XV_HSCALER_CTRL_ADDR_HWREG_COLORMODE_DATA 0x0028
#define XV_HSCALER_CTRL_BITS_HWREG_COLORMODE_DATA 8
#define XV_HSCALER_CTRL_ADDR_HWREG_PIXELRATE_DATA 0x0030
#define XV_HSCALER_CTRL_BITS_HWREG_PIXELRATE_DATA 32
#define XV_HSCALER_CTRL_ADDR_HWREG_HFLTCOEFF_BASE 0x0800
#define XV_HSCALER_CTRL_ADDR_HWREG_HFLTCOEFF_HIGH 0x0bff
#define XV_HSCALER_CTRL_WIDTH_HWREG_HFLTCOEFF     16
#define XV_HSCALER_CTRL_DEPTH_HWREG_HFLTCOEFF     384
#define XV_HSCALER_CTRL_ADDR_HWREG_PHASESH_V_BASE 0x2000
#define XV_HSCALER_CTRL_ADDR_HWREG_PHASESH_V_HIGH 0x3fff
#define XV_HSCALER_CTRL_WIDTH_HWREG_PHASESH_V     18
#define XV_HSCALER_CTRL_DEPTH_HWREG_PHASESH_V     1920

#define XV_VSCALER_CTRL_ADDR_AP_CTRL              0x000
#define XV_VSCALER_CTRL_ADDR_GIE                  0x004
#define XV_VSCALER_CTRL_ADDR_IER                  0x008
#define XV_VSCALER_CTRL_ADDR_ISR                  0x00c
#define XV_VSCALER_CTRL_ADDR_HWREG_HEIGHTIN_DATA  0x00000010
#define XV_VSCALER_CTRL_BITS_HWREG_HEIGHTIN_DATA  16
#define XV_VSCALER_CTRL_ADDR_HWREG_WIDTH_DATA     0x00000018
#define XV_VSCALER_CTRL_BITS_HWREG_WIDTH_DATA     16
#define XV_VSCALER_CTRL_ADDR_HWREG_HEIGHTOUT_DATA 0x00000020
#define XV_VSCALER_CTRL_BITS_HWREG_HEIGHTOUT_DATA 16
#define XV_VSCALER_CTRL_ADDR_HWREG_LINERATE_DATA  0x00000028
#define XV_VSCALER_CTRL_BITS_HWREG_LINERATE_DATA  32
#define XV_VSCALER_CTRL_ADDR_HWREG_VFLTCOEFF_BASE 0x00000800
#define XV_VSCALER_CTRL_ADDR_HWREG_VFLTCOEFF_HIGH 0xbff
#define XV_VSCALER_CTRL_WIDTH_HWREG_VFLTCOEFF     16
#define XV_VSCALER_CTRL_DEPTH_HWREG_VFLTCOEFF     384

typedef struct {
	 U16_T DeviceId;			/**< Unique ID	of device */
	 U32_T BaseAddress;		/**< The base address of the core instance. */
	 U16_T PixPerClk; 		/**< Samples Per Clock supported by core instance */
	 U16_T NumVidComponents;	/**< Number of Video Components */
	 U16_T MaxWidth;			/**< Maximum columns supported by core instance */
	 U16_T MaxHeight; 		/**< Maximum rows supported by core instance */
	 U16_T MaxDataWidth;		/**< Maximum Data width of each channel */
	 U16_T PhaseShift;		/**< Max num of phases (2^PhaseShift) */
	 U16_T ScalerType;		/**< Scaling Algorithm Selected */
	 U16_T NumTaps;			/**< Number of taps */
 } XV_vscaler_Config;

typedef struct {
	 U16_T DeviceId;		   /**< Unique ID  of device */
	 U32_T BaseAddress;	   /**< The base address of the core instance. */
	 U16_T PixPerClk; 	   /**< Samples Per Clock supported by core instance */
	 U16_T NumVidComponents; /**< Number of Video Components */
	 U16_T MaxWidth;		   /**< Maximum columns supported by core instance */
	 U16_T MaxHeight; 	   /**< Maximum rows supported by core instance */
	 U16_T MaxDataWidth;	   /**< Maximum Data width of each channel */
	 U16_T PhaseShift;	   /**< Max num of phases (2^PhaseShift) */
	 U16_T ScalerType;	   /**< Scaling Algorithm Selected */
	 U16_T NumTaps;		   /**< Number of taps */
	 U16_T Is422Enabled;	   /**< Color format YUV422 supported by instance */
} XV_hscaler_Config;

 
 /**
 * Driver instance data. An instance must be allocated for each core in use.
 */
 typedef struct {
	 XV_vscaler_Config Config; /**< Hardware Configuration */
	 U32_T IsReady;			   /**< Device is initialized and ready */
 } XV_vscaler;

 typedef struct {
	 XV_hscaler_Config Config; /**< Hardware Configuration */
	 U32_T IsReady;			   /**< Device is initialized and ready */
 } XV_hscaler;
 
#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1

typedef enum {
	XVIDC_PPC_1 = 1,
	XVIDC_PPC_2 = 2,
	XVIDC_PPC_4 = 4,
	XVIDC_PPC_NUM_SUPPORTED = 3,
} XVidC_PixelsPerClock;

/**
 * Color space format.
 */
typedef enum {
	XVIDC_CSF_RGB = 0,
	XVIDC_CSF_YCRCB_444,
	XVIDC_CSF_YCRCB_422,
	XVIDC_CSF_YCRCB_420,
	XVIDC_CSF_YONLY,
	XVIDC_CSF_NUM_SUPPORTED,
	XVIDC_CSF_UNKNOWN
} XVidC_ColorFormat;


/**
 * This typedef eumerates the Scaler Type
 */
typedef enum
{
  XV_VSCALER_BILINEAR = 0,
  XV_VSCALER_BICUBIC,
  XV_VSCALER_POLYPHASE
}XV_VSCALER_TYPE;

typedef enum
{
  XV_HSCALER_BILINEAR = 0,
  XV_HSCALER_BICUBIC,
  XV_HSCALER_POLYPHASE
}XV_HSCALER_TYPE;

/**
 * This typedef enumerates the supported taps
 */
typedef enum
{
  XV_VSCALER_TAPS_6  = 6,
  XV_VSCALER_TAPS_8  = 8,
  XV_VSCALER_TAPS_10 = 10,
  XV_VSCALER_TAPS_12 = 12
}XV_VSCALER_TAPS;

typedef enum
{
  XV_HSCALER_TAPS_6  = 6,
  XV_HSCALER_TAPS_8  = 8,
  XV_HSCALER_TAPS_10 = 10,
  XV_HSCALER_TAPS_12 = 12
}XV_HSCALER_TAPS;

typedef struct
{
  XV_vscaler Vsc; /*<< Layer 1 instance */
  U8_T UseExtCoeff;
  short coeff[XV_VSCALER_MAX_V_PHASES][XV_VSCALER_MAX_V_TAPS];
}XV_Vscaler_l2;

typedef struct
{
  XV_hscaler Hsc; /*<< Layer 1 instance */
  U8_T UseExtCoeff;
  short coeff[XV_HSCALER_MAX_H_PHASES][XV_HSCALER_MAX_H_TAPS];
  U64_T phasesH[XV_HSCALER_MAX_LINE_WIDTH];
}XV_Hscaler_l2;

enum
{
    AVER_XILINX_MAX_VIDEO_DESC_LIST_COUNT=4,
    AVER_XILINX_USR_VIDEO_DESC_LIST_COUNT=8,
    AVER_XILINX_MAX_DESC_PER_DESC_LIST=2048,
    AVER_XILINX_DEFAULT_AUDIO_BUF_SIZE=11520,//11520, //7680
    AVER_XILINX_AUDIO_BUF_COUNT=2,
};


typedef struct
{
    U32_T phys_L;
    U32_T phys_H;
    U32_T length;
    U32_T ctrl;
}aver_xilinx_desc_t;

typedef enum
{
  AVER_XILINX_I2C_BUS_0,
  AVER_XILINX_I2C_BUS_1,
  //AVER_XILINX_I2C_BUS_2, //reserved two I2C
  AVER_XILINX_I2C_BUS_3,
  //AVER_XILINX_I2C_BUS_4,        
  AVER_XILINX_I2C_BUS_NUM,  
}aver_xilinx_i2c_bus_e;

typedef enum
{
    AVER_XILINX_GPIO_0,
    AVER_XILINX_GPIO_1,
    AVER_XILINX_GPIO_2,
    AVER_XILINX_GPIO_3,
    AVER_XILINX_GPIO_4,
    AVER_XILINX_GPIO_5,
    AVER_XILINX_GPIO_6,
    AVER_XILINX_GPIO_7,
    AVER_XILINX_GPIO_8,        
    AVER_XILINX_GPIO_NUM,            
}aver_xilinx_gpio_e;

typedef enum
{
    AVER_XILINX_GPIO_0_MASK=Bit(AVER_XILINX_GPIO_0),
    AVER_XILINX_GPIO_1_MASK=Bit(AVER_XILINX_GPIO_1),
    AVER_XILINX_GPIO_2_MASK=Bit(AVER_XILINX_GPIO_2),
    AVER_XILINX_GPIO_3_MASK=Bit(AVER_XILINX_GPIO_3),
    AVER_XILINX_GPIO_4_MASK=Bit(AVER_XILINX_GPIO_4),
    AVER_XILINX_GPIO_5_MASK=Bit(AVER_XILINX_GPIO_5),            
    AVER_XILINX_GPIO_6_MASK=Bit(AVER_XILINX_GPIO_6),
    AVER_XILINX_GPIO_7_MASK=Bit(AVER_XILINX_GPIO_7),
    AVER_XILINX_GPIO_8_MASK=Bit(AVER_XILINX_GPIO_8),
    AVER_XILINX_GPIO_MASK=((1<<8)-1),        
}aver_xilinx_gpio_mask_e;

typedef struct
{
    aver_xilinx_gpio_mask_e gpio_mask;
    aver_xilinx_gpio_mask_e gpio_value;
    int i2c_bus_speed[AVER_XILINX_I2C_BUS_NUM];
    U32_T  audio_buffer_size; 
}aver_xilinx_cfg_t;

typedef struct 
{
    int width;
    int height;
}framesize_t;

typedef struct
{
    int x;
    int y;
}pos_t;

typedef enum
{
    AVER_XILINX_CS_YUV422,
    AVER_XILINX_CS_YUV444,
    AVER_XILINX_CS_RGB_FULL,
    AVER_XILINX_CS_RGB_LIMITED,
    AVER_XILINX_CS_COUNT,        
}aver_xilinx_colorspace_e;

typedef enum
{
	AVER_XILINX_FMT_YUYV = 0,
	AVER_XILINX_FMT_UYVY,
	AVER_XILINX_FMT_YVYU,
	AVER_XILINX_FMT_VYUY,
	AVER_XILINX_FMT_RGBP,
	AVER_XILINX_FMT_RGBR,
	AVER_XILINX_FMT_RGBO,
	AVER_XILINX_FMT_RGBQ,
	AVER_XILINX_FMT_RGB3,
	AVER_XILINX_FMT_BGR3,
	AVER_XILINX_FMT_RGB4,
	AVER_XILINX_FMT_BGR4,	
}aver_xilinx_pixfmt_e;

typedef enum
{
	AVER_XILINX_AUDIO_32000 = 0,
	AVER_XILINX_AUDIO_44100,
	AVER_XILINX_AUDIO_48000,
	AVER_XILINX_AUDIO_88200,
	AVER_XILINX_AUDIO_96000,
	AVER_XILINX_AUDIO_176400,
	AVER_XILINX_AUDIO_192000,
}aver_xilinx_audio_e;

typedef enum
{
    SCALER_CFG_VALID_MASK=(0x1<<0),
    CLIP_CFG_VALID_MASK=(0x1<<1),
    FRAMERATE_CFG_VALID_MASK=(0x1<<2), 
    SCALER_CFG_SHRINK_MASK=(0x1<<3),      
    CFG_VALID_ALL_MASK=(0x1<<4)-1,
    
}aver_xilinx_video_process_valid_mask_t;

typedef struct _VIDEO_CLIP
{
    S32_T leftoffset;
    S32_T width;
    S32_T topoffset;
    S32_T height;
} video_clip, *pvideo_cilp;

typedef struct _VIDEO_IN_FMT
{   
    BOOL_T is_interlace;
    U32_T    fps;
    S32_T    hactive;
    S32_T    vactive;
    U32_T    colorspace;
} video_in_fmt, *pvideo_in_fmt;

// [Jeff_053_20140917] Removed FramePeriod. Added FrameRateTarget.
typedef struct _VIDEO_OUT_FMT
{    
	S32_T    width;
	S32_T    height;
    U32_T    colorspace;
	U32_T    compress;
	U32_T    framesize;
    video_clip    clip; 
    U32_T    fps;
	U8_T    hflip; 
	U8_T    vflip; 
} video_out_fmt, *pvideo_out_fmt;

typedef struct
{
    aver_xilinx_video_process_valid_mask_t valid_mask;
    video_out_fmt out_videoformat;
    video_in_fmt in_videoformat;
    framesize_t clip_size;
    pos_t clip_start;
    aver_xilinx_audio_e audio_rate;
    U32_T  in_workingmode;
    U32_T  in_hdrworkingmode;
    U32_T  in_ddrmode;
    U32_T  in_colorspacemode; // 0-yuv, 1-rgb limit, 2-rgb full
    U32_T  in_packetsamplingmode; // 0-rgb, 1-422, 2-444	
    U32_T  in_packetcsc_bt;
    U32_T  out_csfourcc;
    eCSC_FORMULA  currentCSC;
    aver_xilinx_pixfmt_e pixel_format;
    U32_T pixel_clock;
    int video_bypass;
    int dual_pixel;
    int packet_colorspace;
}aver_xilinx_video_process_cfg_t;

typedef struct
{
    aver_xilinx_audio_e audio_rate;
}aver_xilinx_audio_cfg_t;

typedef void (*aver_xilinx_audio_cb_t)(void *cxt,U8_T *buffer,U32_T size);

typedef struct
{
    aver_xilinx_audio_cb_t callback;
    void *asso_cxt;
}aver_xilinx_audio_cbinfo_t;

typedef struct
{
    U32_T width;
    U32_T height;
    BOOL_T is_interlace;
}aver_xilinx_frame_info_t;



typedef void (*aver_xilinx_callback_t)(void *);

//void aver_xilinx_reset_rx(handle_t aver_xilinx_handle);
//U32_T aver_xilinx_get_version(handle_t aver_xilinx_handle);
handle_t aver_xilinx_init(cxt_mgr_handle_t cxt_mgr,void* pci_handle,aver_xilinx_cfg_t *setup_info);
void aver_xilinx_init_registers(handle_t aver_xilinx_handle, aver_xilinx_cfg_t *setup_info);
void aver_xilinx_sha204_init(handle_t aver_xilinx_handle);
void aver_xilinx_get_hdcp_state(handle_t aver_xilinx_handle,int *hdcp_state);
void aver_xilinx_set_hdcp_state(handle_t aver_xilinx_handle,int hdcp_state);
int aver_xilinx_get_audioinfo(handle_t aver_xilinx_handle);

int aver_xilinx_get_bright(handle_t aver_xilinx_handle);
int aver_xilinx_get_contrast(handle_t aver_xilinx_handle);
int aver_xilinx_get_hue(handle_t aver_xilinx_handle);
int aver_xilinx_get_saturation(handle_t aver_xilinx_handle);
void aver_xilinx_set_bchs(handle_t aver_xilinx_handle, int val, int select);
void aver_xilinx_color_adjust_control(handle_t aver_xilinx_handle);

void aver_xilinx_get_i2c_bus_cfg(handle_t aver_xilinx_handle,aver_xilinx_i2c_bus_e bus,i2c_model_bus_cfg_t *i2c_bus_cfg);
void aver_xilinx_add_trace(handle_t aver_xilinx_handle,void *trace_handle);
void aver_xilinx_enable_video_streaming(handle_t aver_xilinx_handle,BOOL_T enable);
void aver_xilinx_reset(handle_t aver_xilinx_handle, U32_T module);
void aver_xilinx_set_audio_dma(handle_t aver_xilinx_handle,aver_xilinx_audio_cfg_t *cfg);
void aver_xilinx_start_audio_streaming(handle_t aver_xilinx_handle,aver_xilinx_audio_cbinfo_t *cb_info);
void aver_xilinx_stop_audio_streaming(handle_t aver_xilinx_handle);
void aver_xilinx_add_to_cur_desclist(handle_t aver_xilinx_handle,unsigned long phys_addr,unsigned size);
void aver_xilinx_active_current_desclist(handle_t aver_xilinx_handle,aver_xilinx_callback_t callback,void *cb_cxt);
void aver_xilinx_config_video_process(handle_t aver_xilinx_handle,aver_xilinx_video_process_cfg_t *vip_cfg);
void aver_xilinx_get_frameinfo(handle_t aver_xilinx_handle,aver_xilinx_frame_info_t *frameinfo,U32_T pixelclock); 
void aver_xilinx_hdmi_hotplug(handle_t aver_xilinx_handle);
int aver_xilinx_flash_dump(handle_t aver_xilinx_handle,int start_block, int blocks, U8_T *flash);
int aver_xilinx_flash_update(handle_t aver_xilinx_handle,int start_block, int blocks, U8_T *flash);
int aver_xilinx_read_register(handle_t aver_xilinx_handle, unsigned int offset, unsigned char n_bytes, unsigned int *data);
int aver_xilinx_write_register(handle_t aver_xilinx_handle, unsigned int offset, unsigned char n_bytes, unsigned int data);
int aver_xilinx_set_gpio_output(handle_t aver_xilinx_handle, int pin_num, int level);

//void aver_xilinx_dual_pixel(handle_t aver_xilinx_handle,int csc_dual_pixel);
//void aver_xilinx_video_bypass(handle_t aver_xilinx_handle,int video_bypass);

#ifdef __cplusplus
}
#endif

#endif /* AVER750_H */

