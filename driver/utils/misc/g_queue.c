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
 
#include "g_queue.h"
#include "debug.h"

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
        

         



