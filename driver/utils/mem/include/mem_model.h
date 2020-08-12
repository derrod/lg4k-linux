/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * mem_model.h
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

#ifndef MEM_MODEL_H
#define MEM_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif
    
#define MEM_CXT_ID fourcc_id('K','M','E','M')
typedef struct 
{
    void *vaddr;
    unsigned long phys_addr;
    unsigned long size;
}mem_model_dma_buffer_t;

typedef struct
{
    const char *name;
    unsigned long size;
    unsigned long align;
    unsigned long boundary;
}mem_model_dma_pool_cfg_t;

typedef void * mem_model_handle_t;
typedef void * mem_model_pool_handle_t;

void *mem_model_alloc_buffer(unsigned int  size);
void *mem_model_alloc_atomic(unsigned int size);
void mem_model_free_buffer(void *buf);
void mem_model_memset(void *s,int c,unsigned int n);
int mem_model_memcmp(const void *dest, const void *src, unsigned  n);
void *mem_model_memcpy(void *dest, const void *src, unsigned  n);
void *mem_model_memmove(void *dest, const void *src, unsigned  n);
char *mem_model_strdup(const char *str);
void *mem_model_alloc_dma_buffer(mem_model_handle_t mem_model_handle,unsigned int size,mem_model_dma_buffer_t *dma_buffer);
void mem_model_free_dma_buffer(mem_model_handle_t mem_model_handle,mem_model_dma_buffer_t *dma_buffer);
mem_model_dma_buffer_t *mem_model_dma_pool_alloc(mem_model_handle_t mem_model_handle,mem_model_dma_pool_cfg_t *cfg);
mem_model_dma_buffer_t *mem_model_dma_pool_alloc_atomic(mem_model_handle_t mem_model_handle,mem_model_dma_pool_cfg_t *cfg);
mem_model_dma_buffer_t *mem_model_dma_pool_alloc_video(mem_model_handle_t mem_model_handle);
mem_model_dma_buffer_t *mem_model_dma_pool_alloc_audio(mem_model_handle_t mem_model_handle);
void mem_model_dma_pool_free(mem_model_handle_t mem_model_handle,mem_model_dma_buffer_t *dma_buffer);
mem_model_pool_handle_t mem_model_create_pool(mem_model_handle_t mem_model_handle,unsigned int size);
void *mem_model_pool_alloc(mem_model_pool_handle_t pool_handle);
void mem_model_pool_free(mem_model_pool_handle_t pool_handle,void *data);
void mem_model_release_pool(mem_model_pool_handle_t pool_handle);
void *mem_model_new(unsigned int  size);
void mem_model_delete(void *buf);
mem_model_handle_t mem_model_init(cxt_mgr_handle_t cxt_mgr);
#ifdef __cplusplus
}
#endif

#endif /* MEM_MODEL_H */

