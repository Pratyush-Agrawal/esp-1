#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std; 
#define pi 3.142857 
const int m = 8, n = 8; 
  
int dctTransform(int matrix[], int dctval, int dct[]) 
{ 
    int i, j, k, l; 
  


  
    float ci, cj, dct1, sum; 
  
    for (i = 0; i < m; i++) { 
        for (j = 0; j < n; j++) { 
  
            if(dctval==1) {
            if (i == 0) 
                ci = 1 / sqrt(m); 
            else
                ci = sqrt(2) / sqrt(m); 
            if (j == 0) 
                cj = 1 / sqrt(n); 
            else
                cj = sqrt(2) / sqrt(n); 
  
            }
            sum = 0; 
            for (k = 0; k < m; k++) { 
                for (l = 0; l < n; l++) { 
                        if(dctval!=1){
                            if (k == 0) 
                        ci = 1 / sqrt(2.0); 
                     else
                        ci = 1.0; 
                     if (l == 0) 
                        cj = 1 / sqrt(2.0); 
                     else
                        cj = 1.0; 
                        }
                    if(dctval==1)
                    sum+= matrix[k*m +l] *  cos((2 * k + 1) * i * pi / (2 * m)) * cos((2 * l + 1) * j * pi / (2 * n)); 
                    else
                    sum += ci*cj* matrix[k*m +l] *  cos((2 * i + 1) * k * pi / (2 * m)) *  cos((2 * j + 1) * l * pi / (2 * n)); 
                    
                } 
            } 
            if(dctval==1)
            dct[i*m +j] = (int)(ci * cj * sum);
            else
            {
                 sum = sum /4.0;
                 dct[i*m +j] = (sum);
            }
             
        } 
    } 
  
} 
  

int main() 
{ 

    int IR,IC;
    int OR,OC;

	IR=8;
	IC=8;
	OR=8;
	OC=8;
	 
int input_matrix [IR *IC];
	 
int dctv=0;

#if(dctv==1)
	#include "pv_data.h"
#else
	#include "pv_data_out.h"
#endif

int output_matrix[OR*OC];
int imatrix[64];
int omatrix[64];
int srow= ((IR-1)/8) +1;
int scol = ((IC-1)/8)+1;

int nblk= srow * scol;
int y_bound = ((IC-1)/8);
int x_bound = ((IR-1)/8);
int x= 0;
int y =0;

int i,j;
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

                dctTransform(imatrix,dctv, omatrix);     

                for(int i=0 ; i <8 ; i++)
                    {
                        for(int j=0 ; j < 8 ; j++)
                            {
                                global_threadIDx = blockidx * 8 + j;
                                global_threadIDy = blockidy * 8 + i;

                                    if( global_threadIDy < OR && global_threadIDx < OC)
                                       output_matrix[i*OC +j+id]=  omatrix[i*8 + j];  
                                    
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
                    
  

 for (i = 0; i < OR; i++) {
        for (j = 0; j < OC; j++) {

            printf("%d\t", output_matrix[i*OC +j]);
        }
        printf("\n");
    }


    return 0; 
} 
