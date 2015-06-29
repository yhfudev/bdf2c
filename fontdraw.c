/**
 * @file    fontdraw.h
 * @brief   Draw a char
 * @author  Yunhui Fu (yhfudev@gmail.com)
 * @version 1.0
 * @date    2015-06-19
 * @copyright GPL/BSD
 */
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "fontdata.h"
#include "fontdraw.h"
#include "ppmhdr.h"

int
ppm_cavas_setfont (ppm_cavas_t * pppm, fontdata_t * fntptr)
{
    assert (NULL != pppm);
    pppm->fontptr = fntptr;
    return 0;
}

int
ppm_cavas_getfont_width (ppm_cavas_t * pppm)
{
    assert (NULL != pppm);
    assert (NULL != pppm->fontptr);
    return fontdata_get_width (pppm->fontptr);
}

int
ppm_cavas_getfont_height (ppm_cavas_t * pppm)
{
    assert (NULL != pppm);
    assert (NULL != pppm->fontptr);
    return fontdata_get_height (pppm->fontptr);
}

int
ppm_cavas_fontdraw (ppm_cavas_t * pppm, size_t x, size_t y, uint8_t color_front[4], uint8_t color_background[4], unsigned char c)
{
    int i;
    int j;
    uint8_t * data = NULL;
    int w;
    int h;

    assert (NULL != pppm);
    assert (NULL != pppm->fontptr);
    data = fontdata_get_char (pppm->fontptr, c);
    if (NULL == data) {
        return -1;
    }

    w = fontdata_get_width(pppm->fontptr);
    h = fontdata_get_height(pppm->fontptr);
    switch (fontdata_get_type(pppm->fontptr)) {
#define FNTDAT_SHOW(type) \
    case type: \
        for (i = 0; i < h; i ++) { \
            for (j = 0; j < w; j ++) { \
                if (FONTDATA_PIXEL_AT_##type(data, w, h, j, i)) { \
                    ppm_cavas_pixel (pppm, x + j, y + i, color_front); \
                } else { \
                    ppm_cavas_pixel (pppm, x + j, y + i, color_background); \
                } \
            } \
        } \
        break
        //FNTDAT_SHOW(0);
        //FNTDAT_SHOW(1);
        FNTDAT_SHOW(9);
        FNTDAT_SHOW(8);
        FNTDAT_SHOW(11);
    default:
        FNTDAT_SHOW(10);
    }
    return 0;
}

int
ppm_cavas_drawstring (ppm_cavas_t * pppm, size_t x, size_t y, uint8_t color_front[4], uint8_t color_background[4], unsigned char * msg)
{
    int i;
    int sz = strlen ((char *)msg);

    assert (NULL != pppm);
    assert (NULL != pppm->fontptr);

    for (i = 0; i < sz; i ++) {
        ppm_cavas_fontdraw (pppm, x + (fontdata_get_width(pppm->fontptr) + 1) * i, y, color_front, color_background, msg[i]);
    }
    return 0;
}
