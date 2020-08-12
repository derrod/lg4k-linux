/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * cxt_mgr.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 
//#include <linux/device.h>
#include "cxt_mgr.h"
#include "mem_model.h"
#include "debug.h"

typedef struct
{
    queue_t queue;
    queue_t cxt_queue;
    struct device *dev;
} cxt_manager_t;


static queue_t cxt_mgr_head = {&cxt_mgr_head, &cxt_mgr_head};

static void cxt_item_ref(cxt_item_t *cxt_item)
{
    cxt_item->ref_count++;
}

static void cxt_item_unref(cxt_item_t *cxt_item)
{
    cxt_item->ref_count--;
    if (cxt_item->ref_count == 0)
    {
        // char *idc=(char *)&cxt_item->cxt_id;
        // printk("%s %c%c%c%c\n",__func__,idc[0],idc[1],idc[2],idc[3]);

        queue_del(&cxt_item->queue);
        if (cxt_item->release)
            cxt_item->release(cxt_item->context);
        mem_model_free_buffer(cxt_item);
    }
}

cxt_mgr_handle_t get_cxt_manager(device_handle_t device_handle)
{

    return sys_get_drvdata(device_handle);

}

void *cxt_manager_alloc(device_handle_t dev)
{
    cxt_manager_t *cxt_mgr = mem_model_alloc_buffer(sizeof (cxt_manager_t));

    if (cxt_mgr)
    {
        init_queue(&cxt_mgr->queue);
        init_queue(&cxt_mgr->cxt_queue);
        cxt_mgr->dev = dev;
        queue_add_tail(&cxt_mgr->queue, &cxt_mgr_head);
    }
    return (cxt_mgr_handle_t) cxt_mgr;
}

struct device *cxt_manager_get_dev(cxt_mgr_handle_t handle)
{
    cxt_manager_t *cxt_mgr = (cxt_manager_t *) handle;
    if (cxt_mgr)
    {
        return cxt_mgr->dev;
    }
    else
        return NULL;
}

void cxt_manager_release(cxt_mgr_handle_t handle)
{
    cxt_manager_t *cxt_mgr = (cxt_manager_t *) handle;
    cxt_item_t *pos, *next;

    queue_del(&cxt_mgr->queue);
    if (cxt_mgr)
    {

        for_each_queue_entry_safe(pos, next, &cxt_mgr->cxt_queue, queue)
        {
            cxt_item_unref(pos);
        }
    }

    mem_model_free_buffer(cxt_mgr);
    //	printk("%s done\n",__func__);
}

void *cxt_manager_add_cxt(cxt_mgr_handle_t handle, U32_T id, cxt_alloc_func_t *alloc_func, cxt_release_func_t *release_func)
{
    cxt_manager_t *cxt_mgr = (cxt_manager_t *) handle;
    cxt_item_t *cxt = NULL;
    U8_T index = 0;
    if (!cxt_mgr)
        return NULL;
    if (alloc_func)
    {
        cxt_item_t *pos;

        for_each_queue_entry(pos, &cxt_mgr->cxt_queue, queue)
        {
            if (pos->cxt_id == id)
                index++;
        }
        cxt = mem_model_alloc_buffer(sizeof (cxt_item_t));
        if (cxt)
        {
            init_queue(&cxt->queue);
            cxt->cxt_id = id;
            cxt->index = index;
            cxt->allocate = alloc_func;
            cxt->release = release_func;
            cxt->context = cxt->allocate();
            cxt->context->manager = handle;
            cxt->dev = cxt_mgr->dev;
            if (!cxt->context)
            {
                mem_model_free_buffer(cxt);
                cxt = NULL;
            }
            else
            {
                queue_add_tail(&cxt->queue, &cxt_mgr->cxt_queue);
                cxt_item_ref(cxt);
            }


        }
    }

    return (cxt) ? cxt->context : NULL;
}

void cxt_manager_ref_context(void *context)
{

    cxt_manager_t *cxt_mgr = get_cxt_manager_from_context(context);
    cxt_item_t *pos, *found = NULL;
    char *idc;

    if (cxt_mgr)
    {
        //debug_msg("%s %p cxt_mgr %p\n",__func__,context,cxt_mgr);
        for_each_queue_entry(pos, &cxt_mgr->cxt_queue, queue)
        {
            idc = (char *) &pos->cxt_id;
            //debug_msg("%p %c%c%c%c\n",pos->context, idc[0], idc[1], idc[2], idc[3]);

            if (pos->context == context)
            {
                //debug_msg("Found %p %c%c%c%c\n",context, idc[0], idc[1], idc[2], idc[3]);
                found = pos;
                cxt_item_ref(pos);
                break;
            }
        }
    }
    if (!cxt_mgr)
    {
        //		printk("%s can't get cxt_mgr from context\n",__func__);
    }
}

void *cxt_manager_get_context(cxt_mgr_handle_t handle, U32_T cxt_id, U8_T index)
{
    cxt_manager_t *cxt_mgr = (cxt_manager_t *) handle;
    cxt_item_t *pos, *found = NULL;

    if (cxt_mgr)
    {
        
        for_each_queue_entry(pos, &cxt_mgr->cxt_queue, queue)
        {
            //printk("%s %p %x\n",__func__,pos,pos->cxt_id);
            if (pos->cxt_id == cxt_id && pos->index == index)
            {
                found = pos;
                break;
            }
        }
    }
    return (found) ? found->context : NULL;
}

void cxt_manager_unref_context(void *context)
{
    cxt_manager_t *cxt_mgr = get_cxt_manager_from_context(context);
    cxt_item_t *pos, *found = NULL;
    char *idc;

    if (cxt_mgr)
    {
       
        for_each_queue_entry(pos, &cxt_mgr->cxt_queue, queue)
        {
            if (pos->context == context)
            {
                idc = (char *) &pos->cxt_id;
                //debug_msg("%s %c%c%c%c\n", __func__, idc[0], idc[1], idc[2], idc[3]);
                found = pos;
                cxt_item_unref(pos);
                break;
            }
        }
    }

}

cxt_mgr_handle_t get_cxt_manager_from_context(void *context)
{
    cxt_handle_t *cxt_handle = context;
    return cxt_handle->manager;
}

void cxt_item_release(cxt_item_t *cxt_item)
{
    queue_del(&cxt_item->queue);
    cxt_item_unref(cxt_item);
}


