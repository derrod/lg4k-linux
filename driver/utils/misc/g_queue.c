/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * g_queue.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " "%s, %d: " fmt, __func__, __LINE__
 
#include "g_queue.h"
#include "debug.h"

void init_queue(queue_t *item)
 {
    item->next= item;
    item->prev = item;
 }

int queue_empty(queue_t *head)
{
    return (head->next == head);
}

void __queue_add(queue_t *new,queue_t *prev,queue_t *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next= new;
 }

 void queue_add_tail(queue_t *new, queue_t *head)
 {
     __queue_add(new, head->prev, head);
 } 

 void __queue_del(queue_t * prev, queue_t * next)
 {
         next->prev = prev;
         prev->next= next;
 } 
 
void __queue_del_entry(queue_t *entry)
{
        __queue_del(entry->prev, entry->next);
}

void queue_del(queue_t *entry)
{
        __queue_del(entry->prev, entry->next);
        entry->next = entry;
        entry->prev = entry;
}


void init_g_queue(g_queue_t *head)
{
    init_queue(&head->queue);
    head->data=NULL;
}

BOOL_T g_queue_empty(g_queue_t *head)
{
    return queue_empty(&head->queue);
}

void g_queue_add_tail(g_queue_t *new, g_queue_t *head)
{
    if(head->data)
    {
        debug_msg("Invalid g_queue head\n");
        return;
    }
     queue_add_tail(&head->queue,&new->queue);
}
 
void g_queue_del(g_queue_t *entry)
{
    queue_del(&entry->queue);
}

g_queue_t *g_queue_first_entry(g_queue_t *head)
{
    g_queue_t *item=queue_first_entry_or_null(&head->queue, g_queue_t, queue);
    
    return item;
}

g_queue_t *g_queue_last_entry(g_queue_t *head)
{
    g_queue_t *item=NULL;
    if(!g_queue_empty(head))
    {
        item=queue_last_entry(&head->queue, g_queue_t, queue);
    }
    return item;
}

g_queue_t *g_queue_next_entry(g_queue_t *item)
{
    g_queue_t *next=queue_next_entry(item,queue);
    
    return next;
}
        

         



