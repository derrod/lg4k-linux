///*****************************************
//  Copyright (C) 2009-2018
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_DEV_DEFINE.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2018/01/03
//   @fileversion: iTE6805_MCUSRC_1.12
//******************************************/

#include "ite6805_config.h"
#define MS_TimeOut(x) ((x)+1)

#define LVDS 	0
#define TTL		1

#define MAX_PORT_NUM 2
#define PORT0 0
#define PORT1 1

#define SDR 0
#define DDR 1

#define HPD_LOW		0
#define HPD_HIGH	1
#define ENABLE		1
#define DISABLE		0
#define VIDEO_ON	1
#define VIDEO_OFF	0
#define AUDIO_ON	1
#define AUDIO_OFF	0
#define SCDT_ON		BIT7
#define SCDT_OFF	0
#define AVMUTE_ON	1
#define AVMUTE_OFF	0

#define MODE_HDMI	0x00
#define MODE_MHL	0x01
#define MODE_DVI	0x02

#define MODE_SINGLE 0
#define MODE_DUAL	1
#define MODE_5V_ON	1
#define MODE_5V_OFF	0
#define TRISTATE_ON		1
#define TRISTATE_OFF	0
#define HDCP_ENABLE		1
#define HDCP_DISABLE	0

#define MODE_POWER_STANDBY 2
#define MODE_POWER_SAVING 1
#define MODE_POWER_NORMAL 0

#define RAP_CONTENT_OFF 0
#define RAP_CONTENT_ON	1

#define TMDSCLKVALUE_480P 	35
#define TMDSCLKVALUE_1080P 	160	// for TMDS > 160 then set RS to 00, otherwise set to 3F

#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

#define RGB444			0
#define YCbCr422		1
#define YCbCr444		2
#define YCbCr420        3

#define CD8BIT			4
#define CD10BIT	 		5
#define CD12BIT			6
#define CD16BIT			7

//define in HDMI SPEC 2.0 PAGE 84
#define Audio_Sampling_1024K	0x35
#define Audio_Sampling_768K		0x09
#define Audio_Sampling_512K		0x3B
#define Audio_Sampling_384K		0x05
#define Audio_Sampling_256K		0x1B
#define Audio_Sampling_192K		0x0E
#define Audio_Sampling_176P4K	0x0C
#define Audio_Sampling_128K		0x2B	// new define
#define Audio_Sampling_96K		0x0A
#define Audio_Sampling_88P2K	0x08
#define Audio_Sampling_64K		0x0B	// new define
#define Audio_Sampling_48K		0x02
#define Audio_Sampling_44P1K	0x00
#define Audio_Sampling_32K		0x03

#define REG_RX_AVI_DB0 0x14 // REG_RX_214
#define REG_RX_AVI_DB1 0x15 // REG_RX_215
#define REG_RX_AVI_DB2 0x16 // REG_RX_216
#define REG_RX_AVI_DB3 0x17 // REG_RX_217
#define REG_RX_AVI_DB4 0x18 // REG_RX_218
#define REG_RX_AVI_DB5 0x19 // REG_RX_219
#define REG_RX_AVI_DB6 0x1A // REG_RX_21A
#define REG_RX_AVI_DB7 0x1B // REG_RX_21B
#define REG_RX_AVI_DB8 0x1C // REG_RX_21C
#define REG_RX_AVI_DB9 0x1D // REG_RX_21D
#define REG_RX_AVI_DB10 0x1E // REG_RX_21E
#define REG_RX_AVI_DB11 0x1F // REG_RX_21F
#define REG_RX_AVI_DB12 0x20 // REG_RX_220
#define REG_RX_AVI_DB13 0x21 // REG_RX_221
#define REG_RX_AVI_DB14 0x22 // REG_RX_222
#define REG_RX_AVI_DB15 0x23 // REG_RX_223

//LVDS define
#define JEIDA 0
#define VESA  1

#define LVDS6BIT	0x00
#define LVDS8BIT	BIT2
#define LVDS10BIT	BIT3

#define MODE_EvenOdd	0
#define MODE_LeftRight	1
#define MODE_DownScale	2
#define MODE_EvenOdd_Plus_DownScale 3

// EDID Support 3D define
#define MHL_3D_Mask					(0x07)
#define MHL_3D_Support_FramePacking BIT0
#define MHL_3D_Support_TopBottom	BIT1
#define MHL_3D_Support_SideBySide	BIT2

#define DRM_DATA_LEN	(28) // n370

typedef enum
{

    eTTL_SepSync_FullBusSDR_RGB444=0,
    eTTL_SepSync_FullBusSDR_YUV444,
	eTTL_SepSync_FullBusSDR_YUV422,
	eTTL_SepSync_FullBusSDR_BYPASS_CSC,

    eTTL_SepSync_FullBusDDR_RGB444,
    eTTL_SepSync_FullBusDDR_YUV444,
	eTTL_SepSync_FullBusDDR_YUV422,
	eTTL_SepSync_FullBusDDR_BYPASS_CSC,

    eTTL_SepSync_HalfBusDDR_RGB444,
    eTTL_SepSync_HalfBusDDR_YUV444,
	eTTL_SepSync_HalfBusDDR_BYPASS_CSC,

	eTTL_EmbSync_FullBusSDR_YUV422,
    eTTL_EmbSync_FullBusDDR_YUV422,
	eTTL_EmbSync_FullBusDDR_BYPASS_CSC,

    eTTL_BTA1004_SDR,
    eTTL_BTA1004_DDR,
    eTTL_VOMreserve
}Video_Output_Configure;

typedef enum
{
    eTTL_SDR=0,
    eTTL_DDR,
    eTTL_HalfBusDDR,
    eTTL_SDR_BTA1004,
    eTTL_DDR_BTA1004
}Video_Data_Path;

typedef enum
{
    eTTL_SepSync=0,
    eTTL_EmbSync,
}Video_Sync_Mode;

typedef struct {
    iTE_u16 HActive ;
    iTE_u16 VActive ;
    iTE_u16 HTotal ;
    iTE_u16 VTotal ;
    iTE_u32 PCLK ;
	iTE_u32 TMDSCLK;
	iTE_u32 FrameRate;
    iTE_u16 HFrontPorch ;
    iTE_u16 HSyncWidth ;
    iTE_u16 HBackPorch ;
    iTE_u16 VFrontPorch ;
    iTE_u16 VSyncWidth ;
    iTE_u16 VBackPorch ;
    iTE_u16 ScanMode ;
    iTE_u16 VPolarity ;
    iTE_u16 HPolarity ;
	iTE_u16 ColorDepth;
	iTE_u32 N;
	iTE_u32 CTS;
	iTE_u16 VIC;
	iTE_u16 Force_Sampling_Frequency;
} _iTE6805_VTiming ;

typedef enum  {
    STATES_Reset = 0,
	STATES_NorOp,
	STATES_AuthStr,
	STATES_AuthDn,
	STATES_Unknown
} STATES_Type ;

typedef enum {
    STATEV_VideoOff = 0,
	STATEV_Unplug,
	STATEV_WaitSync,
	STATEV_CheckSync,
	STATEV_VidStable,
    STATEV_Unknown
} STATEV_Type ;

typedef enum {
    STATEA_AudioOff = 0,
    STATEA_RequestAudio ,
    STATEA_WaitForReady,
    STATEA_AudioOn ,
    STATEA_Unknown,
    STATEA_Reserved
} STATEA_Type ;

typedef enum {
	STATEEQ_Off = 0 ,
	STATEEQ_ClockONDet_And_TriggerEQ,
	STATEEQ_EQWaitAutoEQUntilDone,
	STATEEQ_EQCheckAutoEQDone,
	STATEEQ_EQCheckBitErr,
	STATEEQ_EQManual,
	STATEEQ_WaitForManualEQ,
	STATEEQ_KeepEQStateUntil5VOff,
	STATEEQ_EQDone = 9,
    STATEEQ_Unknown
} STATEEQ_Type ;

typedef enum {
    STATECEC_None=0,
    STATECEC_ReadyToFire,
    STATECEC_TXFail,
	STATECEC_TXDone,
	STATECEC_RXCheck,
    STATECEC_Pending
} STATECEC_Type;

// AVI Info Frame Related start
typedef enum {
	RGB_RANGE_DEPENDED_ON_VIC = 0,
	RGB_RANGE_LIMIT = 1,
	RGB_RANGE_FULL = 2,
	RGB_RANGE_RESERVE
} AVI_RANGE_Type_RGB ;

typedef enum {
	YUV_RANGE_LIMIT = 0,
	YUV_RANGE_FULL = 1,
	YUV_RANGE_RESERVE1,
	YUV_RANGE_RESERVE2
} AVI_RANGE_Type_YUV ;

typedef enum {
	Color_Format_RGB = 0,
	Color_Format_YUV422 = 1,
	Color_Format_YUV444 = 2,
	Color_Format_YUV420 = 3,
	Color_Format_BYPASS_CSC = 4
} AVI_Color_Format ;

typedef enum {
	Colormetry_UNKNOW,
	Colormetry_ITU601 = 1,
	Colormetry_ITU709 = 2,
	Colormetry_Extend = 3
} AVI_Colormetry ;
// AVI Info Frame Related end

typedef enum {
	HDR_DISABLE,
	HDR_ITU709 = 1,
	HDR_ITU2020 = 2
} HDR_Mode;

// struct
// Data need to be Global, setting here
typedef struct
{
	#if (ENABLE_DETECT_DRM_PKT==TRUE)
	iTE_u8 Flag_HAVE_DRM_PKT;
	iTE_u8 Flag_NEW_DRM_PKT_RECEIVE;
	iTE_u8 DRM_HB[3];	// HDR Header Byte
	iTE_u8 DRM_DB[DRM_DATA_LEN];	// HDR Data Byte /*n370, 26 is too small. */
	#endif

	#if (DYNAMIC_HDCP_ENABLE_DISABLE==TRUE)
	iTE_u8 STATE_HDCP; // for customer control HDCP enable/disable by this parameter
	iTE_u8 STATE_HDCP_FINAL; // for record HDCP STATE only
	#endif

	iTE_u8 ChipID; // A0 B0 A1
	// SYS Related
	STATES_Type STATES;
    STATEV_Type STATEV;
    STATEA_Type STATEA;
	STATEEQ_Type STATEEQ;	// EQ
	STATECEC_Type STATECEC;	// CEC

	// Current state
	iTE_u8 CurrentPort;
	iTE_u8 CurrentPowerMode;

	// For Debug
	iTE_u8 DumpREG;

	// User Define Output Setting US : UserSetting
	Video_Data_Path		US_Video_Out_Data_Path; // eTTL_DDR, eTTL_HalfBusDDR, eTTL_SDR_BTA1004, eTTL_DDR_BTA1004
	Video_Sync_Mode		US_Video_Sync_Mode;		// eTTL_SepSync, eTTL_EmbSync
	AVI_Color_Format	US_Output_ColorFormat;
	//AVI_Colormetry		US_RGBQuantizationRange;	by Andynien, 20180223 base on source v1.13
	iTE_u8				US_Flag_PYPASS_CSC;

    // AVI Info Frame
    AVI_Color_Format	AVIInfoFrame_Input_ColorFormat;
    AVI_Colormetry		AVIInfoFrame_Colorimetry;
    AVI_RANGE_Type_RGB	AVIInfoFrame_RGBQuantizationRange;
    AVI_RANGE_Type_YUV	AVIInfoFrame_YUVQuantizationRange;
    iTE_u8				AVIInfoFrame_VIC;
	iTE_u8				AVIInfoFrame_VideoInputMode;
	iTE_u8				AVIInfoFrame_ExtendedColorimetry;

	iTE_u8 MHL_DiscoveryDone ;
	iTE_u8 MHL_RAP_Content_State;

	iTE_u8 Flag_VidStable_Done ;
	iTE_u8 Flag_DownScale ;
	iTE_u8 Flag_Pixel_Mode ;
	iTE_u8 Flag_InputMode;
	iTE_u8 Flag_AVI_ColorChange;

	//Added by Jeff_20180601
	//AVerMedia user data
	iTE_u8 AVT_Flag_Enable_Dither;

} _iTE6805_DATA;


// MHL 3D Related start
typedef struct
{
    iTE_u8 uc3DEdidStart;
    iTE_u8 uc3DBlock;
    iTE_u8 uc3DInfor[32];
    iTE_u8 ucVicStart;
    iTE_u8 ucVicCnt;
    iTE_u8 uc3DTempCnt;
    iTE_u8 ucDtdCnt;
    iTE_u8 bVSDBspport3D;
	iTE_u8 SVD_LIST[16];
}_iTE6805_PARSE3D_STR;

typedef struct
{
	iTE_u8 VIC;
	iTE_u8 HB0;
	iTE_u8 HB1;
	iTE_u8 HB2;
	iTE_u8 PB0;
	iTE_u8 PB1;
	iTE_u8 PB2;
	iTE_u8 PB3;
	iTE_u8 PB4;
	iTE_u8 PB5;
	iTE_u8 PB6;
	iTE_u8 PB7;
}DIG_DE3DFRAME;
/*
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
*/
// MHL 3D Related end





// kuro end
