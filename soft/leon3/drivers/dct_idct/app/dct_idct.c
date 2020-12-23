#include "libesp.h"
#include "cfg.h"
#include "d_idct.h"
#include "dct_ann_big.h"

static unsigned in_words_adj;
static unsigned out_words_adj;
static unsigned in_len;
static unsigned out_len;
static unsigned in_size;
static unsigned out_size;
static unsigned out_offset;
static unsigned size;

/* User-defined code */
static int validate_buffer(token_t *out, token_t *gold)
{
int j;
	unsigned errors_x = 0;
	unsigned errors_1 = 0;
	unsigned errors_2 = 0;
	unsigned errors_3 = 0;
	unsigned errors_4 = 0;
	unsigned errors = 0;
	int golden, design;
	int VAL;


	for( j = 0; j < output_rows * output_cols; j++) {
		golden= (int)(gold[ j]);
		design= (int)(out[j]);
	   	 // printf("GOLDEN VALUE  = %d \t   DESIGN VALUE = %d \t ELEMENT = %d  \n ", golden, design, j );
		VAL = fabs(golden - design);
	    	if (VAL == 1) {
			// printf(" MISMATCH FOUND : Difference Between Golden value and Design Output is 1 \n");
		    	errors_1++;
        	} else if( VAL ==2) {
			// printf(" MISMATCH FOUND : Difference Between Golden value and Design Output is 2 \n");
			errors_2++;
		} else if( VAL==3) {
			// printf(" MISMATCH FOUND : Difference Between Golden value and Design Output is 3 \n");
			errors_3++;
		} else if (VAL >=4) {
			// printf(" MISMATCH FOUND : Difference Between Golden value and Design Output is more than 4  \n");
			errors_4++;
		}
	}
	errors = errors_1 + errors_2 + errors_3 + errors_4;	
	errors_x =  errors_3 + errors_4;	
	printf(" Elements with difference 1 are : %d \n",errors_1);
        printf(" Elements with difference 2 are : %d \n",errors_2);
        printf(" Elements with difference 3 are : %d \n",errors_3);
        printf(" Elements with difference with more than 4 are : %d \n",errors_4);
	printf(" Total Elements with error : %d out of : %d \n" ,  (errors), (output_rows * output_cols));
	printf(" Total Elements with difference more than 2 ::::> %d out of  %d \n" ,  (errors_x), (output_rows * output_cols));

	return errors_x;
}

static void init_buffer(token_t *in)
{
        int j;

                for (j = 0; j < input_rows * input_cols; j++){
			if(dct_istrue==1)
                        	in[j] = (rand() % (256));
			else
				in[j] = (rand() % (200 + 72  + 1)) -72;
		}

}



/* User-defined code */
static void init_parameters()
{
	if (DMA_WORD_PER_BEAT(sizeof(token_t)) == 0) {
		in_words_adj = input_rows * input_cols;
		out_words_adj = output_rows * output_cols;
	} else {
		in_words_adj = round_up(input_rows * input_cols, DMA_WORD_PER_BEAT(sizeof(token_t)));
		out_words_adj = round_up(output_rows * output_cols, DMA_WORD_PER_BEAT(sizeof(token_t)));
	}
	in_len = in_words_adj * (1);
	out_len =  out_words_adj * (1);
	in_size = in_len * sizeof(token_t);
	out_size = out_len * sizeof(token_t);
	out_offset = in_len;
	size = (out_offset * sizeof(token_t)) + out_size;
}


int main(int argc, char **argv)
{
	int errors;

	token_t *gold;
	token_t *buf;

	int*  c_out_basic; 
	c_out_basic = (int*) malloc (output_rows*output_cols* sizeof(int) );
	int*  c_out_aan; 
	c_out_aan = (int*) malloc (output_rows*output_cols* sizeof(int) );

	init_parameters();

	buf = (token_t *) esp_alloc(size);
	cfg_000[0].hw_buf = buf;
    
	gold = malloc(out_size);

	if(dct_istrue==1)
		init_buffer(buf);
	else
		#include "app_data.h"

	printf("\n====== %s ======\n\n", cfg_000[0].devname);
	/* <<--print-params-->> */
	printf("  .input_cols = %d\n", input_cols);
	printf("  .input_rows = %d\n", input_rows);
	printf("  .dct_istrue = %d\n", dct_istrue);
	printf("  .output_rows = %d\n", output_rows);
	printf("  .output_cols = %d\n", output_cols);
	printf("\n  ** START **\n");

	esp_run(cfg_000, NACC);
	
	struct timespec th_start;
        struct timespec th_end;

	gettime(&th_start);
	d_idct(input_rows, input_cols, output_rows, output_cols,buf, c_out_basic, dct_istrue);
	gettime(&th_end);
    	float dct_idct_time = ts_subtract(&th_start, &th_end);
	
	struct timespec th2_start;
    	struct timespec th2_end;

    	gettime(&th2_start);
    	aan_dct(input_rows, input_cols, output_rows, output_cols,buf, c_out_aan, dct_istrue);
    	gettime(&th2_end);
    	float dct_aan_time = ts_subtract(&th2_start, &th2_end);
		
	printf("\n Time for dct_idct C code implementation is = %f \n", dct_idct_time);
	printf("\n Time for AAN C code implementation is = %f \n", dct_aan_time);
	printf("\n  ** DONE **\n");

	if(dct_istrue !=1)
		errors = validate_buffer(&buf[out_offset], gold);
	else
		errors = validate_buffer(&buf[out_offset], c_out_basic);

	free(gold);
	esp_free(buf);
	float err_rate=0.05;
	float num_err =    ( (float) errors/(output_rows* output_cols));

	if (  num_err  < err_rate)
		{
	
		printf("#################################\n");
		printf("#########  Test PASSED ##########\n");
		printf("## Percent of Elements with error = %f \n", (num_err *100));
		printf("#################################\n");
		}
	else
		{
		printf("#################################\n");
                printf("#########  Test FAILED ##########\n");
                printf("## Percent of Elements with error = %f \n", (num_err * 100));
                printf("#################################\n");
                }


	printf("\n====== %s ======\n\n", cfg_000[0].devname);

	return errors;
}
