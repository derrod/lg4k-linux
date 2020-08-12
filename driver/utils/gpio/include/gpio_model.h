/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * gpio_model.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 
#ifndef __GPIO_H__
#define __GPIO_H__

#define GPIO_CXT_ID fourcc_id('G','P','I','O')


typedef enum
{
	GPIOIC_DIR_IN,
	GPIOIC_DIR_OUT,

	GPIOIC_DIR_UNKNOWN=-1,
}gpio_ic_direction_t;

typedef enum
{
	GPIOIC_VALUE_LOW,
	GPIOIC_VALUE_HIGH,

	GPIOIC_VALUE_UNKNOWN=-1,
}gpio_model_pin_value_t;


typedef struct gpio_ic_ops
{
	int (*set_direction)(void *context, unsigned no,gpio_ic_direction_t dir);
	gpio_ic_direction_t (*get_direction)(void *context, unsigned no);
	int (*set)(void *context,unsigned no, gpio_model_pin_value_t value);
	gpio_model_pin_value_t (*get)(void *context,unsigned no);
}gpio_ic_ops_t;


typedef void *gpio_model_pin_handle_t;
#define GPIO_MODEL_BAD_PIN_HANDLE (void *)NULL
typedef enum
{
	GPIO_MODEL_OK=0,
	GPIO_MODEL_ERROR_NO_SUCH_NAME=-1,
	GPIO_MODEL_ALREADY_REGISTERED=-2,
	GPIO_MODEL_ALLOC_FAIL=-3,
	GPIO_MODEL_BAD_HANDLE=-4,
}gpio_model_error_t;

typedef void *gpio_model_handle_t;

gpio_model_handle_t gpio_model_init(cxt_mgr_handle_t cxt_mgr);
gpio_model_error_t 		gpio_model_register_ic(gpio_model_handle_t gpio_model_context,const char *gpio_ic_name,gpio_ic_ops_t *ops,void *context);
gpio_model_error_t 		gpio_model_unregister_ic(gpio_model_handle_t gpio_model_context,const char *gpio_ic_name);
gpio_model_error_t 		gpio_model_register_pin(gpio_model_handle_t gpio_model_context,const char *gpio_ic_name,const char *pin_name,int pin_no);
gpio_model_pin_handle_t gpio_model_request_pin_handle(gpio_model_handle_t gpio_model_context,const char *pin_name);
gpio_model_error_t 		gpio_model_pin_set(gpio_model_pin_handle_t handle,gpio_model_pin_value_t value);
gpio_model_pin_value_t	gpio_model_pin_get(gpio_model_pin_handle_t handle);
gpio_model_error_t 		gpio_model_pin_set_direction(gpio_model_pin_handle_t handle,gpio_ic_direction_t dir);
gpio_ic_direction_t 	gpio_model_pin_get_direction(gpio_model_pin_handle_t handle);
void 					gpio_model_pin_release(gpio_model_pin_handle_t handle);

#endif
