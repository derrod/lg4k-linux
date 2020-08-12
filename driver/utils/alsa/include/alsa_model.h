/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * alsa_model.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#ifndef UTILS_ALSA_INCLUDE_ALSA_MODEL_H_
#define UTILS_ALSA_INCLUDE_ALSA_MODEL_H_

#define ALSA_CXT_ID fourcc_id('A','L','S','A')
#define enumtobitmask(value) BIT##value=Bit(value)

typedef enum
{
	_ALSA_MODEL_FMT_S16_LE,
	_ALSA_MODEL_FMT_S24_LE,
	ALSA_MODEL_FMT_COUNT,

	enumtobitmask(_ALSA_MODEL_FMT_S16_LE),
	enumtobitmask(_ALSA_MODEL_FMT_S24_LE),
}alsa_model_fmt_e;

typedef enum
{
	_ALSA_MODEL_RATE_32K,
	_ALSA_MODEL_RATE_44_1K,
	_ALSA_MODEL_RATE_48K,
	_ALSA_MODEL_RATE_96K,
	_ALSA_MODEL_RATE_192K,
	ALSA_MODEL_RATE_COUNT,

	enumtobitmask(_ALSA_MODEL_RATE_32K),
	enumtobitmask(_ALSA_MODEL_RATE_44_1K),
	enumtobitmask(_ALSA_MODEL_RATE_48K),
	enumtobitmask(_ALSA_MODEL_RATE_96K),
	enumtobitmask(_ALSA_MODEL_RATE_192K),


}alsa_model_rate_e;


typedef struct
{
	const char *name;
	int capture_count;
}alsa_model_pcm_info_t;

typedef enum
{
    ALSA_MODEL_OPEN_CB,
    ALSA_MODEL_CLOSE_CB,        
    ALSA_MODEL_CAPTURE_START_CB,
    ALSA_MODEL_CAPTURE_STOP_CB,
    ALSA_MODEL_MAX_CB_NO,        
}alsa_model_callback_e;

enum
{
    ALSA_MODEL_DEFAULT_PERIOD_SIZE=4096,
    ALSA_MODEL_DEFAULT_MAX_PERIOD_NUM=16,//4
};

typedef void (*alsa_model_callback_fun_t)(void *data);

typedef struct
{
	const char *name;
	alsa_model_fmt_e support_fmt_mask;
	alsa_model_rate_e support_rate_mask;
	int pcm_count;
	alsa_model_pcm_info_t *pcm_info;
        unsigned period_size;
        unsigned max_period_num;
}alsa_model_setup_t;

typedef void *alsa_model_handle_t;

alsa_model_handle_t alsa_model_init(cxt_mgr_handle_t cxt_mgr,alsa_model_setup_t *alsa_setup_info);
void alsa_model_register_callback(alsa_model_handle_t handle,alsa_model_callback_e no,alsa_model_callback_fun_t cb_func,void *cxt);
void alsa_model_feed_data(alsa_model_handle_t handle, U8_T *buf, SIZE_T length);
void alsa_model_suspend(alsa_model_handle_t handle);
void alsa_model_resume(alsa_model_handle_t handle);

#endif /* UTILS_ALSA_INCLUDE_ALSA_MODEL_H_ */
