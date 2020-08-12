///*****************************************
//  Copyright (C) 2009-2017
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <IT6664_Typedef.h>
//   @author Hojim.Tseng@ite.com.tw
//   @date   2017/11/03
//   @fileversion: ITE_HDMI2_SPLITER_1.14
//******************************************/
#ifndef _IT6664_TYPEDEF_H_
#define _IT6664_TYPEDEF_H_
//#include <linux/kernel.h>
 

#define _HPDMOS_
#ifdef _HPDMOS_
    #define PORT1_HPD_ON	0
    #define PORT1_HPD_OFF	1
#else
    #define PORT1_HPD_ON	1
    #define PORT1_HPD_OFF	0
#endif

//////////////////////////////////////////////////
// data type
//////////////////////////////////////////////////
#define iTE_FALSE	0
#define iTE_TRUE		1

#define FALSE 		0
#define TRUE 		1
#define SUCCESS 	0
#define FAIL 		-1
#define ON 			1
#define OFF        	0
#define HIGH       	1
#define LOW        	0


typedef enum _iTE_STATUS {
    iTE_SUCCESS = 0,
    iTE_FAIL
//    SYS_RESERVED
} iTE_STATUS;

#ifndef NULL
	#define NULL ((void *) 0)
#endif

typedef  unsigned char bit;

typedef char iTE_s8, *iTE_ps8;
typedef unsigned char iTE_u8, *iTE_pu8;
typedef short iTE_s16, *iTE_ps16;
typedef unsigned short iTE_u16, *iTE_pu16;


#ifndef _MCU_8051_				// IT6350
typedef unsigned long iTE_u32, *iTE_pu32;			//typedef unsigned int iTE_u32, *iTE_pu32; by AndyNien 20171018
typedef long iTE_s32, *iTE_ps32;					//typedef int iTE_s32, *iTE_ps32; by AndyNien 20171018
#else
typedef unsigned long iTE_u32, *iTE_pu32;
typedef long iTE_s32, *iTE_ps32;
#endif

//typedef int iTE_u1 ;
//#define _CODE __attribute__ ((section ("._OEM_BU1_RODATA ")))

#ifdef _MCU_8051_
	typedef bit iTE_u1 ;
	#define _CODE code
    #define _BIT       bit
    #define _BDATA     bdata
    #define _DATA      data
    #define _IDATA     idata
    #define _XDATA     xdata
    #define _FAR_       far
    #define _REENTRANT reentrant
#else
	#ifdef WIN32
		typedef iTE_u16 iTE_u1 ;
		#define _CODE
	#else
		typedef iTE_u16 iTE_u1 ;
	//	#define _CODE
		#define _CODE __attribute__ ((section ("._OEM_BU1_RODATA ")))
	#endif
    #define _BIT       BOOL
    #define _BDATA
    #define _DATA
    #define _IDATA
    #define _XDATA
    #define _FAR
    #define _REENTRANT
#endif

#define CD8BIT			4
#define CD10BIT	 		5
#define CD12BIT			6
#define CD16BIT			7

#define OUT8B           		0
#define OUT10B          		1
#define OUT12B          		2

#define RGB444			0
#define YCbCr422			1
#define YCbCr444			2

#define RGB444_SDR		0
#define YUV444_SDR		1
#define RGB444_DDR		2
#define YUV444_DDR		3
#define YUV422_EMB_SYNC_SDR	4
#define YUV422_EMB_SYNC_DDR	5
#define YUV422_SEP_SYNC_SDR	6
#define YUV422_SEP_SYNC_DDR	7
#define CCIR656_EMB_SYNC_SDR	8
#define CCIR656_EMB_SYNC_DDR	9
#define CCIR656_SEP_SYNC_SDR	10
#define CCIR656_SEP_SYNC_DDR	11
#define RGB444_HALF_BUS		12
#define YUV444_HALF_BUS		13
#define BTA1004_SDR				14
#define BTA1004_DDR				15

#define GP00				(0)
#define GPB3				(1)
#define GPB4				(2)
#define GPF6				(3)
#define GPF7				(4)
#define GPA0				(5)
#define GPA1				(6)
#define GPG3				(7)
#define GPG4				(8)
#define GPG6				(9)
#define GPA6				(10)
#define GPI0				(11)
#define GPI1				(12)
#define GPI2				(13)
#define GPI3				(14)
#define GPI4				(15)
#define GPI5				(16)
#define GPG5				(17)

enum{
	HDMI_TX_A = 0x00,
	HDMI_TX_B = 0x01,
	HDMI_TX_C = 0x02,
	HDMI_TX_D = 0x03,
};

#define IT6662_HPD_H				(0x03)
#define IT6662_HPD_L				(0x01)
#define IT6662_HPD_AUTO			(0x00)

#define IT6662_CEC_A				(0x80 | (HDMI_TX_A << 4))
#define IT6662_CEC_B				(0x80 | (HDMI_TX_B << 4))
#define IT6662_CEC_C				(0x80 | (HDMI_TX_C << 4))
#define IT6662_CEC_D				(0x80 | (HDMI_TX_D << 4))
#endif

