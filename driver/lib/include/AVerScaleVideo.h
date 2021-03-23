#ifndef __DOWNSCALEVIDEO_H_
#define __DOWNSCALEVIDEO_H_

//#define USE_ASM

// Removed by Jeff_20180827: Remove enum definition to AVerType.h
/*
enum{
	AVM_CLRSPACE_YUY2 = 0,
	AVM_CLRSPACE_RGB24,
	AVM_CLRSPACE_YV12,
	AVM_CLRSPACE_NV12,
	AVM_CLRSPACE_R,
	AVM_CLRSPACE_GB,
	AVM_CLRSPACE_MAX,
	//below color space was not supported scaling
	AVM_CLRSPACE_IYU2,
	AVM_CLRSPACE_AYUV,
	AVM_CLRSPACE_V410,
	AVM_CLRSPACE_Y410,
	AVM_CLRSPACE_YVYU,
	AVM_CLRSPACE_UYVY,
	AVM_CLRSPACE_V210,
	AVM_CLRSPACE_Y210,
	AVM_CLRSPACE_I420,
	AVM_CLRSPACE_RGB565,
	AVM_CLRSPACE_RGB555,
	AVM_CLRSPACE_RGB32,
	AVM_CLRSPACE_ARGB32,
	AVM_CLRSPACE_XRGB,
};
enum{
	AVM_VSCAL_BILINEAR = 0, 
	AVM_VSCAL_BICUBIC, 
};
*/

#ifndef LPVOID
typedef void * LPVOID;
#endif

#ifndef BOOL
typedef unsigned int BOOL;
#endif

#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef WORD
typedef unsigned short WORD;
#endif

#ifndef DWORD
typedef unsigned long DWORD;
#endif

#ifndef LONG
typedef long LONG;
#endif

#ifndef LONGLONG
typedef long long LONGLONG;
#endif

enum{
    AVM_CLRSPACE_YUY2 = 0,
    AVM_CLRSPACE_RGB24,
    AVM_CLRSPACE_YV12,
    AVM_CLRSPACE_NV12,
    AVM_CLRSPACE_R,
    AVM_CLRSPACE_GB,
    AVM_CLRSPACE_MAX,
    //below color space was not supported scaling
    AVM_CLRSPACE_IYU2,
    AVM_CLRSPACE_AYUV,
    AVM_CLRSPACE_V410,
    AVM_CLRSPACE_Y410,
    AVM_CLRSPACE_YVYU,
    AVM_CLRSPACE_UYVY,
    AVM_CLRSPACE_V210,
    AVM_CLRSPACE_Y210,
    AVM_CLRSPACE_I420,
    AVM_CLRSPACE_RGB565,
    AVM_CLRSPACE_RGB555,
    AVM_CLRSPACE_RGB32,
    AVM_CLRSPACE_ARGB32,
    AVM_CLRSPACE_XRGB,
};

enum{
    AVM_VSCAL_BILINEAR = 0,
    AVM_VSCAL_BICUBIC,
};

//Added by Jeff_20180827+
BOOL IsScalingSupported(DWORD dwColorSpace);
DWORD GetStride(DWORD dwWidth, DWORD dwColorSpace);
BOOL AVerDoScale(BYTE** ppInitData, BYTE* pSrc, BYTE* pDst, DWORD dwSrcW, DWORD dwSrcH,DWORD dwDstW, DWORD dwDstH, DWORD dwColorSpace, DWORD dwScalingAlg/* = AVM_VSCAL_BILINEAR*/);
BOOL AVerDoCopy(BYTE* pbSrc, BYTE* pbDest, DWORD dwSrcW, DWORD dwSrcH,DWORD dwDstW, DWORD dwDstH, DWORD dwColorSpace);
//Added by Jeff_20180827-

BOOL ScaleVideoInit(BYTE** ppInitData,DWORD dwColorSpace, DWORD dwScalingAlg, DWORD dwSrcW, DWORD dwSrcH,DWORD dwDstW, DWORD dwDstH);

BOOL ScaleVideoUninit(BYTE** ppInitData);

BOOL ScaleVideo(BYTE* pDst, BYTE* pSrc, BYTE** ppInitData, DWORD dwSrcW, DWORD dwSrcH,DWORD dwDstW, DWORD dwDstH,
					LONG lSrcStride,LONG lDstStride);

//20171215+
void CopyVideoFrame(BYTE* pbDest, BYTE* pbSrc, int BytesPerLine, int LinesPerFrame
/*BYTE* pbSrc, BYTE* pbDest, int iWidth, int iHeight, int iSrcStride, int iDestStride*/);
//20171215-

void CopyVideoFrameEx(BYTE* pbSrc, BYTE* pbDest, int iWidth, int iHeight, int iSrcStride, int iDestStride, DWORD dwColorSpace);

//DWORD TranslateFourccToAvmClrSpace(DWORD Compression, DWORD SubFourcc); //Added by Jeff_20180802

BOOL AVerVideoProcess(BYTE* pbSrc, BYTE* pbDest, DWORD dwWidth, DWORD dwHeight, DWORD dwColorSpace, DWORD dwFrameExtent, BOOL bInterlace, BOOL bNeedInterlace, BOOL bNeedFlip);

#endif
