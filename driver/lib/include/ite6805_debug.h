///*****************************************
//  Copyright (C) 2009-2017
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <debug.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2017/12/01
//   @fileversion: iTE6805_MCUSRC_1.10
//******************************************/

//#include "debug.h"

#ifndef _DEBUG_H_
#define _DEBUG_H_


#define Debug_message 1

#ifndef Debug_message
#define Debug_message 1
#endif

#define ITEPrint(fmt, ...)
#if Debug_message
#define MHLRX_DEBUG_PRINTF(x)  ITEPrint(x)			// AndyNien
#define EQ_DEBUG_PRINTF(x) ITEPrint(x)				// AndyNien
#define VIDEOTIMNG_DEBUG_PRINTF(x)    ITEPrint(x)	// AndyNien
#define iTE6805_DEBUG_INT_PRINTF(x)  ITEPrint (x)	// AndyNien
#define HDMIRX_VIDEO_PRINTF(x)  ITEPrint(x)			// AndyNien
#define HDMIRX_AUDIO_PRINTF(x)  ITEPrint(x)			// AndyNien
#define HDMIRX_DEBUG_PRINT(x) 	ITEPrint(x)			// AndyNien
#define CEC_DEBUG_PRINTF(x) ITEPrint(x)				// AndyNien
#define EDID_DEBUG_PRINTF(x) ITEPrint(x)			// AndyNien
#define RCP_DEBUG_PRINTF(x)  ITEPrint(x)			// AndyNien
#define MHL3D_DEBUG_PRINTF(x) ITEPrint(x)			// AndyNien
#define MHL_MSC_DEBUG_PRINTF(x) ITEPrint(x)			// AndyNien
#define HDCP_DEBUG_PRINTF(x) ITEPrint(x)			// AndyNien
#define HDCP_DEBUG_PRINTF1(x) ITEPrint(x)			// AndyNien
#define HDCP_DEBUG_PRINTF2(x) ITEPrint(x)			// AndyNien
#define REG_PRINTF(x) ITEPrint(x)					// AndyNien

#else
#define MHLRX_DEBUG_PRINTF(x)
#define EQ_DEBUG_PRINTF(x)
#define VIDEOTIMNG_DEBUG_PRINTF(x)
#define iTE6805_DEBUG_INT_PRINTF(x)
#define HDMIRX_VIDEO_PRINTF(x)
#define HDMIRX_AUDIO_PRINTF(x)
#define HDMIRX_DEBUG_PRINT(x)
#define CEC_DEBUG_PRINTF(x)
#define EDID_DEBUG_PRINTF(x)
#define IT680X_DEBUG_PRINTF(x)
#define RCP_DEBUG_PRINTF(x)
#define MHL3D_DEBUG_PRINTF(x)
#define MHL_MSC_DEBUG_PRINTF(x)
#define REG_PRINTF(x)

#endif





#endif
