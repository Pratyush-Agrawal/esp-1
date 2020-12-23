#include <stdio.h>
#include <math.h>
#define pi 3.142857 
#include "dct_ann.h" 
#include "idct.h" 
  
  

int  aan_dct(int IR, int IC, int OR, int OC, int input_matrix[],int output_matrix[], int dct) 
{ 


int imatrix[64];
int srow= ((IR-1)/8) +1;
int scol = ((IC-1)/8)+1;

int y_bound = ((IC-1)/8);
int x= 0;
int y =0;

        for(int blockidy=0; blockidy < srow ; blockidy++)
           {    
               for(int blockidx=0; blockidx < scol ; blockidx++)
               {

                     int global_threadIDx;
                     int global_threadIDy;
                     int id = x * 8 * IC + y *8 ;

                for(int i=0 ; i <8 ; i++)
                    {
                        for(int j=0 ; j < 8 ; j++)
                            {
                                global_threadIDx = blockidx * 8 + j;
                                global_threadIDy = blockidy * 8 + i;

                                    if( global_threadIDy < IR && global_threadIDx < IC)
                                        imatrix[i*8 + j] = input_matrix[i*IC +j+id];
                                    else
                                        imatrix[i*8 + j] = 0;
                            }
                    }
		
		if(dct==1)
 	        	aan_dct_int(imatrix);     
		else
			aan_idct(imatrix);

                for(int i=0 ; i <8 ; i++)
                    {
                        for(int j=0 ; j < 8 ; j++)
                            {
                                global_threadIDx = blockidx * 8 + j;
                                global_threadIDy = blockidy * 8 + i;

                                    if( global_threadIDy < OR && global_threadIDx < OC)
                                       output_matrix[i*OC +j+id]=  imatrix[i*8 + j];  
                                    
                            }
                    }
                  
               if(y==y_bound)
                   {
                        y=0;
                        x++;
                   }
                 else y++;
               
               }
           }
                    
  

    return 0; 
} 
