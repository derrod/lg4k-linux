/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * board_i2c.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 
//#include <linux/kernel.h>
#include "cxt_mgr.h"
#include "i2c_model.h"
#include "board.h"
#include "board_i2c.h"
#include "gpio_model.h"
#include "ite6805.h"
#include "sha204.h"
#include "debug.h"
#include "board_gpio.h"
#include "sys.h"


static const char *i2c_bus_name[BOARD_I2C_BUS_NUM]=
{
    [I2C_BUS_COM]="I2C_BUS_COM", //reserved one I2C
   
};


static ite6805_cfg_t ite6805_cfg={
    .out_ctrl={
        .out_fmt=ITE6805_OUT_FORMAT_SDR_ITU656_24_MODE0,
        .out_colorspace=ITE6805_OUT_YUV709,  
        //.out_colorspace=ADV7619_OUT_DEFAULT,   
    },
    
};


static i2c_dev_info_t cx511h_i2c_bus1_devices[]=
{
    {
        .name=ITE6805_DRVNAME,
	.addr=0x58, //(0x98>>1)
	.context=&ite6805_cfg, //color space
    },
    {
        NULL,
    }
    
};

static i2c_dev_info_t *board_i2c_bus_devices[SUPPORT_BOARD_NUM][BOARD_I2C_BUS_NUM]=
{
  [CL511H][I2C_BUS_COM]= &cx511h_i2c_bus1_devices[0], //reserved one I2C
  
};


const char *board_get_i2c_bus_name(int no)
{
    if(no < 0 || no >= BOARD_I2C_BUS_NUM)
           return NULL;
    return i2c_bus_name[no];
}

int board_i2c_init(cxt_mgr_handle_t cxt_mgr,unsigned long dev_id)
{
	i2c_model_handle_t i2c_mgr=cxt_manager_get_context(cxt_mgr,I2C_CXT_ID,0);
	int err=0;
	int ret=0;

	
	if(i2c_mgr)
	{
            int i;
            i2c_dev_info_t *dev_info;
            
            //board_set_gpio(cxt_mgr,BOARD_GPIO_RESET_CL511,GPIOIC_VALUE_LOW);
            //sys_msleep(100);
            //board_set_gpio(cxt_mgr,BOARD_GPIO_RESET_CL511,GPIOIC_VALUE_HIGH);
            //sys_msleep(100);
            
             
            
            for(i=0;i<BOARD_I2C_BUS_NUM;i++)
            {
                if(board_i2c_bus_devices[dev_id][i])
                {    
                    dev_info=&board_i2c_bus_devices[dev_id][i][0];
                    while((err==0) && (dev_info->name!=NULL))
                    {
                        if((ret=i2c_model_bus_attach_device(i2c_mgr,i2c_bus_name[i],dev_info->name,dev_info->addr,dev_info->context))==0)
                        {	
                            dev_info++;
                            debug_msg("board_i2c_init ok\n");
                        }else
                        {
                            err=-1;
                            debug_msg("error in attach %s %d\n",dev_info->name,ret);//SHA204 
                            break;
                        }
                    }
                    if(err)
                    {
                        debug_msg("board_i2c_init err\n");
                        break;
                    }
                }
                
            }    
        }
        return err;
}

int board_i2c_read(cxt_mgr_handle_t cxt_mgr, U8_T channel, U8_T slv_addr, U32_T sub_addr, U8_T *buf, U8_T buf_len)
{
    i2c_model_handle_t i2c_mgr = NULL;
    const char *i2c_bus_name = NULL;
    i2c_model_bus_handle_t bus = NULL;
    int ret = -1;

    i2c_mgr=cxt_manager_get_context(cxt_mgr,I2C_CXT_ID,0);
    if (!i2c_mgr)
    {
        debug_msg("ERROR : I2C manager is NULL");
        return ret;
    }

    i2c_bus_name = board_get_i2c_bus_name(channel);
    if (!i2c_bus_name)
    {
        debug_msg("ERROR : I2C bus name is NULL");
        return ret;
    }

    bus = i2c_model_get_bus(i2c_mgr, i2c_bus_name);
    if (!bus)
    {
        debug_msg("ERROR : I2C bus handle is NULL");
        return ret;
    }

    if (i2c_model_read(bus, slv_addr, sub_addr, buf, buf_len))
    {
        ret = 0;
    }

    return ret;
}

int board_i2c_write(cxt_mgr_handle_t cxt_mgr, U8_T channel, U8_T slv_addr, U32_T sub_addr, U8_T *buf, U8_T buf_len)
{
    i2c_model_handle_t i2c_mgr = NULL;
    const char *i2c_bus_name = NULL;
    i2c_model_bus_handle_t bus = NULL;
    int ret = -1;

    i2c_mgr=cxt_manager_get_context(cxt_mgr,I2C_CXT_ID,0);
    if (!i2c_mgr)
    {
        debug_msg("error : i2c manager is NULL");
        return ret;
    }

    i2c_bus_name = board_get_i2c_bus_name(channel);
    if (!i2c_bus_name)
    {
        debug_msg("ERROR : i2c bus name is NULL");
        return ret;
    }

    bus = i2c_model_get_bus(i2c_mgr, i2c_bus_name);
    if (!bus)
    {
        debug_msg("error : i2c bus handle is NULL");
        return ret;
    }

    if (i2c_model_write(bus, slv_addr, sub_addr, buf, buf_len))
    {
        ret = 0;
    }

    return ret;
}
