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

typedef void (* fontdata_cb_draw_t)(void *userdata, int x, int y, char is_dot);
int fontdata_draw (fontdata_t * pfontdata, unsigned char c, void *userdata, fontdata_cb_draw_t cb);

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

extern const fontdata_t g_fontdata_32x50_sevendigit[] PROGMEM;
extern const fontdata_t g_fontdata_16x16_bigfont[] PROGMEM;

#endif
