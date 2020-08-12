/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * v4l2_model_videobuf2.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#ifndef V4L2_MODEL_VIDEOBUF2_H_
#define V4L2_MODEL_VIDEOBUF2_H_



void *v4l2_model_vb2_init(struct vb2_queue *q,v4l2_model_devicetype_t dev_type,v4l2_model_buffer_type_e queue_type,struct device *dev, void *priv);
void  v4l2_model_vb2_release(void *context);

#define vb2q_to_v4l2context(x) \
    container_of(x, v4l2_model_context_t, queue)

#define vb2b_to_v4l2context(x) \
    container_of(x->vb2_queue, v4l2_model_context_t, queue)


#endif /* V4L2_MODEL_VIDEOBUF2_H_ */
