/* Copyright (c) 2011-2019 Columbia University, System Level Design Group */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdio.h>
#ifndef __riscv
#include <stdlib.h>
#endif

#include <esp_accelerator.h>
#include <esp_probe.h>
#include <fixed_point.h>
#include <math.h>
typedef int32_t token_t;

static unsigned DMA_WORD_PER_BEAT(unsigned _st)
{
        return (sizeof(void *) / _st);
}


#define SLD_DCT_IDCT 0x067
#define DEV_NAME "sld,dct_idct"

/* <<--params-->> */
const int32_t input_cols = 8;
const int32_t input_rows = 8;
const int32_t dct_istrue = 1;
const int32_t output_rows = 8;
const int32_t output_cols = 8;

static unsigned in_words_adj;
static unsigned out_words_adj;
static unsigned in_len;
static unsigned out_len;
static unsigned in_size;
static unsigned out_size;
static unsigned out_offset;
static unsigned mem_size;

/* Size of the contiguous chunks for scatter/gather */
#define CHUNK_SHIFT 20
#define CHUNK_SIZE BIT(CHUNK_SHIFT)
#define NCHUNK(_sz) ((_sz % CHUNK_SIZE == 0) ?		\
			(_sz / CHUNK_SIZE) :		\
			(_sz / CHUNK_SIZE) + 1)

/* User defined registers */
/* <<--regs-->> */
#define DCT_IDCT_INPUT_COLS_REG 0x50
#define DCT_IDCT_INPUT_ROWS_REG 0x4c
#define DCT_IDCT_DCT_ISTRUE_REG 0x48
#define DCT_IDCT_OUTPUT_ROWS_REG 0x44
#define DCT_IDCT_OUTPUT_COLS_REG 0x40


static int validate_buf(token_t *out, token_t *gold)
{
	int j;
	unsigned errors = 0;
	unsigned errors_x = 0;
	unsigned errors_1 = 0;
	unsigned errors_2 = 0;
	unsigned errors_3 = 0;
	unsigned errors_4 = 0;
	int golden, design;
	int VAL;

	 for( j = 0; j < output_rows * output_cols; j++) {
		golden= (int)(gold[j]);
		design= (int)(out[j]);
 	//	printf("GOLDEN VALUE  = %d \t   DESIGN VALUE = %d \t ELEMENT = %d  \n ", golden, design, j );
		VAL = fabs(golden - design);
	    	if (VAL == 1){
			// printf(" MISMATCH FOUND : Difference between golden value and output from design is 1 \n");
			 errors_1++;
        	} else if (VAL ==2) {
			// printf(" MISMATCH FOUND : Difference between golden value and output from design is 2 \n");
			errors_2++;
		} else if (VAL==3) {
			// printf(" MISMATCH FOUND : Difference between golden value and output from design is 3 \n");
			errors_3++;
		} else if (VAL >=4) {
			// printf(" MISMATCH FOUND : Difference between golden value and output from design is more than 3 \n");
			errors_4++;
		}
	}

	errors = errors_1 + errors_2 + errors_3 + errors_4;
	errors_x = errors_3 + errors_4;
//	printf(" Elements with difference 1 are : %d \n",errors_1);
//	printf(" Elements with difference 2 are : %d \n",errors_2);
//	printf(" Elements with difference 3 are : %d \n",errors_3);
//	printf(" Elements with difference 4 are : %d \n",errors_4);
//	printf(" Total Elements with errors are : %d \n",errors);
	
return errors_x;
}



int main(int argc, char * argv[])
{
	int i;
	int n;
	int ndev;
	struct esp_device *espdevs;
	struct esp_device *dev;
	unsigned done;
	unsigned **ptable;
	token_t *mem;
	token_t *gold;
	unsigned errors = 0;

	if (DMA_WORD_PER_BEAT(sizeof(token_t)) == 0) {
		in_words_adj = input_rows * input_cols;
		out_words_adj = output_rows * output_cols;
	} else {
		in_words_adj = round_up(input_rows * input_cols, DMA_WORD_PER_BEAT(sizeof(token_t)));
		out_words_adj = round_up(output_rows * output_cols, DMA_WORD_PER_BEAT(sizeof(token_t)));
	}
	in_len = in_words_adj * (1);
	out_len = out_words_adj * (1);
	in_size = in_len * sizeof(token_t);
	out_size = out_len * sizeof(token_t);
	out_offset  = in_len;
	mem_size = (out_offset * sizeof(token_t)) + out_size;


	// Search for the device
	printf("Scanning device tree... \n");

	ndev = probe(&espdevs, SLD_DCT_IDCT, DEV_NAME);
	if (ndev == 0) {
		printf("dct_idct not found\n");
		return 0;
	}
	for (n = 0; n < ndev; n++) {

		dev = &espdevs[n];

		// Check DMA capabilities
		if (ioread32(dev, PT_NCHUNK_MAX_REG) == 0) {
			printf("  -> scatter-gather DMA is disabled. Abort.\n");
			return 0;
		}

		if (ioread32(dev, PT_NCHUNK_MAX_REG) < NCHUNK(mem_size)) {
			printf("  -> Not enough TLB entries available. Abort.\n");
			return 0;
		}

		// Allocate memory
		gold = aligned_malloc(out_size);
		mem = aligned_malloc(mem_size);
		printf("  memory buffer base-address = %p\n", mem);

		// Alocate and populate page table
		ptable = aligned_malloc(NCHUNK(mem_size) * sizeof(unsigned *));
		for (i = 0; i < NCHUNK(mem_size); i++)
			ptable[i] = (unsigned *) &mem[i * (CHUNK_SIZE / sizeof(token_t))];

		printf("  ptable = %p\n", ptable);
		printf("  nchunk = %lu\n", NCHUNK(mem_size));

		printf("  Generate input : Loading data from barec_data.h...\n");
		#include "barec_data.h"
		// Pass common configuration parameters

		iowrite32(dev, SELECT_REG, ioread32(dev, DEVID_REG));
		iowrite32(dev, COHERENCE_REG, ACC_COH_NONE);

#ifndef __sparc
		iowrite32(dev, PT_ADDRESS_REG, (unsigned long long) ptable);
#else
		iowrite32(dev, PT_ADDRESS_REG, (unsigned) ptable);
#endif
		iowrite32(dev, PT_NCHUNK_REG, NCHUNK(mem_size));
		iowrite32(dev, PT_SHIFT_REG, CHUNK_SHIFT);

		// Use the following if input and output data are not allocated at the default offsets
		iowrite32(dev, SRC_OFFSET_REG, 0x0);
		iowrite32(dev, DST_OFFSET_REG, 0x0);

		// Pass accelerator-specific configuration parameters
		/* <<--regs-config-->> */
		iowrite32(dev, DCT_IDCT_INPUT_COLS_REG, input_cols);
		iowrite32(dev, DCT_IDCT_INPUT_ROWS_REG, input_rows);
		iowrite32(dev, DCT_IDCT_DCT_ISTRUE_REG, dct_istrue);
		iowrite32(dev, DCT_IDCT_OUTPUT_ROWS_REG, output_rows);
		iowrite32(dev, DCT_IDCT_OUTPUT_COLS_REG, output_cols);

		// Flush (customize coherence model here)
		esp_flush(ACC_COH_NONE);

		// Start accelerators
		printf("  Start...\n");
		iowrite32(dev, CMD_REG, CMD_MASK_START);

		// Wait for completion
		done = 0;
		while (!done) {
		printf("  Waiting for done...\n");
			done = ioread32(dev, STATUS_REG);
			done &= STATUS_MASK_DONE;
		}
		iowrite32(dev, CMD_REG, 0x0);

		printf("  Done\n");
		printf("  validating...\n");

		/* Validation */
		errors = validate_buf(&mem[out_offset], gold);
		float err_rate=0.01f;
		float num_err =  ((float)errors/(output_rows* output_cols));

		if(  num_err  < err_rate) {
			printf("#################################\n");
			printf("#########  Test PASSED ##########\n");
			printf("## Percent of Elements with error = %f \n", (num_err *100));
			printf("#################################\n");
		}
		else {
			printf("#################################\n");
                	printf("#########  Test FAILED ##########\n");
                	printf("## Percent of Elements with error = %f \n", (num_err * 100));
                	printf("#################################\n");
                }



		aligned_free(ptable);
		aligned_free(mem);
		aligned_free(gold);
	}

	return 0;
}
