///*****************************************
//  Copyright (C) 2009-2018
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6664_Config.h>
//   @author Hojim.Tseng@ite.com.tw
//   @date   2018/02/22
//   @fileversion: ITE_HDMI2_SPLITER_1.18
//******************************************/
#ifndef _IT6664_CONFIG_H_
#define _IT6664_CONFIG_H_


#define IT6663_C 0x01


//function define
//--------------------------------------------------

#define EnIntEDID	FALSE //FALSE; //use Ext EDID
#define AutoEQ			 // Auto EQ
#define SIP_Mean  22000
#define Disable_RXHDCP
//#define YUV_DS_Set_Limit_Range
//#define Support4096DS			// remove by AndyNien, 20180330
#define Support_HDCP_DownVersion // Just For IT6664 test
//#define Compose3D_EDID

#define HPD_Debounce		TRUE
#define EnRxHP1Rpt			FALSE
#define EnRxHP2Rpt 			FALSE


// the following 2 parameter define the 6663 working mode, it's important
//////////////////////////////////////
//	   GPDRG	GPDRC
//
//     0x00    0x00 00	2k
//     0x80    0x20 a0	4k
//     0x80    0x00 80	Copy_Mode
//     0x00    0x20	20	Compose_Mode
//
//enum
//{
//	Default_EDID2k = 0x00,
//	Default_EDID4k = 0xA0,
//	Copy_Mode = 0x80,
//	Compose_Mode = 0x20,
//};
extern  iTE_s8  GPDRG;		// #define	GPDRG 			ECReg(EC_Register_Base_Address + 0x1607), by AndyNien, 20171030
extern  iTE_s8  GPDRC;		// #define	GPDRC 			ECReg(EC_Register_Base_Address+0x1603), by AndyNien, 20171030

							// dummy
extern  iTE_s8  GPDRA;		// #define	GPDRA 			ECReg(EC_Register_Base_Address+0x1601)
extern  iTE_s8  GPDRB; 		// #define	GPDRB 			ECReg(EC_Register_Base_Address + 0x1602)
extern  iTE_s8  GPDRE;		// #define	GPDRE 			ECReg(EC_Register_Base_Address+0x1605)
extern  iTE_s8  GPDRF;		// #define	GPDRG 			ECReg(EC_Register_Base_Address + 0x1607), by AndyNien, 20171030

extern const iTE_s8  KSI;

//--------------------------------------------------

// Normal define

#define TxPortNum   2
#define IT6663
#ifndef EnableTXP0HDCP
#define EnableTXP0HDCP FALSE
#endif// EnableTXP0HDCP

#ifndef EnableTXP1HDCP
    #define EnableTXP1HDCP TRUE
#endif// EnableTXP1HDCP

#ifndef EnableTXP2HDCP
    #define EnableTXP2HDCP TRUE
#endif// EnableTXP2HDCP

#ifndef EnableTXP3HDCP
    #define EnableTXP3HDCP FALSE
#endif// EnableTXP3HDCP

#define EDID_CopyPx  	2			
#define FixPort1_1080P	FALSE
#define FixPort2_1080P	FALSE
#define FixPort_Opt 	((FixPort1_1080P<<1) + (FixPort2_1080P<<2))
		//#pragma message("EnableIT6663")



#endif

