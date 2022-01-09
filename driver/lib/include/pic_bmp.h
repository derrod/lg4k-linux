/*
 * pic_bmp.h
 *
 *  Created on: Jun 12, 2020
 *      Author: jeff
 */

#ifndef PIC_INCLUDE_PIC_BMP_H_
#define PIC_INCLUDE_PIC_BMP_H_

#define PCI_BMP_CXT_ID fourcc_id('P','B','M','P')

typedef void *pic_bmp_handle_t;

typedef enum _pic_bmp_pixfmt_enum_t
{
    PIC_BMP_PIXFMT_YUYV,
    PIC_BMP_PIXFMT_YV12,
    PIC_BMP_PIXFMT_NV12,
    PIC_BMP_PIXFMT_RGB24,
    PIC_BMP_PIXFMT_OTHERS,
} pic_bmp_pixfmt_enum_t;

void pic_bmp_init(cxt_mgr_handle_t cxt_mgr
        , const char *no_signal_filename
        , const char *out_of_range_filename
        , const char *copy_protection_filename);

void pic_bmp_prepare(pic_bmp_handle_t pic_bmp_handle, unsigned long src_width, unsigned long src_height, unsigned long dst_width, unsigned long dst_height);

void load_no_signal_image(pic_bmp_handle_t pic_bmp_handle, unsigned char *dst, int width, int height, pic_bmp_pixfmt_enum_t pixfmt_out);
void load_out_of_range_image(pic_bmp_handle_t pic_bmp_handle, unsigned char *dst, int width, int height, pic_bmp_pixfmt_enum_t pixfmt_out);
void load_copy_protection_image(pic_bmp_handle_t pic_bmp_handle, unsigned char *dst, int width, int height, pic_bmp_pixfmt_enum_t pixfmt_out);
void load_black_image(pic_bmp_handle_t pic_bmp_handle, unsigned char *dst, int width, int height);

#endif /* PIC_INCLUDE_PIC_BMP_H_ */
