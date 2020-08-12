/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * gpio_model.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include "cxt_mgr.h"
#include "gpio_model.h"

typedef struct gpio_model
{
	BASIC_CXT_HANDLE_DECLARE;
	struct list_head gpio_ic_list;
	struct list_head pin_handle_list;
}gpio_model_t;

typedef struct pin_desc_s
{
	struct list_head list;
	const char *pin_name;
	unsigned pin_no;
}pin_desc_t;

typedef struct gpio_ic_s
{
	struct list_head list;
	const char *name;
	gpio_ic_ops_t ops;
	struct list_head pin_list;
	void *context;
}gpio_ic_t;

typedef struct pin_handle
{
	struct list_head list;
	pin_desc_t *pin_desc;
	gpio_ic_t *gpio_ic;
}pin_handle_t;



static void *gpio_model_alloc(void);
static void gpio_model_release(void *context);
static gpio_ic_t *gpio_model_new_ic(const char *name,gpio_ic_ops_t *ops,void *context);
static void gpio_model_free_ic(gpio_ic_t *ic);
static gpio_ic_t *gpio_model_find_ic(gpio_model_t *model,const char *gpio_ic_name);
static pin_desc_t *gpio_model_find_pin(gpio_ic_t *ic,const char *pin_name);
static pin_desc_t *gpio_model_new_pindesc(const char *pin_name,unsigned int pin_no);
static void gpio_model_free_pindesc(pin_desc_t *pin_desc);


//static void gpio_module_probe(struct sagitta_module *module, struct sagitta_dev *sdev)
//{
//	struct gpio_module *this=(struct gpio_module *)module;
//
//	this->context=gpio_model_alloc();
//	printk("%s context %p\n",__func__,this->context);
//}
//
//static void gpio_module_disconnect(struct sagitta_module *module, struct sagitta_dev *sdev)
//{
//	struct gpio_module *this=(struct gpio_module *)module;
//	gpio_model_t *model=(gpio_model_t *)this->context;
//
//	if(model)
//	{
//		gpio_model_release(model);
//	}
//
//
//}
//
//void *gpio_module_get_context(struct sagitta_dev *sdev)
//{
//	struct sagitta_module *mod,*tmp;
//	list_for_each_entry_safe(mod,tmp,&sdev->root->modules,list)
//	{
//		if(strncmp(mod->name,GPIO_MODULE_ID_STR,strlen(mod->name))==0)
//		{
//			struct gpio_module *this=(struct gpio_module *)mod;
//			return this->context;
//		}
//	}
//	return NULL;
//
//}


static gpio_ic_t *gpio_model_new_ic(const char *name,gpio_ic_ops_t *ops,void *context)
{
	gpio_ic_t *new_ic;

	new_ic=kzalloc(sizeof(gpio_ic_t),GFP_KERNEL);
	if(new_ic)
	{
		INIT_LIST_HEAD(&new_ic->list);
		INIT_LIST_HEAD(&new_ic->pin_list);
		new_ic->name=name;
		memcpy(&new_ic->ops,ops,sizeof(gpio_ic_ops_t));
		new_ic->context=context;
	}
	return new_ic;
}

static void gpio_model_free_ic(gpio_ic_t *ic)
{

	if(!list_empty(&ic->pin_list))
	{
		pin_desc_t *pos,*tmp;
		list_for_each_entry_safe(pos,tmp,&ic->pin_list,list)
		{
			gpio_model_free_pindesc(pos);
		}
	}
	list_del(&ic->list);

	kfree(ic);
}

static pin_desc_t *gpio_model_new_pindesc(const char *pin_name,unsigned int pin_no)
{
	pin_desc_t *new_pin;

	new_pin=kzalloc(sizeof(pin_desc_t),GFP_KERNEL);
	if(new_pin)
	{
		INIT_LIST_HEAD(&new_pin->list);

		new_pin->pin_name=pin_name;
		new_pin->pin_no=pin_no;
	}
	return new_pin;
}

static void gpio_model_free_pindesc(pin_desc_t *pin_desc)
{
	list_del(&pin_desc->list);
	kfree(pin_desc);
}

static pin_handle_t *gpio_model_new_pin_handle(pin_desc_t *pin_desc,gpio_ic_t *gpio_ic)
{
	pin_handle_t *new_pin_handle;

	new_pin_handle=kzalloc(sizeof(pin_handle_t),GFP_KERNEL);
	if(new_pin_handle)
	{
		INIT_LIST_HEAD(&new_pin_handle->list);
		new_pin_handle->pin_desc=pin_desc;
		new_pin_handle->gpio_ic=gpio_ic;

	}
	return new_pin_handle;
}

static void gpio_model_free_pin_handle(pin_handle_t *pin_handle)
{
	list_del(&pin_handle->list);
	kfree(pin_handle);
}

static gpio_ic_t *gpio_model_find_ic(gpio_model_t *model,const char *gpio_ic_name)
{

	if(!list_empty(&model->gpio_ic_list))
	{
		gpio_ic_t *pos,*tmp;
		list_for_each_entry_safe(pos,tmp,&model->gpio_ic_list,list)
		{
			if(strncmp(pos->name,gpio_ic_name,strlen(pos->name))==0)
			{
				return pos;
			}
		}
	}
	return NULL;
}

static pin_desc_t *gpio_model_find_pin(gpio_ic_t *ic,const char *pin_name)
{

	if(!list_empty(&ic->pin_list))
	{
		pin_desc_t *pos,*tmp;
		list_for_each_entry_safe(pos,tmp,&ic->pin_list,list)
		{
			if(strncmp(pos->pin_name,pin_name,strlen(pos->pin_name))==0)
			{
				return pos;
			}
		}
	}
	return NULL;
}

//void gpio_module_init(struct sagitta_module *module)
//{
//	printk("%s\n",__func__);
//    module->probe = gpio_module_probe;
//	module->disconnect = gpio_module_disconnect;
//}
//
//void gpio_module_exit(struct sagitta_module *module)
//{
//
//}

static void *gpio_model_alloc()
{
	gpio_model_t *model=(gpio_model_t *)kzalloc(sizeof(gpio_model_t),GFP_KERNEL);

	if(model)
	{
		INIT_LIST_HEAD(&model->gpio_ic_list);
		INIT_LIST_HEAD(&model->pin_handle_list);
	}
	return model;
}

static void gpio_model_release(void *context)
{
	gpio_model_t *model=(gpio_model_t *)context;

	if(model)
	{
		if(!list_empty(&model->pin_handle_list))
		{
			pin_handle_t *pos,*tmp;
			list_for_each_entry_safe(pos,tmp,&model->pin_handle_list,list)
			{
				gpio_model_free_pin_handle(pos);
			}
		}
		if(!list_empty(&model->gpio_ic_list))
		{
			gpio_ic_t *pos,*tmp;
			list_for_each_entry_safe(pos,tmp,&model->pin_handle_list,list)
			{
				gpio_model_free_ic(pos);
			}
		}

		kfree(model);
	}
}

gpio_model_error_t 	gpio_model_register_ic(gpio_model_handle_t gpio_model_context,const char *gpio_ic_name,gpio_ic_ops_t *ops,void *context)
{
	gpio_model_t *model=(gpio_model_t *)gpio_model_context;
	gpio_ic_t *new_ic;

	if(gpio_model_find_ic(model,gpio_ic_name))
	{
		return GPIO_MODEL_ALREADY_REGISTERED;
	}

	new_ic=gpio_model_new_ic(gpio_ic_name,ops,context);
	if(new_ic==NULL)
	{
		return GPIO_MODEL_ALLOC_FAIL;

	}
	list_add_tail(&new_ic->list,&model->gpio_ic_list);

	return GPIO_MODEL_OK;
}

gpio_model_error_t 	gpio_model_unregister_ic(gpio_model_handle_t gpio_model_context,const char *gpio_ic_name)
{
	gpio_model_t *model=(gpio_model_t *)gpio_model_context;
	gpio_ic_t *ic=gpio_model_find_ic(model,gpio_ic_name);


	if(ic)
	{

		gpio_model_free_ic(ic);
		return GPIO_MODEL_OK;
	}

	return GPIO_MODEL_ERROR_NO_SUCH_NAME;
}

gpio_model_error_t 	gpio_model_register_pin(gpio_model_handle_t gpio_model_context,const char *gpio_ic_name,const char *pin_name,int pin_no)
{
	gpio_model_t *model=(gpio_model_t *)gpio_model_context;
	gpio_ic_t *ic=gpio_model_find_ic(model,gpio_ic_name);

	if(ic)
	{
		pin_desc_t *pin_desc;
		pin_desc=gpio_model_find_pin(ic,pin_name);
		if(pin_desc)
			return GPIO_MODEL_ALREADY_REGISTERED;
		pin_desc=gpio_model_new_pindesc(pin_name,pin_no);
		list_add_tail(&pin_desc->list,&ic->pin_list);

		return GPIO_MODEL_OK;
	}

	return GPIO_MODEL_ERROR_NO_SUCH_NAME;
}


gpio_model_pin_handle_t gpio_model_request_pin_handle(gpio_model_handle_t gpio_model_context,const char *pin_name)
{
	gpio_model_t *model=(gpio_model_t *)gpio_model_context;

	if(!list_empty(&model->gpio_ic_list))
	{
		gpio_ic_t *ic,*tmp_ic;
		list_for_each_entry_safe(ic,tmp_ic,&model->gpio_ic_list,list)
		{
			pin_desc_t *pin_desc=NULL;

			pin_desc=gpio_model_find_pin(ic,pin_name);
			if(pin_desc)
			{
				pin_handle_t *pin_handle;
				pin_handle=gpio_model_new_pin_handle(pin_desc,ic);
				if(pin_handle)
				{
					list_add_tail(&pin_handle->list,&model->pin_handle_list);
					return (void *)pin_handle;
				}
				break;
			}
		}
	}


	return GPIO_MODEL_BAD_PIN_HANDLE;
}

void gpio_model_pin_release(gpio_model_pin_handle_t handle)
{
	pin_handle_t *pin_handle=(pin_handle_t *)handle;

	gpio_model_free_pin_handle(pin_handle);

}

gpio_model_error_t gpio_model_pin_set(gpio_model_pin_handle_t handle,gpio_model_pin_value_t value)
{
	pin_handle_t *pin_handle=(pin_handle_t *)handle;

	if(pin_handle==GPIO_MODEL_BAD_PIN_HANDLE)
		return GPIO_MODEL_BAD_HANDLE;

	if(pin_handle->gpio_ic->ops.set)
	{
		pin_handle->gpio_ic->ops.set(pin_handle->gpio_ic->context,pin_handle->pin_desc->pin_no,value);
	}
	return GPIO_MODEL_OK;
}

gpio_model_pin_value_t gpio_model_pin_get(gpio_model_pin_handle_t handle)
{
	gpio_model_pin_value_t value=GPIOIC_VALUE_UNKNOWN;
	pin_handle_t *pin_handle=(pin_handle_t *)handle;

	if(pin_handle==GPIO_MODEL_BAD_PIN_HANDLE)
		return GPIO_MODEL_BAD_HANDLE;

	if(pin_handle->gpio_ic->ops.get)
	{
		value=pin_handle->gpio_ic->ops.get(pin_handle->gpio_ic->context,pin_handle->pin_desc->pin_no);
	}

	return value;
}

gpio_model_error_t 		gpio_model_pin_set_direction(gpio_model_pin_handle_t handle,gpio_ic_direction_t dir)
{
	pin_handle_t *pin_handle=(pin_handle_t *)handle;

	if(pin_handle==GPIO_MODEL_BAD_PIN_HANDLE)
		return GPIO_MODEL_BAD_HANDLE;

	if(pin_handle->gpio_ic->ops.set_direction)
	{
			pin_handle->gpio_ic->ops.set_direction(pin_handle->gpio_ic->context,pin_handle->pin_desc->pin_no,dir);
	}
	return GPIO_MODEL_OK;

}

gpio_ic_direction_t 	gpio_model_pin_get_direction(gpio_model_pin_handle_t handle)
{
	pin_handle_t *pin_handle=(pin_handle_t *)handle;
	gpio_ic_direction_t dir=GPIOIC_DIR_UNKNOWN;

	if(pin_handle==GPIO_MODEL_BAD_PIN_HANDLE)
		return GPIO_MODEL_BAD_HANDLE;

	if(pin_handle->gpio_ic->ops.get_direction)
	{
		dir=pin_handle->gpio_ic->ops.get_direction(pin_handle->gpio_ic->context,pin_handle->pin_desc->pin_no);
	}

	return dir;
}

gpio_model_handle_t gpio_model_init(cxt_mgr_handle_t cxt_mgr)
{
	gpio_model_t *gpio_model_context=NULL;
	if(cxt_mgr)
	{
		gpio_model_context=cxt_manager_add_cxt(cxt_mgr,GPIO_CXT_ID,gpio_model_alloc,gpio_model_release);
	}
	return (gpio_model_handle_t )gpio_model_context;
}



