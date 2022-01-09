/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * board_gpio.h
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
 
#ifndef BOARD_GPIO_H
#define BOARD_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    BOARD_GPIO_RESET_CL511,
    BOARD_GPIO_EDID_WP,
    BOARD_GPIO_EDID_W1,
    BOARD_GPIO_EDID_W2,
    BOARD_GPIO_HPD_RX_CTL,
    BOARD_GPIO_NUM,        
}board_gpio_e;

int board_gpio_init(cxt_mgr_handle_t cxt_mgr);
void board_set_gpio(cxt_mgr_handle_t cxt_mgr,board_gpio_e no,gpio_model_pin_value_t value);
gpio_model_pin_value_t board_get_gpio(cxt_mgr_handle_t cxt_mgr,board_gpio_e no);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_GPIO_H */

