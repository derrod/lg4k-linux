/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * g_queue.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#ifndef G_QUEUE_H
#define G_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "cxt_mgr.h"    
#include "typedef.h"
#include "queue.h"
    
typedef struct
{
    queue_t queue;
    void *data;    
}g_queue_t;


void init_g_queue(g_queue_t *item);
BOOL_T g_queue_empty(g_queue_t *head);
void g_queue_add_tail(g_queue_t *new, g_queue_t *head);
void g_queue_del(g_queue_t *entry);
g_queue_t *g_queue_first_entry(g_queue_t *head);
g_queue_t *g_queue_last_entry(g_queue_t *head);
g_queue_t *g_queue_next_entry(g_queue_t *item);

#define for_each_g_queue_entry(pos, head)                          \
        for (pos = g_queue_first_entry(head);        \
            &pos->queue != (head->queue);                                    \
            pos = g_queue_next_entry(pos))

#define for_each_g_queue_entry_safe(pos, n, head)                  \
        for (pos = g_queue_first_entry(head),        \
            n = g_queue_next_entry(pos);                       \
            &pos->queue != (head->queue);                                    \
            pos = n, n = g_queue_next_entry(pos))    


#ifdef __cplusplus
}
#endif

#endif /* G_QUEUE_H */

