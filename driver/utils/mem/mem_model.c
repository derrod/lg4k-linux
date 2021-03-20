/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * mem_model.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " "%s, %d: " fmt, __func__, __LINE__

#include <linux/version.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/mempool.h>
#include <linux/dma-mapping.h>
#include <linux/dmapool.h>
#include <linux/vmalloc.h>
#include "cxt_mgr.h"
#include "mem_model.h"
#include "debug.h"
#include "aver_xilinx.h"

#define MIN_NR 1
typedef struct
{
    BASIC_CXT_HANDLE_DECLARE;
    struct device *dev;
    queue_t dma_pool_queue;
    mempool_t *poolitem_mempool;
    mempool_t *bufferitem_mempool;
    queue_t mem_pool_queue;
}mem_model_cxt_t;


typedef struct
{
    queue_t queue;
    mem_model_dma_pool_cfg_t cfg;
    struct dma_pool *dma_pool;
    queue_t buffer_queue;
}mem_model_dma_pool_item_t;

typedef struct
{
    queue_t queue;
    mem_model_dma_buffer_t dma_buffer;
    
}mem_model_dma_buffer_item_t;

typedef struct
{
    queue_t queue;
    mempool_t *pool;
    unsigned int size;
    unsigned int ref;
}mem_model_pool_item_t;

        


static void *mem_model_alloc(void);
static void mem_model_release(void *context);

static mem_model_dma_pool_item_t *mem_model_alloc_pool_item(mem_model_cxt_t *mem_model_cxt);
static void mem_model_free_pool_item(mem_model_cxt_t *mem_model_cxt,mem_model_dma_pool_item_t *pool_item);
static mem_model_dma_pool_item_t *find_match_dma_pool_cfg(mem_model_cxt_t *mem_model_cxt,mem_model_dma_pool_cfg_t *cfg);
static mem_model_dma_buffer_item_t *mem_model_new_buffer_item(mem_model_cxt_t *mem_model_cxt);
static void mem_model_free_buffer_item(mem_model_cxt_t *mem_model_cxt,mem_model_dma_buffer_item_t *buffer_item);
static mem_model_dma_buffer_t *mem_model_dma_pool_alloc_mem(mem_model_handle_t mem_model_handle,mem_model_dma_pool_cfg_t *cfg,gfp_t gfp_flags);

void *mem_model_alloc_buffer(unsigned int  size)
{
    void *buf=NULL;
    
    buf=kzalloc(size,GFP_KERNEL);
    return buf;
}

void *mem_model_alloc_atomic(unsigned int  size)
{
    void *buf=NULL;
    
    buf=kzalloc(size,GFP_ATOMIC);
    return buf;
    
}
void mem_model_free_buffer(void *buf)
{
    kfree(buf);
}

void mem_model_memset(void *s,int c,unsigned int n)
{
    memset(s,c,n);
}

int mem_model_memcmp(const void *dest, const void *src, unsigned  n)
{
    return memcmp(dest,src,n);
}

void *mem_model_memmove(void *dest, const void *src, unsigned  n)
{
    return memmove(dest,src,n);
}

void *mem_model_memcpy(void *dest, const void *src, unsigned  n)
{
    return memcpy(dest,src,n);
}

char *mem_model_strdup(const char *str)
{
    return kstrdup(str,GFP_KERNEL);
}

void *mem_model_alloc_dma_buffer(mem_model_handle_t mem_model_handle,unsigned int size,mem_model_dma_buffer_t *dma_buffer)
{
    mem_model_cxt_t *mem_model_cxt=mem_model_handle;
    void *vaddr=NULL;
    if(dma_buffer)
    {
               
        dma_addr_t dma_addr;
        
        #if LINUX_VERSION_CODE >= KERNEL_VERSION(5,0,0)
        vaddr=dma_alloc_coherent(mem_model_cxt->dev, size, &dma_addr, GFP_KERNEL);
        //mesg("%s..\n",__func__);
        #else
        vaddr=dma_zalloc_coherent(mem_model_cxt->dev, size, &dma_addr, GFP_KERNEL);
        //mesg("%s\n",__func__);
        #endif

        if(vaddr)
        {
            dma_buffer->phys_addr=(unsigned long int)dma_addr;
            dma_buffer->vaddr=vaddr;
            dma_buffer->size=size;
        }
        
    }
    return vaddr;
}

void mem_model_free_dma_buffer(mem_model_handle_t mem_model_handle,mem_model_dma_buffer_t *dma_buffer)
{
    mem_model_cxt_t *mem_model_cxt=mem_model_handle;
    if(dma_buffer)
    {
         dma_free_coherent(mem_model_cxt->dev, dma_buffer->size, dma_buffer->vaddr,dma_buffer->phys_addr);
        
    }
    
}


static mem_model_dma_pool_item_t *mem_model_alloc_pool_item(mem_model_cxt_t *mem_model_cxt)
{
    mem_model_dma_pool_item_t *pool_item=NULL;
    
    if(mem_model_cxt)
    {
        pool_item=mempool_alloc(mem_model_cxt->poolitem_mempool,GFP_KERNEL);
        if(pool_item)
        {
            init_queue(&pool_item->queue);
            init_queue(&pool_item->buffer_queue);
        }
    }
    return pool_item;
}

static void mem_model_free_pool_item(mem_model_cxt_t *mem_model_cxt,mem_model_dma_pool_item_t *pool_item)
{
    if(mem_model_cxt)
    {
        queue_del(&pool_item->queue);
        mempool_free(pool_item,mem_model_cxt->poolitem_mempool); 
    }
}

static mem_model_dma_buffer_item_t *mem_model_new_buffer_item(mem_model_cxt_t *mem_model_cxt)
{
    mem_model_dma_buffer_item_t *buffer_item=NULL;
    
    if(mem_model_cxt)
    {
        buffer_item=mempool_alloc(mem_model_cxt->bufferitem_mempool,GFP_KERNEL);
        if(buffer_item)
        {
            init_queue(&buffer_item->queue);
        }
    }
    return buffer_item;
}

static void mem_model_free_buffer_item(mem_model_cxt_t *mem_model_cxt,mem_model_dma_buffer_item_t *buffer_item)
{
    if(buffer_item)
    {
        queue_del(&buffer_item->queue);
        mempool_free(buffer_item,mem_model_cxt->bufferitem_mempool); 
    }
}




static mem_model_dma_pool_item_t *find_match_dma_pool_cfg(mem_model_cxt_t *mem_model_cxt,mem_model_dma_pool_cfg_t *cfg)
{
    mem_model_dma_pool_item_t *pos,*found=NULL;
    for_each_queue_entry(pos,&mem_model_cxt->dma_pool_queue,queue)
    {
        if(pos->cfg.name==cfg->name)
        {
            found=pos;
            break;
        }
    }
    return found;
}


static mem_model_dma_buffer_t *mem_model_dma_pool_alloc_mem(mem_model_handle_t mem_model_handle,mem_model_dma_pool_cfg_t *cfg,gfp_t gfp_flags)
{
    mem_model_cxt_t *mem_model_cxt=mem_model_handle;
    mem_model_dma_pool_item_t *dma_pool=NULL;
    mem_model_dma_buffer_t *dma_buffer=NULL;
    
    
    dma_pool=find_match_dma_pool_cfg(mem_model_cxt,cfg);
    if(!dma_pool)
    {
        struct dma_pool *pool=NULL;
        
        pool=dma_pool_create(cfg->name, mem_model_cxt->dev,cfg->size, cfg->align, cfg->boundary);
        if(pool)
        {
            dma_pool=mem_model_alloc_pool_item(mem_model_cxt);
            if( dma_pool)
            {
                dma_pool->dma_pool=pool;
                dma_pool->cfg=*cfg;
                queue_add_tail(&dma_pool->queue,&mem_model_cxt->dma_pool_queue);
            }
        }
    }
    
    if(dma_pool)
    {
        dma_addr_t dma_addr;
        void *vaddr=NULL;
        
        vaddr=dma_pool_alloc(dma_pool->dma_pool,gfp_flags,&dma_addr);
        if(vaddr)
        {
            mem_model_dma_buffer_item_t  *buffer_item=NULL;
            buffer_item=mem_model_new_buffer_item(mem_model_cxt);
            if( buffer_item)
            {
                
                
                dma_buffer=&buffer_item->dma_buffer;
                //dma_buffer->phys_addr=(unsigned long int)dma_addr;
                dma_buffer->phys_addr=dma_addr;
                dma_buffer->vaddr=vaddr;
                dma_buffer->size=dma_pool->cfg.size;
                queue_add_tail(&buffer_item->queue,&dma_pool->buffer_queue);
                debug_msg("%s dma_addr %llx dma_buffer->phys_addr %lx\n",__func__,dma_addr,dma_buffer->phys_addr);
            }
        }   
    }

    return dma_buffer;
}

mem_model_dma_buffer_t *mem_model_dma_pool_alloc(mem_model_handle_t mem_model_handle,mem_model_dma_pool_cfg_t *cfg)
{
    return mem_model_dma_pool_alloc_mem(mem_model_handle,cfg,GFP_KERNEL);
}

mem_model_dma_buffer_t *mem_model_dma_pool_alloc_atomic(mem_model_handle_t mem_model_handle,mem_model_dma_pool_cfg_t *cfg)
{
    return mem_model_dma_pool_alloc_mem(mem_model_handle,cfg,GFP_ATOMIC);
}

mem_model_dma_pool_cfg_t dma_pool_cfg ={
        .name = "aver_xilinx",
        .size = sizeof (aver_xilinx_desc_t) * AVER_XILINX_MAX_DESC_PER_DESC_LIST,
        .align = 0,
        .boundary = 0,
};

mem_model_dma_buffer_t *mem_model_dma_pool_alloc_video(mem_model_handle_t mem_model_handle)
{
    return mem_model_dma_pool_alloc_mem(mem_model_handle,&dma_pool_cfg,GFP_ATOMIC);
}

mem_model_dma_pool_cfg_t audio_dma_pool_cfg ={
                .name = "aver_xilinx_aud",
                .size = 11520*4,
                .align = 0,
                .boundary = 0,
};

mem_model_dma_buffer_t *mem_model_dma_pool_alloc_audio(mem_model_handle_t mem_model_handle)
{
    return mem_model_dma_pool_alloc_mem(mem_model_handle,&audio_dma_pool_cfg,GFP_ATOMIC);
}

void mem_model_dma_pool_free(mem_model_handle_t mem_model_handle,mem_model_dma_buffer_t *dma_buffer)
{
    mem_model_cxt_t *mem_model_cxt=mem_model_handle;
    mem_model_dma_buffer_item_t *buffer_item;
    
    
    if(dma_buffer)
    {
        buffer_item=container_of(dma_buffer,mem_model_dma_buffer_item_t,dma_buffer);
        
        mem_model_free_buffer_item(mem_model_cxt,buffer_item);
    }
    
}

mem_model_pool_handle_t mem_model_create_pool(mem_model_handle_t mem_model_handle,unsigned int size)
{
    mem_model_cxt_t *mem_model_cxt = (mem_model_cxt_t *) mem_model_handle;
    mempool_t *pool = NULL;
    mem_model_pool_item_t *pool_item = NULL;
    mem_model_pool_item_t *pos;

    if(!queue_empty(&mem_model_cxt->mem_pool_queue))
    {
        for_each_queue_entry(pos, &mem_model_cxt->mem_pool_queue, queue)
        {
            if (pos->size == size)
            {
                pool_item = pos;
                break;
            }
        }
    }

    pool = mempool_create_kmalloc_pool(1, size);
    if (pool)
    {
        if (!pool_item)
            pool_item = mem_model_alloc_buffer(sizeof (mem_model_pool_item_t));
        if (pool_item)
        {
            pool_item->pool = pool;
            pool_item->size = size;
            pool_item->ref++;
            init_queue(&pool_item->queue);
            queue_add_tail(&pool_item->queue, &mem_model_cxt->mem_pool_queue);
        }
        else
        {
            mempool_destroy(pool);
        }
    }
    return pool_item;
}



void *mem_model_pool_alloc(mem_model_pool_handle_t pool_handle)
{
    // avoid allocating large memory
    mem_model_pool_item_t *pool_item=pool_handle;
    void *buffer=NULL;
    
    if(pool_item)
    {
        if(pool_item->pool)
        {
            buffer=mempool_alloc(pool_item->pool,GFP_KERNEL);
        }
    }
    return buffer;
}

void mem_model_pool_free(mem_model_pool_handle_t pool_handle,void *buffer)
{
    mem_model_pool_item_t *pool_item=pool_handle;
        
    if(pool_item)
    {
        if(pool_item->pool)
        {
            mempool_free(buffer,pool_item->pool);
        }
    }
    
}

void mem_model_release_pool(mem_model_pool_handle_t pool_handle)
{
    mem_model_pool_item_t *pool_item=pool_handle;
    
    debug_msg("%s %p\n",__func__,pool_item);
    pool_item->ref--;
    if(pool_item->ref==0)
    {   
        mempool_destroy(pool_item->pool);
        queue_del(&pool_item->queue);
        mem_model_free_buffer(pool_item);
    }
    
    
}

void *mem_model_new(unsigned int  size)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,12,0)
    return kvzalloc(size, GFP_KERNEL);
#else
    return kmalloc(size, GFP_KERNEL|__GFP_NORETRY);
#endif
}

void mem_model_delete(void *buf)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,12,0)
    if (buf) kvfree(buf);
#else
    if (buf) kfree(buf);
#endif
}

static void *mem_model_alloc(void)
{
    mem_model_cxt_t *mem_model_cxt=mem_model_alloc_buffer(sizeof(mem_model_cxt_t));
    
    if(mem_model_cxt)
    {
        
    }
    return mem_model_cxt;
}

static void mem_model_release(void *context)
{
    mem_model_cxt_t *mem_model_cxt=(mem_model_cxt_t *)context;
    if(mem_model_cxt)
    {
        mem_model_dma_pool_item_t *pos,*next;
        for_each_queue_entry_safe(pos,next,&mem_model_cxt->dma_pool_queue,queue)
        {
            if(!queue_empty(&pos->buffer_queue))
            {
                mem_model_dma_buffer_item_t *cur_buffer,*next_buffer;
                for_each_queue_entry_safe(cur_buffer,next_buffer,&pos->buffer_queue,queue)
                {
                    pr_info("%s %lx size %lx\n",__func__,cur_buffer->dma_buffer.phys_addr,cur_buffer->dma_buffer.size);
                    mem_model_free_buffer_item(mem_model_cxt,cur_buffer);
                }
            }
            mem_model_free_pool_item(mem_model_cxt,pos);
        }
        
        if(mem_model_cxt->poolitem_mempool)
            mempool_destroy(mem_model_cxt->poolitem_mempool);
        if(mem_model_cxt->bufferitem_mempool)
            mempool_destroy(mem_model_cxt->bufferitem_mempool);
        mem_model_free_buffer(mem_model_cxt);
    }
}

mem_model_handle_t mem_model_init(cxt_mgr_handle_t cxt_mgr)
{
    mem_model_cxt_t *mem_model_cxt=NULL;
    enum
    {
        NO_ERROR=0,
        NO_CXT_MGR,
        ALLOC_FAIL,
        CREATE_POOL_FAIL,
    }err=NO_ERROR;
    
    do
    {
        if(!cxt_mgr)
        {
            err=NO_CXT_MGR;
            break;
        }
        mem_model_cxt=cxt_manager_add_cxt(cxt_mgr,MEM_CXT_ID,mem_model_alloc,mem_model_release);
        if(!mem_model_cxt)
        {
            err=ALLOC_FAIL;
            break;
        }
        mem_model_cxt->dev=cxt_manager_get_dev(cxt_mgr);
        mem_model_cxt->poolitem_mempool=mempool_create_kmalloc_pool(MIN_NR,sizeof(mem_model_dma_pool_item_t));
        if(!mem_model_cxt->poolitem_mempool)
        {
            err=CREATE_POOL_FAIL;
            break;
        }
        mem_model_cxt->bufferitem_mempool=mempool_create_kmalloc_pool(MIN_NR,sizeof(mem_model_dma_buffer_item_t));
        if(!mem_model_cxt->bufferitem_mempool)
        {
            err=CREATE_POOL_FAIL;
            break;
        }
        init_queue(&mem_model_cxt->dma_pool_queue);
        init_queue(&mem_model_cxt->mem_pool_queue);
        
        
    }while(0);
    if(err!=NO_ERROR)
    {
        debug_msg("%s err %d\n",__func__,err);
        switch(err)
        {
        case ALLOC_FAIL:
        case NO_CXT_MGR:
            break;
        default:
            cxt_manager_unref_context(mem_model_cxt);
            mem_model_cxt=NULL;
            break;
        }
    }
       
    return (mem_model_handle_t )mem_model_cxt;
}
