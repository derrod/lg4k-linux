/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * v4l2_model_ioctl.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 

#ifndef V4L2_MODEL_IOCTL_H_
#define V4L2_MODEL_IOCTL_H_

#include <linux/videodev2.h>
#include "typedef.h"
#include "v4l2_model_device.h"


int v4l2_model_ioctl_querycap(struct file *file, void *fh, struct v4l2_capability *cap);
int v4l2_model_ioctl_enum_fmt_vid_cap(struct file *file, void *fh, struct v4l2_fmtdesc *f);
int v4l2_model_ioctl_g_fmt_vid_cap(struct file *file, void *fh,struct v4l2_format *f);
int v4l2_model_ioctl_try_fmt_vid_cap(struct file *file, void *fh, struct v4l2_format *f);
int v4l2_model_ioctl_s_fmt_vid_cap(struct file *file, void *fh,struct v4l2_format *f);
int v4l2_model_ioctl_enum_framesizes(struct file *file, void *fh, struct v4l2_frmsizeenum *fsize);
int v4l2_model_ioctl_enum_input(struct file *file, void *fh, struct v4l2_input *inp);
int v4l2_model_ioctl_g_input(struct file *file, void *fh, unsigned int *i);
int v4l2_model_ioctl_s_input(struct file *file, void *fh, unsigned int i);
int v4l2_model_ioctl_enum_frameintervals(struct file *file, void *fh,struct v4l2_frmivalenum *fival);
int v4l2_model_ioctl_g_parm(struct file *file, void *fh,struct v4l2_streamparm *a);
int v4l2_model_ioctl_s_parm(struct file *file, void *fh,struct v4l2_streamparm *a);
int v4l2_model_ioctl_queryctrl(struct file *file, void *fh,struct v4l2_queryctrl *a);
int v4l2_model_ioctl_g_ctrl(struct file *file, void *fh,struct v4l2_control *a);
int v4l2_model_ioctl_s_ctrl(struct file *file, void *fh,struct v4l2_control *a);
int v4l2_model_ioctl_cropcap(struct file *file, void *fh,struct v4l2_cropcap *a);
int v4l2_model_ioctl_g_flash(struct file *file, struct v4l2_dump_flash *flash);
int v4l2_model_ioctl_s_flash(struct file *file, struct v4l2_dump_flash *flash);
int v4l2_model_ioctl_g_hdcp_state(struct file *file, unsigned int *arg);
int v4l2_model_ioctl_s_hdcp_state(struct file *file,unsigned int *arg);
int v4l2_model_ioctl_g_dv_timings(struct file *file, void *fd,
                                struct v4l2_dv_timings *timings);
int v4l2_model_ioctl_s_dv_timings(struct file *file, void *fd,
                                struct v4l2_dv_timings *timings);

int v4l2_model_ioctl_enum_std(struct file *file, void *fd,
                                struct v4l2_standard *std);

int v4l2_model_ioctl_s_std(struct file *file, void *fd,
            v4l2_std_id std);
int v4l2_model_ioctl_g_std(struct file *file, void *fd,
            v4l2_std_id *std);
int v4l2_model_ioctl_querystd(struct file *file, void *fd,
            v4l2_std_id *std);

int v4l2_model_g_tuner(struct file *file, void *fd,
            struct v4l2_tuner *tuner);
int v4l2_model_s_tuner(struct file *file, void *fd,
            const struct v4l2_tuner *tuner);

int v4l2_model_ioctl_g_i2c(struct file *file, struct i2c_t* arg);
int v4l2_model_ioctl_s_i2c(struct file *file, struct i2c_t* arg);
int v4l2_model_ioctl_g_reg(struct file *file, struct reg_t* arg);
int v4l2_model_ioctl_s_reg(struct file *file, struct reg_t* arg);
#endif /* V4L2_MODEL_IOCTL_H_ */
