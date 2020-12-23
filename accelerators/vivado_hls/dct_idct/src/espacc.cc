#include "../inc/espacc_config.h"
#include "../inc/espacc.h"
#include "hls_stream.h"
#include "hls_math.h"
#include <cstring>

#if (BASIC==1)
  #include "dct_idct.h"
#else
  #include "dct_ann.h"
  #include "idct_ann.h"
#endif





void load(FPDATA _inbuff[SIZE_IN_CHUNK_DATA], dma_word_t *in1,
          /* <<--compute-params-->> */
   const unsigned input_cols,
	 const unsigned input_rows,
	 const unsigned dct_istrue,
	 const unsigned output_rows,
	 const unsigned output_cols,
      
	  dma_info_t &load_ctrl, int blockidy, int blockidx , unsigned int load_offset)
{
load_data:


	unsigned int row = input_rows;
	unsigned int col = input_cols;
 	unsigned int shift_right = VALUES_PER_WORD -1;
	unsigned int dma_index;
	unsigned int dma_length = 8 >> shift_right;
  	unsigned int chunk =  (blockidy  * input_cols +  blockidx ) <<3 ;
  	unsigned int get_data;
  	unsigned int put_data;
  	unsigned int col_shift = col >> shift_right;

  #pragma HLS loop_merge

            load_loop1: for(unsigned int ii =0 ; ii < 8; ii++)
              {
          
                 load_loop2: for(unsigned int jj =0 ; jj < 8 ; jj+=VALUES_PER_WORD)
                    {
                      #pragma HLS LATENCY max=2 min=0
                      #pragma HLS loop_flatten
                      
                      if(jj==0)
                        {                        
                          dma_index = (chunk + (ii * col)) >> shift_right ;
                          load_ctrl.index = dma_index;
                          load_ctrl.length = dma_length;
                          load_ctrl.size = SIZE_WORD_T;
                        }
                      get_data = dma_index + (jj>>shift_right);
                      put_data = ii*8 + jj ;
                      int global_blockIDx = ((blockidx <<3) + jj )>>shift_right;

                      load_label0:for(unsigned j = 0; j < VALUES_PER_WORD; j++)
                                  {
                                    if( (dma_index < load_offset) && (global_blockIDx < col_shift) )
                                        _inbuff[ put_data + j] =  in1[get_data].word[j];
                                    else
                                        _inbuff[ put_data + j] = 0;
                                      
                                  }
                     }//end-of-load_loop2

              }//end-of-load_loop1
/*
printf("LOAD ********* \n");      
for(int i=0; i < 8 ; i++) {
	for (int j=0; j < 8 ; j++) {
		printf(" %d \t", (int) _inbuff[ i * 8 + j] );
	}
	printf("\n");
 }
*/
}//END--LOAD

void store(word_t _outbuff[SIZE_OUT_CHUNK_DATA], dma_word_t *out,
          /* <<--compute-params-->> */
	 const unsigned input_cols,
	 const unsigned input_rows,
	 const unsigned dct_istrue,
	 const unsigned output_rows,
	 const unsigned output_cols,
  
	 dma_info_t &store_ctrl, int blockidy, int blockidx, unsigned int store_offset, unsigned int dma_bound)
{
store_data:


  	unsigned int row = output_rows;
  	unsigned int col=  output_cols;
	unsigned int dma_index;
  	unsigned int global_blockIDx;
  	unsigned int shift_right = VALUES_PER_WORD -1;
  	unsigned int dma_length = 8 >> (shift_right);
  	unsigned int chunk =  (blockidy  * input_cols +  blockidx ) <<3 ;
  	unsigned int get_data, put_data;

  #pragma HLS loop_merge

 	//printf(" DMA BOUND IS = %d \n", dma_bound); 
 	//printf(" STORE OFFSET IS = %d \n", store_offset); 
    store_loop1:for (unsigned ii = 0; ii < 8; ii++)
        {
             #pragma HLS unroll
            store_loop2:for(unsigned int jj =0 ; jj <8; jj+=VALUES_PER_WORD)
              {

                    #pragma HLS loop_flatten
                    get_data = ii*8 + jj;

                    if(jj==0)
                    {
                      dma_index = (chunk +(store_offset + (ii *  col ))) >> shift_right ;
		      printf(" DMA INDEX = %d \n", dma_index);                         
                      store_ctrl.index = dma_index;
                      store_ctrl.length = dma_length;
                      store_ctrl.size = SIZE_WORD_T;
                    }
                    put_data = dma_index + (jj >> shift_right);
		    //printf(" PUT DATA is %d \n", put_data);
                    global_blockIDx = ((blockidx<<3) + jj)>>shift_right;
		    //printf(" GLOBAL BLOCIDX = %d and COL_SHIFT RIGTH = %d \n ", global_blockIDx , (col>>shift_right));

                   	store_label1: for(unsigned j = 0; j < VALUES_PER_WORD; j++) {
                                        if( dma_index < (dma_bound) && global_blockIDx < (col>>shift_right))                
	                    		        out[put_data].word[j] = _outbuff[get_data+ j];
                          	      }
              }
        }

/*
printf("STORE******* \n");
for(int i=0; i < 8 ; i++) {
        for (int j=0; j < 8 ; j++) {
                printf(" %d \t", (int) _outbuff[ i * 8 + j] );
        }
        printf("\n");
 }
*/

}//END--OF--STORE




void compute(FPDATA _inbuff[SIZE_IN_CHUNK_DATA],
             const unsigned dct_istrue,
             word_t _outbuff[SIZE_OUT_CHUNK_DATA])
{


 printf(" COMPUTE IN ******** \n");
 for(int i=0; i < 8 ; i++) {
         for (int j=0; j < 8 ; j++) {
                    printf(" %d \t", (int) _inbuff[ i * 8 + j] );
                                 }
               printf("\n");
                                 }



			
		    #if (BASIC==1) 
            
              dct_idct(_inbuff,  8,  8,  dct_istrue, _outbuff);


		    #else
              #if (FAST==1)
                  #pragma HLS PIPELINE
                  FPDATA dct_input_mem[64];
                  FPDATA idct_input_mem[64];
                  word_t dct_output_mem[64];
                  word_t idct_output_mem[64];

                  #pragma HLS array_partition variable=dct_input_mem block factor=16 
                  #pragma HLS array_partition variable=idct_input_mem block factor=16 
                  #pragma HLS array_partition variable=dct_output_mem block factor=32
                  #pragma HLS array_partition variable=idct_output_mem block factor=32
                  FPDATA xfer1;

                  compute_input_copy_loop: for (int i =0; i < 64 ; i++) 
                                          {
                                            xfer1 = _inbuff[i];
                                            dct_input_mem[i]= xfer1;
                                            idct_input_mem[i]= xfer1;
                                          }

                          
                  dct_ann( dct_input_mem , dct_output_mem);
                  idct_ann( idct_input_mem , idct_output_mem);
                  
                  compute_output_copy_loop: for (int j =0; j < 64 ; j++)
                                          {
                                            _outbuff[j]= (dct_istrue==1) ? dct_output_mem[j] : idct_output_mem[j] ;
                                          }

              #else
                  if(dct_istrue==1)
                      dct_ann( _inbuff ,_outbuff);
                  else
                      idct_ann( _inbuff , _outbuff);
              #endif
		    #endif



 printf(" COMPUTE  OUT ******** \n");
 for(int i=0; i < 8 ; i++) {
         for (int j=0; j < 8 ; j++) {
                    printf(" %d \t", (int) _outbuff[ i * 8 + j] );
                                 }
               printf("\n");
                                 }


		
}//End--Of--Compute

/**************************************/
/********* MAIN TOP FUNCTION **********/
/*************************************/

void top(dma_word_t *out, dma_word_t *in1,
         /* <<--params-->> */
	 const unsigned conf_info_input_cols,
	 const unsigned conf_info_input_rows,
	 const unsigned conf_info_dct_istrue,
	 const unsigned conf_info_output_rows,
	 const unsigned conf_info_output_cols,
	 dma_info_t &load_ctrl, dma_info_t &store_ctrl)
{

    int blkx =  (((conf_info_input_cols- 1)>>3) +1);
    int blky =  (((conf_info_input_rows- 1)>>3) +1);

    const unsigned load_offset  = (conf_info_input_rows * conf_info_input_cols) >> (VALUES_PER_WORD -1);
    //const unsigned store_offset = (conf_info_output_rows * conf_info_output_cols) ;
    const unsigned  abc= (conf_info_output_rows * conf_info_output_cols) >> (VALUES_PER_WORD-1) ;
    const unsigned store_offset = (conf_info_input_rows * conf_info_input_cols) ;
    const unsigned store_offset_div = store_offset >> (VALUES_PER_WORD -1) ;
    //const unsigned dma_bound    = store_offset_div +load_offset;
    const unsigned dma_bound    = store_offset_div + abc ;

	

  #pragma HLS DATAFLOW
  #pragma HLS loop_merge
  


    // Batching of 8x8 Blocks
batching_Y:for (unsigned block_y = 0; block_y < blky ; block_y++)
    {
		
     batching_X:for (unsigned block_x = 0; block_x < blkx ; block_x++)
            {
              #pragma HLS loop_flatten

	              FPDATA _inbuff[SIZE_IN_CHUNK_DATA];
                #pragma HLS array_partition variable=_inbuff cyclic factor=32
                word_t _outbuff[SIZE_OUT_CHUNK_DATA];
                #pragma HLS array_partition variable=_outbuff  cyclic factor=32 

                load (
                      _inbuff,
                      in1,
                      conf_info_input_cols,
                      conf_info_input_rows,
                      conf_info_dct_istrue,
                      conf_info_output_rows,
                      conf_info_output_cols,
                      load_ctrl, block_y,
                      block_x,
                      load_offset
                     );

                compute (
                         _inbuff,
                          conf_info_dct_istrue,
                         _outbuff
                        );

                store (
                       _outbuff,
                        out,
                        conf_info_input_cols,
                        conf_info_input_rows,	 
                        conf_info_dct_istrue, 
                        conf_info_output_rows, 
                        conf_info_output_cols, 
                        store_ctrl, 
                        block_y, 
                        block_x , 
                        store_offset, 
                        dma_bound
                      );




          }//End--BATCH-X
    }//End--BATCH-Y  
}//End--TOP
