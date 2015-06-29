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

//#define FNT_WIDTH  5
//#define FNT_HEIGHT (7+1)
//#define FNT_GET_PIXEL(fntdata, c, x, y) (*(fntdata + 4 + ((c) * 5) + (x)) & (unsigned char)(0x80 >> (y)) )

typedef unsigned char uint8_t;
typedef uint8_t fontdata_t;
/**
 * fontdata_t format:
 *   uint8_t type;
 *        0 -- row first;
 *        1 -- row first (reverse);
 *        8 -- column first;
 *        9 -- column first (reverse);
 *   uint8_t width(x);
 *   uint8_t height(y);
 *   uint8_t char_offset;
 *   uint8_t char_number;
 *   ... (data)
 */

#ifndef PROGMEM
#define PROGMEM
#endif
extern const fontdata_t g_font5x8_0[] PROGMEM;
extern const fontdata_t g_font6x8_0[] PROGMEM;
extern const fontdata_t g_fontdata_4x6_tiny[] PROGMEM;
extern const fontdata_t g_fontdata_8x16[] PROGMEM;

// depreciate
extern const fontdata_t g_font5x8_1[] PROGMEM;
extern const fontdata_t g_font5x8_2[] PROGMEM;
extern const fontdata_t g_fontdata_6x8_small[] PROGMEM;
extern const fontdata_t g_fontdata_16x24digits[] PROGMEM;
extern const fontdata_t g_fontdata_16x16digits[] PROGMEM;

extern const fontdata_t g_fontdata_medium_digits[] PROGMEM;
extern const fontdata_t g_fontdata_big_digits[] PROGMEM;

//int fontdata_get_width (fontdata_t * data);
//int fontdata_get_height (fontdata_t * data);
#define fontdata_get_type(d)  ((fontdata_t *)(d))[0]
#define fontdata_get_width(d)  ((fontdata_t *)(d))[1]
#define fontdata_get_height(d) ((fontdata_t *)(d))[2]
#define fontdata_get_charoffset(d) ((fontdata_t *)(d))[3]
#define fontdata_get_charnumber(d) ((fontdata_t *)(d))[4]

#define fontdata_get_data(d) (fontdata_t *)(d) + 5

//uint8_t * fontdata_get_char (fontdata_t * data, int charval);
#define fontdata_get_char(d, charval) ( \
    ((fontdata_get_charoffset(d)<=(charval)) && ((charval) < fontdata_get_charoffset(d) + fontdata_get_charnumber(d) ) ) \
    ? (fontdata_get_data(d) + (fontdata_get_width(d) * fontdata_get_height(d) / 8)*((charval) - fontdata_get_charoffset(d)) ) \
    : NULL \
    )

#define FONTDATA_PIXEL_AT_0(chardata, w, h, x, y) (*((chardata) + (((y) * (w) + (x)) / 8)) & (unsigned char)(0x80 >> (((y) * (w) + (x)) % 8)) )
#define FONTDATA_PIXEL_AT_1(chardata, w, h, x, y) (*((chardata) + ((((h) - 1 - (y)) * (w) + (x)) / 8)) & (unsigned char)(0x80 >> ((((h) - 1 - (y)) * (w) + (x)) % 8)) )

#define FONTDATA_PIXEL_AT_8(chardata, w, h, x, y) (*((chardata) + (((x) * (h) + (y)) / 8)) & (unsigned char)(0x80 >> (((x) * (h) + (y)) % 8)) )
// simplified 10:
#define FONTDATA_PIXEL_AT_9(chardata, w, h, x, y) (*((chardata) + (((x) * (h) + ((h) - 1 - (y))) / 8)) & (unsigned char)(0x80 >> (((x) * (h) + ((h) - 1 - (y))) % 8)) )
//#define FONTDATA_PIXEL_AT_ss(chardata, w, h, x, y) (*((chardata) + ((((w) - 1 - (x)) * (h) + (y)) / 8)) & (unsigned char)(0x80 >> ((((w) - 1 - (x)) * (h) + (y)) % 8)) )

// default:
#define FONTDATA_PIXEL_AT_10(chardata, w, h, x, y) (*((chardata) + (((x) * (8) + (y)%8 ) / 8 + ((y)/8)*(w) )) & (unsigned char)(0x80 >> (((x) * (h) + (7-((y)%8)) ) % 8)) )

// 16x24
#define FONTDATA_PIXEL_AT_11(chardata, w, h, x, y) (*((chardata) + (((x) * (h) + (y)) / 8)) & (unsigned char)(0x01 << (((x) * (h) + (y)) % 8)) )


#endif
