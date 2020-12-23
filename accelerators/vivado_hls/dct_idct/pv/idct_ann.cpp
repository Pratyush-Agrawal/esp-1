
#include <iostream>
#include <cstdlib>
#include <cmath>
using namespace std; 
#define pi 3.142857 
const int m = 8, n = 8; 
  

void aan_dct( int i[]) {

  const double m0 = 2.0 * cos(1.0/16.0 *2.0 * pi);

  const double m1 = 2.0 * cos(2.0/16.0 * 2.0 * pi);
  const double m3 = 2.0 * cos(2.0/16.0 * 2.0 * pi); 
  const double m5 = 2.0 * cos(3.0/16.0 * 2.0 * pi);
  const double m2 = m0 -m5;
  const double m4 = m0 +m5;


 const double s0 = cos(0.0/16 * pi)/ sqrt(8);
 const double s1 = (cos(1.0/16 * pi)/2.0);
 const double s2 = (cos(2.0/16 * pi)/2.0);
 const double s3 = (cos(3.0/16 * pi)/2.0);
 const double s4 = (cos(4.0/16 * pi)/2.0);
 const double s5 = (cos(5.0/16 * pi)/2.0);
 const double s6 = (cos(6.0/16 * pi)/2.0);
 const double s7 = (cos(7.0/16 * pi)/2.0);




for(int a=0; a<8 ; a++){


  double g0 = i[0*8 +a] * s0  ;
  double g1 = i[4*8+a] * s4 ;
  double g2 = i[2*8+a] * s2;
  double g3 = i[6*8+a] * s6;
  double g4 =i[5*8+a] * s5;
  double g5 =i[1*8+a] * s1;
  double g6 =i[7*8+a] * s7;
  double g7 =i[3*8+a] * s3;

  double f0 = g0;
  double f1 = g1;
  double f2 = g2;
  double f3 = g3;
  double f4 = g4 - g7;
  double f5 = g5 + g6;
  double f6 = g5 - g6;
  double f7 = g4 + g7;

  double e0 = f0;
  double e1 = f1;
  double e2 = f2 - f3;
  double e3 = f2 + f3;
  double e4 = f4;
  double e5 = f5 - f7;
  double e6 = f6;
  double e7 = f5 + f7;
  double e8 = f4 + f6 ;

  double d0 = e0;
  double d1 = e1;
  double d2 = e2 * m1;
  double d3 = e3;
  double d4 = e4 * m2;
  double d5 = e5 * m3;
  double d6 = e6 * m4;
  double d7 = e7;
  double d8 = e8 * m5;

  double c0 = d0 + d1;
  double c1 = d0 - d1;
  double c2 = d2 - d3;
  double c3 = d3;
  double c4 = d4 + d8;
  double c5 = d5 + d7;
  double c6 = d6 - d8;
  double c7 = d7;
 double c8 = c5 -c6;


  double b0 = c0 + c3;
  double b1 = c1 + c2 ;
  double b2 = c1 - c2;
  double b3 = c0 - c3;
  double b4 = c4 - c8;
  double b5 = c8;
  double b6 = c6 - c7;
  double b7 = c7;



  i[0*8+a] =(int) (b0 + b7);
  i[1*8+a] =(int) (b1 + b6);
  i[2*8+a] =(int) (b2 + b5);
  i[3*8+a] =(int) (b3 + b4);
  i[4*8+a] =(int) (b3 - b4);
  i[5*8+a] =(int) (b2 - b5);
  i[6*8+a] =(int) (b1 - b6);
  i[7*8+a] =(int) (b0 - b7);
}

for(int a=0; a<8 ; a++){

  double g0 = i[a*8 + 0] * s0  ;
  double g1 = i[a*8+ 4] * s4 ;
  double g2 = i[a*8+ 2] * s2;
  double g3 = i[a*8+ 6] * s6;
  double g4 =i[a*8+ 5] * s5;
  double g5 =i[a*8+ 1] * s1;
  double g6 =i[a*8+ 7] * s7;
  double g7 =i[a*8+ 3] * s3;

  double f0 = g0;
  double f1 = g1;
  double f2 = g2;
  double f3 = g3;
  double f4 = g4 - g7;
  double f5 = g5 + g6;
  double f6 = g5 - g6;
  double f7 = g4 + g7;

  double e0 = f0;
  double e1 = f1;
  double e2 = f2 - f3;
  double e3 = f2 + f3;
  double e4 = f4;
  double e5 = f5 - f7;
  double e6 = f6;
  double e7 = f5 + f7;
  double e8 = f4 + f6 ;

  double d0 = e0;
  double d1 = e1;
  double d2 = e2 * m1;
  double d3 = e3;
  double d4 = e4 * m2;
  double d5 = e5 * m3;
  double d6 = e6 * m4;
  double d7 = e7;
  double d8 = e8 * m5;

double c0 = d0 + d1;
  double c1 = d0 - d1;
  double c2 = d2 - d3;
  double c3 = d3;
  double c4 = d4 + d8;
  double c5 = d5 + d7;
  double c6 = d6 - d8;
  double c7 = d7;
 double c8 = c5 -c6;


  double b0 = c0 + c3;
  double b1 = c1 + c2 ;
  double b2 = c1 - c2;
  double b3 = c0 - c3;
  double b4 = c4 - c8;
  double b5 = c8;
  double b6 = c6 - c7;
  double b7 = c7;



  i[a*8+0] =(int) (b0 + b7);
  i[a*8+1] =(int) (b1 + b6);
  i[a*8+2] =(int) (b2 + b5);
  i[a*8+3] =(int) (b3 + b4);
  i[a*8+4] =(int) (b3 - b4);
  i[a*8+5] =(int) (b2 - b5);
  i[a*8+6] =(int) (b1 - b6);
  i[a*8+7] =(int) (b0 - b7);

}

int zz,j;
     for (zz = 0; zz < m; zz++) {
        for (j = 0; j < n; j++) {
            cout << i[zz *m +j] << "\t";
        }
        cout << "\n";
    }


}

int main() 
{ 
    int input_matrix[m*n];
	
    #include "pv_data_out.h"
  
    
    aan_dct(input_matrix);
    
    return 0; 
} 
