///*****************************************
//  Copyright (C) 2009-2018
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_SYS.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2018/01/03
//   @fileversion: iTE6805_MCUSRC_1.12
//******************************************/
#ifndef _ITE6805_SYS_H_
#define _ITE6805_SYS_H_

// IRQ
void iTE6805_hdmirx_port0_SYS_irq(void *ite6805_handle);
void iTE6805_hdmirx_port1_SYS_irq(void *ite6805_handle);
void iTE6805_hdmirx_common_irq(void *ite6805_handle);
void iTE6805_hdmirx_common_irq_for_HPD(void *ite6805_handle);

// finite state machine
void iTE6805_vid_fsm(void *ite6805_handle);
void iTE6805_aud_fsm(void *ite6805_handle);
void iTE6805_vid_chg(void *ite6805_handle, STATEV_Type NewState);
void iTE6805_aud_chg(void *ite6805_handle, STATEA_Type NewState);

// output setting
void iTE6805_Port_Select(void *ite6805_handle, iTE_u8 ucPortSel);
void iTE68051_Video_Output_Setting(void *ite6805_handle);
void iTE68052_Video_Output_Setting(void *ite6805_handle);
void iTE6805_Enable_Video_Output(void *ite6805_handle);
void iTE6805_Enable_Audio_Output(void *ite6805_handle);

// INT function
void iTE6805_INT_5VPWR_Chg(void *ite6805_handle, iTE_u8 ucport);
void iTE6805_INT_HDMI_DVI_Mode_Chg(void *ite6805_handle, iTE_u8 ucport);
void iTE6805_INT_SCDT_Chg(void *ite6805_handle);

// Identify chip
void iTE6805_Identify_Chip(void *ite6805_handle);

void iTE6805_Port_Detect(void *ite6805_handle);


#if (DYNAMIC_HDCP_ENABLE_DISABLE==TRUE)
void iTE6805_HDCP_Detect(void *ite6805_handle);
#endif

#if (ENABLE_DETECT_DRM_PKT==TRUE)
void iTE6805_DRM_Detect(void *ite6805_handle);
void iTE6805_GET_DRMInfoframe(void *ite6805_handle, iTE_u8 *pInfoframe);
void iTE6805_CHECK_DRMInfoframe(void *ite6805_handle, iTE_u8 *pFlag_HAVE_DRM_PKT, iTE_u8 *pFlag_NEW_DRM_PKT_RECEIVE);
#endif

void iTE6805_GET_AVIInfoframe(void *ite6805_handle, iTE_u8 *pInfoframe);
void iTE6805_GET_AudioInfoframe(void *ite6805_handle, iTE_u8 *pInfoframe);
void iTE6805_GET_SPDInfoframe(void *ite6805_handle, iTE_u8 *pInfoframe);
void iTE6805_GET_VendorSpecificInfoframe(void *ite6805_handle, iTE_u8 *pInfoframe);
void iTE6805_GET_DownScale_Flag(void *ite6805_handle, iTE_u8 *pFlag_DownScale);
void iTE6805_GET_HDMI_ColorType(void *ite6805_handle, iTE_u8 *pFlag_ColorType);
void iTE6805_GET_Pixel_Mode(void *ite6805_handle, iTE_u8 *pFlag_Pixel_Mode);
void iTE6805_GET_Input_Type(void *ite6805_handle, iTE_u8 *pFlag_Input_Type);
void iTE6805_GET_Input_Color(void *ite6805_handle, iTE_u8 *pFlag_Input_Color);

void iTE6805_SET_RGBColorRange(void *ite6805_handle, iTE_u8 Color_Range);

iTE_u8	iTE6805_Detect3DFrame(void *ite6805_handle);
iTE_u8	iTE6805_Check_ColorChange(void *ite6805_handle);
void	iTE6805_Check_ColorChange_Update(void *ite6805_handle);
void iTE6805_INT_ECC_ERROR(void *ite6805_handle);

#endif
