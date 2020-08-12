/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * board_i2c.h
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

#ifndef BOARD_I2C_H
#define BOARD_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    I2C_BUS_COM,
    I2C_BUS_COM_1,
    I2C_BUS_SEU,
    BOARD_I2C_BUS_NUM,    
};

typedef struct
{
	const char *name;
	unsigned short addr;
	void *context;
}i2c_dev_info_t;


const char *board_get_i2c_bus_name(int no);
int board_i2c_init(cxt_mgr_handle_t cxt_mgr,unsigned long dev_id);
int board_i2c_read(cxt_mgr_handle_t cxt_mgr, U8_T channel, U8_T slv_addr, U32_T sub_addr, U8_T *buf, U8_T buf_len);
int board_i2c_write(cxt_mgr_handle_t cxt_mgr, U8_T channel, U8_T slv_addr, U32_T sub_addr, U8_T *buf, U8_T buf_len);


#ifdef __cplusplus
}
#endif

#endif /* BOARD_I2C_H */

