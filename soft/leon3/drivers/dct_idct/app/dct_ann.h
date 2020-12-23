#include <math.h>
#define pi 3.142857 
  

void aan_dct_int( int i[])
 {

  /*-- Code Optimized
  const double a1 = sqrt(.5);
  const double a2 = sqrt(2.) * cos(3./16. * 2 * M_PI);
  const double a3 = a1;
  const double a4 = sqrt(2.) * cos(1./16. * 2 * M_PI);
  const double a5 = cos(3./16. * 2 * M_PI);
  */
 
  const double  a1 = 0.707106;   // sqrt(.5);
  const double  a2 = 0.541196;   // sqrt(2.) * cos(3./16. * 2 * M_PI);
  const double  a3 = a1;
  const double  a4 = 1.30656;    //sqrt(2.) * cos(1./16. * 2 * M_PI);
  const double  a5 = 0.382683;   // cos(3./16. * 2 * M_PI);
  
  //Scaling Factor//
  const double  s0 = 0.353553 ;  //(cos(0)*sqrt(.5)/2)/(1       );
  const double  s1 = 0.254898;   //(cos(1.*M_PI/16)/2)/(-a5+a4+1);
  const double  s2 = 0.270598;   //(cos(2.*M_PI/16)/2)/(a1+1    );
  const double  s3 = 0.300672;   //(cos(3.*M_PI/16)/2)/(a5+1    );
  const double  s4 = s0;
  const double  s5 = 0.449988;   //(cos(5.*M_PI/16)/2)/(1-a5    );
  const double  s6 = 0.653281;   //(cos(6.*M_PI/16)/2)/(1-a1    );
  const double  s7 = 1.28146;    //(cos(7.*M_PI/16)/2)/(a5-a4+1 );

  for(int a=0; a<8 ; a++)
    {
      double b0 = i[0*8 +a] + i[7*8+a];
      double b1 = i[1*8+a] + i[6*8+a];
      double b2 = i[2*8+a] + i[5*8+a];
      double b3 = i[3*8+a] + i[4*8+a];
      double b4 =-i[4*8+a] + i[3*8+a];
      double b5 =-i[5*8+a] + i[2*8+a];
      double b6 =-i[6*8+a] + i[1*8+a];
      double b7 =-i[7*8+a] + i[0*8+a];

      double c0 = b0 + b3;
      double c1 = b1 + b2;
      double c2 =-b2 + b1;
      double c3 =-b3 + b0;
      double c4 =-b4 - b5;
      double c5 = b5 + b6;
      double c6 = b6 + b7;
      double c7 = b7;

      double d0 = c0 + c1;
      double d1 =-c1 + c0;
      double d2 = c2 + c3;
      double d3 = c3;
      double d4 = c4;
      double d5 = c5;
      double d6 = c6;
      double d7 = c7;

      double d8 = (d4 + d6) * a5;

      double e0 = d0;
      double e1 = d1;
      double e2 = d2 * a1;
      double e3 = d3;
      double e4 = -d4 * a2 - d8;
      double e5 = d5 * a3;
      double e6 = d6 * a4 - d8;
      double e7 = d7;

      double f0 = e0;
      double f1 = e1;
      double f2 = e2 + e3;
      double f3 = e3 - e2;
      double f4 = e4;
      double f5 = e5 + e7;
      double f6 = e6;
      double f7 = e7 - e5;

      double g0 = f0;
      double g1 = f1;
      double g2 = f2;
      double g3 = f3;
      double g4 = f4 + f7;
      double g5 = f5 + f6;
      double g6 = -f6 + f5;
      double g7 = f7 - f4;



      i[0*8+a] =(int) (g0 * s0);
      i[4*8+a] =(int) (g1 * s4);
      i[2*8+a] =(int) (g2 * s2);
      i[6*8+a] =(int) (g3 * s6);
      i[5*8+a] =(int) (g4 * s5);
      i[1*8+a] =(int) (g5 * s1);
      i[7*8+a] =(int) (g6 * s7);
      i[3*8+a] =(int) (g7 * s3);
  }

  for(int a=0; a<8 ; a++)
    {


      double b0 = i[a*8 +0] + i[a*8+7];
      double b1 = i[a*8+1] + i[a*8+6];
      double b2 = i[a*8+2] + i[a*8+5];
      double b3 = i[a*8+3] + i[a*8+4];
      double b4 =-i[a*8+4] + i[a*8+3];
      double b5 =-i[a*8+5] + i[a*8+2];
      double b6 =-i[a*8+6] + i[a*8+1];
      double b7 =-i[a*8+7] + i[a*8+0];

      double c0 = b0 + b3;
      double c1 = b1 + b2;
      double c2 =-b2 + b1;
      double c3 =-b3 + b0;
      double c4 =-b4 - b5;
      double c5 = b5 + b6;
      double c6 = b6 + b7;
      double c7 = b7;

      double d0 = c0 + c1;
      double d1 =-c1 + c0;
      double d2 = c2 + c3;
      double d3 = c3;
      double d4 = c4;
      double d5 = c5;
      double d6 = c6;
      double d7 = c7;

      double d8 = (d4 + d6) * a5;

      double e0 = d0;
      double e1 = d1;
      double e2 = d2 * a1;
      double e3 = d3;
      double e4 = -d4 * a2 - d8;
      double e5 = d5 * a3;
      double e6 = d6 * a4 - d8;
      double e7 = d7;

      double f0 = e0;
      double f1 = e1;
      double f2 = e2 + e3;
      double f3 = e3 - e2;
      double f4 = e4;
      double f5 = e5 + e7;
      double f6 = e6;
      double f7 = e7 - e5;

      double g0 = f0;
      double g1 = f1;
      double g2 = f2;
      double g3 = f3;
      double g4 = f4 + f7;
      double g5 = f5 + f6;
      double g6 = -f6 + f5;
      double g7 = f7 - f4;


      i[a*8+0] = (int)(g0 * s0);
      i[a*8+4] = (int)(g1 * s4);
      i[a*8+2] = (int)(g2 * s2);
      i[a*8+6] = (int)(g3 * s6);
      i[a*8+5] = (int)(g4 * s5);
      i[a*8+1] = (int)(g5 * s1);
      i[a*8+7] = (int)(g6 * s7);
      i[a*8+3] = (int)(g7 * s3);
    }



}




