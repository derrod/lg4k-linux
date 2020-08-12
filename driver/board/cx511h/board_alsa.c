/*===================================================================
 * Copyright(c) AVerMedia TECHNOLOGIES, Inc. 2017
 * All rights reserved
 * =================================================================
 * board_alsa.c
 *
 *  Created on: Apr 15, 2017
 *      Author: 
 *      Version:
 * =================================================================
 */
 
//#include "linux/slab.h"
#include "board.h"
#include "cxt_mgr.h"
#include "mem_model.h"
#include "board_alsa.h"
#include "alsa_model.h"
#include "pci_model.h"
#include "i2c_model.h"
#include "debug.h"
#include "aver_xilinx.h"
#include "ite6805.h"

extern int subsystem_id;

typedef struct
{
    BASIC_CXT_HANDLE_DECLARE;
    alsa_model_handle_t alsa_handle;
    handle_t aver_xilinx_handle;
    handle_t i2c_chip_handle[CL511H_I2C_CHIP_COUNT];
}board_alsa_cxt_t;

static board_chip_desc_t cl511h_chip_desc[CL511H_I2C_CHIP_COUNT]=
{
        [CL511H_I2C_CHIP_ITE6805_0]=
        {
            .name=ITE6805_DRVNAME,
            .index=1,
        },

};

static alsa_model_pcm_info_t cl511h_pcm_info=
{
	.name="cl511h capture pcm",
	.capture_count=1,

};

static void *board_alsa_alloc(void);
static void board_alsa_release(void *cxt);

static void *board_alsa_alloc()
{
	board_alsa_cxt_t *cxt;
	cxt=mem_model_alloc_buffer(sizeof(board_alsa_cxt_t));

	return cxt;
}

static void board_alsa_release(void *cxt)
{
	board_alsa_cxt_t *board_alsa=cxt;

	if(board_alsa)
	{
            cxt_manager_unref_context(board_alsa->aver_xilinx_handle);
            mem_model_free_buffer(board_alsa);
	}

}

static void board_alsa_capture_start(void *data)
{
    board_alsa_cxt_t *board_alsa=data;
    aver_xilinx_audio_cbinfo_t audio_cbinfo;
    //handle_t ite6805_handle=board_alsa->i2c_chip_handle[CL511H_I2C_CHIP_ITE6805_0];
    //enum ite6805_audio_sample fe_audioinfo=0;
    //aver_xilinx_audio_cfg_t cfg;
    
    audio_cbinfo.callback=board_alsa_recv_data;
    audio_cbinfo.asso_cxt=board_alsa;
    //cfg.audio_rate = aver_xilinx_get_audioinfo(board_alsa->aver_xilinx_handle);
    //ite6805_get_audioinfo(ite6805_handle,&fe_audioinfo);

    //aver_xilinx_set_audio_dma(board_alsa->aver_xilinx_handle, &cfg);
    aver_xilinx_start_audio_streaming(board_alsa->aver_xilinx_handle,&audio_cbinfo);
}

static void board_alsa_capture_stop(void *data)
{
    board_alsa_cxt_t *board_alsa=data;
    debug_msg("%s\n",__func__);
    
    aver_xilinx_stop_audio_streaming(board_alsa->aver_xilinx_handle);
}
void board_alsa_init(cxt_mgr_handle_t cxt_mgr)
{
	board_alsa_cxt_t *board_alsa=NULL;
        handle_t aver_xilinx_handle;
    i2c_model_handle_t  i2c_mgr=NULL;
	enum
	{
		BOARD_ALSA_OK=0,
		BOARD_ALSA_ERROR_CXT_MGR,
		BOARD_ALSA_ERROR_ALLOC_CXT,
		BOARD_ALSA_ERROR_INIT,
                BOARD_ALSA_ERROR_GET_MAIN_CHIP_HANDLE,
                BOARD_ALSA_ERROR_GET_I2C_MGR
	}err=BOARD_ALSA_OK;
	do
	{
		alsa_model_setup_t alsa_info={0};
		int i;
		if(!cxt_mgr)
		{
			err=BOARD_ALSA_ERROR_CXT_MGR;
				break;
		}
		board_alsa=cxt_manager_add_cxt(cxt_mgr,BOARD_ALSA_CXT_ID,board_alsa_alloc,board_alsa_release);
		if(!board_alsa)
		{
			err=BOARD_ALSA_ERROR_ALLOC_CXT;
			break;
		}
		if (subsystem_id == 0x5730)
		    alsa_info.name="AVerMedia CL511H";
		alsa_info.pcm_count=sizeof(cl511h_pcm_info)/sizeof(alsa_model_pcm_info_t);
		alsa_info.support_fmt_mask=BIT_ALSA_MODEL_FMT_S16_LE | BIT_ALSA_MODEL_FMT_S24_LE;
		alsa_info.support_rate_mask=BIT_ALSA_MODEL_RATE_32K|BIT_ALSA_MODEL_RATE_44_1K|BIT_ALSA_MODEL_RATE_48K| BIT_ALSA_MODEL_RATE_96K | BIT_ALSA_MODEL_RATE_192K;
		alsa_info.pcm_info=&cl511h_pcm_info;
                alsa_info.period_size=7680*4;//12*1024;
                alsa_info.max_period_num=128;        
		board_alsa->alsa_handle=alsa_model_init(cxt_mgr,&alsa_info);
		if(!board_alsa->alsa_handle)
		{
			err=BOARD_ALSA_ERROR_INIT;
			break;
		}

        aver_xilinx_handle=cxt_manager_get_context(cxt_mgr,AVER_XILINX_CXT_ID,0);
        if(aver_xilinx_handle==NULL)
        {
            err=BOARD_ALSA_ERROR_GET_MAIN_CHIP_HANDLE;
            break;
        }
        board_alsa->aver_xilinx_handle=aver_xilinx_handle;

        i2c_mgr=cxt_manager_get_context(cxt_mgr,I2C_CXT_ID,0);
        if(i2c_mgr==NULL)
        {
            err=BOARD_ALSA_ERROR_GET_I2C_MGR;
            break;
        }

        for(i=0;i<CL511H_I2C_CHIP_COUNT;i++)
        {
            board_alsa->i2c_chip_handle[i]=i2c_model_get_nth_driver_handle(i2c_mgr,cl511h_chip_desc[i].name,cl511h_chip_desc[i].index);
            debug_msg("board_alsa i2c_chip_handle[%d] %p\n",i,board_alsa->i2c_chip_handle[i]);
        }

        alsa_model_register_callback(board_alsa->alsa_handle,ALSA_MODEL_CAPTURE_START_CB,board_alsa_capture_start,board_alsa);
        alsa_model_register_callback(board_alsa->alsa_handle,ALSA_MODEL_CAPTURE_STOP_CB,board_alsa_capture_stop,board_alsa);
        cxt_manager_ref_context(aver_xilinx_handle);
	}while(0);
	if(err!=BOARD_ALSA_OK)
	{
		switch(err)
		{
                case BOARD_ALSA_ERROR_GET_I2C_MGR:
                case BOARD_ALSA_ERROR_GET_MAIN_CHIP_HANDLE:    
		case BOARD_ALSA_ERROR_INIT:
			cxt_manager_unref_context(board_alsa);
		case BOARD_ALSA_ERROR_ALLOC_CXT:
		case BOARD_ALSA_ERROR_CXT_MGR:
			break;
		default:
			break;
		}
	}

}

void board_alsa_recv_data(void *board_alas_cxt,unsigned char *buffer,U32_T size)
{
	board_alsa_cxt_t *board_alsa=(board_alsa_cxt_t *)board_alas_cxt;
//        int i;
	
//        if(buffer[0])
//        {
//            mesg("%s buffer %p size %d\n",__func__,buffer,size);
//            for(i=0;i<32;i++)
//            {
//                mesg("%02x ",buffer[i]);
//            }
//            mesg("\n");
//        }
	alsa_model_feed_data(board_alsa->alsa_handle,buffer,size);
}
