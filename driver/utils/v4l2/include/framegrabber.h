/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * framegrabber.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 
#ifndef MODULE_V4L2_INCLUDE_FRAMEGRABBER_H_
#define MODULE_V4L2_INCLUDE_FRAMEGRABBER_H_

#define FG_CXT_ID fourcc_id('F','R','G','A')

#define EnumtoBitMask(e) e##_BIT = 1 << e

typedef enum
{
	FRAMEGRABBER_INPUT_STATUS_OK=0,
	FRAMEGRABBER_INPUT_STATUS_NO_POWER,
	FRAMEGRABBER_INPUT_STATUS_NO_SIGNAL,
	FRAMEGRABBER_INPUT_STATUS_NO_SYNC,
}framegrabber_input_status_e;

//typedef enum
//{
//	EnumtoBitMask(FRAMEGRABBER_INPUT_STATUS_OK),
//	EnumtoBitMask(FRAMEGRABBER_INPUT_STATUS_NO_POWER),
//	EnumtoBitMask(FRAMEGRABBER_INPUT_STATUS_NO_SIGNAL),
//	EnumtoBitMask(FRAMEGRABBER_INPUT_STATUS_NO_SYNC),
//
//}framegrabber_input_status_bitmask_e;

typedef enum
{
	FRAMEGRABBER_STATUS_V4L_START_STREAMING=0,
	FRAMEGRABBER_STATUS_SIGNAL_LOCKED,

}framegrabber_status_e;

typedef enum
{
	EnumtoBitMask(FRAMEGRABBER_STATUS_V4L_START_STREAMING),
	EnumtoBitMask(FRAMEGRABBER_STATUS_SIGNAL_LOCKED),

}framegrabber_status_bitmask_e;

typedef enum
{
	FRAMEGRABBER_FRAMEMODE_PROGRESS=0,
	FRAMEGRABBER_FRAMEMODE_INTERLACED,
	FRAMEGRABBER_FRAMEMODE_COUNT,
}framegrabber_framemode_e;


typedef enum
{
	FRAMEGRABBER_OK=0,
	FRAMEGRABBER_ERROR_NO_V4LDEV,
	FRAMEGRABBER_ERROR_ALLOC_FAIL,
	FRAMEGRABBER_ERROR_INPUT_NULL,
	FRAMEGRABBER_ERROR_INPUT_OVERFLOW,
	FRAMEGRABBER_ERROR_INPUT_INVALID,
	FRAMEGRABBER_ERROR_NO_PIXFMT_SETUP,
	FRAMEGRABBER_ERROR_NO_SUPPORT_PIXFMT,
	FRAMEGRABBER_ERROR_INVALID_INPUT,
	FRAMEGRABBER_ERROR_INVALID_FRAMESIZE,
	FRAMEGRABBER_ERROR_NO_MATCH_SUBDEV,
	FRAMEGRABBER_ERROR_INVALID_SUBDEV_ARG,
	FRAMEGRABBER_ERROR_NOSUPPORT_SUBDEV_OP,
	FRAMEGRABBER_ERROR_NOIMPLEMENT_SUBDEV_OP,

}framegrabber_error_e;




typedef enum
{
	FRAMEGRABBER_PIXFMT_YUYV=0,
	FRAMEGRABBER_PIXFMT_UYVY,
#if 0
	FRAMEGRABBER_PIXFMT_YVYU,
	FRAMEGRABBER_PIXFMT_VYUY,
	FRAMEGRABBER_PIXFMT_RGB565,
	FRAMEGRABBER_PIXFMT_RGB565X,
	FRAMEGRABBER_PIXFMT_RGB555,
	FRAMEGRABBER_PIXFMT_RGB555X,
	FRAMEGRABBER_PIXFMT_RGB24,
#endif
	FRAMEGRABBER_PIXFMT_BGR24,
	FRAMEGRABBER_PIXFMT_RGB32,
#if 0
	FRAMEGRABBER_PIXFMT_BGR32,
#endif
	FRAMEGRABBER_PIXFMT_NV12,
    FRAMEGRABBER_PIXFMT_YV12,
    FRAMEGRABBER_PIXFMT_NV12M,
    FRAMEGRABBER_PIXFMT_YV12M,
	FRAMEGRABBER_PIXFMT_MAX,
}framegrabber_pixfmt_e;

typedef enum
{
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_YUYV),
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_UYVY),
#if 0
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_YVYU),
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_VYUY),
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_RGB565),
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_RGB565X),
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_RGB555),
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_RGB555X),
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_RGB24),
#endif
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_BGR24),
    EnumtoBitMask(FRAMEGRABBER_PIXFMT_RGB32),
#if 0
    EnumtoBitMask(FRAMEGRABBER_PIXFMT_BGR32),
#endif
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_NV12),
	EnumtoBitMask(FRAMEGRABBER_PIXFMT_YV12),
    EnumtoBitMask(FRAMEGRABBER_PIXFMT_NV12M),
    EnumtoBitMask(FRAMEGRABBER_PIXFMT_YV12M),
	FRAMEGRABBER_PIXFMT_BITMSK		= (1<<FRAMEGRABBER_PIXFMT_MAX)-1,
}framegrabber_pixfmt_bitmask_e;

typedef enum
{
    FRAMESIZE_640x480=0,
    FRAMESIZE_720x480,
    FRAMESIZE_720x576,
    FRAMESIZE_800x600,
    FRAMESIZE_1024x768,
    FRAMESIZE_1280x720,
    FRAMESIZE_1280x768,
    FRAMESIZE_1280x800,
    FRAMESIZE_1280x1024,
    FRAMESIZE_1360x768,
    FRAMESIZE_1440x900,
    FRAMESIZE_1680x1050,
    FRAMESIZE_1920x1080,
    FRAMESIZE_1920x1200,
    FRAMESIZE_2560x1080,
    FRAMESIZE_2560x1440,
    FRAMESIZE_3840x2160,
    FRAMESIZE_4096x2160,
    FRAMEGRABBER_SUPPORT_FRAMESIZE_NUM,
}framegrabber_framesize_e;

typedef enum
{
    EnumtoBitMask(FRAMESIZE_640x480),
    EnumtoBitMask(FRAMESIZE_720x480),
    EnumtoBitMask(FRAMESIZE_720x576),
    EnumtoBitMask(FRAMESIZE_800x600),
    EnumtoBitMask(FRAMESIZE_1024x768),
    EnumtoBitMask(FRAMESIZE_1280x720),
    EnumtoBitMask(FRAMESIZE_1280x768),
    EnumtoBitMask(FRAMESIZE_1280x800),
    EnumtoBitMask(FRAMESIZE_1280x1024),
    EnumtoBitMask(FRAMESIZE_1360x768),
    EnumtoBitMask(FRAMESIZE_1440x900),
    EnumtoBitMask(FRAMESIZE_1680x1050),
    EnumtoBitMask(FRAMESIZE_1920x1080),
    EnumtoBitMask(FRAMESIZE_1920x1200),
    EnumtoBitMask(FRAMESIZE_2560x1080),
    EnumtoBitMask(FRAMESIZE_2560x1440),
    EnumtoBitMask(FRAMESIZE_3840x2160),
    EnumtoBitMask(FRAMESIZE_4096x2160),
    FRAMEGRABBER_FRAMESIZE_BITMSK = (1<<FRAMEGRABBER_SUPPORT_FRAMESIZE_NUM)-1,
}framegrabber_framesize_mask_e;


typedef enum
{
    REFRESHRATE_15=0,
    REFRESHRATE_24,
    REFRESHRATE_25,
    REFRESHRATE_30,
    REFRESHRATE_50,
    REFRESHRATE_60,
    REFRESHRATE_100,
    REFRESHRATE_120,
    REFRESHRATE_144,
    REFRESHRATE_240,
    FRAMEGRABBER_SUPPORT_REFERSHRATE_NUM,
}framegrabber_refreshrate_e;

typedef enum
{
	EnumtoBitMask(REFRESHRATE_15),
	EnumtoBitMask(REFRESHRATE_24),
	EnumtoBitMask(REFRESHRATE_25),
	EnumtoBitMask(REFRESHRATE_30),
	EnumtoBitMask(REFRESHRATE_50),
	EnumtoBitMask(REFRESHRATE_60),
	EnumtoBitMask(REFRESHRATE_100),
	EnumtoBitMask(REFRESHRATE_120),
	EnumtoBitMask(REFRESHRATE_144),
	EnumtoBitMask(REFRESHRATE_240),
	FRAMEGRABBER_FRAMEINTERVAL_BITMSK = (1<<FRAMEGRABBER_SUPPORT_REFERSHRATE_NUM)-1,
}framegrabber_frameinterval_mask_e;

typedef enum
{
	YUYV = 0,
	UYVY,
	YVYU,
	VYUY,
    YVU420,
	YVU420M, //YV12
    NV12,
    NV12M,
	RGBP,
	RGBR,
	RGBO,
	RGBQ,
	RGB3,
	BGR3,
	BA24,
	AR24,
}framegrabber_pixfmt_enum_t;

typedef struct  {
	const char *name;
	U32_T   fourcc;          /* v4l2 format id */
	U8_T    depth;
    U8_T    num_planes;
	BOOL_T  is_yuv;
	framegrabber_pixfmt_enum_t pixfmt_out;
}framegrabber_pixfmt_t;

typedef struct
{
    int brightness;
    int contrast;
    int hue;
    int saturation;
    int bchs_flag;
    //adv7619_colorspace_e colorspace;
}framegrabber_bchs_t;

typedef enum
{
    FRAMEGRABBER_BCHS_BRIGHTNESS,
    FRAMEGRABBER_BCHS_CONTRAST,
    FRAMEGRABBER_BCHS_HUE,        
    FRAMEGRABBER_BCHS_SATURATION,
    FRAMEGRABBER_BCHS_DISABLE,
}framegrabber_bchs_e;

typedef struct framegrabber_interface_s
{
	void (*s_input)(struct framegrabber_interface_s *handle,int input);
	void (*notify)(struct framegrabber_interface_s *handle,char *fromname,int notification,void *arg);
	void (*s_out_pixfmt)(struct framegrabber_interface_s *handle,U32_T fourcc);
	void (*s_framesize)(struct framegrabber_interface_s *handle,int width,int height);
	void (*stream_on)(struct framegrabber_interface_s *handle);
	void (*stream_off)(struct framegrabber_interface_s *handle);
	//void (*bchs_get)(struct framegrabber_interface_s *handle);
	void (*brightness_get)(struct framegrabber_interface_s *handle,int *brightness);
	void (*contrast_get)(struct framegrabber_interface_s *handle,int *contrast);
	void (*hue_get)(struct framegrabber_interface_s *handle,int *hue);
	void (*saturation_get)(struct framegrabber_interface_s *handle,int *saturation);
	void (*bchs_set)(struct framegrabber_interface_s *handle);
	int (*flash_read)(struct framegrabber_interface_s *handle, int start_block, int blocks, U8_T *flash_dump);
	int (*flash_update)(struct framegrabber_interface_s *handle, int start_block, int blocks, U8_T *flash_update);
    void (*hdcp_state_get)(struct framegrabber_interface_s *handle, int *hdcp_state);
    void (*hdcp_state_set)(struct framegrabber_interface_s *handle, int hdcp_state);
	int (*hdcp_flag_read)(struct framegrabber_interface_s *handle, unsigned int *hdcp_flag);
    int (*hdcp_flag_write)(struct framegrabber_interface_s *handle, unsigned int hdcp_flag);
	int fg_bchs_value;
	int fg_bchs_selection;

	int (*i2c_read)(struct framegrabber_interface_s *handle, unsigned char channel, unsigned int slave, unsigned int sub, unsigned char sublen, unsigned char *data, unsigned int datalen, unsigned int is_10bit);
	int (*i2c_write)(struct framegrabber_interface_s *handle, unsigned char channel, unsigned int slave, unsigned int sub, unsigned char sublen, unsigned char *data, unsigned int datalen, unsigned int is_10bit);
	int (*reg_read)(struct framegrabber_interface_s *handle, unsigned int offset, unsigned char n_bytes, unsigned int *data);
	int (*reg_write)(struct framegrabber_interface_s *handle, unsigned int offset, unsigned char n_bytes, unsigned int data);

}framegrabber_interface_t,*framegrabber_handle_t;

typedef struct
{
	const char *name;
	framegrabber_frameinterval_mask_e support_framesize_info[FRAMEGRABBER_SUPPORT_FRAMESIZE_NUM];

}framegrabber_setup_input_info_t;

typedef struct frame_size
{
	int width;
	int height;
}framegrabber_frame_size_t;


typedef struct
{
	const char *name;
	framegrabber_setup_input_info_t *input_setup_info;
	framegrabber_pixfmt_bitmask_e	support_out_pixfmt_mask;
	unsigned max_supported_width;
        unsigned max_frame_size;
}framegrabber_property_t;

typedef enum
{
	SUBDEV_NOP=0,
	SUBDEV_LOG_STATUS,
	SUBDEV_IO_PIN_CONFIG,
	SUBDEV_INIT,
	SUBDEV_LOAD_FW,
	SUBDEV_RESET,
	SUBDEV_S_GPIO,
	SUBDEV_QUERYCTL,
	SUBDEV_G_CTRL,
	SUBDEV_S_CTRL,
	SUBDEV_G_EXT_CTRL,
	SUBDEV_S_EXT_CTRL,
	SUBDEV_TRY_EXT_CTRL,
	SUBDEV_QUERYMENU,
	SUBDEV_IOCTL,
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,15,0)
//	SUBDEV_COMPAT_IOCTL32,
//#endif
	SUBDEV_G_REGISTER,
	SUBDEV_S_REGISTER,
	SUBDEV_S_POWER,
	SUBDEV_S_INTERRUPT_SERVICE_ROUTINE,
	SUBDEV_SUBSCRIBE_EVENT,
	SUBDEV_UNSUBSCRIBE_EVENT,

	SUBDEV_S_CLOCK_FREQ,
	SUBDEV_S_I2S_CLOCK_FREQ,
	SUBDEV_S_A_ROUTING,
	SUBDEV_S_A_STREAM,

	SUBDEV_S_V_ROUTING,
	SUBDEV_S_CRYSTAL_FREQ,
	SUBDEV_G_STD,
	SUBDEV_S_STD,
	SUBDEV_G_STD_OUTPUT,
	SUBDEV_S_STD_OUTPUT,
	SUBDEV_QUERYSTD,
	SUBDEV_G_TVNORMS,
	SUBDEV_G_TVNORMS_OUTPUT,
	SUBDEV_G_INPUT_STATUS,
	SUBDEV_S_V_STREAM,
	SUBDEV_CROPCAP,
	SUBDEV_G_CROP,
	SUBDEV_S_CROP,
	SUBDEV_G_PARM,
	SUBDEV_S_PARM,
	SUBDEV_G_FRAME_INTERVAL,
	SUBDEV_S_FRAME_INTERVAL,
	SUBDEV_ENUM_FRAMESIZES,
	SUBDEV_ENUM_FRAMEINTERVALS,
	SUBDEV_S_DV_TIMINGS,
	SUBDEV_G_DV_TIMINGS,
	SUBDEV_QUERY_DV_TIMINGS,
	SUBDEV_S_RX_BUFFER,

//	SUBDEV_GET_FMT,
//	SUBDEV_SET_FMT,
//	SUBDEV_GET_EDID,
//	SUBDEV_SET_EDID,

	SUBDEV_OP_NUM,

}framegrabber_subdev_op_e;

#define FRAMEGRABBER_MAX_SUBDEV_OP_ARG_NUM 4
typedef struct
{
	int argc;
	void *args[FRAMEGRABBER_MAX_SUBDEV_OP_ARG_NUM];
}framegrabber_subdev_op_arg_t;

enum framegrabber_audio_sample {
	AUDIO_RATE_32000, //0
	AUDIO_RATE_44100, //1
	AUDIO_RATE_48000, //2
	AUDIO_RATE_88200,
	AUDIO_RATE_96000,
	AUDIO_RATE_176400,
	AUDIO_RATE_192000,
};

//framegrabber_handle_t framegrabber_setup(framegrabber_property_t *prop,framegrabber_interface_t *operation,framegrabber_error_e *error);
//void framegrabber_release(framegrabber_handle_t handle);
framegrabber_handle_t framegrabber_init(cxt_mgr_handle_t cxt_mgr,framegrabber_property_t *prop,framegrabber_interface_t *operation);
void framegrabber_set_data(framegrabber_handle_t handle,void *data);
void *framegrabber_get_data(framegrabber_handle_t handle);
//void framegrabber_associate_v4l2dev(framegrabber_handle_t handle,struct v4l2_device *v4l2_dev);
void framegrabber_start(framegrabber_handle_t handle);
int framegrabber_g_input_num(framegrabber_handle_t handle);
const char *framegrabber_g_input_name(framegrabber_handle_t handle,int input);
int framegrabber_g_input(framegrabber_handle_t handle);
framegrabber_input_status_e framegrabber_g_input_status(framegrabber_handle_t handle);
void framegrabber_s_input_status(framegrabber_handle_t handle,framegrabber_input_status_e input_status);
void framegrabber_mask_s_status(framegrabber_handle_t handle,framegrabber_status_bitmask_e mask,framegrabber_status_bitmask_e status);
framegrabber_status_bitmask_e framegrabber_g_status(framegrabber_handle_t handle);
int framegrabber_s_input(framegrabber_handle_t handle,int input);
const framegrabber_pixfmt_t *framegrabber_g_out_pixelfmt(framegrabber_handle_t handle);
const framegrabber_pixfmt_t *framegrabber_g_support_pixelfmt_by_index(framegrabber_handle_t handle,int index);
const framegrabber_pixfmt_t *framegrabber_g_support_pixelfmt_by_fourcc(framegrabber_handle_t handle,U32_T fourcc);
int framegrabber_s_out_pixelfmt(framegrabber_handle_t handle,U32_T fourcc);
int framegrabber_g_support_framesize(framegrabber_handle_t handle,int width,int height);
int framegrabber_g_supportframesize(framegrabber_handle_t handle,int index,int *width,int *height);
int framegrabber_g_framesize_supportrefreshrate(framegrabber_handle_t handle,int width,int height,int index);
void framegrabber_notify(framegrabber_handle_t handle,char *fromname,int notification,void *arg);
unsigned framegrabber_g_out_planesize(framegrabber_handle_t handle, int plane);
unsigned framegrabber_g_out_planarbuffersize(framegrabber_handle_t handle, int plane);
unsigned framegrabber_g_out_framebuffersize(framegrabber_handle_t handle);
unsigned framegrabber_g_max_framebuffersize(framegrabber_handle_t handle);
void framegrabber_s_out_framesize(framegrabber_handle_t handle,int width,int height);
void framegrabber_g_out_framesize(framegrabber_handle_t handle,int *width,int *height);
void framegrabber_g_input_framesize(framegrabber_handle_t handle,int *width,int *height);
void framegrabber_s_input_framesize(framegrabber_handle_t handle,int width,int height);
int framegrabber_g_input_framerate(framegrabber_handle_t handle);
int framegrabber_g_input_denominator(framegrabber_handle_t handle);
void framegrabber_s_input_framerate(framegrabber_handle_t handle,int framerate,int denominator);
int framegrabber_g_out_framerate(framegrabber_handle_t handle);
void framegrabber_s_out_framerate(framegrabber_handle_t handle,int framerate);
int framegrabber_g_input_interlace(framegrabber_handle_t handle);
void framegrabber_s_input_interlace(framegrabber_handle_t handle,BOOL_T interlace);
void framegrabber_start_streaming(framegrabber_handle_t handle,BOOL_T on);
void framegrabber_s_input_framemode(framegrabber_handle_t handle,framegrabber_framemode_e framemode);
void framegrabber_s_input_audioinfo(framegrabber_handle_t handle,enum framegrabber_audio_sample audioinfo);
int framegrabber_g_input_audioinfo(framegrabber_handle_t handle);
framegrabber_framemode_e framegrabber_g_input_framemode(framegrabber_handle_t handle);
unsigned framegrabber_g_out_bytesperline(framegrabber_handle_t handle, int plane);

int framegrabber_g_input_bchs(framegrabber_handle_t handle,int bchs_select);
void framegrabber_s_input_bchs(framegrabber_handle_t handle,int bchs_value,int bchs_select);
void framegrabber_s_input_dualmode(framegrabber_handle_t handle,int dual_pixel);
int framegrabber_g_input_dualmode(framegrabber_handle_t handle);

int framegrabber_g_hdcp_state(framegrabber_handle_t handle);
void framegrabber_s_hdcp_state(framegrabber_handle_t handle,int hdcp_state);

int framegrabber_g_flash(framegrabber_handle_t handle, int start_block, int blocks, U8_T *flash_dump);
int framegrabber_s_flash(framegrabber_handle_t handle, int start_block, int blocks, U8_T *flash_update);

void framegrabber_s_hdcp_flag(framegrabber_handle_t handle, unsigned int hdcp_flag);
int framegrabber_g_hdcp_flag(framegrabber_handle_t handle);
//int framegrabber_call_subdev(framegrabber_handle_t handle,char *name,framegrabber_subdev_op_e op, ...);
//void *framegrabber_get_v4l2_context(framegrabber_handle_t handle);

int framegrabber_g_i2c(framegrabber_handle_t handle, unsigned char channel, unsigned int slave, unsigned int sub, unsigned char sublen, unsigned char *data, unsigned int datalen, unsigned int is_10bit);
int framegrabber_s_i2c(framegrabber_handle_t handle, unsigned char channel, unsigned int slave, unsigned int sub, unsigned char sublen, unsigned char *data, unsigned int datalen, unsigned int is_10bit);
int framegrabber_g_reg(framegrabber_handle_t handle, unsigned int offset, unsigned char n_bytes, unsigned int *data);
int framegrabber_s_reg(framegrabber_handle_t handle, unsigned int offset, unsigned char n_bytes, unsigned int data);
#endif /* MODULE_V4L2_INCLUDE_FRAMEGRABBER_H_ */
