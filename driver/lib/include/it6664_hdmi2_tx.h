///*****************************************
//  Copyright (C) 2009-2018
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6664_hdmi2_tx.h>
//   @author Hojim.Tseng@ite.com.tw
//   @date   2018/02/22
//   @fileversion: ITE_HDMI2_SPLITER_1.18
//******************************************/
#ifndef _IT6664_HDMI2_TX_H_
#define _IT6664_HDMI2_TX_H_

#include "it6664_extern.h"

#define rol(x,y) (((x) << (y)) | (((iTE_u32)x) >> (32-y)))

#define FixRand					FALSE
#define EnHDCPAutoMute 			FALSE
#define EnAutoReAuth			TRUE
#define EnHDCPATC 				TRUE
#define GenAn 					TRUE
#define EnRiChk2DoneInt  		FALSE
#define EnSyncDet2FailInt 		FALSE
#define EnSyncDetChk 			FALSE
#define EnHDCP1p1 				FALSE
#define EnDDCMasterSel 			FALSE //TRUE: FW only, FALSE: HDCP > SCDC
#define EnHDCP2ListSRMChk 		TRUE // enable hdcp2tx repeater ID list SRM check
#define EnHDCP2RxIDSRMChk 		FALSE
#define EnRepWaitTxMgm 			FALSE // whwn HDCP2Tx as a repeater, wait upstream Tx send Mgm
#define EnRepWaitTxEks 			FALSE // whwn HDCP2Tx as a repeater, wait upstream Tx send Eks
#define EnHDCP2ATC 				FALSE
#define DisR0ShortRead 			TRUE  // 980 only support hdcp combine read
#define DisRiShortRead 			TRUE  // for ASTRO HDCP CTS
#define EncDis 					FALSE // only for internal test, when set true, EnSyncDetChk must set FALSE
#define EnRiPjInt 				FALSE
#define EnDynAudOnOff  			FALSE
#define EnDynAVMute 	 		FALSE
#define EncPauseTest			FALSE
#define TestSpecSHA 			FALSE
#define EnRxH2SHA1  			TRUE
#define EnTxHP2KSVChk 			TRUE//FALSE //For HDCP2.2 source test

#define EnEDIDParse 		 	TRUE
#define EnPHYTest 				FALSE //TRUE: for ATC electrical test => disable EnH2DetRR and EnFlagPolling
#define ChkHFVSDB  				TRUE  // check HF_VSDB data
#define ChkSCDC 	 			TRUE  // check SCDC present
#define EnVSIFPkt 				FALSE
#define EnReHDCPTest 	 		FALSE
#define PGColBar  				TRUE


#define DDCWAITTIME     		1
#define DDCWAITNUM     			20
#define HDCPFireMax 			5
#define HP2ReauthMax 			3

#define ChkKSVListMax 			105
#define LClimit 				0          // HDCP2.2 Locality check limitation
#define PGColMod 				0//RGB444

void setup_txpatgen(void *ite6664_handle);
void h2tx_irq(void *ite6664_handle, iTE_u8 port);
void h2tx_pwron(void *ite6664_handle, iTE_u8 port);
void h2tx_pwrdn(void *ite6664_handle, iTE_u8 port);
void hdcpsts(void *ite6664_handle, iTE_u8 val,iTE_u8 port);
void setup_h2txafe(void *ite6664_handle, iTE_u8 port);
void h2tx_enout(void *ite6664_handle, iTE_u8 port);
void setrx_ksv_list(void *ite6664_handle, iTE_u8 devcnt, iTE_u8 depth, iTE_u8 err_devcnt, iTE_u8 err_depth);
#ifdef SHA_Debug
void SHA_Simple(iTE_u8 *p,iTE_u16 msgcnt,iTE_u8 *output);
void SHATransform(iTE_u32 * h, iTE_u32 * w);
#endif
iTE_u8 txrunhdcp(void *ite6664_handle, iTE_u8 port);
void txrunhdcp2(void *ite6664_handle, iTE_u8 port);
void hdcprd(void *ite6664_handle, iTE_u8 port, iTE_u8 offset, iTE_u8 bytenum );
iTE_u8 scdcrd(void *ite6664_handle, iTE_u8 port, iTE_u8 offset, iTE_u8 bytenum);
iTE_u8 scdcwr(void *ite6664_handle, iTE_u8 port, iTE_u8 offset, iTE_u8 data);
void chktx_hdcp2_ksv(void *ite6664_handle, iTE_u8 port);
iTE_u8 setup_h2scdc(void *ite6664_handle, iTE_u8 port);
iTE_u8 ddcwait(void *ite6664_handle, iTE_u8 port);
void FireHDCP2(void *ite6664_handle, iTE_u8 port);
void it6664_txvar_init(void *ite6664_handle);
void SetTXSource(void *ite6664_handle, iTE_u8 port);
void SetTxLED(iTE_u8 port,iTE_u8 sts);
iTE_u8 CheckBKsv(iTE_u8 ksv[5]);
void it6664_hdmitx_video_state(void *ite6664_handle, iTE_u8 port);
void it6664_hdcp_state(void *ite6664_handle, iTE_u8 port);
void it6664_hdmitx_handler(void *ite6664_handle);
void Show_status(void *ite6664_handle);
void it6664_TXCED_monitor(void *ite6664_handle, iTE_u8 port);
void cal_rxh1_sha(void *ite6664_handle, iTE_u8 DevCnt );
iTE_u8 SelDsVic(void *ite6664_handle);
void h2txVclkrst(void *ite6664_handle, iTE_u8 port);
void SetDeepColor(void *ite6664_handle, iTE_u8 port,iTE_u8 src);

#endif
