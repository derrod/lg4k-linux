/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * sys.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#ifndef SYS_H
#define SYS_H

#ifdef __cplusplus
extern "C" {
#endif
    

typedef enum
{
    SYS_SPINLOCK_LEVEL_PROC=0, 
    SYS_SPINLOCK_LEVEL_BH,
    SYS_SPINLOCK_LEVEL_IRQ,
        
}sys_spin_lock_level_e;

typedef struct
{
    const char *name; 
}sys_spin_lock_t;


typedef void *sys_atomic_t;

typedef void *device_handle_t;

enum sys_fop_flag_e
{
    SYS_FOP_FLAG_READ = 0,
    SYS_FOP_FLAG_WRITE,
    SYS_FOP_FLAG_NUM,
};

enum sys_fop_flag_bitmask_e
{
    SYS_FOP_FLAG_READ_BIT           = (1<<SYS_FOP_FLAG_READ),
    SYS_FOP_FLAG_WRITE_BIT          = (1<<SYS_FOP_FLAG_WRITE),
    SYS_FOP_FLAG_BIT_MASK           = (1<<SYS_FOP_FLAG_NUM)-1,
};

void sys_msleep(unsigned ms);
void sys_udelay(unsigned us);
void sys_mdelay(unsigned ms);
int sys_sprintf(char *buf, const char * fmt, ...);
int sys_sscanf(const char *buf, const char *fmt, ...);
unsigned long long sys_gettimestamp(void); // nano sec
void *sys_get_drvdata(device_handle_t device_handle);
sys_spin_lock_t *sys_new_spinlock(const char *name,sys_spin_lock_level_e level);
void sys_spin_lock(sys_spin_lock_t *lock);
void sys_spin_unlock(sys_spin_lock_t *lock);
void sys_release_spinlock(sys_spin_lock_t *lock);
sys_atomic_t sys_new_atomic(void);
void sys_release_atomic(sys_atomic_t atomic);
void sys_atomic_set(sys_atomic_t atomic,int value);
int  sys_atomic_get(sys_atomic_t atomic);

void sys_signal_sem (void* handle);
void sys_wait_sem(void* handle);
void sys_del_timer(void* handle);
void sys_wait_sem_timer(void* handle, unsigned int timeout);
void *sys_new_wait_sem(void);
void sys_release_waitsem(void* handle);

void *sys_fopen(const char *filename, enum sys_fop_flag_e flag);
void sys_fclose(void *fp);
SIZE_T sys_fread(void *fp, void *buf, SIZE_T size, SIZE_T count, SIZE_T offset);

#ifdef __cplusplus
}
#endif

#endif /* SYS_H */

