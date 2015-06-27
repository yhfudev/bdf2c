#!/bin/sh

fontlist=(
    unifont-7.0.06.bdf
    dejavusanyuanti.bdf
    unifont.bdf
    wenquanyi_12pt.bdf
    )

DN_BASE=../

make clean; make
for FN_FNT in ${fontlist[*]} ; do
    FN_PPM=${FN_FNT}.ppm
    FN_C=${FN_FNT}.c
    echo ./bdf2c -i ${DN_BASE}/${FN_FNT} -o ${DN_BASE}/${FN_C} -p ${DN_BASE}/${FN_PPM}
    ./bdf2c -i ${DN_BASE}/${FN_FNT} -o ${DN_BASE}/${FN_C} -p ${DN_BASE}/${FN_PPM}
done

gcc -g -DDEBUG=1 -o tstppm fontdata.c fontdraw.c ppmhdr.c
./tstppm
#exit 0
