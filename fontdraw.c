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

#include "fontdraw.h"
#include "fontdata.h"

extern int ppm_cavas_pixel (ppm_cavas_t * pppm, size_t x, size_t y, uint8_t color[4]);

int
ppm_cavas_fontdraw (ppm_cavas_t * pppm, size_t x, size_t y, uint8_t color_front[4], uint8_t color_background[4], char c)
{
    int i;
    int j;
    for (i = 0; i < FNT_HEIGHT; i ++) {
        for (j = 0; j < FNT_WIDTH; j ++) {
            if (FNT_GET_PIXEL(c, j, (FNT_HEIGHT - i - 1))) {
                ppm_cavas_pixel (pppm, x + j, y + i, color_front);
            } else {
                ppm_cavas_pixel (pppm, x + j, y + i, color_background);
            }
        }
    }
    return 0;
}

int
ppm_cavas_drawstring (ppm_cavas_t * pppm, size_t x, size_t y, uint8_t color_front[4], uint8_t color_background[4], char * msg)
{
    int i;
    int sz = strlen (msg);
    for (i = 0; i < sz; i ++) {
        ppm_cavas_fontdraw (pppm, x + (FNT_WIDTH + 1) * i, y, color_front, color_background, msg[i]);
    }
    return 0;
}
