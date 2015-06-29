#!/bin/sh

rm -f tstppm
gcc -g -DDEBUG=1 -o tstppm fontdata.c fontdata2.c fontdraw.c ppmhdr.c
./tstppm
exit 0

DN_BASE=../

if [ 1 = 1 ]; then
fontlist=(
    #unifont-7.0.06.bdf
    #dejavusanyuanti.bdf
    #unifont.bdf
    #wqy-bitmapfont-gb18030/wenquanyi_9pt.bdf
    #wqy-bitmapfont-gb18030/wenquanyi_10pt.bdf
    #wqy-bitmapfont-gb18030/wenquanyi_11pt.bdf
    #wqy-bitmapfont-gb18030/wenquanyi_12pt.bdf
    #wqy-bitmapfont-gb18030/wenquanyi_13px.bdf
    #wqy-bitmapfont-gb18030/LiberationSans-9pt.bdf
    #wqy-bitmapfont-gb18030/LiberationSans-10pt.bdf
    #wqy-bitmapfont-gb18030/LiberationSans-11pt.bdf
    #wqy-bitmapfont-gb18030/LiberationSans-12pt.bdf
    #../../u8glib-git/tools/font/bdf/5x8.bdf
    ../../u8glib-git/tools/font/bdf/6x9.bdf
    ../../u8glib-git/tools/font/bdf/6x10.bdf
    )

make clean; make
for FN_FNT in ${fontlist[*]} ; do
    FN_PPM=${FN_FNT}.ppm
    FN_C=${FN_FNT}.c
    echo ./bdf2c -i ${DN_BASE}/${FN_FNT} -o ${DN_BASE}/${FN_C} -p ${DN_BASE}/${FN_PPM}
    ./bdf2c -i ${DN_BASE}/${FN_FNT} -o ${DN_BASE}/${FN_C} -p ${DN_BASE}/${FN_PPM}
done
fi
