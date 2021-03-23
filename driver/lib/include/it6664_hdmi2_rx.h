///*****************************************
//  Copyright (C) 2009-2017
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6664_hdmi2_rx.h>
//   @author Hojim.Tseng@ite.com.tw
//   @date   2017/11/03
//   @fileversion: ITE_HDMI2_SPLITER_1.14
//******************************************/
#ifndef _IT6664_HDMI2_RX_H_
#define _IT6664_HDMI2_RX_H_

#include "it6664_extern.h"
//#include "IT6664_DefaultEDID.h"


#define EnFWDSFmt 					FALSE
#define RXReportAutoEQ				TRUE
#define RXReportBitErr				TRUE
#define EnCDSetTX                   FALSE
#define EnBlockHDMIMode				FALSE
#define EnSCDTOffResetRX			FALSE

#define Disable_RXHDCP


typedef enum AutoEQ_type
{
	STAT_EQ_WaitInt = 0,
	STAT_EQ_rst,
	STAT_EQ_Start,
	STAT_EQ_WaitSarEQDone,
	STAT_EQ_CheckBiterr,
	STAT_EQ_AutoEQDone
}AutoEQstate;

typedef enum _PARSE3D_STA{
    PARSE3D_START,
    PARSE3D_LEN,
    PARSE3D_STRUCT_H,
    PARSE3D_STRUCT_L,
    PARSE3D_MASK_H,
    PARSE3D_MASK_L,
    PARSE3D_VIC,
    PARSE3D_DONE
}PARSE3D_STA;


void h2rx_pwron(void *ite6664_handle, iTE_u8 port);
void it6664_h2rx_pwdon(void *ite6664_handle);
void h2rx_irq(void *ite6664_handle);
void get_vid_info(void *ite6664_handle);
void DefaultEdidSet(void *ite6664_handle);
void SetCscConvert(void *ite6664_handle);
void Check_BitErr(void *ite6664_handle);
void SetRxHpd(void *ite6664_handle, iTE_u8 sts);
void it6664_hdmirx(void *ite6664_handle);
void  Dump_IT666xReg(void *ite6664_handle);
void it6664_RXHDCP_OFF(void *ite6664_handle, iTE_u8 sts);
void it6664_SetFixEQ(void *ite6664_handle, iTE_u8 val);
void RX_FiFoRst(void *ite6664_handle);
void EDID_ParseVSDB_3Dblock(struct PARSE3D_STR *pstParse3D);

#ifdef AutoEQ
void it6664_Set_SAREQ(void *ite6664_handle, iTE_u8 SKEWOPT);
void it6664_SigleRSSKEW(void *ite6664_handle);
iTE_u8 it6664_RPTSAREQ(void *ite6664_handle, iTE_u8 SKEWOPT,iTE_u8 HDMI_mode);
void it6664_Read_SKEW(void *ite6664_handle);
iTE_u8 it6664_RptBitErr_ms(void *ite6664_handle, iTE_u8 Threshold);
void it6664_AutoEQ_State(void *ite6664_handle);
void Check_AMP(void *ite6664_handle, iTE_u8 Rec_Channel);
void it6664_ManuEQ(void *ite6664_handle, iTE_u8 mode);
void it6664_EQ14(void *ite6664_handle, iTE_u8 val);
void it6664_EQRst(void *ite6664_handle);
void it6664_EQchkOldResult(void *ite6664_handle, iTE_u8 hdmiver);
#endif



#endif
