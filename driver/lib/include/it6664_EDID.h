///*****************************************
//  Copyright (C) 2009-2017
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6664_EDID.h>
//   @author Hojim.Tseng@ite.com.tw
//   @date   2017/11/03
//   @fileversion: ITE_HDMI2_SPLITER_1.14
//******************************************/
#ifndef _IT6664_EDID_H_
#define _IT6664_EDID_H_
//#include "..\..\IT6350\code\api\debug\debug_print.h"
//#include "IT6664_Typedef.h"
#include "it6664_config.h"
#include "it6664_extern.h"

typedef struct
{
	iTE_u8 ScreenSize[2];
	iTE_u8 EstablishTimeing[3];
	iTE_u8 DTDsupport[4];
	iTE_u8 STD;
	iTE_u8 STDTiming[8*2];
	iTE_u8 block1;
	iTE_u8 DTD[4][18];			// add by AndyNien, 20180207
}EDIDBlock0;
typedef struct
{
	iTE_u8 Audiodes;
	iTE_u8 Format[32];
}Audio_data;
typedef struct
{
	iTE_u8 Viccnt;
	iTE_u8 vic[32];
	iTE_u8 Data[32];		// include tag+size, CTA tag

}Video_data;
typedef struct
{
	iTE_u8 DTDnumber;
	iTE_u8 DTD[4][18];;
}DTD_data;
typedef struct       // add by AndyNien, 20180207
{
	iTE_u8 SupportColorimetry;
	iTE_u8 Data[4];		// include tag+size, CTA tag
}Colorimetry_data;	
typedef struct       // add by AndyNien, 20180207
{
	iTE_u8 SupportVideoCapability;
	iTE_u8 Data[4];		// include tag+size, CTA tag
}VideoCapability_data;
typedef struct       // add by AndyNien, 20180207
{
	iTE_u8 SupportVideoVendorSpecificVDB;
	iTE_u8 Data[28];		// include tag+size, CTA tag
}VendorSpecificVDB_data;

typedef struct       // add by AndyNien, 20180207
{
	iTE_u8 SupportStaticMetadata;
	iTE_u8 Data[42];		// include tag+size, CTA tag
}StaticMetadata_data;

typedef struct       // add by AndyNien, 20180207
{
	iTE_u8 Support420_Only;
	iTE_u8 Data[32];		// include tag+size, CTA tag
}x420_Only_data;

typedef struct
{
	iTE_u8 Data[32];
	iTE_u8 SCDCPresent;
	iTE_u8 RR_Capable;
	iTE_u8 LTE_340M_scramble;
	iTE_u8 IndependentView;
	iTE_u8 DualView;
	iTE_u8 OSD_3D;
	iTE_u8 DC_420;
	iTE_u8 Byte1;
	iTE_u8 Byte2;
}VSDB2_content;
typedef struct
{
	iTE_u8 Data[32];
	iTE_u8 phyaddr0;
	iTE_u8 phyaddr1;
	iTE_u8 DCSupport;
	iTE_u8 Vdsb8_CNandPresent;
	iTE_u8 VideoLatency;
	iTE_u8 AudioLatency;
	iTE_u8 I_VideoLatency;
	iTE_u8 I_AudioLatency;
	iTE_u8 HDMI_VicLen;
	iTE_u8 HDMI_3DLen;
	iTE_u8 HDMIVic[4];
	iTE_u8 HDMI3D_Present;
	iTE_u8 Support3D;
	iTE_u8 _3D_Struct_All_15_8;
	iTE_u8 _3D_Struct_All_7_0;
	iTE_u8	_3D_MASK_All_15_8;
	iTE_u8	_3D_MASK_All_7_0;
	iTE_u8 _3D_Order[10];
}VSDB1_content;

typedef struct
{
	Audio_data audio_info;
	Video_data video_info;
	iTE_u8 speaker_info;
	VSDB2_content vsdb2;
	VSDB1_content vsdb1;
	//iTE_u8 HDR_Content[2];
	DTD_data DTD_info;			// add by AndyNien, 20180207
	Colorimetry_data Colorimetry_info;	// add by AndyNien, 20180207
	VideoCapability_data Capability_info;	// add by AndyNien, 20180207
	VendorSpecificVDB_data VendorSpecificVDB_info;	// add by AndyNien, 20180207
	StaticMetadata_data	StaticMetadata_info;
	x420_Only_data x420_Only_info;
}EDIDBlock1;
typedef enum _CEA_TAG
{
	CEA_TAG_NONE 	= 0,
	CEA_TAG_ADB		= 1,
	CEA_TAG_VDB		= 2,
	CEA_TAG_VSDB	= 3,
	CEA_MODE_SPK	= 4,
	CEA_MODE_DTC	= 5,
	CEA_MODE_REV	= 6,
	CEA_MODE_EXT	= 7
}CEA_TAG;

typedef enum 
{
	CEA_CTA_VCDB = 0,				// video capability data block
	CEA_CTA_VSVDB = 1,					// Vendor-Specific Video Data block
	CEA_CTA_VDDDB = 2,					// VESA Display Device Data Block
	CEA_CTA_VDTBE = 3,					// VESA Video Timing block Extension
	CEA_CTA_HVDB = 4,					// HDMI video Data Block
	CEA_CTA_COLORIMETRY = 5,			// Colorimetry Data Block
	CEA_CTA_HDR_STATIC_METADATA = 6,	
	CEA_CTA_HDR_DYNAMIC_METADATA = 7,
	CEA_CTA_420_ONLY = 14,
	CEA_CTA_420_CAPABILITY_MAP,
}CEA_CTA_TAG;
#if 0
typedef enum _Edid_Mode
{
	None_EDID = 5, 
	Default_EDID2k = 4,
	Default_EDID4k = 3,
	Local_Mode = 2,		//
	Copy_Mode = 1,
	Compose_Mode = 0,
} Edid_Mode;
#else
enum
{
	Default_EDID2k = 0x00,
	Default_EDID4k = 0xA0,
	Copy_Mode = 0x80,
	Compose_Mode = 0x20,
};
#endif

void it6664_EdidMode_Switch(void *ite6664_handle);
iTE_u8 it6664_Edid_block1_parse(void *ite6664_handle, iTE_u8 port,iTE_u8 tmp[]);
iTE_u8 it6664_Edid_block0_parse(void *ite6664_handle, iTE_u8 port,iTE_u8 tmp[]);
iTE_u8 it6664_ComposeEDIDBlock1_ExtTag(void *ite6664_handle, iTE_pu8 ptr,iTE_u8 offset , iTE_u8 tag);
iTE_u8 it6664_ComposeEDIDBlock1_VSDB2(void *ite6664_handle, iTE_pu8 ptr,iTE_u8 offset);
iTE_u8 it6664_ComposeEDIDBlock1_VSDB1(void *ite6664_handle, iTE_pu8 ptr,iTE_u8 offset);
iTE_u8 it6664_ComposeEDIDBlock1_Speaker(void *ite6664_handle, iTE_pu8 ptr,iTE_u8 offset);
iTE_u8 it6664_ComposeEDIDBlock1_Audio(void *ite6664_handle, iTE_pu8 ptr,iTE_u8 offset);
iTE_u8 it6664_ComposeEDIDBlock1_VIC(void *ite6664_handle, iTE_pu8 ptr,iTE_u8 offset,iTE_u8 sup_vsdb2);
void it6664_block0_compose(void *ite6664_handle, iTE_pu8 arry);
void it6664_block1_compose(void *ite6664_handle, iTE_pu8 arry);
void it6664_block1_modify(void *ite6664_handle, iTE_pu8 edid_in_tmp);
iTE_u8 it6664_Check_HPDsts(void *ite6664_handle);
iTE_u8 it6664_Edid_Parse(void *ite6664_handle, iTE_u8 HPDsts);
void it6664_Edid_Copy(void *ite6664_handle, iTE_u8 hpdsts);
void it6664_Edid_Compose(void *ite6664_handle);
iTE_u8 CalChecksum(iTE_u8 edid[],iTE_u8 block);
iTE_u8 it6664_EDIDCompare(iTE_pu8 arry1,iTE_pu8 arry2);
void EDID_48Bit_Remove(iTE_pu8 arry1);
iTE_u8 CheckHPDCnt(void *ite6664_handle);
void it6664_Edid_DataInit(void *ite6664_handle, iTE_u8 port);
iTE_u8 it6664_DTD2VIC(void *ite6664_handle, iTE_u8 table[]);
void it6664_Audio_Parse(void *ite6664_handle, iTE_u8 port, iTE_u8 table[], iTE_u8 offset, iTE_u8 *number);
void it6664_Video_Parse(void *ite6664_handle, iTE_u8 port,iTE_u8 table[],iTE_u8 offset);
void it6664_Vsdb1_Parse(void *ite6664_handle, iTE_u8 port,iTE_u8 table[],iTE_u8 offset);
void it6664_Vsdb2_Parse(void *ite6664_handle, iTE_u8 port,iTE_u8 table[],iTE_u8 offset);
void it6664_EXT_Parse(void *ite6664_handle, iTE_u8 port,iTE_u8 table[],iTE_u8 offset);
iTE_u8 it6664_read_one_block_edid(void *ite6664_handle, iTE_u8 port, iTE_u8 block, iTE_u8 *edid_buffer);

//#if 0
//void Show_EDID(iTE_pu8 ptr);
//iTE_u8 it6664_read_edid(iTE_u8 port, iTE_u8 block, iTE_u8 offset, iTE_u16 length, iTE_u8 *edid_buffer);
//#endif



//#define Edid_Wb(u8Offset, u8Count, pu8Data)	iTE_I2C_WriteBurst(RXEDID_Addr, u8Offset, u8Count, pu8Data)
//#define Edid_Rb(u8Offset, u8Count, pu8Data)	iTE_I2C_ReadBurst(RXEDID_Addr, u8Offset, u8Count, pu8Data)
//#define Edid_Set(u8Offset, u8InvMask, u8Data)	iTE_I2C_SetByte(RXEDID_Addr, u8Offset, u8InvMask, u8Data)
//#define Edid_W(u8Offset, u8Data)		iTE_I2C_WriteByte(RXEDID_Addr, u8Offset, u8Data)
//#define Edid_R(u8Offset)			iTE_I2C_ReadByte(RXEDID_Addr, u8Offset)


#endif
