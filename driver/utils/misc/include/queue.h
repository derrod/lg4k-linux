/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * queue.h
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

#ifndef QUEUE_H
#define QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#define queue_entry(ptr, type, member) \
        container_of(ptr, type, member)    

#define queue_first_entry(ptr, type, member) \
         queue_entry((ptr)->next, type, member)        

#define queue_first_entry_or_null(ptr, type, member) \
    (!queue_empty(ptr) ? queue_first_entry(ptr, type, member) : NULL)    
    
#define queue_next_entry(pos, member) \
        queue_entry((pos)->member.next, typeof(*(pos)), member)
    
#define queue_last_entry(ptr, type, member) \
         queue_entry((ptr)->prev, type, member)    

#define for_each_queue_entry(pos, head, member)                          \
        for (pos = queue_first_entry(head, typeof(*pos), member);        \
            &pos->member != (head);                                    \
            pos = queue_next_entry(pos, member))
    
#define for_each_queue_entry_safe(pos, n, head, member)                  \
        for (pos = queue_first_entry(head, typeof(*pos), member),        \
            n = queue_next_entry(pos, member);                       \
            &pos->member != (head);                                    \
            pos = n, n = queue_next_entry(n, member))    


    
typedef struct queue_item_s {
        struct queue_item_s *next, *prev;
}queue_t; 



static inline void init_queue(queue_t *item)
 {
    item->next= item;
    item->prev = item;
 }

static inline int queue_empty(queue_t *head)
{
    return (head->next == head);
}

static inline void __queue_add(queue_t *new,queue_t *prev,queue_t *next)
{
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next= new;
 }

 static inline void queue_add_tail(queue_t *new, queue_t *head)
 {
     __queue_add(new, head->prev, head);
 } 

 static inline void __queue_del(queue_t * prev, queue_t * next)
 {
         next->prev = prev;
         prev->next= next;
 } 
 
static inline void __queue_del_entry(queue_t *entry)
{
        __queue_del(entry->prev, entry->next);
}

static inline void queue_del(queue_t *entry)
{
        __queue_del(entry->prev, entry->next);
        entry->next = entry;
        entry->prev = entry;
}


#ifdef __cplusplus
}
#endif

#endif /* QUEUE_H */

