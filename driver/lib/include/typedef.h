/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * typedef.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#ifndef TYPEDEF_H
#define TYPEDEF_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void *handle_t;

#ifndef NULL
#define NULL ((void *)0)
#endif    

#define Bit(x) (1<<(x))
#define BITMASK(x) ((1<<(x))-1) 

#ifndef offsetof    
#define offsetof(TYPE, MEMBER)  ((unsigned long )&((TYPE *)0)->MEMBER)    
#endif

typedef unsigned char U8_T;
typedef char S8_T;
typedef unsigned short U16_T;
typedef short S16_T;
typedef unsigned int U32_T;
typedef int S32_T;
typedef unsigned long SIZE_T;
typedef unsigned long long U64_T;
typedef long long S64_T;
#ifndef FALSE
typedef enum 
{
    FALSE,
    TRUE,
} BOOL_T;
#endif


#ifdef __cplusplus
}
#endif

#endif /* TYPEDEF_H */

