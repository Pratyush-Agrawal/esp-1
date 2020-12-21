#ifndef __ESP_CFG_000_H__
#define __ESP_CFG_000_H__

#include "libesp.h"

typedef int32_t token_t;

/* <<--params-def-->> */
#define INPUT_COLS 8
#define INPUT_ROWS 8
#define DCT_ISTRUE 1
#define OUTPUT_ROWS 8
#define OUTPUT_COLS 8

/* <<--params-->> */
const int32_t input_cols = INPUT_COLS;
const int32_t input_rows = INPUT_ROWS;
const int32_t dct_istrue = DCT_ISTRUE;
const int32_t output_rows = OUTPUT_ROWS;
const int32_t output_cols = OUTPUT_COLS;

#define NACC 1

esp_thread_info_t cfg_000[] = {
	{
		.run = true,
		.devname = "dct_idct.0",
		.type = dct_idct,
		/* <<--descriptor-->> */
		.desc.dct_idct_desc.input_cols = INPUT_COLS,
		.desc.dct_idct_desc.input_rows = INPUT_ROWS,
		.desc.dct_idct_desc.dct_istrue = DCT_ISTRUE,
		.desc.dct_idct_desc.output_rows = OUTPUT_ROWS,
		.desc.dct_idct_desc.output_cols = OUTPUT_COLS,
		.desc.dct_idct_desc.src_offset = 0,
		.desc.dct_idct_desc.dst_offset = 0,
		.desc.dct_idct_desc.esp.coherence = ACC_COH_NONE,
		.desc.dct_idct_desc.esp.p2p_store = 0,
		.desc.dct_idct_desc.esp.p2p_nsrcs = 0,
		.desc.dct_idct_desc.esp.p2p_srcs = {"", "", "", ""},
	}
};

#endif /* __ESP_CFG_000_H__ */
