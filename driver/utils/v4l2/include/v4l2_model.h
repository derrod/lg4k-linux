/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * v4l2_model.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#ifndef V4L2_MODEL_H_
#define V4L2_MODEL_H_

#define V4L2_MODEL_MAX_INPUT 8
#define V4L2_MODEL_MAX_SUPPORT_PIXELFMT 4

//#if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,0)
#define VIDEO_MAX_PLANES 8
//#endif

#define V4L2_MODULE_ID_STR MODULE_STR(v4l2)
#define V4L2_CXT_ID fourcc_id('V','4','L','2')

#include "cxt_mgr.h"
#include "framegrabber.h"

typedef enum {
	DEVICE_TYPE_GRABBER = 0,
	DEVICE_TYPE_VBI,
	DEVICE_TYPE_RADIO,
	DEVICE_TYPE_SUBDEV,
} v4l2_model_devicetype_t;

typedef enum {
	V4L2_MODEL_CAPS_CAPTURE = 0,
	V4L2_MODEL_CAPS_CAPTURE_MPLANE,
	V4L2_MODEL_CAPS_VBI_CAPTURE,
	V4L2_MODEL_CAPS_TUNER,
	V4L2_MODEL_CAPS_AUDIO,
	V4L2_MODEL_CAPS_RADIO,
	V4L2_MODEL_CAPS_READWRITE,
	V4L2_MODEL_CAPS_STREAMING,
	V4L2_MODEL_CAPS_MAX,
} v4l2_model_cap_e;

typedef enum {
	V4L2_MODEL_CAPS_CAPTURE_BIT = 1 << V4L2_MODEL_CAPS_CAPTURE,
	V4L2_MODEL_CAPS_CAPTURE_MPLANE_BIT =
	    1 << V4L2_MODEL_CAPS_CAPTURE_MPLANE,
	V4L2_MODEL_CAPS_VBI_CAPTURE_BIT = 1 << V4L2_MODEL_CAPS_VBI_CAPTURE,
	V4L2_MODEL_CAPS_TUNER_BIT = 1 << V4L2_MODEL_CAPS_TUNER,
	V4L2_MODEL_CAPS_AUDIO_BIT = 1 << V4L2_MODEL_CAPS_AUDIO,
	V4L2_MODEL_CAPS_RADIO_BIT = 1 << V4L2_MODEL_CAPS_RADIO,
	V4L2_MODEL_CAPS_READWRITE_BIT = 1 << V4L2_MODEL_CAPS_READWRITE,
	V4L2_MODEL_CAPS_STREAMING_BIT = 1 << V4L2_MODEL_CAPS_STREAMING,
	V4L2_MODEL_CAPS_BITMASK = (1 << V4L2_MODEL_CAPS_MAX) - 1,
} v4l2_model_cap_bitmask_e;

typedef enum {
    V4L2_MODEL_FPS_15 = 0,
    V4L2_MODEL_FPS_24,
    V4L2_MODEL_FPS_25,
    V4L2_MODEL_FPS_30,
    V4L2_MODEL_FPS_50,
    V4L2_MODEL_FPS_60,
    V4L2_MODEL_FPS_120,
    V4L2_MODEL_FPS_144,
    V4L2_MODEL_FPS_240,
    V4L2_MODEL_FPS_MAX,
} v4l2_model_frameinterval_e;

typedef enum {
    V4L2_MODEL_FPS_15_BIT = 1 << V4L2_MODEL_FPS_15,
    V4L2_MODEL_FPS_24_BIT = 1 << V4L2_MODEL_FPS_24,
    V4L2_MODEL_FPS_25_BIT = 1 << V4L2_MODEL_FPS_25,
    V4L2_MODEL_FPS_30_BIT = 1 << V4L2_MODEL_FPS_30,
    V4L2_MODEL_FPS_50_BIT = 1 << V4L2_MODEL_FPS_50,
    V4L2_MODEL_FPS_60_BIT = 1 << V4L2_MODEL_FPS_60,
    V4L2_MODEL_FPS_120_BIT = 1 << V4L2_MODEL_FPS_120,
    V4L2_MODEL_FPS_144_BIT = 1 << V4L2_MODEL_FPS_144,
    V4L2_MODEL_FPS_240_BIT = 1 << V4L2_MODEL_FPS_240,
} v4l2_model_frameinterval_bitmask_e;

typedef enum {
    V4L2_MODEL_VIDEOFORMAT_UNSUPPORTED = -1,
    V4L2_MODEL_VIDEOFORMAT_640X480P60 = 0,
    V4L2_MODEL_VIDEOFORMAT_720x480I60,
    V4L2_MODEL_VIDEOFORMAT_720X480P60,
    V4L2_MODEL_VIDEOFORMAT_720X576P50,
    V4L2_MODEL_VIDEOFORMAT_800X600P60,
    V4L2_MODEL_VIDEOFORMAT_1024X768P60,

    V4L2_MODEL_VIDEOFORMAT_1280X720P50,
    V4L2_MODEL_VIDEOFORMAT_1280X720P60,
    V4L2_MODEL_VIDEOFORMAT_1280X768P60,
    V4L2_MODEL_VIDEOFORMAT_1280X800P60,
    V4L2_MODEL_VIDEOFORMAT_1280X1024P60,
    V4L2_MODEL_VIDEOFORMAT_1360X768P60,
    V4L2_MODEL_VIDEOFORMAT_1440X900P60,
    V4L2_MODEL_VIDEOFORMAT_1680X1050P60,

    V4L2_MODEL_VIDEOFORMAT_1920X1080P24,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P25,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P30,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P50,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P60,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P120,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P144,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P240,

    V4L2_MODEL_VIDEOFORMAT_1920X1200P24,
    V4L2_MODEL_VIDEOFORMAT_1920X1200P25,
    V4L2_MODEL_VIDEOFORMAT_1920X1200P30,
    V4L2_MODEL_VIDEOFORMAT_1920X1200P50,
    V4L2_MODEL_VIDEOFORMAT_1920X1200P60,
    V4L2_MODEL_VIDEOFORMAT_2560X1080P60,
    V4L2_MODEL_VIDEOFORMAT_2560X1080P120,
    V4L2_MODEL_VIDEOFORMAT_2560X1080P144,
    V4L2_MODEL_VIDEOFORMAT_2560X1440P60,
    V4L2_MODEL_VIDEOFORMAT_2560X1440P120,
    V4L2_MODEL_VIDEOFORMAT_2560X1440P144,
    V4L2_MODEL_VIDEOFORMAT_3840X2160P24,
    V4L2_MODEL_VIDEOFORMAT_3840X2160P25,
    V4L2_MODEL_VIDEOFORMAT_3840X2160P30,
    V4L2_MODEL_VIDEOFORMAT_3840X2160P50,
    V4L2_MODEL_VIDEOFORMAT_3840X2160P60,
    V4L2_MODEL_VIDEOFORMAT_4096X2160P24,
    V4L2_MODEL_VIDEOFORMAT_4096X2160P25,
    V4L2_MODEL_VIDEOFORMAT_4096X2160P30,
    V4L2_MODEL_VIDEOFORMAT_4096X2160P50,
    V4L2_MODEL_VIDEOFORMAT_4096X2160P60,
    V4L2_MODEL_VIDEOFORMAT_NUM,
} v4l2_model_videoformat_e;

typedef enum {
    V4L2_MODEL_VIDEOFORMAT_640X480P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_640X480P60,
    V4L2_MODEL_VIDEOFORMAT_720x480I60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_720x480I60,
    V4L2_MODEL_VIDEOFORMAT_720X480P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_720X480P60,
    V4L2_MODEL_VIDEOFORMAT_720X576P50_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_720X576P50,
    V4L2_MODEL_VIDEOFORMAT_800X600P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_800X600P60,
    V4L2_MODEL_VIDEOFORMAT_1024X768P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1024X768P60,
    V4L2_MODEL_VIDEOFORMAT_1280X720P50_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1280X720P50,
    V4L2_MODEL_VIDEOFORMAT_1280X720P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1280X720P60,
    V4L2_MODEL_VIDEOFORMAT_1280X768P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1280X768P60,
    V4L2_MODEL_VIDEOFORMAT_1280X800P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1280X800P60,
    V4L2_MODEL_VIDEOFORMAT_1280X1024P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1280X1024P60,
    V4L2_MODEL_VIDEOFORMAT_1360X768P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1360X768P60,
    V4L2_MODEL_VIDEOFORMAT_1440X900P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1440X900P60,
    V4L2_MODEL_VIDEOFORMAT_1680X1050P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1680X1050P60,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P24_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1080P24,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P25_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1080P25,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P30_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1080P30,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P50_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1080P50,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1080P60,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P120_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1080P120,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P144_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1080P144,
    V4L2_MODEL_VIDEOFORMAT_1920X1080P240_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1080P240,
    V4L2_MODEL_VIDEOFORMAT_1920X1200P24_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1200P24,
    V4L2_MODEL_VIDEOFORMAT_1920X1200P25_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1200P25,
    V4L2_MODEL_VIDEOFORMAT_1920X1200P30_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1200P30,
    V4L2_MODEL_VIDEOFORMAT_1920X1200P50_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1200P50,
    V4L2_MODEL_VIDEOFORMAT_1920X1200P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_1920X1200P60,
    V4L2_MODEL_VIDEOFORMAT_2560X1080P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_2560X1080P60,
    V4L2_MODEL_VIDEOFORMAT_2560X1080P120_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_2560X1080P120,
    V4L2_MODEL_VIDEOFORMAT_2560X1080P144_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_2560X1080P144,
    V4L2_MODEL_VIDEOFORMAT_2560X1440P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_2560X1440P60,
    V4L2_MODEL_VIDEOFORMAT_2560X1440P120_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_2560X1440P120,
    V4L2_MODEL_VIDEOFORMAT_2560X1440P144_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_2560X1440P144,
    V4L2_MODEL_VIDEOFORMAT_3840X2160P24_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_3840X2160P24,
    V4L2_MODEL_VIDEOFORMAT_3840X2160P25_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_3840X2160P25,
    V4L2_MODEL_VIDEOFORMAT_3840X2160P30_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_3840X2160P30,
    V4L2_MODEL_VIDEOFORMAT_3840X2160P50_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_3840X2160P50,
    V4L2_MODEL_VIDEOFORMAT_3840X2160P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_3840X2160P60,
    V4L2_MODEL_VIDEOFORMAT_4096X2160P24_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_4096X2160P24,
    V4L2_MODEL_VIDEOFORMAT_4096X2160P25_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_4096X2160P25,
    V4L2_MODEL_VIDEOFORMAT_4096X2160P30_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_4096X2160P30,
    V4L2_MODEL_VIDEOFORMAT_4096X2160P50_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_4096X2160P50,
    V4L2_MODEL_VIDEOFORMAT_4096X2160P60_BIT = 1L << V4L2_MODEL_VIDEOFORMAT_4096X2160P60,
    V4L2_MODEL_VIDEOFORMAT_ALL = (1L << V4L2_MODEL_VIDEOFORMAT_NUM) - 1,
} v4l2_model_videoformat_bitmask;

typedef enum {
	V4L2_MODEL_BUF_TYPE_VMALLOC,
	V4L2_MODEL_BUF_TYPE_DMA_CONT,
	V4L2_MODEL_BUF_TYPE_DMA_SG,
} v4l2_model_buffer_type_e;

typedef struct {
	unsigned long addr;
	unsigned long size;
} v4l2_model_buf_desc_t;

typedef struct {
	int planes;
	v4l2_model_buffer_type_e buf_type;
	int buf_count[VIDEO_MAX_PLANES];
	v4l2_model_buf_desc_t *buf_info[VIDEO_MAX_PLANES];
} v4l2_model_buffer_info_t;

typedef enum {
	V4L2_MODEL_OK = 0,
	V4L2_MODEL_ERROR_INVALID_CALLBACK_NO = -1,
	V4L2_MODEL_ERROR_DUPLICATE_CALLBACK = -2,
	V4L2_MODEL_ERROR_INVALID_CONTEXT = -3,
	V4L2_MODEL_ERROR_NO_DEV = -4,
	V4L2_MODEL_ERROR_ALLOC = -5,
	V4L2_MODEL_ERROR_REGISTER_V4L2 = -6,
	V4L2_MODEL_ERROR_VIDEO_BUF = -7,
	V4L2_MODEL_ERROR_REGISTER_VIDEO = -8,
} v4l2_model_error_e;
typedef enum {
	V4L2_MODEL_CALLBACK_STREAMON = 0,	//whenever user space wants to start grabbing data. That may happen in response to a VIDIOC_STREAMON ioctl(), but the videobuf2 implementation of the read() system call can also use it
	V4L2_MODEL_CALLBACK_STREAMOFF,	//when user space no longer wants data; this callback should not return until DMA has been stopped.the driver should forget any references it may have to those buffers.
	V4L2_MODEL_CALLBACK_QUEUE_SETUP,
	V4L2_MODEL_CALLBACK_BUFFER_PREPARE,	//invoked when user space queues the buffer
	V4L2_MODEL_CALLBACK_BUFFER_INIT,	//invoked when each new buffer after it has been allocated
	V4L2_MODEL_CALLBACK_BUFFER_QUEUE,	//invoked when pass actual ownership of the buffer to the driver,
	V4L2_MODEL_CALLBACK_BUFFER_FINISH,	// invoked just before the buffer is passed back to user space (process context)
	V4L2_MODEL_CALLBACK_BUFFER_CLEANUP,	//invoked just before a buffer is freed so that the driver
	V4L2_MODEL_CALLBACK_NUM,
} v4l2_model_callback_e;

typedef struct {
	int width;
	int height;
} v4l2_model_size_t;
typedef struct {
	int width;
	int height;
	int depth;
	BOOL_T is_interlace;
	U32_T frame_rate;
} v4l2_model_output_fmt_t;

typedef enum {
	BASIC_TYPE,
	QUEUE_SETUP_TYPE,
	BUFFER_PREPARE_TYPE,

} v4l2_model_cb_info_type_e;

typedef struct {
	v4l2_model_buffer_info_t *buffer_info;
} V4L2_MODEL_BUFFER_PREPARE_CB_INFO_T;

typedef struct {
	v4l2_model_buffer_info_t *buffer_info;
} V4L2_MODEL_BUFFER_INIT_CB_INFO_T;

typedef struct {
	int buffer_num;
} V4L2_MODEL_QUEUE_SETUP_CB_INFO_T;

typedef struct {
	void *asso_data;
	v4l2_model_cb_info_type_e type;
	union {
		V4L2_MODEL_BUFFER_PREPARE_CB_INFO_T buffer_prepare_info;
		V4L2_MODEL_BUFFER_INIT_CB_INFO_T buffer_init_info;
		V4L2_MODEL_QUEUE_SETUP_CB_INFO_T queue_setup_info;
	} u;
} v4l2_model_callback_parameter_t;
typedef void (*v4l2_model_callback_t) (v4l2_model_callback_parameter_t *
				       cb_info);

//typedef v4l2_model_callback_parameter_t V4L2_MODEL_CALLBACK_STREAMON_PARAMER_T;
//typedef v4l2_model_callback_parameter_t V4L2_MODEL_CALLBACK_STREAMOFF_PARAMER_T;
//typedef struct
//{
//      int buffer_num;
//}V4L2_MODEL_CALLBACK_QUEUE_SETUP_PARAMER_T;
//typedef v4l2_model_callback_parameter_t V4L2_MODEL_CALLBACK_BUFFER_PREPARE_PARAMER_T;
//typedef v4l2_model_callback_parameter_t V4L2_MODEL_CALLBACK_BUFFER_INIT_PARAMER_T;
//typedef v4l2_model_callback_parameter_t V4L2_MODEL_CALLBACK_BUFFER_QUEUE_PARAMER_T;
//typedef v4l2_model_callback_parameter_t V4L2_MODEL_CALLBACK_BUFFER_FINISH_PARAMER_T;
//typedef v4l2_model_callback_parameter_t V4L2_MODEL_CALLBACK_BUFFER_CLEANUP_PARAMER_T;

typedef enum {
	V4L2_MODEL_INPUT_TYPE_TUNER = 0,
	V4L2_MODEL_INPUT_TYPE_CAMERA,
} v4l2_model_input_type_e;

typedef enum {
	V4L2_MODEL_INPUT_STATUS_OK = 0,
	V4L2_MODEL_INPUT_STATUS_NO_POWER,
	V4L2_MODEL_INPUT_STATUS_NO_SIGNAL,
	V4L2_MODEL_INPUT_STATUS_NO_SYNC,
} v4l2_model_input_status_e;

typedef struct {
	const char *name;
	v4l2_model_input_type_e type;
	v4l2_model_input_status_e status;
	U32_T support_videoformat_mask;	//v4l2_model_input_timing_bitmask
	int detected_timing;

} v4l2_model_input_info_t;

typedef struct {
	struct i2c_adapter *adap;
	struct i2c_board_info *board_info;
} v4l2_model_i2c_subdev_info_t;

typedef struct {
	v4l2_model_devicetype_t type;
	const char driver_name[16];
	const char card_name[32];
	const char card_name_1[32];
	U32_T capabilities;	//v4l2_model_cap_bitmask_e
	v4l2_model_i2c_subdev_info_t *i2c_subdev_info;
	v4l2_model_buffer_type_e buffer_type;

} v4l2_model_device_setup_t;

typedef long (*v4l2_model_ioctl_hook_func_t) (void *context, unsigned int cmd,
					      void *arg);
typedef enum {
	HOOK_PREV_IOCTL = 0,
	HOOK_POST_IOCTL,
	HOOK_IOCTL_TYPE_NUM,
} v4l2_model_ioctl_hook_type_e;

//struct sagitta_v4l2_format
//{
//    U32_T pixelformat;
//};

typedef void *v4l2_model_handle_t;

//void *v4l2_model_init(struct device *dev, v4l2_model_device_setup_t *setup_info,framegrabber_handle_t framegrabber_handle);
v4l2_model_handle_t v4l2_model_init(cxt_mgr_handle_t cxt_mgr,
				    v4l2_model_device_setup_t * device_info,
				    framegrabber_handle_t framegrabber_handle);
//void v4l2_model_release(void *context);

void v4l2_model_disconnect(v4l2_model_handle_t context);

void v4l2_model_next_buffer(v4l2_model_handle_t context,
			    v4l2_model_buffer_info_t ** buffer_info);

void v4l2_model_buffer_done(v4l2_model_handle_t context);
//void v4l2_model_buffer_done(v4l2_model_buffer_info_t *next_buffer, void *context);
//
void v4l2_model_queue_cancel(v4l2_model_handle_t context);

int v4l2_model_register_callback(v4l2_model_handle_t context,
				 v4l2_model_callback_e callback_no,
				 v4l2_model_callback_t callback,
				 void *asso_data);
int v4l2_model_unregister_callback(v4l2_model_handle_t context,
				   v4l2_model_callback_e callback_no);
int v4l2_model_hook_ioctl(v4l2_model_handle_t v4l2_context,
			  v4l2_model_ioctl_hook_type_e hooktype,
			  v4l2_model_ioctl_hook_func_t ioctl_hook, void *data);

//struct frame_size *v4l2_model_get_frame_size(int index);
int v4l2_model_get_current_frame_size(v4l2_model_handle_t context,
				      struct frame_size *frame_size);
struct v4l2_device *v4l2_model_get_v4l2_device(v4l2_model_handle_t context);
void v4l2_model_streamoff(v4l2_model_handle_t context);
void v4l2_model_feed_video_data(v4l2_model_handle_t context, void *buffer,
				SIZE_T size);
void v4l2_model_reset_video_data(v4l2_model_handle_t context);
//void *v4l2_model_get_context(struct sagitta_dev *sdev);
#endif				/* V4L2_MODEL_H_ */
