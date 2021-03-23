/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * pci_model.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " "%s, %d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include "linux/pci.h"
#include "linux/interrupt.h"
#include "cxt_mgr.h"
#include "mem_model.h"
#include "pci_model.h"
#include "alsa_model.h"
#include "debug.h"

typedef struct
{
    pci_model_flags_e flags;
    unsigned long driver_data;
}pci_model_driver_data_t;

typedef struct 
{
    BASIC_CXT_HANDLE_DECLARE;
    struct pci_driver driver;
    pci_model_probe_func_t probe_func;
    pci_model_suspend_func_t suspend_func;
    pci_model_resume_func_t resume_func;
    pci_model_remove_func_t remove_func;
    struct pci_device_id *id_table;
    pci_model_driver_data_t driver_data;
}pci_model_driver_cxt_t;

typedef struct
{
    u64 phys_addr;
    u64 size;
    u64 __iomem *mmio;
}bar_info_t;

#define MAX_BAR_COUNT 6

typedef struct
{
    BASIC_CXT_HANDLE_DECLARE;
    struct pci_dev *pci_dev;
    pci_model_irq_func_t irq_func;
    void *irq_func_cxt;
    int bar_count;
    bar_info_t bar_info[MAX_BAR_COUNT];
    int msi_enabled;
}pci_model_cxt_t;

static pci_model_driver_cxt_t *pci_model_drv_cxt=NULL;
static void *pci_model_alloc(void);
static void pci_model_release(void *cxt);

int subsystem_id;
static int user_disable_msi;

static void *pci_model_alloc()
{
    pci_model_cxt_t *pci_cxt=mem_model_alloc_buffer(sizeof(pci_model_cxt_t));
       
    return pci_cxt;
}

static void pci_model_release(void *cxt)
{
	pci_model_cxt_t *pci_cxt=cxt;
        mesg_debug("%s\n",__func__);

	
	if(pci_cxt)
	{
            
            mem_model_free_buffer(pci_cxt);
	}
}

static irqreturn_t pci_model_irq(int irq, void *dev_id)
{
    int handled=0;
    pci_model_cxt_t *pci_cxt=dev_id;
    if(pci_cxt->irq_func)
    {
        handled=pci_cxt->irq_func(pci_cxt->irq_func_cxt);
    }    
    
    
    return IRQ_RETVAL(handled);
}

static u16 pci_get_subsystem(struct pci_dev *pdev)
{
    u16 sub_id;

    pci_read_config_word(pdev, PCI_SUBSYSTEM_ID	, &sub_id);
    return sub_id;
}
 
static int pci_model_probe(struct pci_dev *pci_dev,const struct pci_device_id *pci_id)
{
    int ret=0;
    struct device *dev=&pci_dev->dev;
    cxt_mgr_handle_t cxt_mgr=cxt_manager_alloc(dev);
    pci_model_cxt_t *pci_cxt=NULL;
    int i;
    pci_model_driver_data_t *driver_data=(pci_model_driver_data_t *)pci_id->driver_data;
    pci_model_flags_e flags=driver_data->flags;
    int sub_system;
    
    enum
    {
        NO_ERROR=0,
        NO_DRV_CXT,
        NO_CXT_MGR,
        ERROR_ALLOC_CXT,
        ERROR_ENABLE_PCI_DEV,
        ERROR_REQUEST_REGIONS,
        ERROR_ENABLE_MSI,
        ERROR_SET_DMA_MASK,
        ERROR_REQUEST_IRQ,
    }err=NO_ERROR;

    pr_info("pci_model_probe prepare\n");
    do
    {
        if(!pci_model_drv_cxt)
        {
            err=NO_DRV_CXT;
            break;
        }
        if(!cxt_mgr)
        {
            err=NO_CXT_MGR;
            break;
        }
        dev_set_drvdata(dev,cxt_mgr);
        pci_cxt=cxt_manager_add_cxt(cxt_mgr,PCI_CXT_ID,pci_model_alloc,pci_model_release);
        if(!pci_cxt)
        {
            err=ERROR_ALLOC_CXT;
            break;
        }
        pci_cxt->pci_dev=pci_dev;
        if(pci_enable_device(pci_dev))
        {
            err=ERROR_ENABLE_PCI_DEV;
            break;
        }
        pci_set_master(pci_dev);
        if(pci_request_regions(pci_dev,pci_name(pci_dev)))
        {
            err=ERROR_REQUEST_REGIONS;
            break;
            
        }
        if( !(flags & PCI_MODEL_FORCE_DMA_32) &&  pci_find_capability(pci_dev,PCI_CAP_ID_EXP))
        {
            mesg_debug("PCI Express\n");
            
            if(!pci_set_dma_mask(pci_dev, DMA_BIT_MASK(64)))
            {
                pr_info("set 64bit DMA mask\n");
            }else 
                if(!pci_set_dma_mask(pci_dev, DMA_BIT_MASK(32)))
            {
                pr_info("set 32bit DMA mask\n");
            }else
            {
                
                mesg_err("No suitable DMA available\n");
                err=ERROR_SET_DMA_MASK;
                break;
            }
        }else
        {
            if(!pci_set_dma_mask(pci_dev, DMA_BIT_MASK(32)))
            {
                pr_info("set 32bit DMA mask\n");
                
            }else
            {
                mesg_err("No suitable DMA available\n");
                err=ERROR_SET_DMA_MASK;
                break;
            }   
        }
        if(err!=NO_ERROR)
            break;
        
        user_disable_msi = flags & PCI_MODEL_DISABLE_MSI;
        pci_cxt->msi_enabled = false;
        if(!user_disable_msi && pci_find_capability(pci_dev,PCI_CAP_ID_MSI))
        {
            mesg_debug("MSI\n");
            if(pci_enable_msi(pci_dev))
            {
                err=ERROR_ENABLE_MSI;
                break;
            }else
            {
                pci_cxt->msi_enabled = true;
                 if(request_irq(pci_dev->irq, pci_model_irq,0, pci_name(pci_dev), pci_cxt) <0)
                 {
                    err=ERROR_REQUEST_IRQ;
                    break;
                 }
            }
        }else
        {
            if(request_irq(pci_dev->irq, pci_model_irq,IRQF_SHARED, pci_name(pci_dev), pci_cxt)<0)
            {
                err=ERROR_REQUEST_IRQ;
                break;
            }
        }
        if(err!=NO_ERROR)
            break;
        
        for(i=0;i<MAX_BAR_COUNT && pci_resource_len(pci_dev,i)!=0 ;i++)
        {
            bar_info_t *bar_info=&pci_cxt->bar_info[i];
            //unsigned int phys_addr=pci_resource_start(pci_dev,i);
            //unsigned int size=pci_resource_len(pci_dev,i);
            u64 phys_addr=pci_resource_start(pci_dev,i);
            u64 size=pci_resource_len(pci_dev,i);
            void *mmio=NULL;
            
            if((mmio=ioremap(phys_addr,size)))
            {
                bar_info->phys_addr=phys_addr;
                bar_info->size=size;
                bar_info->mmio=mmio;
                mesg_debug("%s ioremap %08llx size %llx to %p\n",__func__,phys_addr,size,mmio);
            }else
            {
                mesg_err("%s ioremap %08llx size %llx error\n",__func__,phys_addr,size);
            }
            
            pci_cxt->bar_count++;
        }
        
        sub_system = pci_get_subsystem(pci_dev);
        
        subsystem_id = sub_system;
        
        pr_info("%s sub_id=%x\n",__func__,sub_system);
           
        if(pci_model_drv_cxt->probe_func)
        {
            ret=pci_model_drv_cxt->probe_func(dev,driver_data->driver_data);
            pr_info("board_probe=%d\n",ret);
		}
    
        
    }while(0);
    if(err!=NO_ERROR)
    {
        mesg_err("%s error %d\n",__func__,err);
        pr_err("pci_model_probe fail\n");
        switch(err)
        {      
            case ERROR_REQUEST_REGIONS:
                pci_clear_master(pci_dev);
                pci_disable_device(pci_dev);
                // fall through
            case ERROR_ENABLE_PCI_DEV:
                cxt_manager_unref_context(pci_cxt);
                pci_cxt=NULL;
                // fall through
            case ERROR_ALLOC_CXT:
            case NO_CXT_MGR:
            case NO_DRV_CXT:
                break;
            default:
                pci_release_regions(pci_dev);
                break;
        }
    }
    
    
    return ret;
}

static void pci_model_remove(struct pci_dev *pci_dev)
{
    struct device *dev=&pci_dev->dev;
    cxt_mgr_handle_t cxt_mgr=get_cxt_manager(dev);
    pci_model_cxt_t *pci_cxt;
    mesg_debug("%s\n",__func__);
    
    if(pci_model_drv_cxt)
        if(pci_model_drv_cxt->remove_func)
            pci_model_drv_cxt->remove_func(dev);
    if(cxt_mgr)
    {
        int i;
        pci_cxt=cxt_manager_get_context(cxt_mgr,PCI_CXT_ID,0);
        
        cxt_manager_release(cxt_mgr);
        for(i=0;i<pci_cxt->bar_count;i++)
        {
            iounmap(pci_cxt->bar_info[i].mmio);
        }
        free_irq(pci_dev->irq, pci_cxt);

        if(pci_cxt->msi_enabled)
        {
            pci_disable_msi(pci_dev);
            pci_cxt->msi_enabled = false;
        }
    }
    
    pci_release_regions(pci_dev);
    pci_clear_master(pci_dev);
    pci_disable_device(pci_dev);
        
}

static int pci_model_suspend (struct pci_dev *pci_dev, pm_message_t state)
{
//    int ret=0;
    struct device *dev=&pci_dev->dev;
    cxt_mgr_handle_t cxt_mgr=get_cxt_manager(dev);
    pci_model_cxt_t *pci_cxt = cxt_manager_get_context(cxt_mgr,PCI_CXT_ID,0);

    mesg_debug("%s\n",__func__);

    if (cxt_mgr)
    {
        alsa_model_suspend(cxt_manager_get_context(cxt_mgr,ALSA_CXT_ID,0));

        pci_model_drv_cxt->suspend_func(dev);

        free_irq(pci_dev->irq, pci_cxt);

        if(pci_cxt->msi_enabled)
        {
            pci_disable_msi(pci_dev);
            pci_cxt->msi_enabled = false;
        }
    }

    pci_disable_device(pci_dev);
    pci_save_state(pci_dev);
    pci_set_power_state(pci_dev, pci_choose_state(pci_dev, state));

    return 0;
}

static int pci_model_resume(struct pci_dev *pci_dev)
{
    int ret = 0;
    struct device *dev=&pci_dev->dev;
    cxt_mgr_handle_t cxt_mgr=get_cxt_manager(dev);
    pci_model_cxt_t *pci_cxt = cxt_manager_get_context(cxt_mgr,PCI_CXT_ID,0);

    mesg_debug("%s\n",__func__);

    pci_set_power_state(pci_dev, PCI_D0);
    pci_restore_state(pci_dev);

    ret = pci_enable_device(pci_dev);
    if (ret)
    {
        mesg_debug("pci model resume failed");
        return ret;
    }

    pci_cxt->msi_enabled = false;
    if(!user_disable_msi && pci_find_capability(pci_dev,PCI_CAP_ID_MSI))
    {
        mesg_debug("MSI\n");
        if(pci_enable_msi(pci_dev))
        {
            return -1;
        }else
        {
            pci_cxt->msi_enabled = true;
             if(request_irq(pci_dev->irq, pci_model_irq,0, pci_name(pci_dev), pci_cxt) <0)
             {
                 return -1;
             }
        }
    }else
    {
        if(request_irq(pci_dev->irq, pci_model_irq,IRQF_SHARED, pci_name(pci_dev), pci_cxt)<0)
        {
            return -1;
        }
    }

    pci_set_master(pci_dev);

    pci_model_drv_cxt->resume_func(dev);

    alsa_model_resume(cxt_manager_get_context(cxt_mgr,ALSA_CXT_ID,0));

    return 0;
}

static __attribute__ ((unused)) void pci_model_shutdown(struct pci_dev *pci_dev)
{
    
}

pci_model_handle_t pci_model_get_handle(cxt_mgr_handle_t cxt_mgr)   
{

	pci_model_cxt_t *pci_cxt=NULL;

	if(cxt_mgr)
	{
		pci_cxt=cxt_manager_get_context(cxt_mgr,PCI_CXT_ID,0);
		if(pci_cxt)
			return pci_cxt;
	}
	return NULL;
}

u32 pci_model_mmio_read(pci_model_handle_t handle,int index,unsigned offset)
{
    pci_model_cxt_t *pci_cxt=handle;
    u32 __iomem *mmio;
    u32 ret=0;
    
    do
    {
        if(!pci_cxt)
            break;
        if(offset & 0x3)
        {
            mesg_err("%s offset %x no 32 bit align\n",__func__,offset);
            break;
        }
        if(index <0 || (index>pci_cxt->bar_count))
        {
            mesg_err("%s access wrong index %d\n",__func__,index);
            break;
        }
        mmio=(u32 __iomem *)pci_cxt->bar_info[index].mmio;
        ret=readl(&mmio[offset>>2]);
    }while(0);
    
    return ret;
}

u16 pci_model_mmio_readw(pci_model_handle_t handle,int index,unsigned offset)
{
    pci_model_cxt_t *pci_cxt=handle;
    u16 __iomem *mmio;
    u16 ret=0;
    
    do
    {
        if(!pci_cxt)
            break;
        if(offset & 0x1)
        {
            mesg_err("%s offset %x no 16 bit align\n",__func__,offset);
            break;
        }
        if(index <0 || (index>pci_cxt->bar_count))
        {
            mesg_err("%s access wrong index %d\n",__func__,index);
            break;
        }
        mmio=(u16 __iomem *)pci_cxt->bar_info[index].mmio;
        ret=readw(&mmio[offset>>1]);
    }while(0);
    
    return ret;
}

u8 pci_model_mmio_readb(pci_model_handle_t handle,int index,unsigned offset)
{
    pci_model_cxt_t *pci_cxt=handle;
    u8 __iomem *mmio;
    u8 ret=0;
    
    do
    {
        if(!pci_cxt)
            break;
      
        if(index <0 || (index>pci_cxt->bar_count))
        {
            mesg_err("%s access wrong index %d\n",__func__,index);
            break;
        }
        mmio=(u8 __iomem *)pci_cxt->bar_info[index].mmio;
        ret=readb(&mmio[offset]);
    }while(0);
    
    return ret;
}

void pci_model_mmio_write(pci_model_handle_t handle,int index,unsigned offset,u32 value)
{
    pci_model_cxt_t *pci_cxt=handle;
    u32 __iomem *mmio;

    
    do
    {
        if(!pci_cxt)
            break;
        if(offset & 0x3)
        {
            mesg_err("%s offset %x no 32 bit align\n",__func__,offset);
            break;
        }
        if(index <0 || (index>pci_cxt->bar_count))
        {
            mesg_err("%s access wrong index %d\n",__func__,index);
            break;
        }
        mmio=(u32 __iomem *)pci_cxt->bar_info[index].mmio;
        
        writel(value,&mmio[offset>>2]);
    }while(0);
}

void pci_model_mmio_writew(pci_model_handle_t handle,int index,unsigned offset,u16 value)
{
    pci_model_cxt_t *pci_cxt=handle;
    u16 __iomem *mmio;

    
    do
    {
        if(!pci_cxt)
            break;
        if(offset & 0x1)
        {
            mesg_err("%s offset %x no 16 bit align\n",__func__,offset);
            break;
        }
        if(index <0 || (index>pci_cxt->bar_count))
        {
            mesg_err("%s access wrong index %d\n",__func__,index);
            break;
        }
        mmio=(u16 __iomem *)pci_cxt->bar_info[index].mmio;
        writew(value,&mmio[offset>>1]);
    }while(0);
}


void pci_model_mmio_writeb(pci_model_handle_t handle,int index,unsigned offset,u8 value)
{
    pci_model_cxt_t *pci_cxt=handle;
    u8 __iomem *mmio;

    
    do
    {
        if(!pci_cxt)
            break;
        if(index <0 || (index>pci_cxt->bar_count))
        {
            mesg_err("%s access wrong index %d\n",__func__,index);
            break;
        }
        mmio=(u8 __iomem *)pci_cxt->bar_info[index].mmio;
        writeb(value,&mmio[offset]);
    }while(0);
}





void pci_model_register_isr(pci_model_handle_t handle,pci_model_irq_func_t irq_func,void *data)
{
    pci_model_cxt_t *pci_cxt=handle;
    
    if(pci_cxt)
    {
        if(pci_cxt->irq_func)
        {
            mesg_err("Another ISR already registered\n");
            return;
        }
        pci_cxt->irq_func= irq_func;
        pci_cxt->irq_func_cxt=data;
    }
}
 

int pci_model_driver_init(pci_model_driver_setup_t *pcidrv_setup)
{
	int err; 
	pr_info("pci_model_driver_init\n");
    if(pci_model_drv_cxt==NULL)
    {
        pci_model_drv_cxt=mem_model_alloc_buffer(sizeof(pci_model_driver_cxt_t));
        if(pci_model_drv_cxt)
        {
            struct pci_driver *driver=&pci_model_drv_cxt->driver;
            
            driver->name=pcidrv_setup->name;
            driver->probe=pci_model_probe;
            driver->remove=pci_model_remove;
            driver->suspend=pci_model_suspend;
            driver->resume=pci_model_resume;
//            driver->shutdown=pci_model_shutdown;

            pci_model_drv_cxt->probe_func=pcidrv_setup->prob_func;
            pci_model_drv_cxt->remove_func=pcidrv_setup->remove_func;
            pci_model_drv_cxt->suspend_func=pcidrv_setup->suspend_func;
            pci_model_drv_cxt->resume_func=pcidrv_setup->resume_func;
            
            
            if(pcidrv_setup->id_table)
            {
                int count=0,i;
                pci_model_id_t *id_setup;
                for(count=0,id_setup=(pci_model_id_t *)pcidrv_setup->id_table;id_setup[count].device!=0 || id_setup[count].vendor!=0;count++)
                    ;
                if(count)
                {
                    pci_model_drv_cxt->id_table=mem_model_alloc_buffer(sizeof(struct pci_device_id)*(count+1));
                    if(pci_model_drv_cxt->id_table)
                    {
                        struct pci_device_id *id=pci_model_drv_cxt->id_table;
                        id_setup=(pci_model_id_t *)pcidrv_setup->id_table;
                        for(i=0;i<count;i++)
                        {
                            if(id_setup->device)
                            {   
                                id->device=id_setup->device;
                                pr_info("id->device=%02x\n",id->device);
							}
                            else
                                id->device=PCI_ANY_ID;
                            
                            if(id_setup->vendor)
                                id->vendor=id_setup->vendor;
                            else
                                id->vendor=PCI_ANY_ID;
                            
                            if(id_setup->sub_vendor)
                                id->subvendor=id_setup->sub_vendor;
                            else
                                id->subvendor=PCI_ANY_ID;
                            
                            if(id_setup->sub_device)
                                id->subdevice=id_setup->sub_device;
                            else
                                id->subdevice=PCI_ANY_ID;
                            pci_model_drv_cxt->driver_data.flags=pcidrv_setup->flags;
                            pci_model_drv_cxt->driver_data.driver_data=id_setup->driver_data;
                            id->driver_data=(uintptr_t )&pci_model_drv_cxt->driver_data;
                            id++;
                            id_setup++;
                        }
                        id->vendor=0;
                        id->device=0;
                    } 
                }
                if(pci_model_drv_cxt->id_table)
                    driver->id_table=pci_model_drv_cxt->id_table;
            } 
        
            err = pci_register_driver(&pci_model_drv_cxt->driver);
            if (err ==0)
            {
				pr_info(">>>pci_register_driver ok\n");
			}
            else
            {
				pr_err(">>>pci_register_driver fail\n");
			}
			//pcidrv_setup->subsystem_id = subsystem_id;
			
        }
    }
    return 0;
}

void pci_model_driver_exit()
{
    mesg_debug("%s\n",__func__);
    if(pci_model_drv_cxt)
    {    
        pci_unregister_driver(&pci_model_drv_cxt->driver);
        if(pci_model_drv_cxt->id_table)
            mem_model_free_buffer(pci_model_drv_cxt->id_table);
        mem_model_free_buffer(pci_model_drv_cxt);
        pci_model_drv_cxt=NULL;
    } 
    mesg_debug("%s done\n",__func__);
}
