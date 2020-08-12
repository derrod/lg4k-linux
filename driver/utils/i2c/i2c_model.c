#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include "cxt_mgr.h"
#include "mem_model.h"
#include "i2c_model.h"
#include "debug.h"



typedef struct
{
	BASIC_CXT_HANDLE_DECLARE;
	queue_t i2c_bus_queue;
	queue_t active_i2c_driver_queue;
	int i2c_bus_count;
}i2c_model_t;

typedef struct  {
	queue_t queue;
	const char *bus_name;
    struct i2c_adapter *adap;
    struct i2c_adapter i2c_adap;
    i2c_model_bus_transfer_func_t *xfer_func;
    i2c_model_bus_transfer_I2CWrite *write_func;
    i2c_model_bus_transfer_I2CRead *read_func;
    void *bus_data;
    struct mutex lock;
    handle_t ref_cxt;
    handle_t i2c_mgr;
}i2c_model_bus_t;

typedef struct {
	queue_t queue;
	const char *drv_name;
	unsigned short i2c_addr;
	void *drv_set_info;
	i2c_attach_func_t *attach;
	i2c_dettach_func_t *detach;
	void *drv_cxt;
}i2c_model_driver_item_t;




static i2c_model_driver_t i2c_model_drivers[] __attribute__((section("i2c_model_data")))=
{ 
};

static void *i2c_model_alloc(void);
static void i2c_model_release(void *context);


static void *i2c_model_alloc(void)
{
	i2c_model_t *model=(i2c_model_t *)kzalloc(sizeof(i2c_model_t),GFP_KERNEL);

	if(model)
	{
            init_queue(&model->i2c_bus_queue);
            init_queue(&model->active_i2c_driver_queue);
            model->i2c_bus_count=0;
	}
	return model;
}

static void i2c_model_release(void *context)
{
	i2c_model_t *model=(i2c_model_t *)context;
	i2c_model_bus_t *bus,*next_bus=NULL;
	i2c_model_driver_item_t *drv,*next_drv=NULL;

	if(model)
	{

			for_each_queue_entry_safe(drv,next_drv,&model->active_i2c_driver_queue,queue)
			{
				queue_del(&drv->queue);
				if(drv->detach)
				{
					drv->detach(drv->drv_cxt);
				}
				kfree(drv);
			}

			for_each_queue_entry_safe(bus,next_bus,&model->i2c_bus_queue,queue)
			{
				queue_del(&bus->queue);
				if(bus->ref_cxt)
				{
					cxt_manager_unref_context(bus->ref_cxt);
				}

				printk("i2c_del_adapter %p %s\n",&bus->i2c_adap,bus->bus_name);
				i2c_del_adapter(&bus->i2c_adap);

				mem_model_free_buffer(bus);
			}


		mem_model_free_buffer(model);
	}
}
static int i2c_model_bus_i2c_xfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
    i2c_model_bus_t *bus=container_of(adap,i2c_model_bus_t,i2c_adap);
    i2c_model_msg_t i2c_model_msg[num];
    int ret=-1; 
    int i;
    
    
    for(i=0;i<num;i++)
    {
        i2c_model_msg[i].addr=msgs[i].addr;
        i2c_model_msg[i].buf=msgs[i].buf;
        i2c_model_msg[i].len=msgs[i].len;
        i2c_model_msg[i].flags=0;
        if(msgs[i].flags & I2C_M_RD)
            i2c_model_msg[i].flags |= I2C_MODEL_READ;
        if(msgs[i].flags & I2C_M_TEN)
            i2c_model_msg[i].flags |= I2C_MODEL_10BIT_ADDR;
        
    }
    if(bus->xfer_func)
    {
        mutex_lock(&bus->lock);
        ret=bus->xfer_func(bus->ref_cxt,bus->bus_data,&i2c_model_msg[0],num);
              
        mutex_unlock(&bus->lock);
    }
    
    return ret;
}

static u32 i2c_model_bus_functionality(struct i2c_adapter *adap)
{
    return I2C_FUNC_I2C;
}

static struct i2c_algorithm i2c_model_bus_algo={
    .master_xfer = i2c_model_bus_i2c_xfer,
    .functionality = i2c_model_bus_functionality,
};
	

int i2c_model_transfer(i2c_model_bus_handle_t bus_handle,i2c_model_msg_t i2c_model_msg[],int num)
{
    i2c_model_bus_t *bus=bus_handle;

    struct i2c_msg msgs[num];
    int i;
    
    for(i=0;i<num;i++)
    {
        msgs[i].addr=i2c_model_msg[i].addr;
        msgs[i].buf=i2c_model_msg[i].buf;
        msgs[i].len=i2c_model_msg[i].len;
        msgs[i].flags=0;
        if(i2c_model_msg[i].flags & I2C_MODEL_READ)
            msgs[i].flags |= I2C_M_RD;
            
        if(i2c_model_msg[i].flags & I2C_MODEL_10BIT_ADDR)
            msgs[i].flags |= I2C_M_TEN;
        
    }

    return i2c_transfer(&bus->i2c_adap,msgs,num);
    
    
}

int i2c_model_read(i2c_model_bus_handle_t bus_handle, U8_T slaveAddr, U32_T subAddr, U8_T* pBuf, U8_T bufLen)
{
    i2c_model_bus_t *bus=bus_handle;
    int ret=-1;

    if(bus->read_func)
    {
        mutex_lock(&bus->lock);
        ret=bus->read_func(bus->ref_cxt, 0, slaveAddr, subAddr, 1, pBuf, bufLen);
              
        mutex_unlock(&bus->lock);
    }
    
    return ret;
}

int i2c_model_write(i2c_model_bus_handle_t bus_handle, U8_T slaveAddr, U32_T subAddr, U8_T* pBuf, U8_T bufLen)
{
    i2c_model_bus_t *bus=bus_handle;
    int ret=-1;

    if(bus->write_func)
    {
        mutex_lock(&bus->lock);
        ret=bus->write_func(bus->ref_cxt, 0, slaveAddr, subAddr, 1, pBuf, bufLen);
              
        mutex_unlock(&bus->lock);
    }
    
    return ret;
}

i2c_model_bus_handle_t i2c_model_new_bus(i2c_model_handle_t handle,const char *bus_name,i2c_model_bus_cfg_t *bus_cfg)
{
    i2c_model_t *model=(i2c_model_t *)handle;
    i2c_model_bus_t *bus=NULL;
    enum
    {
        NO_ERROR,
        NO_BUS_CFG,
        NO_BUS_METHOD,
        ERROR_ALLOC_CXT,
        ERR_ADD_I2C_ADAPTER,
        
    }err=NO_ERROR;
    do
    {
        if(!bus_cfg)
        {
            err=NO_BUS_CFG;
            break;
        }
        if(!bus_cfg->i2c_transfer_func)
        {
            err=NO_BUS_METHOD;
            break;
        }
        bus=mem_model_alloc_buffer(sizeof(i2c_model_bus_t));
        if(!bus)
        {
            err=ERROR_ALLOC_CXT;
            break;
        }
        mutex_init(&bus->lock);
        init_queue(&bus->queue);
        bus->bus_name=bus_name;
        bus->xfer_func=bus_cfg->i2c_transfer_func;
        bus->write_func=bus_cfg->i2c_write_func;
        bus->read_func=bus_cfg->i2c_read_func;
        bus->ref_cxt=bus_cfg->ref_cxt;
        bus->bus_data=bus_cfg->bus_data;
        strncpy(bus->i2c_adap.name,bus_name,sizeof(bus->i2c_adap.name));
        
        bus->i2c_adap.owner= THIS_MODULE;
        bus->i2c_adap.algo=&i2c_model_bus_algo;        	
        bus->i2c_adap.dev.parent=cxt_manager_get_dev(get_cxt_manager_from_context(model));
        if(i2c_add_adapter(&bus->i2c_adap)) 
        {
            err=ERR_ADD_I2C_ADAPTER;
            break;
	}
        bus->i2c_mgr=model;
				
        if(bus->ref_cxt)
        {
            cxt_manager_ref_context(bus->ref_cxt);
        }
        queue_add_tail(&bus->queue,&model->i2c_bus_queue);
    }while(0);
    if(err!=NO_ERROR)
    {
        debug_msg("%s err %d\n",__func__,err);
        switch(err)
        {
            case ERR_ADD_I2C_ADAPTER:
                mem_model_free_buffer(bus);
            case ERROR_ALLOC_CXT:
            case NO_BUS_METHOD:
            case NO_BUS_CFG:
                break;
            default:
                break;
        }
        
    }
    return bus;
    
}

i2c_model_bus_handle_t i2c_model_get_bus(i2c_model_handle_t handle,const char *bus_name)
{
    i2c_model_t *model = (i2c_model_t *)handle;
    i2c_model_bus_t *bus, *found=NULL;

    for_each_queue_entry(bus,&model->i2c_bus_queue,queue)
    {
        if(bus->bus_name==bus_name)
        {
            found=bus;
            break;
        }
    }

    return found;
}

i2c_model_handle_t i2c_model_bus_get_mgr(i2c_model_bus_handle_t bus_handle)
{
    i2c_model_bus_t *bus=bus_handle;
    
    return bus->i2c_mgr;
}


int i2c_model_add_bus(i2c_model_handle_t handle,const char *bus_name,void *adap,handle_t ref_cxt)
{
	i2c_model_t *model=(i2c_model_t *)handle;
	i2c_model_bus_t *bus=NULL;
	i2c_model_error_e err=I2C_MODEL_OK;
        struct i2c_adapter *i2c_adap=adap;

	do
	{
			bus = kzalloc(sizeof(i2c_model_bus_t), GFP_KERNEL);
			if(!bus)
			{
				err=I2C_MODEL_ALLOC_FAIL;
				break;
			}
			if(!adap)
			{
				err=I2C_MODEL_INVALID_I2C_ADAPTER;
				break;
			}
			if(i2c_add_adapter(i2c_adap)) {
				err=I2C_MODEL_ERR_ADD_I2C_ADAPTER;
				break;
			}
			mutex_init(&bus->lock);
			init_queue(&bus->queue);
			bus->adap=i2c_adap;
			bus->bus_name=bus_name;
			queue_add_tail(&bus->queue,&model->i2c_bus_queue);
			if(ref_cxt)
			{

				bus->ref_cxt=ref_cxt;
				cxt_manager_ref_context(ref_cxt);
			}
			//printk("%s %p %s\n",__func__,bus->adap,bus->bus_name);
	}while(0);
	if(err!=I2C_MODEL_OK)
	{
		switch(err)
		{
			case I2C_MODEL_INVALID_I2C_ADAPTER:
				if(bus)
					kfree(bus);
			case I2C_MODEL_ALLOC_FAIL:
				break;
			default:
				break;
		}
		return err;
	}
	return 0;
}


i2c_model_error_e i2c_model_del_bus(i2c_model_handle_t handle,const char *bus_name)
{
	i2c_model_t *model=(i2c_model_t *)handle;
	i2c_model_bus_t *bus,*found=NULL;
	i2c_model_error_e ret=I2C_MODEL_OK;

	for_each_queue_entry(bus,&model->i2c_bus_queue,queue)
	{
		if(bus->bus_name==bus_name)
		{
			found=bus;
			queue_del(&found->queue);
			break;
		}
	}

    if(found)
    {
    	i2c_del_adapter(bus->adap);
    	kfree(bus);
    }else
    {
    	ret=I2C_MODEL_BUS_NOT_FOUND;
    }

    return ret;
}

i2c_model_error_e i2c_model_bus_attach_device(i2c_model_handle_t handle,const char *bus_name,const char *drv_name,unsigned short addr,void *dev_setupinfo)
{
	i2c_model_t *model=(i2c_model_t *)handle;
	i2c_model_driver_t *pdriver=i2c_model_drivers,*found=NULL;
	i2c_model_bus_t *bus=NULL,*pos;

	while(pdriver->drv_name)
	{
		if(strncmp(pdriver->drv_name,drv_name,strlen(pdriver->drv_name))==0)
		{
			found=pdriver;
            // SHA204 not ready
			printk("I2C_MODEL_BUS pdriver->drv_name ok %s and %s\n",pdriver->drv_name,drv_name); //
			break;
		}
		pdriver++;
	}
	if(!found)
		return I2C_MODEL_DRV_NOT_FOUND;
	for_each_queue_entry(pos,&model->i2c_bus_queue,queue)
	{
		if(pos->bus_name==bus_name)
		{
			bus=pos;
			break;
		}
	}
	if(!bus)
		return I2C_MODEL_BUS_NOT_FOUND;
	do
	{
		i2c_model_driver_item_t *item=(i2c_model_driver_item_t *)kzalloc(sizeof(i2c_model_driver_item_t), GFP_KERNEL);
		if(item)
		{
			init_queue(&item->queue);
			item->drv_name=found->drv_name;
			item->i2c_addr = addr;
			item->drv_set_info = dev_setupinfo;
			item->attach=found->attach;
			item->detach=found->detach;
			if(found->attach)
			{
				item->drv_cxt=found->attach(bus,addr,dev_setupinfo);
				if(item->drv_cxt==NULL)
				{
					return I2C_MODEL_ATTACH_DRV_FAIL;
				}

			}
			queue_add_tail(&item->queue,&model->active_i2c_driver_queue);
		}else
		{
			return I2C_MODEL_ALLOC_FAIL;
		}

	}while(0);
	return I2C_MODEL_OK;
}

void *i2c_model_get_driver_handle(i2c_model_handle_t handle,const char *drv_name)
{
	i2c_model_t *model=(i2c_model_t *)handle;
	i2c_model_driver_item_t *drv;
	for_each_queue_entry(drv,&model->active_i2c_driver_queue,queue)
	{
            if(strncmp(drv->drv_name,drv_name,strlen(drv->drv_name))==0)
            {
            	return drv->drv_cxt;
            }
	}
	return NULL;
}

void *i2c_model_get_nth_driver_handle(i2c_model_handle_t handle,const char *drv_name,int nth)
{
	i2c_model_t *model=(i2c_model_t *)handle;
	i2c_model_driver_item_t *drv;
        int count=0;
        
        
	for_each_queue_entry(drv,&model->active_i2c_driver_queue,queue)
	{
        
            if(strncmp(drv->drv_name,drv_name,strlen(drv->drv_name))==0)
            {
                count++;
            
                if(nth==count)
                {
              
                    return drv->drv_cxt;
                }
            }
	}
	return NULL;
}

//i2c_model_error_e i2c_model_use_driver(i2c_model_handle_t context,const char *drv_name)
//{
//	i2c_model_t *model=(i2c_model_t *)context;
//	i2c_model_driver_t *pdriver=i2c_model_drivers,*found=NULL;
//	int ret;
//
//
//	while(pdriver->drv_name)
//	{
//		if(strncmp(pdriver->drv_name,drv_name,strlen(pdriver->drv_name))==0)
//		{
//			found=pdriver;
//			break;
//		}
//		pdriver++;
//	}
//
//	if(found)
//	{
//		i2c_model_driver_item_t *item=(i2c_model_driver_item_t *)kzalloc(sizeof(i2c_model_driver_item_t), GFP_KERNEL);
//		if(item)
//		{
//			INIT_LIST_HEAD(&item->list);
//			item->drv_name=found->drv_name;
//			item->attach=found->attach;
//			item->detach=found->detach;
//			list_add_tail(&item->list,&model->active_i2c_driver_list);
//			ret=0;
//		}else
//			ret=I2C_MODEL_ALLOC_FAIL;
//	}else
//	{
//		ret=I2C_MODEL_DRIVER_NOT_FOUND;
//	}
//	return ret;
//}
//
//
//void i2c_model_attach(i2c_model_handle_t context)
//{
//	i2c_model_t *model=(i2c_model_t *)context;
//	i2c_model_driver_item_t *pos;
//
//	list_for_each_entry(pos,&model->active_i2c_driver_list,list)
//	{
//		if(pos->attach)
//		{
//			printk("%s attach\n",pos->drv_name);
//			pos->attach();
//		}
//	}
//
//}
//
//void i2c_model_detach(i2c_model_handle_t context)
//{
//	i2c_model_t *model=(i2c_model_t *)context;
//	i2c_model_driver_item_t *pos;
//
//	list_for_each_entry(pos,&model->active_i2c_driver_list,list)
//	{
//		if(pos->detach)
//		{
//			printk("%s detach\n",pos->drv_name);
//			pos->detach();
//		}
//	}
//}

i2c_model_handle_t i2c_model_init(cxt_mgr_handle_t cxt_mgr)
{
	i2c_model_t *i2c_model_context=NULL;
	if(cxt_mgr)
	{
		i2c_model_context=cxt_manager_add_cxt(cxt_mgr,I2C_CXT_ID,i2c_model_alloc,i2c_model_release);
	}
	return (i2c_model_handle_t )i2c_model_context;
}



