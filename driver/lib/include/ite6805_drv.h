///*****************************************
//  Copyright (C) 2009-2018
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_DRV.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2018/01/03
//   @fileversion: iTE6805_MCUSRC_1.12
//******************************************/

#ifndef _ITE6805_DRV_H_
#define _ITE6805_DRV_H_

#include "ite6805.h"


#define BANK 0x0F
#define BANKM 0x07

#define PORT1_HPD_ON	0
#define PORT1_HPD_OFF	1


void iTE6805_Set_LVDS_Video_Path(void *ite6805_handle, iTE_u8 LaneNum);
void iTE6805_Set_TTL_Video_Path(void *ite6805_handle);
void iTE6805_Set_AVMute(void *ite6805_handle, iTE_u8 AVMUTE_STATE);
void iTE6805_Set_ColorDepth(void *ite6805_handle);
void iTE6805_Set_DNScale(void *ite6805_handle);
void iTE6805_Set_HPD_Ctrl(void *ite6805_handle, iTE_u16 PORT_NUM, iTE_u16 HPD_State);
void iTE6805_Set_Video_Tristate(void *ite6805_handle, iTE_u8 VIDEO_STATE);
void iTE6805_Set_Audio_Tristate(void *ite6805_handle, iTE_u8 AUDIO_STATE);
void iTE6805_Set_1B0_By_PixelClock(void *ite6805_handle);

#if (ENABLE_6805_POWER_SAVING_MODE==TRUE)
void iTE6805_Set_Power_Mode(void *ite6805_handle, iTE_u8 Mode);
#endif

#if (DYNAMIC_HDCP_ENABLE_DISABLE==TRUE)
void iTE6805_Set_HDCP(void *ite6805_handle, iTE_u8 HDCP_STATE);
#endif

void iTE6805_Reset_ALL_Logic(void *ite6805_handle, iTE_u8 PORT_NUM);
void iTE6805_Reset_Video_Logic(void *ite6805_handle);
void iTE6805_Reset_Audio_Logic(void *ite6805_handle);
void iTE6805_Reset_Video_FIFO(void *ite6805_handle);

iTE_u8 iTE6805_Check_HDMI_OR_DVI_Mode(void *ite6805_handle, iTE_u8 PORT_NUM);
iTE_u8 iTE6805_Check_CLK_Vaild(void *ite6805_handle);
iTE_u8 iTE6805_Check_SCDT(void *ite6805_handle);
iTE_u8 iTE6805_Check_AUDT(void *ite6805_handle);
iTE_u8 iTE6805_Check_AVMute(void *ite6805_handle);
iTE_u8 iTE6805_Check_5V_State(void *ite6805_handle, iTE_u8 PORT_NUM);
iTE_u8 iTE6805_Check_Single_Dual_Mode(void *ite6805_handle);
iTE_u8 iTE6805_Check_4K_Resolution(void *ite6805_handle);
iTE_u8 iTE6805_Check_HDMI2(void *ite6805_handle);
iTE_u8 iTE6805_Check_EQ_Busy(void *ite6805_handle);
iTE_u8 iTE6805_Check_TMDS_Bigger_Than_1G(void *ite6805_handle);
iTE_u8 iTE6805_Check_Scramble_State(void *ite6805_handle);
#if (ENABLE_6805_POWER_SAVING_MODE==TRUE)
void iTE6805_Check_MHL_Mode_Change_Need_Power_On(void *ite6805_handle);
#endif


void	iTE6805_Get_AVIInfoFrame_Info(void *ite6805_handle);
void	iTE6805_Get_VID_Info(void *ite6805_handle);
void	iTE6805_Get_PCLK(void *ite6805_handle);
iTE_u16 iTE6805_Get_VIC_Number(void *ite6805_handle);

void	iTE6805_Show_AUD_Info(void *ite6805_handle);
void	chgbank(void *ite6805_handle, iTE_u16 bank );

void 	iTE6805_OCLK_Cal(void *ite6805_handle);
iTE_u16 iTE6805_OCLK_Load(void *ite6805_handle);

void iTE6805_Init_fsm(void *ite6805_handle);
void iTE6805_Init_TTL_VideoOutputConfigure(void *ite6805_handle);
void iTE6805_Init_CAOF(void *ite6805_handle);

#endif

