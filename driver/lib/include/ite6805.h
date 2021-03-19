#ifndef _ITE68051_SRC_H_
#define _ITE68051_SRC_H_

#include "cxt_mgr.h"
#include "mem_model.h"
#include "i2c_model.h"
#include "sys.h"
#include "trace.h"
#include "debug.h"
#include "task_model.h"
#include "ite6664.h"
#include "it6664_typedef.h"
#include "it6664_hdmi2_rx.h"
#include "ite6805_dev_define.h"

#define ITE6805_ID ite6805
#define ITE6805_DRVNAME I2C_MODEL_MODULE_STR(ite6805)

#define P0 0
#define P1 1
#define P2 2
#define P3 3

//////////////////////////////////////////////////////////////////////////////////////////
// Other Config
//////////////////////////////////////////////////////////////////////////////////////////
#define ADDR_HDMI   0x90
#define	ADDR_MHL	0xE0	//Software programmable I2C address of iTE6805 MHL
#define	ADDR_EDID	0xA8	//Software programmable I2C address of iTE6805 EDID RAM
#define ADDR_CEC	0xC8	//Software programmable I2C address of iTE6805 CEC
#define ADDR_LVDS	0xB4	//Software programmable I2C address of iTE6805 LVDS

//////////////////////////////////////////////////
// MCU 8051data type
//////////////////////////////////////////////////

#define _HPDMOS_
#ifdef _HPDMOS_
    #define PORT1_HPD_ON	0
    #define PORT1_HPD_OFF	1
#else
    #define PORT1_HPD_ON	1
    #define PORT1_HPD_OFF	0
#endif

#ifndef EDID_SELECT_TABLE
#define EDID_SELECT_TABLE	(16)  // 8->16 only 3840 EDID
#endif


// move from sha1.cpp
#define WCOUNT 17

// move from	ite68051_eq.cpp
#define Table_CED_Max_Index 5 // iTE6805_EQ_Table size + 1(auto EQ result)

// move from ite6805_cec_cmdtable.cpp
#define DIRECTED	0x80
#define BCAST1_4	0x40
#define BCAST2_0	0x20	/* broadcast only allowed for >= 2.0 */
#define BCAST		(BCAST1_4 | BCAST2_0)
#define BOTH		(BCAST | DIRECTED)
#define DRM_HDR_METADATA_SIZE	31
#define AVI_INFOFRAME_SIZE		19

enum eDeviceITE68051
{
	DEVICE_ITE68051_0			//1'st 68051
								//DEVICE_68051_1,
};

typedef enum
{
	CSCMtx_RGB2YUV_ITU601_16_235,
	CSCMtx_RGB2YUV_ITU601_00_255,
	CSCMtx_RGB2YUV_ITU709_16_235,
	CSCMtx_RGB2YUV_ITU709_00_255,
	CSCMtx_YUV2RGB_ITU601_16_235,
	CSCMtx_YUV2RGB_ITU601_00_255,
	CSCMtx_YUV2RGB_ITU709_16_235,
	CSCMtx_YUV2RGB_ITU709_00_255,
	CSCMtx_YUV2RGB_BT2020_00_255,	// add by AndyNien, 20180223, base on v1.13 source
	CSCMtx_RGB_00_255_RGB_16_235,
	CSCMtx_RGB_16_235_RGB_00_255,
	CSCMtx_Unknown,
} _CSCMtx_Type;

typedef enum eVideoStatus {             //@enum Video decoder status
    VS_HVLOCK,                  //@emem horizontal and vertical locked
    VS_HVLOCKDATA,              //@emem completely horizontal and vertical locked byte
    VS_HRES,                    //@emem horizontal resolution detected
    VS_VRES,                    //@emem vertical resolution detected
    VS_INTL,                    //@emem status bit for interlace detection
    VS_TOTH,                    //@emem total horizontal pixels detected
    VS_TOTV,                    //@emem total vertical lines detected
    VS_FPS,                     //@emem input frame rate 
    VS_FPS_OUT,                 //@emem output frame rate      // [Ian_012_20131004]
    VS_OUTPUT                   //@emem stable output video   // [Ian_008_20130913] 
}eVideoStatus;

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
    RGB_CRM_LIMITED	= 0,
	RGB_CRM_FULL	= 1,
	RGB_CRM_AUTO	= 2,
}eRGBColorRangeMode;

enum eOutputRange
{
	FORCE_LIMITED_RANGE	= 0,
	FORCE_FULL_RANGE	= 1,
	FOLLOW_INPUT		= 2,
};

enum eSnapShotOption
{
	SS_FORCE_LIMITED_RANGE	= 0,
	SS_FORCE_FULL_RANGE		= 1,
	SS_FOLLOW_INPUT			= 2,
};

enum eVideoLogicSource {        //@enum Video decoder input in logic
    VIDEO_SOURCE1 = 0,          //@emem input is tuner						// h012 Component
    VIDEO_SOURCE2,              //@emem input is CVBS channel				// h012 VGA
    VIDEO_SOURCE3,              //@emem input is S-Video channel			// h012 HDMI
    MAX_VIDEO_LOGIC_SOURCE,     //@emem Video decoder logic input number 
    VIDEO_NONE                  //this value used as no logic input map
};

enum eAudioLogicSource {        //@enum Audio source logic definition
    AUDIO_SOURCE1=0,            //@emem The input is of type tuner
    AUDIO_SOURCE2,				//@emem The input is a Line In channel 1
    AUDIO_SOURCE3,              //@emem The input is a SPDIFDigital In signal channel 1
    MAX_AUDIO_LOGIC_SOURCE,     //MAX number of logic source
    AUDIO_NONE                  //@emem Audio input is none, disable, MUTE.
};


typedef enum
{
    ITE6805_OUT_FORMAT_SDR_ITU656_8 = 0x00,
    ITE6805_OUT_FORMAT_SDR_ITU656_10 = 0x01,
    ITE6805_OUT_FORMAT_SDR_ITU656_12_MODE0 = 0x02,
    ITE6805_OUT_FORMAT_SDR_ITU656_12_MODE1 = 0x06,
    ITE6805_OUT_FORMAT_SDR_ITU656_12_MODE2 = 0x0a,
    ITE6805_OUT_FORMAT_DDR_422_8 = 0x20,
    ITE6805_OUT_FORMAT_DDR_422_10 = 0x21,
    ITE6805_OUT_FORMAT_DDR_422_12_MODE0 = 0x22,
    ITE6805_OUT_FORMAT_DDR_422_12_MODE1 = 0x23,
    ITE6805_OUT_FORMAT_DDR_422_12_MODE2 = 0x24,
    ITE6805_OUT_FORMAT_SDR_444_24 = 0x40,
    ITE6805_OUT_FORMAT_SDR_444_30 = 0x41,
    ITE6805_OUT_FORMAT_SDR_444_36_MODE0 = 0x42,
    ITE6805_OUT_FORMAT_SDR_444_36_MODE1 = 0x46,
    ITE6805_OUT_FORMAT_SDR_444_24_MODE3 = 0x4C,
    ITE6805_OUT_FORMAT_SDR_444_24_MODE4 = 0x50,
    ITE6805_OUT_FORMAT_SDR_444_30_MODE4 = 0x51,
    ITE6805_OUT_FORMAT_SDR_444_36_MODE4 = 0x52,
    ITE6805_OUT_FORMAT_SDR_444_2X24_INTERLEAVE_MODE0 = 0x54,            
    ITE6805_OUT_FORMAT_DDR_444_24 = 0x60,
    ITE6805_OUT_FORMAT_DDR_444_30 = 0x61,
    ITE6805_OUT_FORMAT_DDR_444_36 = 0x62,
    ITE6805_OUT_FORMAT_SDR_ITU656_16 = 0x80,
    ITE6805_OUT_FORMAT_SDR_ITU656_20 = 0x81,
    ITE6805_OUT_FORMAT_SDR_ITU656_24_MODE0 = 0x82,
    ITE6805_OUT_FORMAT_SDR_ITU656_24_MODE1 = 0x86,
    ITE6805_OUT_FORMAT_SDR_ITU656_24_MODE2 = 0x8a,
    ITE6805_OUT_FORMAT_SDR_422_20_MODE3    =0x8d,
    ITE6805_OUT_FORMAT_SDR_422_16_MODE4    =0x90,
    ITE6805_OUT_FORMAT_SDR_422_20_MODE4    =0x91,
    ITE6805_OUT_FORMAT_SDR_422_24_MODE4    =0x92,
    ITE6805_OUT_FORMAT_SDR_422_2X16_INTERLEAVE_MODE0 = 0x94,                    
    ITE6805_OUT_FORMAT_SDR_422_2X20_INTERLEAVE_MODE0 = 0x95,        
    ITE6805_OUT_FORMAT_SDR_422_2X24_INTERLEAVE_MODE0 = 0x96,        
    ITE6805_OUT_FORMAT_PAR_8_MODE0=0xc0,
    ITE6805_OUT_FORMAT_PAR_10_MODE0=0xc1,
    ITE6805_OUT_FORMAT_PAR_12_MODE0=0xc2,
    
}ite6805_out_format_e;

// [Jeff_056_20141009]
typedef struct _RXITE68051_VIDEO_POS_PARAM
{
	iTE_s32 Min;
	iTE_s32 Max;
	iTE_s32 DefaultStart;
	iTE_s32 DefaultEnd;
	iTE_s32 Offset;
} RXITE68051_VIDEO_POS_PARAM, *PRXITE68051_VIDEO_POS_PARAM;

// [Jeff_056_20141009]
typedef struct _RXITE68051_VIDEO_POS
{
	RXITE68051_VIDEO_POS_PARAM Hor;
	RXITE68051_VIDEO_POS_PARAM Ver;
} RXITE68051_VIDEO_POS, *PRXITE68051_VIDEO_POS;

// h012+s
#define	ITE_68051_BRIGHTNESS_MAX			127
#define	ITE_68051_BRIGHTNESS_DEFAULT		0
#define	ITE_68051_BRIGHTNESS_MIN			-128

#define	ITE_68051_CONTRAST_MAX			255
#define	ITE_68051_CONTRAST_DEFAULT		128
#define	ITE_68051_CONTRAST_MIN			0

#define	ITE_68051_SATURATION_MAX			255
#define	ITE_68051_SATURATION_DEFAULT		128
#define	ITE_68051_SATURATION_MIN			0

#define	ITE_68051_HUE_MAX				127
#define	ITE_68051_HUE_DEFAULT			0
#define	ITE_68051_HUE_MIN				-128

typedef enum
{
    ITE6805_LOCK,
    ITE6805_UNLOCK,  
    ITE6805_HDCP,           
    ITE6805_HPD_LOW,
    ITE6805_HPD_HIGH,
}ite6805_event_e;

typedef enum
{
    XTAL_FREQ_28_63636MHz,
    XTAL_FREQ_27MHz,
    XTAL_FREQ_24_576MHz,        
    XTAL_FREQ_24MHz,
}ite6805_xtal_freq_e;

typedef enum  {
    ITE6805_OP_CH_SEL_GBR_YUV = 0,
    ITE6805_OP_CH_SEL_GRB_YVU = 1,
    ITE6805_OP_CH_SEL_BGR_UYV = 2,
    ITE6805_OP_CH_SEL_RGB_VYU = 3,
    ITE6805_OP_CH_SEL_BRG_UVY = 4,
    ITE6805_OP_CH_SEL_RBG_VUY = 5,
}ite6805_out_ch_order_e;

typedef enum 
{
	ITE6805_OUT_DEFAULT,
    ITE6805_OUT_YUV601,
    ITE6805_OUT_YUV709,
    ITE6805_OUT_RGB_LIMITED,
    ITE6805_OUT_RGB_FULL,
}ite6805_out_colorspace_e;

typedef struct
{
    ite6805_out_format_e out_fmt;
    ite6805_out_ch_order_e out_ch_order;
    ite6805_out_colorspace_e out_colorspace;
    
    unsigned blank_data:1;  // 0: no blank data 1: blank data
    unsigned insert_av_codes:1; // 0:No AV codes 1:AV codes 
    unsigned invert_cbcr:1; // 0: follow out_format 1: swap cb,cr
    unsigned output_bus_lsb_to_msb:1; // 0: MSB to LSB 1: LSB to MSB

}ite6805_out_ctrl_t;

typedef struct
{
    U8_T sw_map_addr;
    U8_T txcom_map_addr;
    U8_T rxp0_map_addr;
    U8_T txp0_map_addr;
    U8_T edid_map_addr;
    
    ite6805_xtal_freq_e xtal_freq;
    ite6805_out_ctrl_t out_ctrl;
    
}ite6805_cfg_t;

typedef enum
{
    CS_YUV,
    CS_RGB_FULL,
    CS_RGB_LIMIT,
}ite6805_colorspace_e;

enum ite6805_audio_sample {
	ITE6805_AUDIO_RATE_32000, //0
	ITE6805_AUDIO_RATE_44100, //1
	ITE6805_AUDIO_RATE_48000, //2
	ITE6805_AUDIO_RATE_88200,
	ITE6805_AUDIO_RATE_96000,
	ITE6805_AUDIO_RATE_176400,
	ITE6805_AUDIO_RATE_192000,
};

typedef struct
{
    unsigned width;
    unsigned height;
    BOOL_T is_interlace;
    unsigned framerate;
    unsigned denominator;
    U32_T    pixel_clock;
    ite6805_colorspace_e packet_colorspace;
    int dual_pixel;
    int sampling_mode;
    int dual_pixel_like;
    int ddr_mode;
    //int packet_colorspace;
    //unsigned int ite6805_hdcp_flag;
}ite6805_frameinfo_t;

typedef void (*ite6805_callback_t)(void *cxt,ite6805_event_e event);

typedef struct 
{
    void* bus_handle;
    trace_attr_handle_t attr_handles[4];
    U8_T id;
    ite6805_callback_t callback_func;
    ite6805_event_e lock;
    void *cb_cxt;
    task_handle_t task;
    task_model_handle_t taskmgr;
    ite6664_cxt_t *ite6664_handle;

    iTE_u32 RCLKVALUE;
    //iTE_u8  CEC_timeunit;

	// iTE6805_EDID.cpp
    iTE_u8  rxphyadr[2][2];	// for EDID RAM function

    // iTE6805_EQ.cpp
    iTE_u8 DFE_VALUE[14][3][3];  // equal as EQ RS_Value
    iTE_u8 *iTE6805_EQ_Table;

    iTE_u8 CED_Err[Table_CED_Max_Index][6]; // index 0 for autoEQ, 1:4 are Manual EQ
    iTE_u32 CED_Err_Total[3];

	iTE_u8 gEQ_B_TargetRS;
	iTE_u8 gEQ_G_TargetRS;
	iTE_u8 gEQ_R_TargetRS;

	iTE_u8 gEQ_B_BitErr_Valid;
	iTE_u8 gEQ_G_BitErr_Valid;
	iTE_u8 gEQ_R_BitErr_Valid;

	iTE_u8 gEQ_B_BitErr_Valid_Changed;
	iTE_u8 gEQ_G_BitErr_Valid_Changed;
	iTE_u8 gEQ_R_BitErr_Valid_Changed;

	iTE_u8 gEQ_B_BitErr_Valid_index;
	iTE_u8 gEQ_G_BitErr_Valid_index;
	iTE_u8 gEQ_R_BitErr_Valid_index;

	iTE_u8 Flag_Done_EQ;
	iTE_u8 Flag_Done_SAREQ;
	iTE_u8 Flag_Fail_EQ;
	iTE_u8 Flag_Fail_SAREQ;
	iTE_u8 Flag_Need_Trigger_RSSKEW;
	iTE_u8 Flag_Need_Check_EQResult;
	iTE_u8 Flag_AMP_ALL_Timeout;
	iTE_u8 Flag_Need_Adjust_RSValue;

	iTE_u8 Flag_Trigger_EQ;
	iTE_u8 Flag_Trigger_SAREQ;

	iTE_u8 Current_EQ_Report_BirErr_Count_ADD;
	iTE_u8 Current_EQ_Report_BirErr_Count;
	iTE_u8 Current_EQ_TableIndex;
	iTE_u8 Current_AutoEQ_Timeout_Count;

	iTE_u16 Valid_CED[3];
	iTE_u16 AMP_TimeOut[3];

	// iTE6805_SYS.cpp
	iTE_u8 Current_ECCAbnormal_Count;
	iTE_u8 Current_ECCError_Count;
	iTE_u8 Current_SCDTCheck_Count;
	iTE_u8 Current_AudioCheck_Count;
	iTE_u8 Current_AudioMonitor_Count;

	iTE_u8 Current_Ch0_SymbolLockRst_Count;
	iTE_u8 Current_Ch1_SymbolLockRst_Count;
	iTE_u8 Current_Ch2_SymbolLockRst_Count;

	iTE_u8 Current_AudioSamplingFreq_ErrorCount;

	iTE_u8 deskewerrcnt;
	iTE_u8 abnormalcnt;

	// pre Frame data container
	iTE_u8 prevAVIDB[2];
	iTE_u8 prevAudioB0_Status;		// compare mask 0xF0
	iTE_u8 prevAudioB1_Status;		// compare mask 0xFF
	iTE_u8 prevAudioB2_CHStatus;	// compare mask BIT1

									// Flag
	iTE_u8 Flag_NewAVIInfoFrame;
	iTE_u8 Flag_FirstTimeAudioMonitor;
	iTE_u8 Flag_HDCP_Trigger_AutoEQ_Again;
	iTE_u8 Flag_FirstTimeParameterChange;
	iTE_u8 Flag_Disable_NewAVIInfoFrame_When_Parameter_Change;
	iTE_u8 Flag_First_Time_VidStable_Done;

#if (ENABLE_6805_POWER_SAVING_MODE==TRUE)
	iTE_u8 Flag_Need_Power_Normal_Mode_Setting;
	iTE_u8 iTE6805_POWER_MODE;
#endif

	iTE_u8 LAST_PORT;
	iTE_u8 LAST_SCDT_STATE;

#if (ENABLE_DETECT_DRM_PKT==TRUE)
	iTE_u8 Current_count_no_DRM_PKT;
#endif

	//iTE_u8 bAVIInfoFrame_Input_ColorFormat;
	//iTE_u8 bAVIInfoFrame_Colorimetry;
	//iTE_u8 bAVIInfoFrame_ExtendedColorimetry;
	//iTE_u8 bAVIInfoFrame_RGBQuantizationRange;
	//iTE_u8 bAVIInfoFrame_YUVQuantizationRange;

	// MainFun.cpp
	_iTE6805_DATA			iTE6805_DATA;
	_iTE6805_VTiming		iTE6805_CurVTiming;
    _iTE6805_VTiming		iTE6805_OldVTiming;
	_iTE6805_PARSE3D_STR	iTE6805_EDID_Parse3D;

	ite6805_frameinfo_t     frameinfo;
	//eOutputRange		m_OutputRange;			// Determine ADV7604 output color range
	//eSnapShotOption		m_SnapShotOption;		// Determine the color range of CSC formula for snapshot
	//eRGBColorRangeMode	m_RgbCRM;
	short		m_BrightnessDefault;	// Brightness default value will be changed according to m_RgbCRM
	HDCP_LEVEL_TYPE		m_68051_HDCPLevel;		// this is only used for iTE6805_Set_HDCP() to judge level 1.4, 2.2, none
	iTE_u8				m_68051_HDCP_Enable_Flag;		// this is only used for iTE6805_Set_HDCP() to judge level 1.4, 2.2, none
	HDCP_LEVEL_TYPE		m_6664_HDCPLevel;		// this is only used for iTE6805_Set_HDCP() to judge level 1.4, 2.2, none

	iTE_u8				m_CurrentHDCP_Flag;
	iTE_u8				m_6664_HDCP_Enable_Flag;		// this is only used for iTE6805_Set_HDCP() to judge level 1.4, 2.2, none

	iTE_u8    m_WorkingMode;		// 0xff-not changed , 480/720/4k2k-0,1,2
	iTE_u8    m_OutputColorSpaceMode;	// 	
	iTE_u8    m_SamplingMode;		// 0-rgb, 1-422, 2-444
	iTE_u8    m_DDRMode;
	iTE_u8    m_DualPixelMode;
	iTE_u8    m_PacketFmtChange;

	iTE_u8    m_Old_PacketFmtChange;
	iTE_u8    m_Old_DDRMode;
	iTE_u8    m_Old_DualPixelMode;
	iTE_u8    m_Old_HDRMode;
	iTE_u8    m_PacketCSC_BT;
	iTE_u8    m_Old_PacketCSC_BT;
	iTE_u32    m_Old_PacketVideoLock;


	iTE_u32    m_PacketRepetition;
	iTE_u32    m_PacketVideoLock;
	iTE_u32    m_PacketActWidth;
    iTE_u32    m_PacketActHeight;
	iTE_u32    m_PacketVIC;
	iTE_u32    m_PacketInputSamplingMode;				// 0: rgb, 1:422, 2:444
	iTE_u32    m_PacketOutputSamplingMode;

	iTE_u32    m_PacketInterlace;
	iTE_u32    m_PacketDeepColorBits;
	iTE_u32    m_PacketOutputColorSpace;
	//iTE_u32    m_PacketOutputColorSpace;
	iTE_u32    m_PacketOutputRGBQuantizationRange;
	iTE_u32    m_PacketAudioSamplingFreq;
	iTE_u32    m_oldPacketAudioSamplingFreq;
	iTE_u32    m_PacketAudioType;
	iTE_u32    m_PacketAudioChannelCount;
    iTE_u32    m_OldPacketAudioChannelCount;
	iTE_u32    m_PacketAudioSamplingBits;
	iTE_u32    m_PacketHDCP;
	iTE_u32    m_PacketPCLK;
	iTE_u32    m_PacketFrameRate;
	iTE_u32    m_PacketDDRMode;
	iTE_u32    m_PacketDualPixelMode;
	iTE_u32    m_OldPacketHDCP;
	iTE_u32    m_AudioSamplingRate;
	iTE_u32    m_PacketLockRetryCount;
	iTE_u32    m_InterlaceBitRetryCount;
    RXITE68051_VIDEO_POS m_VideoPos;

    iTE_u8    m_VendorSpecificInfoframe[31];
	iTE_u8    m_AVIInfoframe[AVI_INFOFRAME_SIZE];			// add by AndyNien, 20171109
	
    iTE_u8    m_SPDInfoframe[21];			// size ??
    iTE_u8    m_AudioInfoframe[14];		// size 14
    iTE_u8    m_DRMInfoframe[DRM_HDR_METADATA_SIZE];
    iTE_u8    m_Old_DRMInfoframe[DRM_HDR_METADATA_SIZE];
}ite6805_cxt_t;

typedef struct {
	iTE_s32	Width;
	iTE_s32	Height;
} RX_RESOLUTION_INFO;

typedef enum 
{
    CAPTURE_BT601_COMPUTER = 0, //Y = 0.257*R + 0.504*G + 0.098*B + 16	// YUV_601_FULL
    CAPTURE_BT601_STUDIO,       //Y = 0.299*R + 0.587* G + 0.114*B		// YUV_601_LIMIT
    CAPTURE_BT709_COMPUTER,     //Y = 0.1826R + 0.6142G + 0.062B + 16	// YUV_709_FULL
    CAPTURE_BT709_STUDIO,       //Y = 0.2126R + 0.7152G + 0.0722B		// YUV_709_LIMIT
	CAPTURE_BT2020_COMPUTER,     //Y = ??????R + ?????G + ?????B + 16	// YUV_2020_FULL
	CAPTURE_BT2020_STUDIO,       //Y = ?????R + ?????G + ?????B		// YUV_2020_LIMIT
}eCSC_FORMULA;

enum eCSC_BT{
	COLORMETRY_ITU601 = 1,
	COLORMETRY_ITU709 = 2,
	COLORMETRY_ITU2020 = 3
};

// so, we need to do switch, 
typedef enum 
{
	DEVICE_480_MODE,
	DEVICE_720_MODE,
	DEVICE_4k2k_MODE,
}eConfigITE68051;

typedef enum 
{
	VIDEO_YUV_FMT,
	VIDEO_RGB_FULL_FMT,
	VIDEO_RGB_LIMIT_FMT,
}xConfigRGB;

typedef enum 
{
	VIDEO_RGB_MODE,
	VIDEO_422_MODE,
	VIDEO_444_MODE,
	VIDEO_420_MODE,
}eVideoTypeITE68051;

// enum for types of warming message
enum eImageType
{
    IMAGE_NO_SIGNAL        =0,
    IMAGE_INVALID_SETTINGS =1,
	IMAGE_HDCP_PROTECTION  =2,	//Add HDCP type
    IMAGE_UNDEF
};

void ite6805_add_trace(handle_t ite66805_handle,void *trace_handle);    
void ite6805_set_out_format(handle_t ite6805_handle,ite6805_out_format_e format);
void ite6805_register_callback(handle_t ite6805_handle,ite6805_callback_t callback_func,void *cxt);
void iTE6805_Hardware_Init(handle_t ite6805_handle);
void ite6805_get_workingmode(handle_t ite6805_handle, U32_T* pmode);
void ite6805_get_colorspace(handle_t ite6805_handle, U32_T* pcolorspace);
void ite6805_get_sampingmode(handle_t ite6805_handle, U32_T* psampingmode);
void ite6805_get_ddrmode(handle_t ite6805_handle, U32_T* pddrmode);
void ite6805_get_hdcp_level(handle_t ite6805_handle, int *hdcp);
void ite6805_get_hdcp_state(handle_t ite6805_handle, int *hdcp_state);
void ite6805_set_hdcp_state(handle_t ite6805_handle, int hdcp_state);
void ite6805_get_DDRPixe_mode(void *ite6805_handle, unsigned int* pddrmode, unsigned int* ppixmode);

iTE_u8 hdmirxrd(void *bus_handle, iTE_u8 RegAddr);
iTE_u8 hdmirxwr(void *bus_handle, iTE_u8 RegAddr, iTE_u8 DataIn);
iTE_u8 hdmirxbrd(void *bus_handle, iTE_u8 RegAddr, iTE_u8 RegBytes, iTE_u8 *buffer);
iTE_u8 hdmirxset(void *bus_handle, iTE_u8  offset, iTE_u8  mask, iTE_u8  ucdata);
iTE_u8 hdmirxbwr(void *bus_handle, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *rddata);
iTE_u8 iTE6805_BitErr_Check_Again(void *ite6805_handle);
iTE_u8 iTE6805_Check_PORT0_IS_MHL_Mode(void* ite6805_handle, iTE_u8 PORT_NUM);

void iTE6805_EQ_chg(void *ite6805_handle, STATEEQ_Type NewState);
void iTE6805_EDID_Init(void *ite6805_handle);

void x_IssueHotPlug(void *ite6805_handle, iTE_u16 HPD_State);

void ite6805_get_frameinfo(handle_t ite6805_handle,ite6805_frameinfo_t *frameinfo);
void ite6805_get_audioinfo(handle_t ite6805_handle,enum ite6805_audio_sample *audioinfo);

void ite6805_set_freerun_screen(handle_t ite6805_handle,BOOL_T is_blue_screen);

void ite6805_set_bchs(handle_t ite6805_handle,int *BCHSinfo,int *bchs_select);
void ite6805_get_brightness(handle_t ite6805_handle,int *BCHSinfo);
void ite6805_get_contrast(handle_t ite6805_handle,int *BCHSinfo);
void ite6805_get_hue(handle_t ite6805_handle,int *BCHSinfo);
void ite6805_get_saturation(handle_t ite6805_handle,int *BCHSinfo);

void ite6805_get_board_id(handle_t ite6805_handle,int *board_id);

void ite6805_release_task(handle_t ite6805_handle);
void ite6805_create_task(handle_t ite6805_handle);
void ite6805_power_off(handle_t ite6805_handle);
void ite6805_power_on(handle_t ite6805_handle);

#endif
