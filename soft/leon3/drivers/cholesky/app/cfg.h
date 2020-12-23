#ifndef __ESP_CFG_000_H__
#define __ESP_CFG_000_H__

#include "libesp.h"

typedef int32_t token_t;

/* <<--params-def-->> */
#define INPUT_ROWS 8
#define OUTPUT_ROWS 8

/* <<--params-->> */
const int32_t input_rows = INPUT_ROWS;
const int32_t output_rows = OUTPUT_ROWS;

#define NACC 1

esp_thread_info_t cfg_000[] = {
	{
		.run = true,
		.devname = "cholesky.0",
		.type = cholesky,
		/* <<--descriptor-->> */
		.desc.cholesky_desc.input_rows = INPUT_ROWS,
		.desc.cholesky_desc.output_rows = OUTPUT_ROWS,
		.desc.cholesky_desc.src_offset = 0,
		.desc.cholesky_desc.dst_offset = 0,
		.desc.cholesky_desc.esp.coherence = ACC_COH_NONE,
		.desc.cholesky_desc.esp.p2p_store = 0,
		.desc.cholesky_desc.esp.p2p_nsrcs = 0,
		.desc.cholesky_desc.esp.p2p_srcs = {"", "", "", ""},
	}
};

#endif /* __ESP_CFG_000_H__ */
