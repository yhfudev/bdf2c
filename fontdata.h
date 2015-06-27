/**
 * @file    fontdata.h
 * @brief   base font data
 * @author  Yunhui Fu (yhfudev@gmail.com)
 * @version 1.0
 * @date    2015-06-19
 * @copyright GPL/BSD
 */
#ifndef FONT_DATA_H
#define FONT_DATA_H

#define FNT_WIDTH  5
#define FNT_HEIGHT (7+1)
#define FNT_GET_PIXEL(c, x, y) (*(g_basefont + ((c) * 5) + (x)) & (unsigned char)(0x80 >> (y)) )

#ifndef PROGMEM
#define PROGMEM
#endif
extern const unsigned char g_basefont[] PROGMEM;

#endif
