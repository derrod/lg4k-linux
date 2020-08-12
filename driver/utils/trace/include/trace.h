/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * trace.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
 
#ifndef __TRACE_H__
#define __TRACE_H__

//#include <linux/kobject.h>
#define TRACE_NAME trace

#define TRACE2STR(NAME) MODULE_STR(NAME)
#define TRACE_NAME_STR TRACE2STR(TRACE_NAME)


#define TRACE_CXT_ID fourcc_id('T','R','C','E')


typedef struct 
{
    const char *name;
    unsigned int (*show)(void *cxt, char *buf);
    unsigned int (*store)(void *cxt, const char *buf, unsigned int count);
    void *cxt;
}trace_attr_setup_t;

typedef void *trace_attr_handle_t;

trace_attr_handle_t trace_attr_create(trace_attr_setup_t *);
void trace_attr_free(trace_attr_handle_t attr_handle);
void trace_model_add_attr(void *trace_context,trace_attr_handle_t attr);

void trace_model_update(void *trace_context);
void *trace_model_init(void *cxt_mgr);
void *trace_model_get_handle(void *pcxt_mgr);


#endif
