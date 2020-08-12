#ifndef __I2C_H__
#define __I2C_H__



#define I2C_CXT_ID fourcc_id('I','2','C','_')

typedef void *i2c_model_handle_t;

typedef enum
{
	I2C_MODEL_OK=0,
	I2C_MODEL_DRV_NOT_FOUND=-1,
	I2C_MODEL_ALLOC_FAIL=-2,
	I2C_MODEL_INVALID_I2C_ADAPTER=-3,
	I2C_MODEL_BUS_NOT_FOUND=-4,
	I2C_MODEL_ATTACH_DRV_FAIL=-5,
	I2C_MODEL_ERR_ADD_I2C_ADAPTER=-6,

}i2c_model_error_e;



typedef enum 
{
    I2C_MODEL_READ=Bit(0),
    I2C_MODEL_10BIT_ADDR=Bit(1),        
}i2c_model_msg_flag_e;
        
        
typedef struct
{
    U16_T addr;
    U8_T  *buf;
    U16_T len;
    i2c_model_msg_flag_e flags;
}i2c_model_msg_t;

typedef handle_t i2c_model_bus_handle_t;
typedef int i2c_model_bus_transfer_func_t(void *ref_cxt, void *bus_data,i2c_model_msg_t *msg,int num);
typedef int i2c_model_bus_transfer_I2CWrite(void *ref_cxt, U8_T bus, U8_T slaveAddr, U32_T subAddr, U8_T addrLen, U8_T* pBuf, U8_T bufLen);
typedef int i2c_model_bus_transfer_I2CRead(void *ref_cxt, U8_T bus, U8_T slaveAddr, U32_T subAddr, U8_T addrLen, U8_T* pBuf, U8_T bufLen);
typedef struct
{
    i2c_model_bus_transfer_func_t *i2c_transfer_func;
    i2c_model_bus_transfer_I2CWrite *i2c_write_func;
    i2c_model_bus_transfer_I2CRead *i2c_read_func;
    void *bus_data;
    handle_t ref_cxt;
}i2c_model_bus_cfg_t;

typedef void *i2c_attach_func_t(i2c_model_bus_handle_t bus_handle,unsigned short i2c_addr,void *drv_set_info);
typedef void i2c_dettach_func_t(void *);



typedef struct i2c_model_driver_s
{
	const char *drv_name;
        
	i2c_attach_func_t *attach;
	i2c_dettach_func_t *detach;
        
        
	void *dummy;
}i2c_model_driver_t;


#define I2C_MODEL_DRV_ID(id) id
#define I2C_MODEL_CONCAT(a,b) a##b
#define I2C_MODEL_MODULE_STR(x) #x
#define I2C_MODEL_DRV_DECLARE(name) \
static i2c_attach_func_t I2C_MODEL_CONCAT(name,_attach);\
static i2c_dettach_func_t I2C_MODEL_CONCAT(name,_detach);\
i2c_model_driver_t name##_i2c_model_data  __attribute__((section("i2c_model_data"))); \
i2c_model_driver_t name##_i2c_model_data  \
={ \
	.drv_name=I2C_MODEL_MODULE_STR(name), \
	.attach=I2C_MODEL_CONCAT(name,_attach), \
	.detach=I2C_MODEL_CONCAT(name,_detach),	\
} \


void i2c_model_attach(i2c_model_handle_t i2c_context);
void i2c_model_detach(i2c_model_handle_t i2c_context);
i2c_model_error_e i2c_model_use_driver(i2c_model_handle_t context,const char *drv_name);
i2c_model_bus_handle_t i2c_model_new_bus(i2c_model_handle_t handle,const char *bus_name,i2c_model_bus_cfg_t *bus_cfg);
int i2c_model_add_bus(i2c_model_handle_t handle,const char *bus_name,void *adap,handle_t ref_cxt);
i2c_model_handle_t i2c_model_init(cxt_mgr_handle_t cxt_mgr);
i2c_model_error_e i2c_model_bus_attach_device(i2c_model_handle_t handle,const char *bus_name,const char *drv_name,unsigned short addr,void *dev_setupinfo);
void *i2c_model_get_driver_handle(i2c_model_handle_t handle,const char *drv_name);
void *i2c_model_get_nth_driver_handle(i2c_model_handle_t handle,const char *drv_name,int nth);
i2c_model_bus_handle_t i2c_model_new_bus(i2c_model_handle_t handle,const char *bus_name,i2c_model_bus_cfg_t *bus_cfg);
i2c_model_bus_handle_t i2c_model_get_bus(i2c_model_handle_t handle,const char *bus_name);
int i2c_model_transfer(i2c_model_bus_handle_t bus_handle,i2c_model_msg_t i2c_model_msg[],int num);
int i2c_model_read(i2c_model_bus_handle_t bus_handle, U8_T slaveAddr, U32_T subAddr, U8_T* pBuf, U8_T bufLen);
int i2c_model_write(i2c_model_bus_handle_t bus_handle, U8_T slaveAddr, U32_T subAddr, U8_T* pBuf, U8_T bufLen);
i2c_model_handle_t i2c_model_bus_get_mgr(i2c_model_bus_handle_t bus_handle);

#endif
