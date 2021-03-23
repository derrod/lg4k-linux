///*****************************************
//  Copyright (C) 2009-2017
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6664_IO.h>
//   @author Hojim.Tseng@ite.com.tw
//   @date   2017/11/03
//   @fileversion: ITE_HDMI2_SPLITER_1.14
//******************************************/
#ifndef _IT6664_IO_H_
#define _IT6664_IO_H_
//#include "IT6664_Typedef.h"
#include "it6664_config.h"
//#include "IO_IT6350.h"

#define USING_I2C		(1)

#define SMBusA				(3)
#define SMBusB				(4)
#define SMBusC				(5)
#define I2C_SMBusD			(0)
#define I2C_SMBusE			(1)
#define I2C_SMBusF			(2)
#ifndef IO_1to4
	#define IO_1to4  TRUE
#endif

#if (USING_1to8==TRUE)
#define SWAddr          0x58
#define RXP0Addr        0x70
#define RXEDID_Addr     0xD8
#define RXEDIDAddr      0xa8
#define RXMHLAddr       0xe0
#define RXCECAddr		0xC0
#define TXComAddr       0x96
#define TXP0Addr        0x60
#define TXP1Addr        0x64
#define TXP2Addr        0x68
#define TXP3Addr        0x6C
#else
#define SWAddr          0x58
#define RXP0Addr        0x70
#define RXEDID_Addr     0xD8
#define RXEDIDAddr      0xa8
#define RXMHLAddr       0xe0
#define RXCECAddr		0xC0
#define TXComAddr       0x96
#define TXP0Addr        0x68
#define TXP1Addr        0x6a
#define TXP2Addr        0x6c
#define TXP3Addr        0x6e
#endif

iTE_u8 iTE_I2C_ReadByte(void *ite6664_handle, iTE_u8 u8Adr, iTE_u8 u8Offset);
void iTE_I2C_WriteByte(void *ite6664_handle, iTE_u8 u8Adr,  iTE_u8 u8Offset, iTE_u8 u8Data);
void iTE_I2C_SetByte(void *ite6664_handle, iTE_u8 u8Adr, iTE_u8 u8Offset,iTE_u8 u8InvMask, iTE_u8 u8Data );
void iTE_I2C_ReadBurst(void *ite6664_handle, iTE_u8 u8Adr, iTE_u8 u8Offset,iTE_u8 u8Cnt,iTE_u8* pu8Buf);
void iTE_I2C_WriteBurst(void *ite6664_handle, iTE_u8 u8Adr, iTE_u8 u8Offset,iTE_u8 u8Cnt,iTE_u8* pu8Buf);
iTE_u8 iTE_I2C_ReadByteP(void *ite6664_handle, iTE_u8 u8Adr,iTE_u8 port,iTE_u8 u8Offset);
void iTE_I2C_WriteByteP(void *ite6664_handle, iTE_u8 u8Adr,iTE_u8 port, iTE_u8 u8Offset, iTE_u8 u8Data);
void iTE_I2C_SetByteP(void *ite6664_handle, iTE_u8 u8Adr,iTE_u8 port,iTE_u8 u8Offset,iTE_u8 u8InvMask, iTE_u8 u8Data );
void iTE_I2C_ReadBurstP(void *ite6664_handle, iTE_u8 u8Adr,iTE_u8 port,iTE_u8 u8Offset,iTE_u8 u8Cnt,iTE_u8* pu8Buf);
void iTE_I2C_WriteBurstP(void *ite6664_handle, iTE_u8 u8Adr,iTE_u8 port,iTE_u8 u8Offset,iTE_u8 u8Cnt,iTE_u8* pu8Buf);

#endif
