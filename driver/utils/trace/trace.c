/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * trace.c
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
#include <linux/device.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/list.h>
#include "linux/slab.h"
#include "cxt_mgr.h"
#include "mem_model.h"
#include "trace.h"
#include "debug.h"

typedef struct
{
    BASIC_CXT_HANDLE_DECLARE;
    struct kobject *kobj;
    struct kobject *parent;
    struct attribute **attrs;
    struct attribute_group attr_group;
    queue_t attrs_queue;
    struct kobj_attribute basic_attr;
    struct attribute_group basic_group;
    struct attribute *basic_attrs[2];
    int attr_count;
} trace_context_t;

typedef struct trace_attr_s
{
    queue_t queue;
    struct kobj_attribute attr;
    unsigned int (*show)(void *cxt, char *buf);
    unsigned int (*store)(void *cxt, const char *buf, unsigned int count);
    const char *name;
    void *cxt;
} trace_attr_t;



static void trace_free_attr_list(trace_context_t *trace_cxt);
static void trace_update_group(trace_context_t *trace_cxt);
static void *trace_model_alloc(void);
static void trace_model_release(void *cxt);

#define to_trace_attr(x) container_of(x, trace_attr_t, attr)

static ssize_t trace_attr_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    trace_attr_t *attribute;
    attribute = to_trace_attr(attr);


    if (!attribute->show)
        return -EIO;

    return attribute->show(attribute->cxt, buf);
}

static ssize_t trace_attr_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t len)
{
    trace_attr_t *attribute;
    attribute = to_trace_attr(attr);

    if (!attribute->store)
        return -EIO;

    return attribute->store(attribute->cxt, buf, len);
}

static ssize_t trace_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    trace_context_t *trace_cxt = container_of(attr, trace_context_t, basic_attr);
    int count = 0;
    if (trace_cxt->kobj)
    {
        count += sprintf(buf, "yes\n");
    }
    else
    {
        count += sprintf(buf, "no\n");
    }

    return count;
}




static ssize_t trace_store(struct kobject *kobj, struct kobj_attribute *attr,
                           const char *buf, size_t count)
{
    trace_context_t *trace_cxt = container_of(attr, trace_context_t, basic_attr);
    const char *keyword_debug = "debug";
    const char *keyword_good = "good";
    if (strncmp(buf, keyword_debug, strlen(keyword_debug)) == 0)
    {
        if (!trace_cxt->kobj)
        {
            trace_model_update(trace_cxt);
        }
    }
    else if (strncmp(buf, keyword_good, strlen(keyword_good)) == 0)
    {
        if (trace_cxt->kobj)
        {
            kobject_put(trace_cxt->kobj);
            trace_cxt->kobj = NULL;
        }
        if (trace_cxt->attrs)
        {
            mem_model_free_buffer(trace_cxt->attrs);
            trace_cxt->attrs = NULL;
        }
    }

    return count;
}

trace_attr_handle_t trace_attr_create(trace_attr_setup_t *setup)
{
    trace_attr_t *attr = mem_model_alloc_buffer(sizeof (trace_attr_t));

    if (attr)
    {

        attr->show = setup->show;
        attr->store = setup->store;
        attr->cxt = setup->cxt;
        attr->attr.attr.name = mem_model_strdup(setup->name);
        attr->attr.attr.mode = 0644;
        attr->attr.show = trace_attr_show;
        attr->attr.store = trace_attr_store;

    }
    return (trace_attr_handle_t) attr;
}

void trace_attr_free(trace_attr_handle_t attr_handle)
{
    trace_attr_t *attr = attr_handle;

    if (attr)
    {
        //		printk("%s %s\n",__func__,attr->attr.attr.name);
        if (attr->attr.attr.name)
            mem_model_free_buffer((void *) attr->attr.attr.name);
        queue_del(&attr->queue);
        mem_model_free_buffer(attr);
    }

}

static __attribute__((unused)) void trace_free_attr_list(trace_context_t *trace_cxt)
{
    trace_attr_t *attr, *tmp;

    for_each_queue_entry_safe(attr, tmp, &trace_cxt->attrs_queue, queue)
    {
        printk("%s %s\n", __func__, attr->attr.attr.name);
        queue_del(&attr->queue);
        //mem_model_free(attr);
    }
}

static void trace_update_group(trace_context_t *trace_cxt)
{

    if (trace_cxt->attrs)
    {
        mem_model_free_buffer(trace_cxt->attrs);
        trace_cxt->attrs = NULL;
    }
    if (!trace_cxt->kobj)
    {
        trace_cxt->kobj = kobject_create_and_add(module_name(THIS_MODULE), trace_cxt->parent);
    }
    if (trace_cxt->attr_count)
    {

        trace_cxt->attrs = mem_model_alloc_buffer((trace_cxt->attr_count + 1) * sizeof (struct attribute *));
        ;
        if (trace_cxt->attrs)
        {
            trace_attr_t *attr;
            int i = 0;

            for_each_queue_entry(attr, &trace_cxt->attrs_queue, queue)
            {
                trace_cxt->attrs[i] = &attr->attr.attr;
                i++;
            }
            trace_cxt->attrs[i] = NULL;
            //printk("%d\n", trace_cxt->attr_count);
            for (i = 0; i < trace_cxt->attr_count; i++)
            {
                if (trace_cxt->attrs[i])
                {
                    //printk("[%d] %s\n", i, trace_cxt->attrs[i]->name);
                }

            }

        }
        trace_cxt->attr_group.attrs = trace_cxt->attrs;
    }
    printk("%s done\n", __func__);

}

static void *trace_model_alloc()
{
    trace_context_t *trace_cxt = NULL;

    trace_cxt = mem_model_alloc_buffer(sizeof (trace_context_t));


    return trace_cxt;
}

static void trace_model_release(void *cxt)
{
    trace_context_t *trace_cxt = cxt;

    printk("%s\n", __func__);

    if (trace_cxt)
    {
        uninit_debug();
        if (trace_cxt->kobj)
        {
            sysfs_remove_group(trace_cxt->kobj, &trace_cxt->attr_group);
            kobject_put(trace_cxt->kobj);
        }
        sysfs_remove_group(trace_cxt->parent, &trace_cxt->basic_group);
        if (trace_cxt->attrs)
        {
            //	    	trace_free_attr_list(trace_cxt);
            mem_model_free_buffer(trace_cxt->attrs);
        }
        mem_model_free_buffer(trace_cxt);
    }

}

void trace_model_add_attr(void *trace_context, trace_attr_handle_t attr_handle)
{
    trace_context_t *trace_cxt = (trace_context_t *) trace_context;
    trace_attr_t *attr = (trace_attr_t *) attr_handle;


    init_queue(&attr->queue);
    queue_add_tail(&attr->queue, &trace_cxt->attrs_queue);
    trace_cxt->attr_count++;

}

void trace_model_update(void *trace_context)
{
    trace_context_t *trace_cxt = (trace_context_t *) trace_context;

    trace_update_group(trace_cxt);
    sysfs_update_group(trace_cxt->kobj, &trace_cxt->attr_group);

}

void *trace_model_init(void *cxt_mgr)
{

    trace_context_t *trace_cxt = NULL;
    struct device *dev;
    int retval;

    enum
    {
        NO_ERROR,
        ERR_ALLOC_CXT,
        ERR_NO_DEV_KOBJ,
        ERR_CREATE_GROUP,
    } err = NO_ERROR;


    if (cxt_mgr)
    {

        do
        {

            trace_cxt = cxt_manager_add_cxt(cxt_mgr, TRACE_CXT_ID, trace_model_alloc, trace_model_release);
            if (!trace_cxt)
            {
                err = ERR_ALLOC_CXT;
                break;
            }

            dev = cxt_manager_get_dev(cxt_mgr);
            if (!dev)
            {
                err = ERR_NO_DEV_KOBJ;
                break;
            }

            trace_cxt->parent = &dev->kobj;
            //trace_cxt->kobj = kobject_create_and_add(module_name(THIS_MODULE), trace_cxt->parent);

            init_queue(&trace_cxt->attrs_queue);

            trace_cxt->basic_attr.attr.name = "trace";
            trace_cxt->basic_attr.attr.mode = 0644;
            trace_cxt->basic_attr.show = trace_show;
            trace_cxt->basic_attr.store = trace_store;
            
            trace_cxt->basic_group.attrs = trace_cxt->basic_attrs;
            trace_cxt->basic_group.attrs[0] = &trace_cxt->basic_attr.attr;
            trace_cxt->basic_group.attrs[1] = NULL;
            trace_cxt->attr_count = 0;

            retval = sysfs_create_group(&dev->kobj, &trace_cxt->basic_group);
            if (retval)
            {
                err = ERR_CREATE_GROUP;
                break;
            }
            init_debug(trace_cxt);

        }
        while (0);
        if (err != NO_ERROR)
        {
            printk("%s err %d\n", __func__, err);
            switch (err)
            {
            case ERR_CREATE_GROUP:
                kobject_put(trace_cxt->kobj);
            case ERR_NO_DEV_KOBJ:
                cxt_manager_unref_context(trace_cxt);
                trace_cxt = NULL;
            case ERR_ALLOC_CXT:
                break;
            default:
                break;
            }
        }
    }

    return trace_cxt;
}

void *trace_model_get_handle(cxt_mgr_handle_t cxt_mgr)
{

    trace_context_t *trace_cxt = NULL;

    if (cxt_mgr)
    {
        trace_cxt = cxt_manager_get_context(cxt_mgr, TRACE_CXT_ID, 0);
        if (trace_cxt)
            return trace_cxt;
    }
    return NULL;
}



