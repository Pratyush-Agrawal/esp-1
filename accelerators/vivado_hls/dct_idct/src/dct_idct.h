#include "../inc/espacc.h"


void dct_idct( FPDATA  _inbuff[SIZE_IN_CHUNK_DATA], const unsigned col, const unsigned row, const unsigned dct_istrue, word_t _outbuff[SIZE_OUT_CHUNK_DATA])
{

   int m= row;
   int n =col;

   FPDATA cos_idct[64];
   #include "cos_dct.h"
   FPDATA mult, dct_ci_cj;

   FPDATA ci, cj,  theta1, theta2,theta1_div16_mult_pi, theta2_div16_mult_pi, cos1 ,cos2, sum_exp, sum_extra, cos_product, mult_product;

   FPDATA pi= 3.142857;

   #if(FAST==1)
   FPDATA temp1, temp2 , temp3,temp4, temp5, temp6 ;
   FPDATA sum[64]; FPDATA sum1[32]; FPDATA sum2[16]; FPDATA sum3[8]; 
   #endif   

   FPDATA sum6;
   #pragma HLS loop_merge

   dct_idct_loop_Y:for (int y = 0; y < 8; y++)
      {
                  
         dct_idct_loop_X:for(int x =0 ; x < 8; x++)
            {
              #if(FAST==1) 
               #pragma HLS PIPELINE
              #endif 

               #if(FAST==0)
                  if(dct_istrue==1)
                     {
                        #if(LIB_FUNCTIONS==0)
                           ci= (y == 0) ? 0.35355339059 : 0.5;
                           cj= (x == 0) ? 0.35355339059 : 0.5;
                        #else
                           ci= (y == 0) ? 1 / sqrt(m) : sqrt(2) / sqrt(m);
                           cj= (x == 0) ? 1 / sqrt(n) : sqrt(2) / sqrt(n);
                        #endif
                     }
               #else
                  dct_ci_cj = (y==0 && x==0) ? 0.124999999999 :  (((y!=0 && x==0) ||(y==0 && x!=0))  ?   0.17677669529 : 0.25) ;
               #endif

               sum6=0;      

               dct_idct_loop_V:for (int v = 0; v < 8; v++)
                  {
                     dct_idct_loop_U:for(int u =0 ; u < 8 ; u++)
                        {
                           #pragma HLS LATENCY max=2 min=0
                           #pragma HLS loop_flatten
                           
                           #if(FAST==0) 
                              if(dct_istrue!=1)
                                 {
                                    #if(LIB_FUNCTIONS==0)
                                       ci= (v == 0.0) ?  0.70710678118 : 1.0;
                                       cj= (u == 0.0) ?  0.70710678118 : 1.0;
                                    #else
                                       ci= (v == 0.0) ?  1 / sqrt(2.0) : 1.0;
                                       cj= (u == 0.0) ?  1 / sqrt(2.0) : 1.0;
                                    #endif
                                 }
                           #endif  

                           if(dct_istrue==1)
                              mult = 1;
                           else
                              {
                                 #if(FAST==0)
                                    mult = ci* cj;
                                 #else
                                    mult = (v==0 && u==0) ? 0.4999999999 :  (((v!=0 && u==0) ||(v==0 && u!=0))  ?   0.70710678118 : 1.0) ;
                                 #endif   
                              }
                                       
                           #if(LIB_FUNCTIONS==0)
                              cos1 = (dct_istrue==1) ? cos_idct[v * 8 + y]  : cos_idct [y * 8 + v] ;
                              cos2 = (dct_istrue==1) ? cos_idct[u * 8 + x] :  cos_idct [x * 8 + u] ;
                                       
                           #else
                              theta1= (dct_istrue==1) ? (2 * v + 1) * y  : (2 * y + 1) * v  ;
                              theta2 =(dct_istrue==1) ? (2 * u + 1) * x  : (2 * x + 1) * u  ;
                              theta1_div16_mult_pi = (theta1 * pi) >> 4 ;
                              theta2_div16_mult_pi = (theta2 * pi) >> 4 ;
                              cos1 = hls::cos(theta1_div16_mult_pi);
                              cos2 = hls::cos(theta2_div16_mult_pi);
                           #endif
                           
                           cos_product = cos1 * cos2;
                           mult_product = mult *  _inbuff[(v<<3) +u];

                           #if(FAST==1)
                              sum[(v<<3) + u ]=  cos_product * mult_product  ;
                           #else
                              sum6+=  cos_product * mult_product  ;
                           #endif


                                       
                        }//U
                  }//V

               #if(FAST==1)
                  reduction_loop_32: for(int q=0; q<32; q++)
                              sum1[q] = sum[q] + sum[q + 32];

                  reduction_loop_16: for(int q=0; q<16; q++)
                              sum2[q] = sum1[q] + sum1[q + 16];

                  reduction_loop_8: for(int q=0; q<8; q++)
                              sum3[q] = sum2[q] + sum2[q + 8];


                  temp1 = sum3[0] + sum3[1] ;
                  temp2 = sum3[2] + sum3[3] ; 
                  temp3 = sum3[4] + sum3[5] ;
                  temp4 = sum3[6] + sum3[7] ;
                  temp5 = temp1   + temp2;
                  temp6=  temp3   + temp4;
                  sum6 =  temp5   + temp6;
               #endif

               #if(FAST==0)
                  _outbuff[(y<<3) + x] = (dct_istrue==1) ? (int)( ci * cj * sum6) : (int)(sum6>>2) ;
               #else
                  _outbuff[(y<<3) + x] = (dct_istrue==1) ? (int)( dct_ci_cj * sum6) : (int)(sum6>>2) ;
               #endif 
               


             }//X
      }//Y

}//End--of--Function


