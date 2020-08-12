/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * debug.h
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
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#ifndef DEBUG_H
#define DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#define DEBUG 1    

int mesg(const char *fmt,...);
#define mesg_info(fmt, ...) \
        mesg(KERN_INFO fmt, ##__VA_ARGS__)
#define mesg_emerg(fmt, ...) \
        mesg(KERN_EMERG fmt, ##__VA_ARGS__)
#define mesg_alert(fmt, ...) \
        mesg(KERN_ALERT fmt, ##__VA_ARGS__)
#define mesg_crit(fmt, ...) \
        mesg(KERN_CRIT fmt, ##__VA_ARGS__)
#define mesg_err(fmt, ...) \
        mesg(KERN_ERR fmt, ##__VA_ARGS__)
#define mesg_warning(fmt, ...) \
        mesg(KERN_WARNING fmt, ##__VA_ARGS__)
#define mesg_notice(fmt, ...) \
        mesg(KERN_NOTICE fmt, ##__VA_ARGS__)
#if DEBUG
#define mesg_debug(fmt, ...) \
        debug_msg(KERN_DEBUG fmt, ##__VA_ARGS__)

#else
#define mesg_debug(fmt, ...)
#endif

typedef enum
{
    DDEBUG_FLAG_NONE=0,
    DDEBUG_FLAG_SHOW=(0x1<<0),
    DDEBUG_FLAG_INC_FUNC=(0x1<<1),        
    DDEBUG_FLAG_INC_FILE=(0x1<<2),
    DDEBUG_FLAG_INC_LINENO=(0x1<<3),
    DDEBUG_FLAG_DEFAULT=DDEBUG_FLAG_NONE,        
}ddebug_flags_e;

 typedef 
union
 {
    struct 
    {
         const char *function;
         const char *filename;
         unsigned int lineno;
         ddebug_flags_e flags;
    };
    U8_T data[32];   
 }ddebug_t __attribute__((aligned(16)));

#define DEFINE_DDEBUG_METADATA(name)                \
        static ddebug_t   __attribute__((aligned(16))) __attribute__((section("ddebug"))) \
        name = {          \
             .function = __func__,                           \
             .filename = __FILE__,                           \
             .lineno = __LINE__,                             \
             .flags =  DDEBUG_FLAG_DEFAULT,               \
        }
#if 1
#define debug_msg(fmt,...)                  \
    do                                      \
    {                                       \
        DEFINE_DDEBUG_METADATA(descriptor); \
        if(descriptor.flags & DDEBUG_FLAG_SHOW)                \
        {                                                       \
            if(descriptor.flags ^ DDEBUG_FLAG_SHOW)                                            \
            {                                                   \
                if(descriptor.flags & DDEBUG_FLAG_INC_FILE)                \
                    mesg("FILE %s",descriptor.filename);                           \
                if(descriptor.flags & DDEBUG_FLAG_INC_LINENO)                \
                    mesg("LINE %d",descriptor.lineno);                           \
                if(descriptor.flags & DDEBUG_FLAG_INC_FUNC)                \
                    mesg("FUNC %s",descriptor.function);                           \
                mesg(":",descriptor.function);                           \
            }                                                   \
            mesg(fmt, ##__VA_ARGS__);       \
        }                                   \
    }while(0)
#else
#define debug_msg(fmt,...)    mesg(fmt, ##__VA_ARGS__)        
#endif

void init_debug(handle_t trace_handle);
void uninit_debug(void);

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */

