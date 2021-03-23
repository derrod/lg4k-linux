///*****************************************
//  Copyright (C) 2009-2018
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6664_extern.h>
//   @author Hojim.Tseng@ite.com.tw
//   @date   2018/02/22
//   @fileversion: ITE_HDMI2_SPLITER_1.18
//******************************************/
#ifndef _IT6664_EXTERN_H_
#define _IT6664_EXTERN_H_
//#include "..\..\IT6350\code\api\debug\debug_print.h"
//#include "IO_IT6350.h"
//#include "IT6664_Typedef.h"
#include "it6664_IO.h"
#include "it6664_config.h"


#define P0              0
#define P1              1
#define P2              2
#define P3              3

#define VESA640x350P85      (  0<<8)
#define VESA640x400P85      (  1<<8)
#define VESA720x400P85      (  2<<8)
#define VESA640x480P60      (  3<<8)    // VGA
#define VESA640x480P72      (  4<<8)
#define VESA640x480P75      (  5<<8)
#define VESA640x480P85      (  6<<8)

#define VESA800x600P56      ( 10<<8)
#define VESA800x600P60      ( 11<<8)    // SVGA
#define VESA800x600P72      ( 12<<8)
#define VESA800x600P75      ( 13<<8)
#define VESA800x600P85      ( 14<<8)
#define VESA800x600P120     ( 15<<8)

#define VESA1024x600P60     ( 20<<8)
#define VESA1024x768P60     ( 21<<8)    // XGA
#define VESA1024x768P70     ( 22<<8)
#define VESA1024x768P75     ( 23<<8)
#define VESA1024x768P85     ( 24<<8)

#define VESA1280x768RB60    ( 30<<8)
#define VESA1280x768P60     ( 31<<8)
#define VESA1280x768P75     ( 32<<8)
#define VESA1280x768P85     ( 33<<8)
#define VESA1280x800RB60    ( 34<<8)
#define VESA1280x800P60     ( 35<<8)
#define VESA1280x960P60     ( 36<<8)    // XVGA
#define VESA1280x960P85     ( 37<<8)

#define VESA1280x1024P60    ( 40<<8)    // SXGA
#define VESA1280x1024P75    ( 41<<8)
#define VESA1280x1024P85    ( 42<<8)

#define VESA1360x768P60     ( 50<<8)
#define VESA1366x768RB60    ( 51<<8)
#define VESA1366x768P60     ( 52<<8)
#define VESA1400x1050RB60   ( 53<<8)
#define VESA1440x900RB60    ( 54<<8)
#define VESA1440x900P60     ( 55<<8)    // WXGA

#define VESA1600x900RB60    ( 60<<8)
#define VESA1600x1200RB60   ( 61<<8)    // still not implement
#define VESA1600x1200P60    ( 62<<8)    // UXGA
#define VESA1600x1200P65    ( 63<<8)
#define VESA1600x1200P70    ( 64<<8)
#define VESA1600x1200P75    ( 65<<8)
#define VESA1600x1200P85    ( 66<<8)

#define VESA1680x1050P60    ( 70<<8)
#define VESA1792x1344P60    ( 71<<8)
#define VESA1920x1200RB60   ( 72<<8)
#define VESA1920x1200P60    ( 73<<8)
#define VESA2048x1536RB60   ( 74<<8)    // still not implement (QXGA)
#define VESA2560x1400RB60   ( 75<<8)
#define VESA2560x1600RB60   ( 76<<8)    // (WQXGA)

#define H14B4Kx2KVIDFMT1    ( 80<<8)    // the same as Fmt95
#define H14B4Kx2KVIDFMT2    ( 81<<8)    // the same as Fmt94
#define H14B4Kx2KVIDFMT3    ( 82<<8)    // the same as Fmt93
#define H14B4Kx2KVIDFMT4    ( 83<<8)    // the same as Fmt98

#define HDMI            0
#define DVI             1
#define MHL             2
#define RCLKFreqSel 	0  // 0: 20MHz, 1: 10MHz, 2: 5MHz, 3: 2.5MHz

#define CLK13P5M        0
#define CLK27M          1
#define CLK54M          2
#define CLK108M         3
#define CLK37P125M      4
#define CLK74P25M       5
#define CLK148P5M       6
#define CLK297M         7

#define VID8BIT         0
#define VID10BIT        1
#define VID12BIT        2
#define VID16BIT        3

#define RGB444          0
#define YCbCr422        1
#define YCbCr444        2
#define YCbCr420        3

#define ori 			0
#define csc_6664 			1
#define conv 			2
#define ds				3

#define xvYCC601        0
#define xvYCC709        1
#define sYCC601         2
#define AdobeYCC601     3
#define AdobeRGB        4
#define BT2020cYCC      5
#define BT2020YCC       6

#define DynVESA         0
#define DynCEA          1

#define ITU601          0
#define ITU709          1

#define I2S             0
#define SPDIF           1

#define LPCM            0
#define NLPCM           1
#define HBR             2
#define DSD             3

#define TDM2CH          0x0
#define TDM4CH          0x1
#define TDM6CH          0x2
#define TDM8CH          0x3

#define NOCSC           0
#define RGB2YUV         2
#define YUV2RGB         3

#define FrmPkt          0
#define SbSFull         3
#define TopBtm          6
#define SbSHalf         8

#define DDC75K          0
#define DDC125K         1
#define DDC312K         2

#define AUD16BIT        0x2
#define AUD18BIT        0x4
#define AUD20BIT        0x3
#define AUD24BIT        0xB

#define AUDCAL1         0x4
#define AUDCAL2         0x0
#define AUDCAL3         0x8

#define PICAR_NO        0
#define PICAR4_3        1
#define PICAR16_9       2

#define ACTAR_PIC       8
#define ACTAR4_3        9
#define ACTAR16_9       10
#define ACTAR14_9       11

#define ColorBar        0
#define GrayRamp        2
#define ChessBoard      3
#define SeqData         4

typedef struct {
    iTE_u16 HActive ;
    iTE_u16 VActive ;
    iTE_u16 HTotal ;
    iTE_u16 VTotal ;
    iTE_u16 PCLK ;
    iTE_u16 HFrontPorch ;
    iTE_u16 HSyncWidth ;
    iTE_u16 HBackPorch ;
    iTE_u16 VFrontPorch ;
    iTE_u16 VSyncWidth ;
    iTE_u16 VBackPorch ;
    iTE_u16 FrameRate ;
    iTE_u16 ScanMode ;
    iTE_u16 VPolarity ;
    iTE_u16 HPolarity ;
    iTE_u16 PixRpt;
    float PicAR;
    iTE_u8 *format;
    iTE_u16 VIC;
} VTiming ;

#define PROG 1
#define INTR 0
#define Vneg 0
#define Hneg 0
#define Vpos 1
#define Hpos 1

//extern define
#define HP2_DevDepth			2
#define HP2_DevNum  			4
#define ForceCVOutColor			FALSE
#define ForceEnCV2DS            FALSE
#define FwEnCV2DS               FALSE
#define EnTXHDCPBeforeRX        FALSE
#define ForceTXHDCP 		 	TRUE
#define EnRxHP2VRRpt			FALSE

typedef struct
{
	iTE_u8 TxSrcSel[4];// 0: ori   1: CSC   2: conv  3.DS
	iTE_u8 TXSupport420[4];
	iTE_u8 TXSupportOnly420[4];
	iTE_u8 TXSupport4K60[4];
	iTE_u8 TXSupport1080p[4];
	iTE_u8 TXSupport4K30[4];
	iTE_u8 TXSupportDC420[4];
	iTE_u8 TXSupportHDR[4];
	iTE_u8 TXSupportColorimetry[4];
	iTE_u8 TXSupportVideoCapability[4];
	iTE_u8 TXSupportVendorSpecificVDB[4];
	iTE_u8 TXSupportSpeaker[4];
	iTE_u8 TXSupportAudio[4];		// add AndyNien, 20190124
	iTE_u8 TXSupportVSDB1[4];
	iTE_u8 TXSupportVSDB2[4];

	iTE_u8 TXHPD[4];
	iTE_u8 EDIDParseDone[4];
	iTE_u8 DVI_mode[4];
	iTE_u8 HDCPState[4];
	iTE_u8 VideoState[4];
	iTE_u8 HDCPWaitCnt[4];
	iTE_u8 HDCPFireVer[4];
	iTE_u8 Txout_united[4];
}extern_variables;
typedef struct
{
	iTE_u8 DevCnt_Total;
	iTE_u8 Depth_Total;
	iTE_u8 Err_DevCnt_Total;
	iTE_u8 Err_Depth_Total;
	iTE_u8 EDIDCopyDone;
	iTE_u8 GCP_CD;
	iTE_u8 CD_SET;
	iTE_u8 RXSCDT;
	iTE_u8 RXHDCP;
	iTE_u8 AutoEQsts;
	iTE_u8 Auto14done;
	iTE_u8 Auto20done;
	iTE_u8 EQ14Going;
	iTE_u8 EQ20Going;
	iTE_u8 TxVidStbFlag;
	iTE_u8 TxTMDSStbFlag;
	iTE_u8 InColorMode;
	iTE_u8 SetCVOutColorMode;
	iTE_u8 CVOutColorMode;
	iTE_u8 CSCOutColorMode;
	iTE_u8 CSCOutQ;
	iTE_u8 BUSMODE;
	iTE_u8 TxAFESetDone;
	iTE_u8 CP_Done;
	iTE_u8 CP_Going;
	iTE_u8 CP_Fail;
	iTE_u8 TXH2RSABusy;
	iTE_u8 TXHDCP2Done;
	iTE_u8 RxtoggleHPD;
	iTE_u8 ForceTXHDMI2;
	iTE_u8 TXHPDsts;
	iTE_u8 TXHP2KSVChkDone;
	iTE_u8 HP2_HDCP1DownStm;
	iTE_u8 HP2_HDCP20DownStm;
	iTE_u8 TXClearSCDC;
	iTE_u8 ForceCPDwnVer;
	iTE_u8 TimerIntFlag;
	iTE_u8 Rx_4K30;
	iTE_u8 RXHDCP2_chk;
	iTE_u8 HighFrameRate;//ite_171201
	iTE_u8 ContentType;
	iTE_u8 GetSteamType;
}extern_u8;

typedef struct
{
	iTE_u8 HDCPFireCnt[4];
	//iTE_u8 HDCPPassCnt[4];
	//iTE_u8 HDCPRiChkCnt[4];
	iTE_u8 AV[5][4];
	iTE_u8 M[64];
	iTE_u8 FiFoErrcnt[4];
}it6664_tx;
typedef struct
{
	iTE_u32 RCLK;
	iTE_u32	TXVCLK[4];
}extern_32;

typedef enum TxVideoState_type
{
	Tx_Video_waitInt = 0,
	Tx_Video_Stable,
	Tx_Video_Stable_off,
	Tx_Video_Reset,
	Tx_Video_OK
}TxVideoState;
typedef enum TxHdcpState_type
{
	Tx_CP_WaitInt=0,
	Tx_CP_Reset,
	Tx_CP_Going,
	Tx_CP_Done,
	Tx_CP_ReAuth,
	Tx_CP_check,
	Tx_CP_Timeout_SWRty
}TxHdcpState;
struct PARSE3D_STR
{
    iTE_u8	uc3DEdidStart;
    iTE_u8	uc3DBlock;
    iTE_u8	uc3DInfor[32];
    iTE_u8	ucVicStart;
    iTE_u8	ucVicCnt;
    iTE_u8	uc3DTempCnt;
    iTE_u8	ucDtdCnt;
    iTE_u8	bVSDBspport3D;
};

////extern functions
//extern void detectbus(void);
//extern void chgrxbank(iTE_u8 bankno);
//extern void chgspbank(iTE_u8 bankno);
//extern void cal_oclk(void);
//extern void cal_pclk(iTE_u8 port);
//extern void hdmi2_irq( void );
//extern void h2sp_irq( void );
//extern void mhlrx_irq(void);
//extern void h2tx_irq(iTE_u8 port);
//extern void h2rx_irq(void);
//extern void mhlrx_ini(void);
//extern void h2tx_rst(iTE_u8 port);
//extern void setup_txpatgen(void);
//extern void h2rx_edidwr( iTE_u8 offset, iTE_u8 data );
//extern void it6664_hdmi2sp_initial(void);
//extern void setup_h2txafe(iTE_u8 port);
//extern iTE_u8 it6664_read_one_block_edid(iTE_u8 port, iTE_u8 block, iTE_u8 *edid_buffer);
//extern iTE_u8 ddcwait(iTE_u8 port);
//extern void it6664_var_init(void);
//extern void it6664_txvar_init(void);
//extern void DefaultEdidSet(void);
//extern void it6664_EdidMode_Switch(void);
//extern void h2tx_pwron(iTE_u8 port);
//extern void txrunhdcp2(iTE_u8 port);
//extern void setrx_ksv_list(iTE_u8 devcnt, iTE_u8 depth, iTE_u8 err_devcnt, iTE_u8 err_depth);
//extern void SetRxHpd(iTE_u8 sts);
//extern void hdmi2_work(void);
//extern void hdcpsts(iTE_u8 val,iTE_u8 port);
//extern void get_vid_info(void);
//extern void it6664_hdmitx_handler(void);
//extern void Check_BitErr(void);
//extern void it6664_AutoEQ_State(void);
//extern iTE_u8 it6664_Edid_Parse(iTE_u8 HPDsts);
//extern void it6664_h2rx_pwdon(void);
//extern void  Dump_IT666xReg(void);
//extern void h2rx_rst(iTE_u8 port);
//extern void it6664_TXCED_monitor(iTE_u8 port);
//extern void IT6664_DeviceSelect(iTE_u8 u8Device);
//extern void h2rx_ini(iTE_u8 port);
//extern iTE_u8 setup_h2scdc(iTE_u8 port);
//extern void it6664_RXHDCP_OFF(iTE_u8 sts);
//extern void h2txVclkrst(iTE_u8 port);
//extern void h2tx_pwrdn(iTE_u8 port);
//extern void MHL_CBUS_Event(void);
//
//#ifdef	Support_CEC
//extern void Cec_BlockSel(iTE_u8 u8CecSel);
//extern void Cec_Init(iTE_u8 timerunit,iTE_u8 port);
//extern void Cec_Irq(iTE_u8 port);
//extern void Cec_TxPolling(iTE_u8 u8LogicalAdr);
//#endif

//MHL
#define mhlrxrd(handle, offset)				iTE_I2C_ReadByte(handle, RXMHLAddr, offset)
#define mhlrxwr(handle, offset,data)				iTE_I2C_WriteByte(handle, RXMHLAddr, offset, data)
#define mhlrxbrd(handle, offset,byteno,rddata)		iTE_I2C_ReadBurst(handle, RXMHLAddr, offset, byteno, rddata)
#define mhlrxbwr(handle, offset,byteno,rddata)		iTE_I2C_WriteBurst(handle, RXMHLAddr, offset, byteno, rddata)
#define mhlrxset(handle, offset,mask,data)			iTE_I2C_SetByte(handle, RXMHLAddr, offset, mask, data)
//TX
#define h2txrd(handle, port,offset)				iTE_I2C_ReadByteP(handle, TXP0Addr,port,offset)
#define h2txwr(handle, port,offset,data)			iTE_I2C_WriteByteP(handle, TXP0Addr,port,offset,data)
#define h2txset(handle, port,offset,mask,data)		iTE_I2C_SetByteP(handle, TXP0Addr, port,offset, mask, data)
#define h2txbrd(handle, port,offset,byteno,rddata)		iTE_I2C_ReadBurstP(handle, TXP0Addr,port, offset, byteno, rddata)
#define h2txcomwr(handle, offset,data)			iTE_I2C_WriteByte(handle, TXComAddr, offset, data)
#define h2txcomrd(handle, offset)				iTE_I2C_ReadByte(handle, TXComAddr, offset)
#define h2txcomset(handle, offset,mask,data)			iTE_I2C_SetByte(handle, TXComAddr, offset, mask, data)
#define h2txcombwr(handle, offset,byteno,rddata)		iTE_I2C_WriteBurst(handle, TXComAddr, offset, byteno, rddata)
//SP
#define h2spset(handle, offset,mask,data)			iTE_I2C_SetByte(handle, SWAddr, offset, mask, data)
#define h2spwr(handle, offset,data)				iTE_I2C_WriteByte(handle, SWAddr, offset, data)
#define h2spbrd(handle, offset,byteno,rddata)			iTE_I2C_ReadBurst(handle, SWAddr, offset, byteno, rddata)
#define h2sprd(handle, offset) 				iTE_I2C_ReadByte(handle, SWAddr, offset)
#define h2spbwr(handle, offset,byteno,rddata)			iTE_I2C_WriteBurst(handle, SWAddr, offset, byteno, rddata)
//RX
#define h2rxwr(handle, offset,data)				iTE_I2C_WriteByte(handle, RXP0Addr, offset, data)
#define h2rxrd(handle, offset) 				iTE_I2C_ReadByte(handle, RXP0Addr, offset)
#define h2rxset(handle, offset,mask,data) 			iTE_I2C_SetByte(handle, RXP0Addr, offset, mask, data)
#define h2rxbrd(handle, offset,byteno,rddata)			iTE_I2C_ReadBurst(handle, RXP0Addr,offset, byteno, rddata)
//EDID
#define h2rx_edidwr(handle, offset,data)			iTE_I2C_WriteByte(handle, RXEDID_Addr,offset,data)

#endif
