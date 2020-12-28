#include <stdio.h>
#include <math.h>
#define pi 3.142857 
const int m = 8, n = 8; 
  
void dctTransform(int matrix[], int dctval, int dct[]) 
{ 
    int i, j, k, l; 
    
    float cos_idct[64];
    #include "cos_dct.h"


  
    float ci =0, cj =0, cos1,cos2,  sum; 
  
    for (i = 0; i < 8; i++) 
    { 
        for (j = 0; j < 8; j++)
         { 
  
            if(dctval==1) 
            {
                /* ---Code Optimized
                if (i == 0) 
                    ci = 1 / sqrt(m); 
                else
                    ci = sqrt(2) / sqrt(m); 
                
                if (j == 0) 
                    cj = 1 / sqrt(n); 
                else
                    cj = sqrt(2) / sqrt(n); 
                */

                ci= (i == 0) ? 0.35355339059 : 0.5;
                cj= (j == 0) ? 0.35355339059 : 0.5;
            }
            
            sum = 0; 
            for (k = 0; k < 8; k++) 
                { 
                for (l = 0; l < 8; l++)
                     { 
                        if(dctval!=1)
                            {
                                /* --Code Optimized
                                if (k == 0) 
                                    ci = 1 / sqrt(2.0); 
                                else
                                    ci = 1.0; 
                                if (l == 0) 
                                    cj = 1 / sqrt(2.0); 
                                else
                                    cj = 1.0; 
                                */
                                ci= (k == 0.0) ?  0.70710678118 : 1.0;
                                cj= (l == 0.0) ?  0.70710678118 : 1.0;
                            }

                    /*--Code Optimized
                    if(dctval==1)
                        sum+= matrix[k*m +l] *  cos(  ((2 * k + 1) * i * pi ) / 16   ) * cos(  ((2 * l + 1) * j * pi) /16 ); 
                    else
                        sum += ci*cj* matrix[k*m +l] *  cos((2 * i + 1) * k * pi / (16)) *  cos((2 * j + 1) * l * pi / (16));
                    */
                    cos1 = (dctval==1) ? cos_idct[k * 8 + i]  : cos_idct [i * 8 + k] ;
                    cos2 = (dctval==1) ? cos_idct[l * 8 + j] :  cos_idct [j * 8 + l] ; 

                    if(dctval==1)
                        sum+= matrix[k*m +l] *  cos1 * cos2; 
                    else
                        sum += ci*cj* matrix[k*m +l] *  cos1 *  cos2;
                    
                } 
            }

            if(dctval==1)
                dct[i*m +j] = (int)(ci * cj * sum);
            else
                dct[i*m +j] = (int)(sum /4);
            
             
        } 
    } 
  
} 
  

void  d_idct(int IR, int IC, int OR, int OC, int input_matrix[], int output_matrix[], int dctv) 
{ 

	 
int imatrix[64];
int omatrix[64];
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
                     int ido = x * 8 * OC + y *8 ;

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

                dctTransform(imatrix,dctv, omatrix);     

                for(int i=0 ; i <8 ; i++)
                    {
                        for(int j=0 ; j < 8 ; j++)
                            {
                                global_threadIDx = blockidx * 8 + j;
                                global_threadIDy = blockidy * 8 + i;

                                    if( global_threadIDy < OR && global_threadIDx < OC)
                                       output_matrix[i*OC +j+ido]=  omatrix[i*8 + j];  
                                    
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
                    
  

} 
