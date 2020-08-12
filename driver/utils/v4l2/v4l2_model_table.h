/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * v4l2_model_table.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#ifndef V4L2_MODEL_TABLE_H_
#define V4L2_MODEL_TABLE_H_



typedef struct
{
	struct frame_size frame_size;
	int refresh_rate;
	bool  is_interlace;
} v4l2_model_timing_t;


#define V4L2_MODEL_TIMING(w,h,f,i) \
	{ \
		.frame_size = { \
			.width = w, \
			.height = h, \
		}, \
		.refresh_rate = f, \
		.is_interlace = i, \
	}


v4l2_model_timing_t *v4l2_model_get_support_videoformat(int index);

#endif /* V4L2_MODEL_TABLE_H_ */
