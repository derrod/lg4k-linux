///*****************************************
//  Copyright (C) 2009-2018
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <config.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2018/01/03
//   @fileversion: iTE6805_MCUSRC_1.12
//******************************************/
#ifndef _ITE6805_CONFIG_H_
#define _ITE6805_CONFIG_H_

//#ifdef _iTE6805_
//#pragma message("defined _iTE6805_ config")
//#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// 68051 68052 EVB Option
//////////////////////////////////////////////////////////////////////////////////////////


#ifndef DEMO
#define DEMO FALSE	// TRUE // for demo code MCU pin, TRUE/FALSE by AndyNien, 20180105
#endif
// DEMO code
// if EDID_WP0 = 0, when 4k then output dowscale to 1080p
// if EDID_WP0 = 1, if EDID_WP1 = 0 ,when 4k the output is Odd Even Mode
// if EDID_WP0 = 1, if EDID_WP1 = 1 ,when 4k the output is LeftRight Mode


#ifndef EVB_4096_DOWNSCALE_TO_2048_OR_1920_CONTROL_BY_PIN
#define EVB_4096_DOWNSCALE_TO_2048_OR_1920_CONTROL_BY_PIN FALSE// for EVB DownScale By PIN P0^0
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// 68051 68052 Common Config
//////////////////////////////////////////////////////////////////////////////////////////
// IT68051: iTE68051=TRUE/iTE68052=FALSE
// IT68052: iTE68051=FALSE/iTE68052=TRUE
// iTE6028: EVB Only
//////////////////////////////////////////////////////////////////////////////////////////


#ifndef iTE68051
#define iTE68051 TRUE // iTE68051 iTE68052
#else
//#pragma message("iTE68051 is pre-defined.")
#endif

#ifndef iTE6028
#define iTE6028 TRUE // iTE68051 6028 Init or not
#else
//#pragma message("iTE6028 is pre-defined.")
#endif

#ifndef iTE68052
#define iTE68052 FALSE // iTE68051 iTE68052
#else
//#pragma message("iTE68052 is pre-defined.")
#endif

#if ((!iTE68051)&&(!iTE68052))
#error message("no define ITE68051 and ITE68052")
#endif

#if (iTE68051)
//#pragma message("Defined for IT68051")
#endif

#if (iTE68052)
//#pragma message("Defined for IT68052")
#endif

#define MAIN_PORT PORT0 // please refer EVB_AUTO_DETECT_PORT_BY_PIN for set main port

#define COLOR_DEPTH_BY_GCPINFOFRAME	// If define this config output color depth will set by GCP infoframe

#ifndef COLOR_DEPTH_BY_GCPINFOFRAME
#define Output_Color_Depth 8	// only set to 8 : 8bit , 10 : 10bit, 12 : 12bit
#endif

#ifndef COLOR_RANGE_DEFAULT
#define COLOR_RANGE_DEFAULT	RGB_RANGE_FULL	// RGB_RANGE_LIMIT/RGB_RANGE_FULL
#endif

//////////////////////////////////////////////////////////////////////////////////////////
// 68051 68052 Common Config
//////////////////////////////////////////////////////////////////////////////////////////

// Dynamic Range InformFrame Detect
// for detect HDR pkt by funtion iTE6805_DRM_Detect();
// using void iTE6805_CHECK_DRMInfoframe(iTE_u8 *pFlag_HAVE_DRM_PKT" , iTE_u8  ---> "*pFlag_NEW_DRM_PKT_RECEIVE<---) for detect new DRM PKT
// using void iTE6805_CHECK_DRMInfoframe(iTE_u8 ---> *pFlag_HAVE_DRM_PKT" <--- , iTE_u8 "*pFlag_NEW_DRM_PKT_RECEIVE) for receive MDR PKT or not (change by iTE68051, customer using it for check)
// !!!! and every platform need to adjust MAX_NO_DRM_PKT_RECEIVE_COUNT for judge have DRM PKT or not because every while fsm loop interval is different!!!!
#ifndef ENABLE_DETECT_DRM_PKT
#define ENABLE_DETECT_DRM_PKT TRUE // TRUE or FALSE			// turn on by AndyNien, 20180105
#endif

// this define for dynamic HDCP enable/disable by using variable iTE6805_DATA.STATE_HDCP = HDCP_ENABLE/HDCP_DISABLE
// for customer can change HDCP state in upper code level, default HDCP is enable
// default disable this define
#ifndef DYNAMIC_HDCP_ENABLE_DISABLE
#define DYNAMIC_HDCP_ENABLE_DISABLE TRUE // TRUE or FALSE		// turn on by AndyNien, 20180105
#endif

// DownScale Filter : DownScale Quality setting
#define DownScale_Filter 0x01 // Default :0x01 ---- 0x01 : 1:6:1 , 0x10 : 1:2:1 , 0x00 : bypass


// DDR setting
#define Clock_Delay  0			// arg from 0 to 7, by AndyNien, 20180221, FPGA suggestion, clock phase adjust
#define Clock_Invert 0			// arg 0/1,  by AndyNien, 20180221, FPGA suggestion, clock phase adjustInvert

// SDR setting
//#define Clock_Delay  3	// arg from 0 to 7
//#define Clock_Invert 0	// arg 0/1

#define Output_Color_Depth 8	// only set to 8 : 8bit , 10 : 10bit, 12 : 12bit

#define Reg_IOMode 0x40	// default 0x40, 0x00 / 0x40 / 0x80

#define DownScale_YCbCr_Color_Range 1	// 1 : Full Range  0 : Limit Range

#ifndef Enable_LR_Overlap
#define Enable_LR_Overlap 0	// 1 for enable overlap when LR mode only
#endif


//////////////////////////////////////////////////////////////////////////////////////////
// EQ Config
//////////////////////////////////////////////////////////////////////////////////////////
#define _ENABLE_AUTO_EQ_ FALSE //TRUE	// TRUE or FALSE
#define EQ_INIT_VALUE 0x9F		// EQ Default Value , EQ can be 0x80 &{ 0x7F, 0x7E, 0x3F, 0x3E, 0x1F, 0x1E, 0x0F, 0x0E, 0x07, 0x06, 0x03, 0x02, 0x01, 0x00 };
//#define EQ_KURO_TEST			// for debug EQ


//////////////////////////////////////////////////////////////////////////////////////////
// iTE68051 Config
////////////////////////////////////////////////////S//////////////////////////////////////
#define ENABLE_6805_POWER_SAVING_MODE TRUE
#define ENABLE_6805_POWER_DETECT_ONLY_TMDS_CLOCK_POWER_ON FALSE	// WARNING :  USING THIS DEFINE CTS WILL NOT PASS BUT POWER MORE SAVING
																	// If 5v detect -> ONLY TMDS Clock differential pair power on ,
																	// If Clock detect -> ALL power on (TMDS RGB Channel + Clock and the others)

// If define DEMO, this define will be ignored, iTE68051_4K60_Mode setting will by WP1 pin setting
#ifndef iTE68051_4K60_Mode
#define iTE68051_4K60_Mode MODE_EvenOdd // MODE_EvenOdd or MODE_LeftRight or MODE_DownScale or MODE_EvenOdd_Plus_DownScale
#endif

#define iTE6028_DE_Delay 0xD1 // can be D0~D3, De Delay Level D3>D2>D1>D0



//////////////////////////////////////////////////////////////////////////////////////////
// iTE68051 TTL Config
//////////////////////////////////////////////////////////////////////////////////////////

// iTE68051 RD Suggest all using FullBus DDR Mode
// (6028 no half bus mode and SDR mode can't output 4k60 because LVDS HW limit)
#ifndef eVidOutConfig
#define eVidOutConfig  		eTTL_SepSync_FullBusDDR_BYPASS_CSC 	// eTTL_SepSync_FullBusDDR_BYPASS_CSC, by AndyNien, 20180105
#endif
//eTTL_SepSync_FullBusSDR_RGB444,	// SDR to 66121 can't output 1080p mode
//eTTL_SepSync_FullBusSDR_YUV444,
//eTTL_SepSync_FullBusSDR_YUV422,
//eTTL_SepSync_FullBusSDR_BYPASS_CSC,
//eTTL_SepSync_FullBusDDR_RGB444,
//eTTL_SepSync_FullBusDDR_YUV444,
//eTTL_SepSync_FullBusDDR_YUV422,
//eTTL_SepSync_FullBusDDR_BYPASS_CSC,
//eTTL_SepSync_HalfBusDDR_RGB444,		// 6805A0 6028 do not sup this bus mode, dont using it when 6805A0
//eTTL_SepSync_HalfBusDDR_YUV444,		// 6805A0 6028 do not sup this bus mode, dont using it when 6805A0
//eTTL_SepSync_HalfBusDDR_BYPASS_CSC,	// 6805A0 6028 do not sup this bus mode, dont using it when 6805A0
//eTTL_EmbSync_FullBusSDR_YUV422,
//eTTL_EmbSync_FullBusDDR_YUV422,
//eTTL_EmbSync_FullBusDDR_BYPASS_CSC,
//eTTL_BTA1004_SDR,
//eTTL_BTA1004_DDR

// If TMDS < 162 , setting to (SDR)eTTL_SDR
// If TMDS >= 162 , setting to (DDR)eTTL_DDR
// If set this will ignore eVidOutConfig's SDR DDR setting, default undefined

// If ALL_USING_DUAL_PORT_OUT = TRUE , then all mode is dual pixel mode, two port output.
// default define is FALSE , when FALSE, 4K50 (PCLK >3G) up using dual port (dual pixel mode), else single pixel

#define DUAL_PIXEL_MODE_PCLK_CONDITION 320000
// default PCLK > 3G will output dual pixel mode (two port output)
// this define is for if wanna dual port output change to 1.6G upper , fix the default value 320000 to 160000

//////////////////////////////////////////////////////////////////////////////////////////
// 68052 LVDS 4K output mode config
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef Mode_68052_4K
#define Mode_68052_4K MODE_LeftRight	// MODE_EvenOdd or MODE_LeftRight or
#endif


//////////////////////////////////////////////////////////////////////////////////////////
// 68052 LVDS Config
//////////////////////////////////////////////////////////////////////////////////////////
#define LVDSSwap	FALSE

#ifndef LVDSMode
#define LVDSMode	JEIDA		// JEIDA or VESA
#endif

#ifndef LVDSColDep
#define LVDSColDep	LVDS8BIT	// LVDS6BIT LVDS8BIT LVDS10BIT
#endif

#ifndef LVDSSSC
#define LVDSSSC		DISABLE		// ENABLE or DISABLE
#endif




#endif // _CONFIG_H_
