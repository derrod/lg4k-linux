#ifndef _ITE6664_SRC_H_
#define _ITE6664_SRC_H_

#include "version.h"
#include "typedef.h"
#include "cxt_mgr.h"
#include "it6664_typedef.h"
#include "it6664_extern.h"
#include "it6664_EDID.h"
#include "cxt_mgr.h"
#include "mem_model.h"
#include "i2c_model.h"
#include "sys.h"
#include "trace.h"
#include "debug.h"
#include "task_model.h"

enum eEventTypeITE6664
{
	EN_ITE6664_THREAD_TERMINATE = 0,
	EN_ITE6664_TimeServ,
	EN_ITE6664_MAX
};


#define AUDIO_32K		320000
#define AUDIO_44K		441000
#define AUDIO_48K		480000
#define AUDIO_88K		882000
#define AUDIO_96K		960000
#define AUDIO_176K		1764000
#define AUDIO_192K		1920000
#define AUDIO_TOLERANCE	50000


extern int subsystem_id;

typedef enum
{
    IO_MAP=0,
    TXC_MAP,
    TXP0_MAP,
    RXP0_MAP,
    EDID_MAP,
    ITE6664_MAP_COUNT,
}ite6664_map_e;

enum
{
    STAT_MCU_INIT,
    STAT_CHECK_TRAPING,
    STAT_CHECK_DEV_READY,
    STAT_DEV_INIT,
    STAT_IDLE,
    STAT_IDLE2,
};

#define ITE6664_EDID_BLOCK_SIZE_BYTE 128 //Added by Jeff_20180607

enum eDeviceITE6664
{
	DEVICE_ITE6664_0			//1'st 6664
};


#define ITE6664_ID ite6664
#define ITE6664_DRVNAME I2C_MODEL_MODULE_STR(ite6664)
    
    
typedef enum
{
    ITE6664_XTAL_FREQ_28_63636MHz,
    ITE6664_XTAL_FREQ_27MHz,
    ITE6664_XTAL_FREQ_24_576MHz,        
    ITE6664_XTAL_FREQ_24MHz,
}ite6664_xtal_freq_e;

typedef enum
{
    ITE6664_OUT_FORMAT_SDR_ITU656_8 = 0x00,
    ITE6664_OUT_FORMAT_SDR_ITU656_10 = 0x01,
    ITE6664_OUT_FORMAT_SDR_ITU656_12_MODE0 = 0x02,
    ITE6664_OUT_FORMAT_SDR_ITU656_12_MODE1 = 0x06,
    ITE6664_OUT_FORMAT_SDR_ITU656_12_MODE2 = 0x0a,
    ITE6664_OUT_FORMAT_DDR_422_8 = 0x20,
    ITE6664_OUT_FORMAT_DDR_422_10 = 0x21,
    ITE6664_OUT_FORMAT_DDR_422_12_MODE0 = 0x22,
    ITE6664_OUT_FORMAT_DDR_422_12_MODE1 = 0x23,
    ITE6664_OUT_FORMAT_DDR_422_12_MODE2 = 0x24,
    ITE6664_OUT_FORMAT_SDR_444_24 = 0x40,
    ITE6664_OUT_FORMAT_SDR_444_30 = 0x41,
    ITE6664_OUT_FORMAT_SDR_444_36_MODE0 = 0x42,
    ITE6664_OUT_FORMAT_SDR_444_36_MODE1 = 0x46,
    ITE6664_OUT_FORMAT_SDR_444_24_MODE3 = 0x4C,
    ITE6664_OUT_FORMAT_SDR_444_24_MODE4 = 0x50,
    ITE6664_OUT_FORMAT_SDR_444_30_MODE4 = 0x51,
    ITE6664_OUT_FORMAT_SDR_444_36_MODE4 = 0x52,
    ITE6664_OUT_FORMAT_SDR_444_2X24_INTERLEAVE_MODE0 = 0x54,            
    ITE6664_OUT_FORMAT_DDR_444_24 = 0x60,
    ITE6664_OUT_FORMAT_DDR_444_30 = 0x61,
    ITE6664_OUT_FORMAT_DDR_444_36 = 0x62,
    ITE6664_OUT_FORMAT_SDR_ITU656_16 = 0x80,
    ITE6664_OUT_FORMAT_SDR_ITU656_20 = 0x81,
    ITE6664_OUT_FORMAT_SDR_ITU656_24_MODE0 = 0x82,
    ITE6664_OUT_FORMAT_SDR_ITU656_24_MODE1 = 0x86,
    ITE6664_OUT_FORMAT_SDR_ITU656_24_MODE2 = 0x8a,
    ITE6664_OUT_FORMAT_SDR_422_20_MODE3    =0x8d,
    ITE6664_OUT_FORMAT_SDR_422_16_MODE4    =0x90,
    ITE6664_OUT_FORMAT_SDR_422_20_MODE4    =0x91,
    ITE6664_OUT_FORMAT_SDR_422_24_MODE4    =0x92,
    ITE6664_OUT_FORMAT_SDR_422_2X16_INTERLEAVE_MODE0 = 0x94,                    
    ITE6664_OUT_FORMAT_SDR_422_2X20_INTERLEAVE_MODE0 = 0x95,        
    ITE6664_OUT_FORMAT_SDR_422_2X24_INTERLEAVE_MODE0 = 0x96,        
    ITE6664_OUT_FORMAT_PAR_8_MODE0=0xc0,
    ITE6664_OUT_FORMAT_PAR_10_MODE0=0xc1,
    ITE6664_OUT_FORMAT_PAR_12_MODE0=0xc2,
    
}ite6664_out_format_e;

typedef enum  {
    ITE6664_OP_CH_SEL_GBR_YUV = 0,
    ITE6664_OP_CH_SEL_GRB_YVU = 1,
    ITE6664_OP_CH_SEL_BGR_UYV = 2,
    ITE6664_OP_CH_SEL_RGB_VYU = 3,
    ITE6664_OP_CH_SEL_BRG_UVY = 4,
    ITE6664_OP_CH_SEL_RBG_VUY = 5,
}ite6664_out_ch_order_e;

typedef enum 
{
	ITE6664_OUT_DEFAULT,
    ITE6664_OUT_YUV601,
    ITE6664_OUT_YUV709,
    ITE6664_OUT_RGB_LIMITED,
    ITE6664_OUT_RGB_FULL,
}ite6664_out_colorspace_e;

typedef struct
{
    ite6664_out_format_e out_fmt;
    ite6664_out_ch_order_e out_ch_order;
    ite6664_out_colorspace_e out_colorspace;
    
    unsigned blank_data:1;  // 0: no blank data 1: blank data
    unsigned insert_av_codes:1; // 0:No AV codes 1:AV codes 
    unsigned invert_cbcr:1; // 0: follow out_format 1: swap cb,cr
    unsigned output_bus_lsb_to_msb:1; // 0: MSB to LSB 1: LSB to MSB

}ite6664_out_ctrl_t;

typedef struct
{
    U8_T sw_map_addr;
    U8_T txcom_map_addr;
    U8_T rxp0_map_addr;
    U8_T txp0_map_addr;
    U8_T edid_map_addr;
    
    ite6664_xtal_freq_e xtal_freq;
    ite6664_out_ctrl_t out_ctrl;
    
}ite6664_cfg_t;

typedef enum
{
    ITE6664_LOCK,
    ITE6664_UNLOCK,  
    ITE6664_HDCP,           
}ite6664_event_e;

#define CE511H_ID ce511h
#define CE511H_DRVNAME I2C_MODEL_MODULE_STR(ce511h)

enum    
{
    RXLINK0,
    RXLINK1,
    TXLINK0,
    TXLINK1,
    SLAVE_ADDR_COUNT,
};    
typedef enum
{
    HOTPLUG_LOW,
    HOTPLUG_HIGH,        
}hotplug_pin_e;

typedef void (*hotplug_ctrl_func_t)(void *,hotplug_pin_e);
typedef void (*hw_reset_func_t)(void *);

typedef struct
{
    U8_T slave_addr_cfg[SLAVE_ADDR_COUNT];
    hotplug_ctrl_func_t  hotplug_ctrl_func;
    hw_reset_func_t      hw_reset_func;
}ce511h_cfg_t;


typedef enum
{
    ITE6664_CS_YUV,
    ITE6664_CS_RGB_FULL,
    ITE6664_CS_RGB_LIMIT,
}ite6664_colorspace_e;

typedef struct
{
    int brightness;
    int contrast;
    int saturation;
    int hue;
    //adv7619_colorspace_e colorspace;
}ite6664_bchs_t;

typedef enum
{
    ITE6664_BCHS_BRIGHTNESS,
    ITE6664_BCHS_CONTRAST,
    ITE6664_BCHS_HUE,        
    ITE6664_BCHS_SATURATION,
}ite6664_bchs_e;

typedef struct
{
    unsigned width;
    unsigned height;
    BOOL_T is_interlace;
    unsigned framerate;
    unsigned denominator;
    U32_T    pixel_clock;
    ite6664_colorspace_e packet_colorspace;
    int dual_pixel;
    int sampling_mode;
    int dual_pixel_like;
    //int packet_colorspace;
    unsigned int ite6664_hdcp_flag;
}ite6664_frameinfo_t;

enum ite6664_audio_sample {
	ITE6664_AUDIO_RATE_32000, //0
	ITE6664_AUDIO_RATE_44100, //1
	ITE6664_AUDIO_RATE_48000, //2
	ITE6664_AUDIO_RATE_88200,
	ITE6664_AUDIO_RATE_96000,
	ITE6664_AUDIO_RATE_176400,
	ITE6664_AUDIO_RATE_192000,
};

enum eEdidPort
{
	EDID_MERGE = 0,
	EDID_PASS_THROUGH = 1,
	EDID_LOCAL = 2,
};

typedef enum eHdcpLevel
{
	HDCP_NONE = 0,
	HDCP_14,
	HDCP_22,
	HDCP_HOTPLUG = -1
}HDCP_LEVEL_TYPE;

typedef struct
{
    iTE_u8 g_device;
    i2c_model_bus_handle_t bus_handle;
    U16_T addr;
    U8_T map_addr[ITE6664_MAP_COUNT];
    trace_attr_handle_t attr_handles[4];
    U8_T id;
    void *cb_cxt;
    task_handle_t task;
    task_model_handle_t taskmgr;
    ite6664_frameinfo_t frameinfo; 
    enum ite6664_audio_sample rates;

	// IO_IT6350.cpp
	iTE_u8	u8IntEvent;

	EDIDBlock0 EDID0data[4];//0x6
	EDIDBlock1 EDID1data[4];//0x4a
	iTE_u8 SupportVesaDTD[4];
	iTE_u8 HPD[4];		
	iTE_u8 Compose_block0[128];
	iTE_u8 Compose_block1[128];
	iTE_u8 Device_off;

	// IT6664_hdmi2_rx.cpp
	iTE_u8 stsbak;
	iTE_u8 RetryCnt;
	iTE_u8 preAVI_packet[0x12];


#ifdef AutoEQ
	iTE_u8 CHB_TargetRS[3];
	iTE_u8 Wait;
	iTE_u8 EQ20[3];
	iTE_u8 EQ14[3];
	iTE_u8 DFE[14][3][3];//RS/CH/3
	iTE_u8 fail_ch[3];
	iTE_u8 EQ_sel[3];
	iTE_u8 DFEBak[9];
	iTE_u8 SkewResult;
	iTE_u16 AMP_TimeOut[3];
	iTE_u8  RS_Value[14];		// iTE_u8 _CODE RS_Value[14] = { 0x7F, 0x7E, 0x3F, 0x3E, 0x1F, 0x1E, 0x0F, 0x0E, 0x07, 0x06, 0x03, 0x02, 0x01, 0x00 };
#endif

	iTE_u32 tolerr[3];

	// IT6664_hdmi2_tx.cpp
	iTE_u16 ksvchkcnt[4];
	iTE_u8 retry;

	// IT6664_hdmi2sp.cpp
	iTE_u32 Xcnt;
	iTE_u8 Xtmp;
	iTE_u8 Xtmp1;

	extern_variables ar_gext[3];
	extern_u8	ar_gu8[3];
	it6664_tx ar_txmem[3];
	extern_32 ar_gu32[3];
	extern_variables *gext_var;
	extern_u8 *gext_u8;
	it6664_tx *gmem_tx;
	extern_32 *gext_long;
	struct PARSE3D_STR	st3DParse;


	iTE_u8 u8I2CBus;
	iTE_u8 u8I2CAdr;
	iTE_u8 m_LastEdidBlock0[ITE6664_EDID_BLOCK_SIZE_BYTE];
	iTE_u8 m_LastEdidBlock1[ITE6664_EDID_BLOCK_SIZE_BYTE];
	iTE_u8	m_HDCP_Level;			// the hdcp level that user set
	iTE_u8	m_EnRxHDCP2;
	iTE_u8    m_NeedToDisableHDCPFlag;
	iTE_u8	m_CurrentHDCP_Flag;
    unsigned char g_current_system_state;
    unsigned char g_previous_system_state;
}ite6664_cxt_t;


iTE_u8 GetHDCPLevel(void* handle);
//iTE_u8 GetSingalLockStatus(void);
//extern void it6664_EdidMode_Switch(void *ite6664_cxt);
void *ite6664_attach(i2c_model_bus_handle_t bus_handle,unsigned short i2c_addr,void *drv_set_info);
void ite6664_detach(void *cxt);
void ite6664_power_off(void *cxt);
void ite6664_power_on(void *cxt);


#endif
