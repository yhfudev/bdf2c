#!/bin/sh

FN_FNT=../unifont-7.0.06.bdf
FN_FNT=../wenquanyi_12pt.bdf
FN_FNT=../unifont.bdf
FN_FNT=../dejavusanyuanti.bdf

FN_PPM=${FN_FNT}.ppm
FN_C=${FN_FNT}.c

make
echo ./bdf2c -i ${FN_FNT} -o ${FN_C} -p ${FN_PPM}
./bdf2c -i ${FN_FNT} -o ${FN_C} -p ${FN_PPM}

gcc -g -DDEBUG=1 -o tstppm ppmhdr.c
#./tstppm



