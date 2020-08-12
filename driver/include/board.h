/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * board.h
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

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    CL511H=0,
    SUPPORT_BOARD_NUM,        
}board_type_e;

enum cl511h_input_e
{
    CL511H_HDMI_INPUT=0,
    CL511H_INPUT_COUNT,
};

enum cl511h_i2c_chip_e
{
    CL511H_I2C_CHIP_ITE6805_0,
    CL511H_I2C_CHIP_COUNT,
};

typedef struct
{
    const char *name;
    int index;
}board_chip_desc_t;

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */

