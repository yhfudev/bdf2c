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

struct _ppmcavas_fontdraw_info_t {
    ppm_cavas_t * pppm;
    int offx;
    int offy;
    uint8_t * color_front;
    uint8_t * color_background;
};

void
fontdata_cb_draw_ppmcavas (void *userdata, int x, int y, char is_dot)
{
    struct _ppmcavas_fontdraw_info_t *pdata = userdata;
    if (is_dot) {
        ppm_cavas_pixel (pdata->pppm, pdata->offx + x, pdata->offy + y, pdata->color_front);
    } else {
        ppm_cavas_pixel (pdata->pppm, pdata->offx + x, pdata->offy + y, pdata->color_background);
    }
}

int
ppm_cavas_fontdraw (ppm_cavas_t * pppm, size_t x, size_t y, uint8_t color_front[4], uint8_t color_background[4], unsigned char c)
{
    struct _ppmcavas_fontdraw_info_t pcfdi;
    pcfdi.pppm = pppm;
    pcfdi.offx = x;
    pcfdi.offy = y;
    pcfdi.color_front = color_front;
    pcfdi.color_background = color_background;
    return fontdata_draw ((fontdata_t *)pppm->fontptr, c, &pcfdi, fontdata_cb_draw_ppmcavas);
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
