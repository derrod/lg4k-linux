/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * board_v4l2.h
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

#ifndef BOARD_V4L2_H
#define BOARD_V4L2_H

#ifdef __cplusplus
extern "C" {
#endif

void board_v4l2_init(cxt_mgr_handle_t cxt_mgr, int board_id);
void board_v4l2_suspend(cxt_mgr_handle_t cxt_mgr);
void board_v4l2_resume(cxt_mgr_handle_t cxt_mgr);
//void gc573_adv7619_set_bchs(framegrabber_handle_t handle);
#define BOARD_V4L2_CXT_ID fourcc_id('B','V','4','L')

#ifdef __cplusplus
}
#endif

#endif /* BOARD_V4L2_H */

