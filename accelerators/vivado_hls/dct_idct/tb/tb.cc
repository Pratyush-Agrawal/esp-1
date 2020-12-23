#include "../inc/espacc_config.h"
#include "../inc/espacc.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sstream>
#include <fstream>

int main(int argc, char **argv) {

    printf("*****************START********************\n");
    #if (BASIC==1)
        printf ( " ******** RUNNING BASIC *********** \n");
    #else
		printf( " ********* RUNNING AAN   *********** \n");
    #endif

    /* <<--params-->> */
	const unsigned input_cols = 16;
	const unsigned input_rows = 16;
	const unsigned dct_istrue = 1;
	const unsigned output_rows = 16;
	const unsigned output_cols = 16;

    uint32_t in_words_adj;
    uint32_t out_words_adj;
    uint32_t in_size;
    uint32_t out_size;
    uint32_t dma_in_size;
    uint32_t dma_out_size;
    uint32_t dma_size;


    in_words_adj = round_up(input_rows * input_cols, VALUES_PER_WORD);
    out_words_adj = round_up(output_rows * output_cols, VALUES_PER_WORD);
    in_size = in_words_adj * (1);
    out_size = out_words_adj * (1);

    dma_in_size = in_size / VALUES_PER_WORD;
    dma_out_size = out_size / VALUES_PER_WORD;
    dma_size = dma_in_size + dma_out_size;

    dma_word_t *mem	  =  (dma_word_t*) malloc(dma_size * sizeof(dma_word_t));
    word_t *inbuff	  =  (word_t*) malloc(in_size * sizeof(word_t));
    word_t *outbuff 	  =  (word_t*) malloc(out_size * sizeof(word_t));
    word_t *outbuff_gold  =  (word_t*) malloc(out_size * sizeof(word_t));
    dma_info_t load;
    dma_info_t store;

    // Prepare input data
    std::ifstream f("/home/esp2020/pa2562/esp-fall20/accelerators/vivado_hls/dct_idct/datagen/input.txt");
    std::ifstream fo("/home/esp2020/pa2562/esp-fall20/accelerators/vivado_hls/dct_idct/datagen/output.txt");

    for(unsigned i = 0; i < 1; i++)
        for(unsigned j = 0; j < input_rows * input_cols; j++)
            f>>  inbuff[i * in_words_adj + j] ;

    for(unsigned i = 0; i < dma_in_size; i++)
	    for(unsigned k = 0; k < VALUES_PER_WORD; k++)
	        mem[i].word[k] = inbuff[i * VALUES_PER_WORD + k];

    // Set golden output
    for(unsigned i = 0; i < 1; i++)
        for(unsigned j = 0; j < output_rows * output_cols; j++)
           fo >>  outbuff_gold[i * out_words_adj + j] ;
    


    // Call the TOP function
    top(mem, mem,
        /* <<--args-->> */
	 	 input_cols,
	 	 input_rows,
	 	 dct_istrue,
	 	 output_rows,
	 	 output_cols,
        load, store);

    // Validate
    uint32_t out_offset = dma_in_size;
    for(unsigned i = 0; i < dma_out_size; i++)
	for(unsigned k = 0; k < VALUES_PER_WORD; k++)
	    outbuff[i * VALUES_PER_WORD + k] = mem[out_offset + i].word[k];


    int errors = 0;
    int gold;
    int design;
    float VAL ; 
    float Rate=0.01f ; //1% error
    float error_rate;
    
    for(unsigned j = 0; j < output_rows * output_cols; j++){
		gold=   int(outbuff_gold[ j]);
		design= int(outbuff[j]);
	    std::cout << "GOLDEN VALUE \t  " << gold  << "\t DESIGN VALUE \t " << design << "\t ELEMENT \t " << j << "\n";
		VAL = fabs(gold - design);
	    if (VAL > 2){
		    std::cout << " MISMATCH FOUND : Difference Between Golden value and Design Output is  =  " << (VAL) << "\n";
		    errors++;
        }
	}
    
    error_rate = errors / (output_rows * output_cols);

    if (error_rate > Rate)
	std::cout << "*******TEST FAILED******** with " << errors << " errors.  Elements with error are more than 1% of total elements" << std::endl;
    else {
    std::cout << " \n" ;    
    std::cout << "#*******************************************************************************# \n" ;    
	std::cout << "#******************************** TEST PASSED **********************************# \n" ;
    std::cout << "#*********************** Error Rate is " << error_rate <<" *********************# \n" ;
    std::cout << "#*********** Elements with error are  within 1% of the total elements **********# \n" ;
    std::cout << "#*******************************************************************************# \n" ;
    std::cout << " \n" ; 
    }
    // Free memory

    free(mem);
    free(inbuff);
    free(outbuff);
    free(outbuff_gold);

    return 0;
}
