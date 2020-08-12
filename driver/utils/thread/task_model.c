/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * task_model.c
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
 
#include <linux/module.h>
#include <linux/list.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include "cxt_mgr.h"
#include "mem_model.h"
#include "task_model.h"
#include "debug.h"

typedef struct
 {
    queue_t queue;
    const char *name;
    task_work_func_t work_func;
    void *work_cxt;
 }task_model_task_t;
    

 typedef struct
{
    BASIC_CXT_HANDLE_DECLARE;
    struct workqueue_struct *wq;
    queue_t active_taskcxt_queue;
    queue_t free_taskcxt_queue;
    queue_t task_queue;
    struct tasklet_struct tasklet;
    queue_t DPC_queue;
    atomic_t need_reschedue_DPC;
    mem_model_pool_handle_t taskcxt_pool; 
    mem_model_pool_handle_t task_pool; 
    spinlock_t free_lock;
    spinlock_t active_lock;
    spinlock_t DPC_lock;
    handle_t memmgr;
}task_model_cxt_t;

typedef struct
{
//    task_handle_t handle;
    queue_t queue;
    struct delayed_work dwork;
    task_model_task_t *task;
    bool is_period;
    unsigned long period_jiffies;
    int remain_count;
    task_done_callback_func_t cb_func;
    struct workqueue_struct *wq;
    task_model_cxt_t *task_model_cxt;
}task_cxt_t;





static task_cxt_t *task_model_get_taskcxt(task_model_cxt_t *task_model_cxt,task_model_task_t *task_info);
static void task_model_tasklet_func(unsigned long data);
static void task_model_release_taskcxt(task_model_cxt_t *task_model_cxt,task_cxt_t *task_cxt);
static void task_model_cancel_task(task_cxt_t *task_cxt);
static void task_model_prepare_task(task_model_cxt_t *task_model_cxt,task_cxt_t *task_cxt);
static void task_model_queue_task(task_cxt_t *task_cxt);
static void sys_work_func(struct work_struct *work);
static void task_model_add_free_taskcxt(task_model_cxt_t *task_model_cxt,queue_t *queue);

static void *task_model_alloc(void)
{
    task_model_cxt_t *task_model_cxt=NULL;
    task_model_cxt=mem_model_alloc_buffer(sizeof(task_model_cxt_t));
    return task_model_cxt;
}

static void task_model_release(void *context)
{
   task_model_cxt_t *task_model_cxt=context;
   if(task_model_cxt)
   {
      
       if(!queue_empty(&task_model_cxt->active_taskcxt_queue))
       {
           task_cxt_t *pos,*next;
           for_each_queue_entry_safe(pos,next,&task_model_cxt->active_taskcxt_queue,queue)
           {
               task_model_release_taskcxt(task_model_cxt,pos);
           }
       }
       if(!queue_empty(&task_model_cxt->task_queue))
       {
           task_model_task_t *pos,*next;
           for_each_queue_entry_safe(pos,next,&task_model_cxt->task_queue,queue)
           {
               queue_del(&pos->queue);
               mem_model_pool_free(task_model_cxt->task_pool,pos);
           }
       }
       
       destroy_workqueue(task_model_cxt->wq);
       tasklet_kill(&task_model_cxt->tasklet);
       mem_model_release_pool(task_model_cxt->taskcxt_pool);
       mem_model_release_pool(task_model_cxt->task_pool);
       if(task_model_cxt->memmgr)
           cxt_manager_unref_context(task_model_cxt->memmgr);
       mem_model_free_buffer(task_model_cxt);
       
   }
    
}

task_model_handle_t task_model_init(cxt_mgr_handle_t cxt_mgr)
{
    task_model_cxt_t *task_model_cxt=NULL;
    handle_t mem_mgr=NULL;
    enum
    {
        NO_ERROR,
        NO_CXT_MGR,
        ERROR_ALLOC_CXT,
        ERROR_CREATE_WQ,
        ERROR_GET_MEM_MGR,
        ERROR_ALLOC_TASK_MEMPOOL,
        ERROR_ALLOC_TASKCXT_MEMPOOL,
    }err=NO_ERROR;
    
    do
    {
        const char *name;
        if(!cxt_mgr)
        {
            err=NO_CXT_MGR;
            break;
        }
        task_model_cxt=cxt_manager_add_cxt(cxt_mgr,TASK_MODEL_CXT_ID,task_model_alloc,task_model_release);
        if(!task_model_cxt)
        {
            err=ERROR_ALLOC_CXT;
            break;
        }
        mem_mgr=cxt_manager_get_context(cxt_mgr,MEM_CXT_ID,0);
        if(!mem_mgr)
        {
            err=ERROR_GET_MEM_MGR;
            break;
        }
        task_model_cxt->memmgr=mem_mgr;
        cxt_manager_ref_context(mem_mgr);
        name=module_name(THIS_MODULE);
        task_model_cxt->wq=create_workqueue(name);
        if(!task_model_cxt->wq)
        {
            err=ERROR_CREATE_WQ;
            break;
        }
        task_model_cxt->task_pool=mem_model_create_pool(mem_mgr,sizeof(task_model_task_t));
        if(!task_model_cxt->task_pool)
        {
            err=ERROR_ALLOC_TASK_MEMPOOL;
            break;
        }
        task_model_cxt->taskcxt_pool=mem_model_create_pool(mem_mgr,sizeof(task_cxt_t));
        if(!task_model_cxt->taskcxt_pool)
        {
            err=ERROR_ALLOC_TASKCXT_MEMPOOL;
            break;
        }
        init_queue(&task_model_cxt->task_queue);
        init_queue(&task_model_cxt->active_taskcxt_queue);
        init_queue(&task_model_cxt->free_taskcxt_queue);
        init_queue(&task_model_cxt->DPC_queue);
        spin_lock_init(&task_model_cxt->free_lock);
        spin_lock_init(&task_model_cxt->active_lock);
        spin_lock_init(&task_model_cxt->DPC_lock);
        tasklet_init(&task_model_cxt->tasklet, task_model_tasklet_func, (unsigned long)task_model_cxt);
        atomic_set(&task_model_cxt->need_reschedue_DPC,1);
        
    }while(0);
    if(err!=NO_ERROR)
    {
        mesg_err("%s err %d\n",__func__,err);
        switch(err)
        {
        
            case ERROR_CREATE_WQ:
                cxt_manager_unref_context(task_model_cxt);
                task_model_cxt=NULL;
            case ERROR_GET_MEM_MGR:
            case ERROR_ALLOC_CXT:
            case NO_CXT_MGR:
                break;
            default:
                destroy_workqueue(task_model_cxt->wq);
                break;
        }
    }
    
    return (task_model_handle_t) task_model_cxt;
    
}

static void task_model_prepare_task(task_model_cxt_t *task_model_cxt,task_cxt_t *task_cxt)
{
    if(task_cxt)
    {
        INIT_DELAYED_WORK(&task_cxt->dwork,sys_work_func);
        if(task_model_cxt)
        {
            task_cxt->wq=task_model_cxt->wq;
            spin_lock(&task_model_cxt->active_lock);
            queue_add_tail(&task_cxt->queue,&task_model_cxt->active_taskcxt_queue);
            spin_unlock(&task_model_cxt->active_lock);
            
        }else
        {
            task_cxt->wq=NULL;
        }
    }
}

static void task_model_queue_task(task_cxt_t *task_cxt)
{
    if(task_cxt)
    {
        if(task_cxt->wq)
            queue_delayed_work(task_cxt->wq,&task_cxt->dwork,task_cxt->period_jiffies);
        else
            schedule_delayed_work(&task_cxt->dwork,task_cxt->period_jiffies);
            
    }
}

static void task_model_add_free_taskcxt(task_model_cxt_t *task_model_cxt,queue_t *queue)
{
    unsigned long flags;
    
    spin_lock_irqsave(&task_model_cxt->free_lock,flags);
    queue_add_tail(queue,&task_model_cxt->free_taskcxt_queue);
    spin_unlock_irqrestore(&task_model_cxt->free_lock,flags);
}

static task_cxt_t *task_model_get_free_taskcxt(task_model_cxt_t *task_model_cxt)
{
    unsigned long flags;
    task_cxt_t *taskcxt;
    
    spin_lock_irqsave(&task_model_cxt->free_lock,flags);
    taskcxt=queue_first_entry_or_null(&task_model_cxt->free_taskcxt_queue,task_cxt_t,queue);
    if(taskcxt)
         queue_del(&taskcxt->queue);
    spin_unlock_irqrestore(&task_model_cxt->free_lock,flags);
    
    return taskcxt;
}

static task_cxt_t *task_model_get_DPC_queue(task_model_cxt_t *task_model_cxt)
{
    task_cxt_t *task_cxt=NULL;
    unsigned long flags;
    
    spin_lock_irqsave(&task_model_cxt->DPC_lock,flags);
    task_cxt=queue_first_entry_or_null(&task_model_cxt->DPC_queue,task_cxt_t,queue);
    if(task_cxt)
        queue_del(&task_cxt->queue);
    spin_unlock_irqrestore(&task_model_cxt->DPC_lock,flags);
    return task_cxt;
}

static void task_model_tasklet_func(unsigned long data) //check
{
        task_model_cxt_t *task_model_cxt=(task_model_cxt_t *)data;
        task_cxt_t *task_cxt;
        queue_t task_queue;
        task_cxt_t *pos,*next;
        int count=0;
        
        init_queue(&task_queue);
        while((task_cxt=task_model_get_DPC_queue(task_model_cxt))!=NULL)
        {
            queue_add_tail(&task_cxt->queue,&task_queue);
            count++;
        }
                
        //mesg("[%d",count);
        if(count)
        {
            for_each_queue_entry_safe(pos,next,&task_queue,queue)
            {
                if(pos->task)
                {
                     task_model_task_t *task=pos->task;

                    if(task->work_func)
                       task->work_func(task->work_cxt);
                 }
                 task_model_add_free_taskcxt(task_model_cxt,&pos->queue);   
            }
        }
        //mesg("]\n");
        if(!queue_empty(&task_model_cxt->DPC_queue))
        {
            tasklet_schedule(&task_model_cxt->tasklet);
            //mesg(";");
        }else
        {
            atomic_set(&task_model_cxt->need_reschedue_DPC,1);
            #if 1 //In order to avoid video stop while aging test
            if(!queue_empty(&task_model_cxt->DPC_queue)) {
                printk("interrupt add que Detection\n");
                atomic_set(&task_model_cxt->need_reschedue_DPC,0);
                tasklet_schedule(&task_model_cxt->tasklet);
            }
            #endif
        } 
}

static void sys_work_func(struct work_struct *work)
{
    struct delayed_work *dwork=to_delayed_work(work);
    task_cxt_t *task_cxt=container_of(dwork,task_cxt_t,dwork);
    task_model_cxt_t *task_model_cxt=task_cxt->task_model_cxt;
    
    if(task_cxt->task)
    {
        task_model_task_t *task=task_cxt->task;
        
        if(task->work_func)
            task->work_func(task->work_cxt);
    }
    if(task_cxt->is_period)
    {
        if(task_cxt->remain_count>0)
        {
            task_cxt->remain_count--;
        }
        if(task_cxt->remain_count>0)
        {
            task_model_queue_task(task_cxt);
        }else if(task_cxt->remain_count==0)
        {
            if(task_cxt->cb_func)
                task_cxt->cb_func(task_cxt->task->work_cxt);
            queue_del(&task_cxt->queue);
            task_model_add_free_taskcxt(task_model_cxt,&task_cxt->queue);
            
        }
        else if(task_cxt->remain_count==-1)
        {
            task_model_queue_task(task_cxt);
        }
    }else
    {
        queue_del(&task_cxt->queue);
        task_model_add_free_taskcxt(task_model_cxt,&task_cxt->queue);
        
    }
        
}

static task_cxt_t *task_model_get_taskcxt(task_model_cxt_t *task_model_cxt,task_model_task_t *task)
{
    task_cxt_t *taskcxt=NULL; 
    
    if(task_model_cxt)
    {
        taskcxt=task_model_get_free_taskcxt(task_model_cxt);
                
        if(taskcxt==NULL)
        {
            
            taskcxt=mem_model_pool_alloc(task_model_cxt->taskcxt_pool);
        }
        if(taskcxt)
        {
            //mesg("%s taskcxt %p\n",__func__,taskcxt);
             memset(taskcxt,0,sizeof(task_cxt_t));
             taskcxt->task=task;
             taskcxt->task_model_cxt=task_model_cxt;
             init_queue(&taskcxt->queue);
        }
        
    }
    return taskcxt;
}

static task_model_task_t *task_model_new_task(task_model_cxt_t *task_model_cxt)
{
    task_model_task_t *task=NULL; 
    if(task_model_cxt)
    {
        task=mem_model_pool_alloc(task_model_cxt->task_pool);
        if(task)
        {
            
            memset(task,0,sizeof(task_model_task_t));
            init_queue(&task->queue);
            queue_add_tail(&task->queue,&task_model_cxt->task_queue);
        }
    }
    return task;
}



task_handle_t task_model_create_task(task_model_handle_t task_model_handle,task_work_func_t work_func,void *work_cxt,const char *name)
{
    task_model_task_t *task=NULL;
    
    task=task_model_new_task(task_model_handle);
    if(task)
    {
        task->name=name;
        task->work_func=work_func;
        task->work_cxt=work_cxt;
    }
    debug_msg("%s %s %p\n",__func__,task->name,task);
    
    return task;
}

void task_model_release_task(task_model_handle_t task_model_handle,task_handle_t task_handle)
{
    task_model_cxt_t *task_model_cxt=task_model_handle;
    task_model_task_t *task=task_handle;
    
    if(task)
    {
        task_model_stop_task(task_model_handle,task_handle);
        queue_del(&task->queue);
        mem_model_pool_free(task_model_cxt->task_pool,task);    
    }
    
    
}
static void task_model_cancel_task(task_cxt_t *task_cxt)
{
    if(task_cxt)
    {
        cancel_delayed_work_sync(&task_cxt->dwork);
        queue_del(&task_cxt->queue);
    }
}


void task_model_stop_task(task_model_handle_t task_model_handle,task_handle_t task_handle)
{
    task_model_cxt_t *task_model_cxt=task_model_handle;
    task_model_task_t *task=task_handle;
    
    
    if(task)
    {
        debug_msg("%s %s\n",__func__,task->name);
        if(!queue_empty(&task_model_cxt->active_taskcxt_queue))
        {
           task_cxt_t *pos,*next;
           
           for_each_queue_entry_safe(pos,next,&task_model_cxt->active_taskcxt_queue,queue)
           {
       
               if(pos->task==task)
               {
       
                    task_model_release_taskcxt(task_model_cxt,pos);
               }
           }
       }
       
       if(!queue_empty(&task_model_cxt->DPC_queue))
       {
           task_cxt_t *pos,*next;
       
           for_each_queue_entry_safe(pos,next,&task_model_cxt->DPC_queue,queue)
           {
               
               if(pos->task==task)
               {
                   task_model_release_taskcxt(task_model_cxt,pos);
               }    
           }
       }
    }
    
}

static void task_model_release_taskcxt(task_model_cxt_t *task_model_cxt,task_cxt_t *task_cxt)
{
        
    if(task_cxt)
    {
        task_model_cancel_task(task_cxt);
        mem_model_pool_free(task_model_cxt->taskcxt_pool,task_cxt);
    }
}

//void task_model_release_tasklet(tasklet_handle_t tasklet_handle)
//{
//    tasklet_cxt_t *task_cxt=(tasklet_cxt_t *)tasklet_handle;
//    if(task_cxt)
//    {
//        if(!queue_empty(&task_cxt->queue))
//            queue_del(&task_cxt->queue);
//        mem_model_free_buffer(task_cxt);
//    }
//}


void task_model_schedule_DPC(task_model_handle_t task_model_handle,task_handle_t task_handle)
{
    task_model_cxt_t *task_model_cxt=task_model_handle;
    task_model_task_t *task=task_handle;
    task_cxt_t *task_cxt=task_model_get_taskcxt(task_model_cxt,task);
    unsigned long flags;
    
    if(task_cxt)
    {
        spin_lock_irqsave(&task_model_cxt->DPC_lock,flags);
        queue_add_tail(&task_cxt->queue,&task_model_cxt->DPC_queue);
        spin_unlock_irqrestore(&task_model_cxt->DPC_lock,flags);
        if(atomic_read(&task_model_cxt->need_reschedue_DPC)==1)
        {
            atomic_set(&task_model_cxt->need_reschedue_DPC,0);
            tasklet_schedule(&task_model_cxt->tasklet);
            //mesg(".");
        }
    }
    
}

void task_model_run_task(task_model_handle_t task_model_handle,task_handle_t task_handle)
{
    task_model_cxt_t *task_model_cxt=task_model_handle;
    task_model_task_t *task=task_handle;
    task_cxt_t *task_cxt=task_model_get_taskcxt(task_model_cxt,task);
    
    if(task_cxt)
    {
        
        task_cxt->is_period=false;
        task_cxt->period_jiffies=0;
        
        task_model_prepare_task(task_model_cxt,task_cxt);
        task_model_queue_task(task_cxt);
    }
}

void task_model_run_task_after(task_model_handle_t task_model_handle,task_handle_t task_handle,unsigned int us)
{
    task_model_cxt_t *task_model_cxt=task_model_handle;
    task_model_task_t *task=task_handle;
    task_cxt_t *task_cxt=task_model_get_taskcxt(task_model_cxt,task);
    
    
    if(task_cxt)
    {
        task_cxt->is_period=false;
        task_cxt->period_jiffies=usecs_to_jiffies(us);
        task_model_prepare_task(task_model_cxt,task_cxt);
        task_model_queue_task(task_cxt);
    }
    
}

void task_model_run_task_period(task_model_handle_t task_model_handle,task_handle_t task_handle,unsigned int period_ms,int count,task_done_callback_func_t cb_func)
{
    task_model_cxt_t *task_model_cxt=task_model_handle;
    task_model_task_t *task=task_handle;
    task_cxt_t *task_cxt=task_model_get_taskcxt(task_model_cxt,task);
    
    if(task_cxt)
    {
        task_cxt->is_period=true;
        task_cxt->period_jiffies=msecs_to_jiffies(period_ms);
        task_cxt->remain_count=count;
        task_cxt->cb_func=cb_func;
        task_model_prepare_task(task_model_cxt,task_cxt);
        task_model_queue_task(task_cxt);
        
    }
}

void sys_run_task(task_handle_t task_handle)
{
    task_model_task_t *task=task_handle;
    task_model_run_task(NULL,task);
}

void sys_task_run_task_after(task_handle_t task_handle,unsigned int us)
{
    task_model_task_t *task=task_handle;
    task_model_run_task_after(NULL,task,us);
}

void sys_task_run_period(task_handle_t task_handle,unsigned int period_ms,int count,task_done_callback_func_t cb_func)
{
    task_model_task_t *task=task_handle;
    task_model_run_task_period(NULL,task,period_ms,count, cb_func);
}
