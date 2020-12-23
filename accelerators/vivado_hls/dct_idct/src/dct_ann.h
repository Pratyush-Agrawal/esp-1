  

void dct_ann(FPDATA i[] , word_t o[])

 {
  

  //constant//
  FPDATA a1 = 0.707106;   // sqrt(.5);
  FPDATA a2 = 0.541196;   // sqrt(2.) * cos(3./16. * 2 * M_PI);
  FPDATA a3 = a1;
  FPDATA a4 = 1.30656;    //sqrt(2.) * cos(1./16. * 2 * M_PI);
  FPDATA a5 = 0.382683;   // cos(3./16. * 2 * M_PI);
  
  //Scaling Factor//
  FPDATA s0 = 0.353553 ;  //(cos(0)*sqrt(.5)/2)/(1       );
  FPDATA s1 = 0.254898;   //(cos(1.*M_PI/16)/2)/(-a5+a4+1);
  FPDATA s2 = 0.270598;   //(cos(2.*M_PI/16)/2)/(a1+1    );
  FPDATA s3 = 0.300672;   //(cos(3.*M_PI/16)/2)/(a5+1    );
  FPDATA s4 = s0;
  FPDATA s5 = 0.449988;   //(cos(5.*M_PI/16)/2)/(1-a5    );
  FPDATA s6 = 0.653281;   //(cos(6.*M_PI/16)/2)/(1-a1    );
  FPDATA s7 = 1.28146;    //(cos(7.*M_PI/16)/2)/(a5-a4+1 );

  FPDATA dct_mem[64];
 
  ap_uint<6> index0;
  ap_uint<6> index1;
  ap_uint<6> index2;
  ap_uint<6> index3;
  ap_uint<6> index4;
  ap_uint<6> index5;
  ap_uint<6> index6;
  ap_uint<6> index7;

#pragma HLS array_partition variable=dct_mem block factor=8  

  dct_ann_col: for(int a=0; a<8 ; a++)
    {

    #pragma HLS PIPELINE  
    index0 = a;
    index1 = 8+a;
    index2 = 16+a;
    index3 = 24+a;
    index4 = 32+a;
    index5 = 40+a;
    index6 = 48+a;
    index7 = 56+a;
    
    FPDATA ind0 = i[index0] ;
    FPDATA ind1 = i[index1] ;
    FPDATA ind2 = i[index2] ;
    FPDATA ind3 = i[index3] ;
    FPDATA ind4 = i[index4] ;
    FPDATA ind5 = i[index5] ;
    FPDATA ind6 = i[index6] ;
    FPDATA ind7 = i[index7] ;

    FPDATA b0 = ind0 + ind7;
    FPDATA b1 = ind1 + ind6; 
    FPDATA b2 = ind2 + ind5;
    FPDATA b3 = ind3 + ind4;
    FPDATA b4 =-ind4 + ind3;
    FPDATA b5 =-ind5 + ind2;
    FPDATA b6 =-ind6 + ind1;
    FPDATA b7 =-ind7 + ind0;

    FPDATA c0 = b0 + b3;
    FPDATA c1 = b1 + b2;
    FPDATA c2 =-b2 + b1;
    FPDATA c3 =-b3 + b0;
    FPDATA c4 =-b4 - b5;
    FPDATA c5 = b5 + b6;
    FPDATA c6 = b6 + b7;
    FPDATA c7 = b7;

    FPDATA d0 = c0 + c1;
    FPDATA d1 =-c1 + c0;
    FPDATA d2 = c2 + c3;
    FPDATA d3 = c3;
    FPDATA d4 = c4;
    FPDATA d5 = c5;
    FPDATA d6 = c6;
    FPDATA d7 = c7;

    FPDATA d8 = (d4 + d6) * a5;

    FPDATA e0 = d0;
    FPDATA e1 = d1;
    FPDATA e2 = d2 * a1;
    FPDATA e3 = d3;
    FPDATA e4 = -d4 * a2 - d8;
    FPDATA e5 = d5 * a3;
    FPDATA e6 = d6 * a4 - d8;
    FPDATA e7 = d7;

    FPDATA f0 = e0;
    FPDATA f1 = e1;
    FPDATA f2 = e2 + e3;
    FPDATA f3 = e3 - e2;
    FPDATA f4 = e4;
    FPDATA f5 = e5 + e7;
    FPDATA f6 = e6;
    FPDATA f7 = e7 - e5;

    FPDATA g0 = f0;
    FPDATA g1 = f1;
    FPDATA g2 = f2;
    FPDATA g3 = f3;
    FPDATA g4 = f4 + f7;
    FPDATA g5 = f5 + f6;
    FPDATA g6 = -f6 + f5;
    FPDATA g7 = f7 - f4;


    dct_mem[index0] = (g0 * s0);
    dct_mem[index4] = (g1 * s4);
    dct_mem[index2] = (g2 * s2);
    dct_mem[index6] = (g3 * s6);
    dct_mem[index5] = (g4 * s5);
    dct_mem[index1] = (g5 * s1);
    dct_mem[index7] = (g6 * s7);
    dct_mem[index3] = (g7 * s3);
  }

  dct_ann_row: for(int a=0; a<8 ; a++)
    {

    #pragma HLS PIPELINE 

    ap_uint<6> sh = a<<3;

    FPDATA ind0 = dct_mem[sh];
    FPDATA ind1 = dct_mem[sh +1];
    FPDATA ind2 = dct_mem[sh +2];
    FPDATA ind3 = dct_mem[sh +3];
    FPDATA ind4 = dct_mem[sh +4];
    FPDATA ind5 = dct_mem[sh +5];
    FPDATA ind6 = dct_mem[sh +6];
    FPDATA ind7 = dct_mem[sh +7];




    FPDATA b0 = ind0 + ind7;
    FPDATA b1 = ind1 + ind6;
    FPDATA b2 = ind2 + ind5;
    FPDATA b3 = ind3 + ind4;
    FPDATA b4 =-ind4 + ind3;
    FPDATA b5 =-ind5 + ind2;
    FPDATA b6 =-ind6 + ind1;
    FPDATA b7 =-ind7 + ind0;

    FPDATA c0 = b0 + b3;
    FPDATA c1 = b1 + b2;
    FPDATA c2 =-b2 + b1;
    FPDATA c3 =-b3 + b0;
    FPDATA c4 =-b4 - b5;
    FPDATA c5 = b5 + b6;
    FPDATA c6 = b6 + b7;
    FPDATA c7 = b7;

    FPDATA d0 = c0 + c1;
    FPDATA d1 =-c1 + c0;
    FPDATA d2 = c2 + c3;
    FPDATA d3 = c3;
    FPDATA d4 = c4;
    FPDATA d5 = c5;
    FPDATA d6 = c6;
    FPDATA d7 = c7;

    FPDATA d8 = (d4 + d6) * a5;

    FPDATA e0 = d0;
    FPDATA e1 = d1;
    FPDATA e2 = d2 * a1;
    FPDATA e3 = d3;
    FPDATA e4 = -d4 * a2 - d8;
    FPDATA e5 = d5 * a3;
    FPDATA e6 = d6 * a4 - d8;
    FPDATA e7 = d7;

    FPDATA f0 = e0;
    FPDATA f1 = e1;
    FPDATA f2 = e2 + e3;
    FPDATA f3 = e3 - e2;
    FPDATA f4 = e4;
    FPDATA f5 = e5 + e7;
    FPDATA f6 = e6;
    FPDATA f7 = e7 - e5;

    FPDATA g0 = f0;
    FPDATA g1 = f1;
    FPDATA g2 = f2;
    FPDATA g3 = f3;
    FPDATA g4 = f4 + f7;
    FPDATA g5 = f5 + f6;
    FPDATA g6 = -f6 + f5;
    FPDATA g7 = f7 - f4;



    o[sh+0] = (g0 * s0).to_int();
    o[sh+4] = (g1 * s4).to_int();
    o[sh+2] = (g2 * s2).to_int();
    o[sh+6] = (g3 * s6).to_int();
    o[sh+5] = (g4 * s5).to_int();
    o[sh+1] = (g5 * s1).to_int();
    o[sh+7] = (g6 * s7).to_int();
    o[sh+3] = (g7 * s3).to_int();
  }


}
