  

void idct_ann( FPDATA i[], word_t o[])

 {
  

    FPDATA m0 = 1.84764  ;     // 2.0 * cos(1.0/16.0 *2.0 * pi);
    FPDATA m1 = 1.41377  ;     // 2.0 * cos(2.0/16.0 * 2.0 * pi);
    FPDATA m3 = 1.41377  ;     // 2.0 * cos(2.0/16.0 * 2.0 * pi); 
    FPDATA m5 = 0.764491 ;     //2.0 * cos(3.0/16.0 * 2.0 * pi);
    FPDATA m2 = 1.08315  ;     // m0 -m5;
    FPDATA m4 = 2.61213  ;     //m0 +m5;

    FPDATA s0 = 0.353553  ;    //cos(0.0/16 * pi)/ sqrt(8);
    FPDATA s1 = 0.490385  ;    //(cos(1.0/16 * pi)/2.0);
    FPDATA s2 = 0.46191   ;    //(cos(2.0/16 * pi)/2.0);
    FPDATA s3 = 0.415669  ;    //(cos(3.0/16 * pi)/2.0);
    FPDATA s4 = 0.353442  ;    //(cos(4.0/16 * pi)/2.0);
    FPDATA s5 = 0.277621  ;    //(cos(5.0/16 * pi)/2.0);
    FPDATA s6 = 0.191123  ;    //(cos(6.0/16 * pi)/2.0);
    FPDATA s7 = 0.0972739 ;    //(cos(7.0/16 * pi)/2.0);


    FPDATA idct_mem[64];
    FPDATA temp_mem[8];
    ap_uint<6> index0;


    ap_uint<6> index1;
    ap_uint<6> index2;
    ap_uint<6> index3;
    ap_uint<6> index4;
    ap_uint<6> index5;
    ap_uint<6> index6;
    ap_uint<6> index7;

    #pragma HLS array_partition variable=idct_mem block factor=8 
    

  idct_ann_col: for(int a=0; a<8 ; a++)
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


      FPDATA g0 = i[index0] * s0  ;
      FPDATA g1 = i[index4] * s4 ;
      FPDATA g2 = i[index2] * s2;
      FPDATA g3 = i[index6] * s6;
      FPDATA g4 =i[index5] * s5;
      FPDATA g5 =i[index1] * s1;
      FPDATA g6 =i[index7] * s7;
      FPDATA g7 =i[index3] * s3;

      FPDATA f0 = g0;
      FPDATA f1 = g1;
      FPDATA f2 = g2;
      FPDATA f3 = g3;
      FPDATA f4 = g4 - g7;
      FPDATA f5 = g5 + g6;
      FPDATA f6 = g5 - g6;
      FPDATA f7 = g4 + g7;

      FPDATA e0 = f0;
      FPDATA e1 = f1;
      FPDATA e2 = f2 - f3;
      FPDATA e3 = f2 + f3;
      FPDATA e4 = f4;
      FPDATA e5 = f5 - f7;
      FPDATA e6 = f6;
      FPDATA e7 = f5 + f7;
      FPDATA e8 = f4 + f6 ;

      FPDATA d0 = e0;
      FPDATA d1 = e1;
      FPDATA d2 = e2 * m1;
      FPDATA d3 = e3;
      FPDATA d4 = e4 * m2;
      FPDATA d5 = e5 * m3;
      FPDATA d6 = e6 * m4;
      FPDATA d7 = e7;
      FPDATA d8 = e8 * m5;

      FPDATA c0 = d0 + d1;
      FPDATA c1 = d0 - d1;
      FPDATA c2 = d2 - d3;
      FPDATA c3 = d3;
      FPDATA c4 = d4 + d8;
      FPDATA c5 = d5 + d7;
      FPDATA c6 = d6 - d8;
      FPDATA c7 = d7;
      FPDATA c8 = c5 -c6;


      FPDATA b0 = c0 + c3;
      FPDATA b1 = c1 + c2 ;
      FPDATA b2 = c1 - c2;
      FPDATA b3 = c0 - c3;
      FPDATA b4 = c4 - c8;
      FPDATA b5 = c8;
      FPDATA b6 = c6 - c7;
      FPDATA b7 = c7;



    idct_mem[index0] = (b0 + b7);
    idct_mem[index1] = (b1 + b6);
    idct_mem[index2] = (b2 + b5);
    idct_mem[index3] = (b3 + b4);
    idct_mem[index4] = (b3 - b4);
    idct_mem[index5] = (b2 - b5);
    idct_mem[index6] = (b1 - b6);
    idct_mem[index7] = (b0 - b7);


  }



  idct_ann_row: for(int a=0; a<8 ; a++)
    {
      #pragma HLS PIPELINE  

      ap_uint<6> sh = a<<3;

      FPDATA g0 = idct_mem[sh + 0] * s0  ;
      FPDATA g1 = idct_mem[sh+ 4] * s4 ;
      FPDATA g2 = idct_mem[sh+ 2] * s2;
      FPDATA g3 = idct_mem[sh+ 6] * s6;
      FPDATA g4 = idct_mem[sh+ 5] * s5;
      FPDATA g5 = idct_mem[sh+ 1] * s1;
      FPDATA g6 = idct_mem[sh+ 7] * s7;
      FPDATA g7 = idct_mem[sh+ 3] * s3;

      FPDATA f0 = g0;
      FPDATA f1 = g1;
      FPDATA f2 = g2;
      FPDATA f3 = g3;
      FPDATA f4 = g4 - g7;
      FPDATA f5 = g5 + g6;
      FPDATA f6 = g5 - g6;
      FPDATA f7 = g4 + g7;

      FPDATA e0 = f0;
      FPDATA e1 = f1;
      FPDATA e2 = f2 - f3;
      FPDATA e3 = f2 + f3;
      FPDATA e4 = f4;
      FPDATA e5 = f5 - f7;
      FPDATA e6 = f6;
      FPDATA e7 = f5 + f7;
      FPDATA e8 = f4 + f6 ;

      FPDATA d0 = e0;
      FPDATA d1 = e1;
      FPDATA d2 = e2 * m1;
      FPDATA d3 = e3;
      FPDATA d4 = e4 * m2;
      FPDATA d5 = e5 * m3;
      FPDATA d6 = e6 * m4;
      FPDATA d7 = e7;
      FPDATA d8 = e8 * m5;

      FPDATA c0 = d0 + d1;
      FPDATA c1 = d0 - d1;
      FPDATA c2 = d2 - d3;
      FPDATA c3 = d3;
      FPDATA c4 = d4 + d8;
      FPDATA c5 = d5 + d7;
      FPDATA c6 = d6 - d8;
      FPDATA c7 = d7;
      FPDATA c8 = c5 -c6;


      FPDATA b0 = c0 + c3;
      FPDATA b1 = c1 + c2 ;
      FPDATA b2 = c1 - c2;
      FPDATA b3 = c0 - c3;
      FPDATA b4 = c4 - c8;
      FPDATA b5 = c8;
      FPDATA b6 = c6 - c7;
      FPDATA b7 = c7;



      o[sh+0] =(b0 + b7).to_uint();
      o[sh+1] =(b1 + b6).to_uint();
      o[sh+2] =(b2 + b5).to_uint();
      o[sh+3] =(b3 + b4).to_uint();
      o[sh+4] =(b3 - b4).to_uint();
      o[sh+5] =(b2 - b5).to_uint();
      o[sh+6] =(b1 - b6).to_uint();
      o[sh+7] =(b0 - b7).to_uint();

    }



}



