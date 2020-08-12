/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * pci_model.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#ifndef PCI_MODEL_H
#define PCI_MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*pci_model_probe_func_t)(struct device *dev,unsigned long driver_info);
typedef void (*pci_model_suspend_func_t)(struct device *dev);
typedef void (*pci_model_resume_func_t)(struct device *dev);
typedef void (*pci_model_remove_func_t)(struct device *dev);
typedef int (*pci_model_irq_func_t)(void *cxt);

typedef struct
{
    unsigned short device;
    unsigned short vendor;
    unsigned short sub_device;
    unsigned short sub_vendor;
    unsigned long  driver_data;
}pci_model_id_t;

typedef enum
{
    PCI_MODEL_DEFAULT_FLAG=0,
    PCI_MODEL_DISABLE_MSI=(0x1<<0),
    PCI_MODEL_FORCE_DMA_32=(0x1<<1),
    
}pci_model_flags_e;

typedef struct
{
    const char *name;
    const pci_model_id_t *id_table;
    pci_model_probe_func_t prob_func;
    pci_model_suspend_func_t suspend_func;
    pci_model_resume_func_t resume_func;
    pci_model_remove_func_t remove_func;
    pci_model_irq_func_t irq_func;
    pci_model_flags_e flags;
    void *irq_func_cxt;
    //int subsystem_id;
}pci_model_driver_setup_t;
    
#define PCI_CXT_ID fourcc_id('P','C','I','_')

typedef handle_t pci_model_handle_t;

int pci_model_driver_init(pci_model_driver_setup_t *pci_drv_setup_info);
void pci_model_driver_exit(void);
pci_model_handle_t pci_model_get_handle(cxt_mgr_handle_t cxt_mgr);
void pci_model_register_isr(pci_model_handle_t handle,pci_model_irq_func_t irq_func,void *data);
U32_T pci_model_mmio_read(pci_model_handle_t handle,int index,unsigned offset);
void pci_model_mmio_write(pci_model_handle_t handle,int index,unsigned offset,U32_T value);
U16_T pci_model_mmio_readw(pci_model_handle_t handle,int index,unsigned offset);
void pci_model_mmio_writew(pci_model_handle_t handle,int index,unsigned offset,U16_T value);
U8_T pci_model_mmio_readb(pci_model_handle_t handle,int index,unsigned offset);
void pci_model_mmio_writeb(pci_model_handle_t handle,int index,unsigned offset,U8_T value);


#ifdef __cplusplus
}
#endif

#endif /* PCI_MODEL_H */

