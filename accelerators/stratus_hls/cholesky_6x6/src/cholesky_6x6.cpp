// Copyright (c) 2011-2019 Columbia University, System Level Design Group
// SPDX-License-Identifier: Apache-2.0

#include "cholesky_6x6.hpp"
#include "cholesky_6x6_directives.hpp"

// Functions

#include "cholesky_6x6_functions.hpp"

// Processes

void cholesky_6x6::load_input()
{

    // Reset
    {
        HLS_PROTO("load-reset");

        this->reset_load_input();

        // explicit PLM ports reset if any

        // User-defined reset code

        wait();
    }

    // Config
    /* <<--params-->> */
    int32_t input_rows;
    int32_t output_rows;
    {
        HLS_PROTO("load-config");

        cfg.wait_for_config(); // config process
        conf_info_t config = this->conf_info.read();

        // User-defined config code
        /* <<--local-params-->> */
        input_rows = config.input_rows;
        output_rows = config.output_rows;
    }

    // Load
    {
        HLS_PROTO("load-dma");
        wait();

        bool ping = true;
        uint32_t offset = 0;
        uint32_t loader = 1;
        uint32_t loader_ping = 1;
        uint32_t loader_pong = 2;
        uint32_t dma_loader=0 ;
 	bool DMA_EVEN;
	sc_dt::sc_bv<DMA_WIDTH> rows_bv(input_rows);
	DMA_EVEN = ( rows_bv.range(0,0)==0) ?	 true : false;

        // Batching
        for (uint16_t b = 0; b < 1; b++)
        {
            wait();
            uint32_t length = input_rows * input_rows;
            // Chunking
            for (int rem = length; rem > 0; rem -= input_rows)
            {
                wait();
                // Configure DMA transaction
                uint32_t len =  input_rows ;
		len = (DMA_WORD_PER_BEAT==2 && DMA_EVEN== false) ? len+1 : len;
                dma_loader = (DMA_WORD_PER_BEAT==2 && DMA_EVEN== false) ? ((ping) ? loader_ping<<1 : loader_pong<<1) : ( (DMA_WORD_PER_BEAT==2 && DMA_EVEN== true) ? loader <<1 : loader); 

                dma_info_t dma_info(offset / DMA_WORD_PER_BEAT, dma_loader / DMA_WORD_PER_BEAT, DMA_SIZE);
                offset += (DMA_WORD_PER_BEAT==2 && DMA_EVEN== false) ? ((ping) ? (len-2) : len) : len;

                this->dma_read_ctrl.put(dma_info);

                for (uint16_t i = 0; i < dma_loader; i += DMA_WORD_PER_BEAT)
                {
                    HLS_BREAK_DEP(plm_in_ping);
                    HLS_BREAK_DEP(plm_in_pong);

                    sc_dt::sc_bv<DMA_WIDTH> dataBv;

                    dataBv = this->dma_read_chnl.get();
                    wait();

                    if(DMA_WORD_PER_BEAT==2 && DMA_EVEN==false) //Only Case of 64B odd DMA alignment
		       {
                         for (uint16_t k = 0; k < DMA_WORD_PER_BEAT; k++)
                             {
                        	HLS_UNROLL_SIMPLE;
                        	if (ping)
				   {
                                     if(i==(len-2) && k==0)
                                      plm_in_ping[i ] = dataBv.range(  DATA_WIDTH - 1, 0).to_int64();
                                     else if( i!=(len-2))
                                        plm_in_ping[i + k] = dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH).to_int64();
                                   }
                                else
				   {
                                    if(i==0 && k==0)
                                      plm_in_pong[i ] = dataBv.range((2) * DATA_WIDTH - 1,  DATA_WIDTH).to_int64();
                                    else if(i!=0)
                                      plm_in_pong[i + k-1] = dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH).to_int64();

                                   }
                             } 
                       }
		    else  // Case of 64B even DMA alignment and 32B Odd/Even Alignemnt
		       {
                    	for (uint16_t k = 0; k < DMA_WORD_PER_BEAT; k++)
                    	    {
                        	HLS_UNROLL_SIMPLE;
                        	if (ping)
                            	   plm_in_ping[i + k] = dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH).to_int64();
                        	else
                            	   plm_in_pong[i + k] = dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH).to_int64();
                             }
               	       }
		}//for(i-dma_loader) 
                loader++;
		if(ping)
		  loader_ping++;
		else
		  loader_pong++;
		this->load_compute_handshake();
                ping = !ping;
            }//for(rem)
        }
    }

    // Conclude
    {
        this->process_done();
    }
}



void cholesky_6x6::store_output()
{
    // Reset
    {
        HLS_PROTO("store-reset");

        this->reset_store_output();

        // explicit PLM ports reset if any

        // User-defined reset code

        wait();
    }

    // Config
    /* <<--params-->> */
    int32_t input_rows;
    int32_t output_rows;
    {
        HLS_PROTO("store-config");

        cfg.wait_for_config(); // config process
        conf_info_t config = this->conf_info.read();

        // User-defined config code
        /* <<--local-params-->> */
        input_rows = config.input_rows;
        output_rows = config.output_rows;
    }

    // Store
    {
        HLS_PROTO("store-dma");
        wait();

        bool ping = true;
	int dma_rem_data=0;
	bool DMA_EVEN;
        sc_dt::sc_bv<DMA_WIDTH> rows_bv(input_rows);
        DMA_EVEN = ( rows_bv.range(0,0)==0) ?    true : false;

        uint32_t store_offset = (input_rows * input_rows) * 1;
        uint32_t offset = (DMA_WORD_PER_BEAT==2 && DMA_EVEN==false) ?  store_offset+1 : store_offset;

        wait();
        // Batching
        for (uint16_t b = 0; b < 1; b++)
        {
            wait();
            uint32_t length = output_rows * output_rows;
            // Chunking
            for (int rem = length; rem > 0; rem -= output_rows)
            {

                this->store_compute_handshake();

                // Configure DMA transaction
                uint32_t len =  output_rows ;
		len = (DMA_WORD_PER_BEAT==2 && DMA_EVEN== false) ? len+1 : len;
#if (DMA_WORD_PER_BEAT == 0)
                // data word is wider than NoC links
                dma_info_t dma_info(offset * DMA_BEAT_PER_WORD, len * DMA_BEAT_PER_WORD, DMA_SIZE);
#else
                dma_info_t dma_info(offset / DMA_WORD_PER_BEAT, len / DMA_WORD_PER_BEAT, DMA_SIZE);
#endif
                offset += (DMA_WORD_PER_BEAT==2 && DMA_EVEN== false) ? ((ping) ? (len-2) : len) : len;

                this->dma_write_ctrl.put(dma_info);

#if (DMA_WORD_PER_BEAT == 0)
                // data word is wider than NoC links
                for (uint16_t i = 0; i < len; i++)
                {
                    // Read from PLM
                    sc_dt::sc_int<DATA_WIDTH> data;
                    wait();
                    if (ping)
                        data = plm_out_ping[i];
                    else
                        data = plm_out_pong[i];
                    sc_dt::sc_bv<DATA_WIDTH> dataBv(data);

                    uint16_t k = 0;
                    for (k = 0; k < DMA_BEAT_PER_WORD - 1; k++)
                    {
                        this->dma_write_chnl.put(dataBv.range((k+1) * DMA_WIDTH - 1, k * DMA_WIDTH));
                        wait();
                    }
                    // Last beat on the bus does not require wait(), which is
                    // placed before accessing the PLM
                    this->dma_write_chnl.put(dataBv.range((k+1) * DMA_WIDTH - 1, k * DMA_WIDTH));
                }
#else
                for (uint16_t i = 0; i < len; i += DMA_WORD_PER_BEAT)
                {
                    sc_dt::sc_bv<DMA_WIDTH> dataBv;

                    // Read from PLM
                    wait();

		 if( DMA_WORD_PER_BEAT==2 && DMA_EVEN==false)
                     {
                         for (uint16_t k = 0; k < DMA_WORD_PER_BEAT; k++)
                             {
                        	HLS_UNROLL_SIMPLE;
                        	if (ping)
				   {
                                      if(i==(len-2) && k!=1 )
					 {
                            		   dataBv.range( DATA_WIDTH - 1,  0) = plm_out_ping[i ];
                                           dma_rem_data =  plm_out_ping[i ];
                                          }
                                      else 
					 {
                                           if(i!=(len-2))
                                           dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH) = plm_out_ping[i + k];
                                	 }


                                   } 
				else
				   {
                                      if(i==0)
                                        {
					  if(k==0)
                            		    dataBv.range(DATA_WIDTH - 1,  0) = dma_rem_data;
                                          else
                            		    dataBv.range((2) * DATA_WIDTH - 1,  DATA_WIDTH) = plm_out_pong[i ];
                                        }
                                      else
                                        dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH) = plm_out_pong[i + k-1];


                                  }
                             }//for(k)
                     }
		 else // !( DMA_WORD_PER_BEAT==2 && DMA_EVEN==false)
		     {
                     	for (uint16_t k = 0; k < DMA_WORD_PER_BEAT; k++)
                    	    {
                                HLS_UNROLL_SIMPLE;
                        	if (ping) 
                                    dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH) = plm_out_ping[i + k]; 
                                else
                                    dataBv.range((k+1) * DATA_WIDTH - 1, k * DATA_WIDTH) = plm_out_pong[i + k];
                             }
		    }
                 this->dma_write_chnl.put(dataBv);
	
                }//for(i)
#endif
                ping = !ping;
            }
        }
    }

    // Conclude
    {
        this->accelerator_done();
        this->process_done();
    }
}


void cholesky_6x6::compute_kernel()
{
    // Reset
    {
        HLS_PROTO("compute-reset");

        this->reset_compute_kernel();

        // explicit PLM ports reset if any

        // User-defined reset code

        wait();
    }

    // Config
    /* <<--params-->> */
    int32_t input_rows;
    int32_t output_rows;
    {
        HLS_PROTO("compute-config");

        cfg.wait_for_config(); // config process
        conf_info_t config = this->conf_info.read();

        // User-defined config code
        /* <<--local-params-->> */
        input_rows = config.input_rows;
        output_rows = config.output_rows;
    }


    // Compute
    	bool ping = true;
	FPDATA s=0;
    	FPDATA index_sqrt=0;
   	int index_sqrt_data=0;
   	int output_data=0;
    	FPDATA plm_in_data=0;
    	FPDATA plm_out_data=0;
    	FPDATA plm_diag_data=0;

    {
        for (uint16_t b = 0; b < 1; b++)
        {
            uint32_t in_length = input_rows * input_rows;
            uint32_t out_length = output_rows * output_rows;
	    int i =0;
            int fill =0;
            int temp=0 ;

            for (int in_rem = in_length; in_rem > 0; in_rem -= input_rows)
            {	 
		 HLS_BREAK_DEP(plm_out_ping);
                 HLS_BREAK_DEP(plm_out_pong);
                 HLS_BREAK_DEP(plm_temp);
                 HLS_BREAK_DEP(plm_diag);
		 temp =0;

                this->compute_load_handshake();

                // Computing phase implementation

		if (ping)
		  {
		   for (int j = 0; j < (i+1); j++)
		   	{ 
			   wait();
			   plm_in_data = int2fp<FPDATA, WORD_SIZE>(plm_in_ping[ j]);
			   plm_diag_data = int2fp<FPDATA, WORD_SIZE>(plm_diag[ j]);
                           s = 0;
                           for (int k = 0; k < j; k++) 
			      {
				wait();
				plm_out_data = int2fp<FPDATA, WORD_SIZE> ( plm_out_ping[k]);
                                if(i!=j)
                                  s += plm_out_data  * int2fp<FPDATA, WORD_SIZE> (plm_temp[temp]);
                                else
                                  s += plm_out_data * plm_out_data;
                                temp++;
                              }
                           if(i == j)
			     {
                               index_sqrt = plm_in_data - s;
                               index_sqrt_data =fp2int<FPDATA, WORD_SIZE> ( sqrt(index_sqrt));
                               plm_out_ping[j] =index_sqrt_data;
                               plm_diag[ j] = index_sqrt_data;
                             }
                           else //(i!=j)
			     {
                                  output_data =  fp2int<FPDATA, WORD_SIZE> ((1.0 /plm_diag_data) *( plm_in_data - s));  
                                  plm_out_ping[j] =  output_data;  
                                  plm_temp[fill] =  output_data;
                               	  fill++;
                             }

                        }//for(j)

		  }

	      else //!ping
	 	  {
                     for (int j = 0; j < (i+1); j++)
                        {
			   wait();
			   plm_in_data = int2fp<FPDATA, WORD_SIZE>(plm_in_pong[ j]);
                           plm_diag_data = int2fp<FPDATA, WORD_SIZE>(plm_diag[ j]);

                           s = 0;
                           for (int k = 0; k < j; k++) 
			      {
				wait();
				plm_out_data = int2fp<FPDATA, WORD_SIZE> ( plm_out_pong[k]);
                                if(i!=j)
                                   s += plm_out_data * int2fp<FPDATA, WORD_SIZE> ( plm_temp[temp]);
                                else
                                   s += plm_out_data * plm_out_data;
                                temp++;
                               }

                           if(i == j)
			      {
                                index_sqrt = plm_in_data - s;
                                index_sqrt_data = fp2int<FPDATA, WORD_SIZE>  (sqrt(index_sqrt));
                                plm_out_pong[j] = index_sqrt_data;
                                plm_diag[ j] = index_sqrt_data;
                              }
                           else
			      {
                                     output_data =  fp2int<FPDATA, WORD_SIZE> ((1.0 /plm_diag_data) * (plm_in_data - s));
                                     plm_out_pong[j] =  output_data;
                                     plm_temp[fill] = output_data;
                                     fill++;
			      }

			}

                  }//else

                for (int z =(i+1) ; z < input_rows ; z++)
		    {
			wait();
                        if(ping)
                          plm_out_ping[z] =0;
                        else
                          plm_out_pong[z] =0;
                    }
		
                this->compute_store_handshake();
                ping = !ping;
		i++;
            }

        }

        // Conclude
        {
            this->process_done();
        }
    }
}
