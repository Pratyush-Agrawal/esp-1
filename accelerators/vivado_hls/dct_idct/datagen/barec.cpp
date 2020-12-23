#include <sstream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int  main(int argc, char**argv) {



ifstream fo("/home/esp2020/pa2562/esp-fall20_cpy/accelerators/vivado_hls/idct/datagen/output.txt");
ifstream f("/home/esp2020/pa2562/esp-fall20_cpy/accelerators/vivado_hls/idct/datagen/input.txt");


int irow= atoi(argv[1]);
int orow= atoi(argv[2]);
int icol= atoi(argv[3]);
int ocol= atoi(argv[4]);

int inbuff[irow*icol];
int outbuff_gold[orow*ocol];


for(unsigned i = 0; i < 1; i++)
        for(unsigned j = 0; j < irow * icol; j++)
          f>>  inbuff[ j] ;

for (int num=0 ; num < irow * icol ; num++)
        cout<< "mem [" << num << " ] = "  << inbuff[num] << "; \n";


for(unsigned i = 0; i < 1; i++)
        for(unsigned j = 0; j < orow * ocol; j++)
                fo >>  outbuff_gold[ j] ;
   
for (int num=0 ; num < orow * ocol ; num++)
        cout<< " gold [  " << num << " ] = "  <<outbuff_gold[num] << " ; \n";
   
return 0;

} 
