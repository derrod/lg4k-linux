///*****************************************
//  Copyright (C) 2009-2017
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_EQ.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2017/12/01
//   @fileversion: iTE6805_MCUSRC_1.10
//******************************************/
#ifndef _ITE68051_EQ_H_
#define _ITE68051_EQ_H_

void iTE6805_EQ_fsm(void *ite6805_handle);
void iTE6805_EQ_chg(void *ite6805_handle, STATEEQ_Type NewState);
void iTE6805_hdmirx_port0_EQ_irq(void *ite6805_handle);
void iTE6805_hdmirx_port1_EQ_irq(void *ite6805_handle);
void iTE6805_Trigger_SAREQ(void *ite6805_handle);		// for HDMI2.x
void iTE6805_Trigger_EQ(void *ite6805_handle);			// for HDMI1.x
void iTE6805_Trigger_RSSKEW_EQ(void *ite6805_handle);	// for HDMI2.x and SAREQ Fail
void	iTE6805_BitErr_Get(void *ite6805_handle);
iTE_u8	iTE6805_BitErr_Check(void *ite6805_handle);
iTE_u8	iTE6805_BitErr_Check_Again(void *ite6805_handle);
iTE_u8	iTE6805_BitErr_Check_For_Parity_Error(void *ite6805_handle);
void iTE6805_Report_Skew(void *ite6805_handle);
void iTE6805_Report_EQ(void *ite6805_handle);
void iTE6805_Reset_EQ(void *ite6805_handle);
void iTE6805_Get_DFE(void *ite6805_handle, iTE_u8 Channel_Color);
void iTE6805_Set_DFE(void *ite6805_handle, iTE_u8 EQ_Value, iTE_u8 Type_Channel);
iTE_u8 iTE6805_Find_indexof_DEF(iTE_u8 EQ_Value);
void iTE6805_Set_EQResult_Flag(void *ite6805_handle);
#endif
