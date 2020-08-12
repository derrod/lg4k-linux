/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * task_model.h
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

#ifndef THREAD_MODEL_H
#define THREAD_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif
    
    #define TASK_MODEL_CXT_ID fourcc_id('T','A','S','K')

    typedef void (*task_work_func_t)(void *data);
    typedef void (*task_done_callback_func_t)(void *data);
    typedef void *task_model_handle_t;
    typedef void *task_handle_t;
    

    task_model_handle_t task_model_init(cxt_mgr_handle_t cxt_mgr);
    
    void task_model_stop_task(task_model_handle_t task_model_handle,task_handle_t task_handle);
    task_handle_t task_model_create_task(task_model_handle_t task_model_handle,task_work_func_t work_func,void *work_cxt,const char *name);
    void task_model_release_task(task_model_handle_t task_model_handle,task_handle_t task_handle);
    void task_model_run_task(task_model_handle_t task_model_handle,task_handle_t task_handle);
    void task_model_run_task_after(task_model_handle_t task_model_handle,task_handle_t task_handle,unsigned int us);
    void task_model_run_task_period(task_model_handle_t task_model_handle,task_handle_t task_handle,unsigned int period_ms,int count,task_done_callback_func_t cb_func);
    void sys_run_task(task_handle_t task_handle);
    void sys_task_run_task_after(task_handle_t task_handle,unsigned int us);
    void sys_task_run_period(task_handle_t task_handle,unsigned int period_ms,int count,task_done_callback_func_t cb_func);
    void task_model_schedule_DPC(task_model_handle_t task_model_handle,task_handle_t task_handle);
    

    
#ifdef __cplusplus
}
#endif

#endif /* THREAD_MODEL_H */

