#!/bin/bash
rm *.txt *.h
rm out_data

echo Enter The Input Rows
read irows
echo Enter The Input Columns
read icols

echo Enter The Output Rows
read orows
echo Enter The Output Columns
read ocols

python datagen.py $irows $icols  > pv_data.h

g++ -o out_data dct_blk_odd.cpp

./out_data $irows $icols $orows $ocols  > output.txt

rm -rf bd
rm -rf ld
rm -rf pv

g++ barec.cpp -o bd
./bd $irows $icols $orows $ocols > barec_data.h
g++ app.cpp -o ld
./ld $irows $orows $icols $ocols  > app_data.h
rm  ../../../../soft/leon3/drivers/dct_idct/app/app_data.h
cp app_data.h ../../../../soft/leon3/drivers/dct_idct/app/
g++ data_out.cpp -o pv
./pv  $orows $ocols > pv_data_out.h

