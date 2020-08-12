/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * cxt_mgr.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#ifndef INCLUDE_CXT_MGR_H_
#define INCLUDE_CXT_MGR_H_

#include <linux/kernel.h>
#include "queue.h"
#include "typedef.h"
#include "sys.h"

typedef void *cxt_mgr_handle_t;

typedef void *cxt_alloc_func_t(void);
typedef void cxt_release_func_t(void *);
typedef struct
{
	cxt_mgr_handle_t manager;
}cxt_handle_t;

typedef struct
{
	queue_t queue;
	U32_T	 cxt_id;
	U8_T 	 index;
	cxt_alloc_func_t *allocate;
	cxt_release_func_t *release;
	struct device *dev;
	cxt_handle_t *context;
	int ref_count;
}cxt_item_t;

#define fourcc_id(a, b, c, d)\
	((U32_T)(a) | ((U32_T)(b) << 8) | ((U32_T)(c) << 16) | ((U32_T)(d) << 24))

#define BASIC_CXT_HANDLE_DECLARE cxt_handle_t basic_handle

void *cxt_manager_alloc(device_handle_t dev);
cxt_mgr_handle_t get_cxt_manager(device_handle_t device_handle);
void cxt_manager_release(cxt_mgr_handle_t cxt_mgr);
void *cxt_manager_add_cxt(cxt_mgr_handle_t cxt_mgr,U32_T id,cxt_alloc_func_t *alloc_func,cxt_release_func_t *release_func);
void cxt_manager_ref_context(void *context);
void cxt_manager_unref_context(void *context);
void *cxt_manager_get_context(cxt_mgr_handle_t cxt_mgr,U32_T cxt_id,U8_T index);
void cxt_item_release(cxt_item_t *cxt_item);
struct device *cxt_manager_get_dev(cxt_mgr_handle_t cxt_mgr);
cxt_mgr_handle_t get_cxt_manager_from_context(void *context);


#endif /* INCLUDE_CXT_MGR_H_ */
