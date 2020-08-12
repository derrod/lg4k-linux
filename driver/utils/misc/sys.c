/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * sys.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 #include "typedef.h"
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/time.h>
#include <linux/semaphore.h>
#include <linux/timer.h>
#include <linux/ktime.h>
#include "linux/delay.h"
#include "linux/device.h"
#include "linux/slab.h"
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "sys.h"

typedef struct
{
    sys_spin_lock_t interface;
    sys_spin_lock_level_e level;
    spinlock_t lock;
    unsigned long flags;
}sys_spin_lock_cxt_t;

typedef struct 
{
    struct semaphore sem;
    struct timer_list timer;
    //wait_queue_head_t wait_queue;
}sys_wait_sem_t;

typedef struct
{
    atomic_t value;
}sys_atomic_cxt_t;

void sys_msleep(unsigned ms)
{
    msleep(ms);
}

void sys_mdelay(unsigned ms)
{
    mdelay(ms);
}

void sys_udelay(unsigned us)
{
    udelay(us);
}


int sys_sprintf(char *buf, const char * fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
    
}

int sys_sscanf(const char *buf, const char *fmt, ...)
{
       va_list args;
       int i;
 
       va_start(args, fmt);
       i = vsscanf(buf, fmt, args);
       va_end(args);

       return i;
}

unsigned long long sys_gettimestamp()
{
    struct timespec64 ts;
    unsigned long long timestamp;
    
    ktime_get_ts64(&ts);
    timestamp=ts.tv_sec ;
    timestamp*=1000000000;
    timestamp+=ts.tv_nsec;
    
    return timestamp;
}


void *sys_get_drvdata(device_handle_t device_handle)
{
    struct device *dev=device_handle;

    if(dev)
    {
        return dev_get_drvdata(dev);
    }

    return NULL;    
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
static void i2c_timeout(struct timer_list *t)
{
    sys_wait_sem_t *wait_sem_cxt = from_timer(wait_sem_cxt, t, timer);
    //wake_up(&wait_sem_cxt->wait_queue);
    up(&wait_sem_cxt->sem);
    //aver_xilinx_cxt->com0status = 1;
    //aver_xilinx_cxt->waitstatus = 1;
}

#else
static void i2c_timeout(unsigned long evt)
{
	sys_wait_sem_t *wait_sem_cxt = (sys_wait_sem_t*)evt;

    //wake_up(&aver_xilinx_cxt->wait_queue);
    up(&wait_sem_cxt->sem);
    //aver_xilinx_cxt->com0status = 1;
    //aver_xilinx_cxt->waitstatus = 1;
}
#endif


void sys_signal_sem (void* handle)
{
    sys_wait_sem_t *wait_sem_cxt = (sys_wait_sem_t*)handle;

    if(wait_sem_cxt == NULL)
        return;
    up( &wait_sem_cxt->sem );
}

void sys_wait_sem(void* handle)
{
    sys_wait_sem_t *wait_sem_cxt = (sys_wait_sem_t*)handle;

    if(wait_sem_cxt == NULL)
        return;
    down( &wait_sem_cxt->sem );
}

void sys_del_timer(void* handle)
{
    sys_wait_sem_t *wait_sem_cxt = (sys_wait_sem_t*)handle;

    if(wait_sem_cxt == NULL)
        return;
    del_timer_sync(&wait_sem_cxt->timer);
}

void sys_wait_sem_timer(void* handle, unsigned int timeout)
{
    sys_wait_sem_t *wait_sem_cxt = (sys_wait_sem_t*)handle;

    if(wait_sem_cxt == NULL)
        return;

    if (timeout > 0)
    {
        wait_sem_cxt->timer.expires = jiffies + timeout*HZ/50;//((timeoutMS*HZ+999)/1000) timeout*HZ
    }else{
        wait_sem_cxt->timer.expires = jiffies + HZ;
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
    wait_sem_cxt->timer.function = i2c_timeout;
    wait_sem_cxt->timer.data = (unsigned long)wait_sem_cxt;
#endif

    add_timer(&wait_sem_cxt->timer);
    //wait_event(wait_sem_cxt->wait_queue, *wait_id != 0);
    //del_timer_sync(&wait_sem_cxt->timer);
    down(&wait_sem_cxt->sem);
}


void *sys_new_wait_sem(void)
{
    sys_wait_sem_t *wait_sem_cxt=kzalloc(sizeof(sys_wait_sem_t),GFP_KERNEL);//GFP_KERNEL
    
    if(wait_sem_cxt)
    {
        sema_init(&wait_sem_cxt->sem, 0);
        //init_waitqueue_head(&wait_sem_cxt->wait_queue);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,15,0)
        timer_setup(&wait_sem_cxt->timer, i2c_timeout, 0);
#else
	    init_timer(&wait_sem_cxt->timer);
#endif
    }
    
    return wait_sem_cxt;
}

void sys_release_waitsem(void* handle)
{
    sys_wait_sem_t *wait_sem_cxt = (sys_wait_sem_t*)handle;
    if(wait_sem_cxt)
    {
        kfree(wait_sem_cxt);
    }
}

sys_spin_lock_t *sys_new_spinlock(const char *name,sys_spin_lock_level_e level)
{
    sys_spin_lock_t *lock=NULL;
    sys_spin_lock_cxt_t *lock_cxt=kzalloc(sizeof(sys_spin_lock_cxt_t),GFP_KERNEL);
    
    if(lock_cxt)
    {
        lock_cxt->interface.name=name;
        lock_cxt->level=level;
        spin_lock_init(&lock_cxt->lock);
        lock=&lock_cxt->interface;
    }
    
    return lock;
}

void sys_spin_lock(sys_spin_lock_t *lock)
{
    sys_spin_lock_cxt_t *lock_cxt=container_of(lock,sys_spin_lock_cxt_t,interface);
    
    if(lock_cxt)
    {
        switch(lock_cxt->level)
        {
        case SYS_SPINLOCK_LEVEL_PROC:
            spin_lock(&lock_cxt->lock);
            break;
        case SYS_SPINLOCK_LEVEL_BH:
            spin_lock_bh(&lock_cxt->lock);
            break;
        case SYS_SPINLOCK_LEVEL_IRQ:
            spin_lock_irqsave(&lock_cxt->lock,lock_cxt->flags);
            break;
        default:
            break;
        }   
    }
    
}

void sys_spin_unlock(sys_spin_lock_t *lock)
{
    sys_spin_lock_cxt_t *lock_cxt=container_of(lock,sys_spin_lock_cxt_t,interface);
    
    if(lock_cxt)
    {
        switch(lock_cxt->level)
        {
        case SYS_SPINLOCK_LEVEL_PROC:
            spin_unlock(&lock_cxt->lock);
            break;
        case SYS_SPINLOCK_LEVEL_BH:
            spin_unlock_bh(&lock_cxt->lock);
            break;
        case SYS_SPINLOCK_LEVEL_IRQ:
            spin_unlock_irqrestore(&lock_cxt->lock,lock_cxt->flags);
            break;
        default:
            break;
        }    
    }
}

void sys_release_spinlock(sys_spin_lock_t *lock)
{
    sys_spin_lock_cxt_t *lock_cxt=container_of(lock,sys_spin_lock_cxt_t,interface);
    
    if(lock_cxt)
    {
        kfree(lock_cxt);
    }
    
}

sys_atomic_t sys_new_atomic()
{
    sys_atomic_cxt_t *sys_atomic_cxt=kzalloc(sizeof(sys_spin_lock_cxt_t),GFP_KERNEL);
    if(sys_atomic_cxt)
    {
        
    }
    return sys_atomic_cxt;
    
}

void sys_release_atomic(sys_atomic_t atomic)
{
    sys_atomic_cxt_t *sys_atomic_cxt=atomic;
    if(sys_atomic_cxt)
    {
        kfree(sys_atomic_cxt);
    }
    
}

void sys_atomic_set(sys_atomic_t atomic,int value)
{
    sys_atomic_cxt_t *sys_atomic_cxt=atomic;
    if(sys_atomic_cxt)
    {
        atomic_set(&sys_atomic_cxt->value,value);
    }
}

int  sys_atomic_get(sys_atomic_t atomic)
{
    sys_atomic_cxt_t *sys_atomic_cxt=atomic;
    int value=0;
    if(sys_atomic_cxt)
    {
        value=atomic_read(&sys_atomic_cxt->value);
    }
    return value;
}


void *sys_fopen(const char *filename, enum sys_fop_flag_e flag)
{
    struct file *fp = NULL;
    int new_flag = 0;
    struct kstat stat;

    if (!filename)
    {
        printk("Error: NULL pointer");
        return NULL;
    }

    if (flag & ~SYS_FOP_FLAG_BIT_MASK)
    {
        printk("Error: the flag is not supported ");
        return NULL;
    }

    if ((flag & SYS_FOP_FLAG_READ_BIT) && (flag & SYS_FOP_FLAG_WRITE_BIT))
        new_flag = O_RDWR;
    else if ((flag & SYS_FOP_FLAG_READ_BIT))
        new_flag = O_RDONLY;
    else if ((flag & SYS_FOP_FLAG_WRITE_BIT))
        new_flag = O_WRONLY;

    if (0 == vfs_stat(filename, &stat))
        printk("file size:%lld", stat.size);


    fp = filp_open(filename, new_flag, 0);

    return IS_ERR(fp) ? NULL : fp;
}

void sys_fclose(void *fp)
{
    if (!IS_ERR(fp)) filp_close(fp, 0);
}


SIZE_T sys_fread(void *fp, void *buf, SIZE_T size, SIZE_T count, SIZE_T offset)
{
    mm_segment_t fs;
    loff_t pos = offset;
    SIZE_T res;

    // fs =get_fs();
    // set_fs(KERNEL_DS);

//    vfs_read(fp, buf, count, &pos);
    res = kernel_read(fp, buf, count, &pos);
    if (res > 0) {
//        ((struct file*)fp)->f_pos = pos;
//        printk("read size:%d", (int)res);
    }

    // set_fs(fs);

    return res;
}


    

