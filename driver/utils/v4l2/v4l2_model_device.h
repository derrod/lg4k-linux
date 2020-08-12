/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * v4l2_model_device.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 

#ifndef V4L2_MODEL_DEVICE_H_
#define V4L2_MODEL_DEVICE_H_
#include "framegrabber.h"
#include "pic_bmp.h"

typedef struct i2c_t
{
	unsigned char channel;
	unsigned int slave_address;
	unsigned int sub_address;
	unsigned char sub_address_length; // must less than 4
	unsigned char data[256];
	unsigned int data_length; // must less than 256
	unsigned int is_10bit_address;
}i2c_t;

typedef enum _eNumOfRegAccessByte
{
    ONE_BYTE  = 1,
    TWO_BYTE  = 2,
    FOUR_BYTE = 4
} eNumOfRegAccessByte;

typedef struct reg_t
{
    unsigned int offset;
    unsigned int data;
    eNumOfRegAccessByte n_bytes;
}reg_t;

#if 1
struct v4l2_dump_flash {
	__u32 pad;
	__u32 start_block;
	__u32 blocks;
	__u32 reserved[5];
	__u8  *flash;
};
#endif

typedef struct
{
	v4l2_model_callback_t callback;
	v4l2_model_callback_parameter_t data;
}v4l2_model_callback_item_t;

typedef struct 
{
	struct list_head list;
	v4l2_model_ioctl_hook_func_t ioctl_hook_func;
	void *data;
}v4l2_model_ioctl_hook_item_t;

typedef struct
{
	v4l2_model_buffer_info_t *vbuf_info;
	//unsigned lines;
	unsigned int rcv_size;

}framebuffer_info_t;

typedef struct v4l2_model_context_s
{
	struct v4l2_device v4l2_dev;
	struct video_device vdev;
	struct vb2_queue queue;
	struct mutex lock;
    struct device *dev;

	void  *vb2_context;
	v4l2_model_device_setup_t device_info;
	v4l2_model_callback_item_t callbacks[V4L2_MODEL_CALLBACK_NUM];

	struct list_head ioctl_hook_list[HOOK_IOCTL_TYPE_NUM];
	framegrabber_handle_t framegrabber_handle;
	framebuffer_info_t current_framebuf_info;
	int inibuffer_create;
	int inibuffer_index;
	handle_t adv7619_handle;
	pic_bmp_handle_t pic_bmp_handle;
}v4l2_model_context_t;

 

#define V4L2_MODEL_DEBUG(fmt, ...) //printk(fmt, ##__VA_ARGS__)


#endif /* V4L2_MODEL_DEVICE_H_ */
