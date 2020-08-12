/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * framegrabber_priv.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 
#ifndef MODULE_V4L2_FRAMEGRABBER_PRIV_H_
#define MODULE_V4L2_FRAMEGRABBER_PRIV_H_

#define FRAMEGRABBER_MAX_INPUT_NUM 8

typedef struct
{
	BASIC_CXT_HANDLE_DECLARE;
	const char *name;
	framegrabber_framesize_mask_e framesize_cap;
	framegrabber_refreshrate_e 	framesize_info[FRAMEGRABBER_SUPPORT_FRAMESIZE_NUM];
}framegrabber_input_info_t;


typedef struct
{
	int input_no;
	framegrabber_input_info_t input_info[FRAMEGRABBER_MAX_INPUT_NUM];
	framegrabber_property_t property;
	int current_input;
	framegrabber_input_status_e current_input_status;
	framegrabber_status_bitmask_e current_status;
	framegrabber_pixfmt_bitmask_e pixfmt_cap;
	framegrabber_frame_size_t current_out_framesize;
	framegrabber_frame_size_t current_input_framesize;
	framegrabber_framemode_e framemode;
	enum framegrabber_audio_sample current_audioinfo;
	int current_framerate;
	int current_out_framerate;
	int current_denominator;
	int current_out_pixfmt;
	int current_bchs_value;
	int current_bchs_selection;
	BOOL_T current_input_interlace;
	unsigned max_supported_width;
    unsigned max_frame_size;
	//struct v4l2_device *v4l2_dev;
	void *data;
	int current_dual_pixel;

	framegrabber_interface_t interface;
    unsigned int hdcp_flag;
}framegrabber_context_t;

#endif /* MODULE_V4L2_FRAMEGRABBER_PRIV_H_ */
