/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * board_alsa.h
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */

#ifndef BOARD_CX511_BOARD_ALSA_H_
#define BOARD_CX511_BOARD_ALSA_H_

void board_alsa_init(cxt_mgr_handle_t cxt_mgr);
void board_alsa_recv_data(void *board_alas_cxt,unsigned char *buffer,U32_T size);

#define BOARD_ALSA_CXT_ID fourcc_id('B','A','L','S')
#endif /* BOARD_CX511_BOARD_ALSA_H_ */
