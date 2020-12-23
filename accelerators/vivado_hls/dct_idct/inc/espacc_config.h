#ifndef INC_ESPACC_CONFIG_H
#define INC_ESPACC_CONFIG_H

// User defined constants

// Data type

#define IS_TYPE_FIXED_POINT 0
#define FRAC_BITS 0
#define IS_TYPE_UINT 0
#define IS_TYPE_INT 1
#define IS_TYPE_FLOAT 0

//  if "BASIC" is 1 then standard DCT/IDCT architetcure is used with quadruple for loops, else the AAN architetcure is used.
#define BASIC 0

//  if "LIB_FUNCTION" is 1 then math library fuctions are used in place of pre-calculated values
#define LIB_FUNCTIONS 0

//  if "FAST" is 1 then different HLS pragmas are used which will result in a faster implementation of the design
//  using library functions make the implemenation slower if you want to achieve a faster implementation then define "LIB_FUNCTIONS" as 0
#define FAST 1



// In/out arrays

#define SIZE_IN_CHUNK_DATA 64

#define SIZE_OUT_CHUNK_DATA 64

#endif
