/**
 * @file    ppmhdr.h
 * @brief   PPM file format
 * @author  Yunhui Fu (yhfudev@gmail.com)
 * @version 1.0
 * @date    2015-06-19
 * @copyright GPL/BSD
 */
#ifndef _PPM_HDR_H
#define _PPM_HDR_H

#include <stddef.h> // size_t
#include <stdint.h> // uint32_t

#include "fontdata.h"

typedef struct _ppm_file_t {
    size_t xmax;
    size_t ymax;
    size_t depth; // 255
    FILE *fp;
    off_t off_data; // start of data
} ppm_file_t;

typedef struct _ppm_cavas_t {
    size_t xmax;
    size_t ymax;
    size_t bit; // 8bit for ARGB
    size_t buffer_size; // the byte size of the following buffer
    const fontdata_t *fontptr;
    uint8_t buffer[4];
} ppm_cavas_t;

ppm_cavas_t * ppm_cavas_resize (ppm_cavas_t * pppm, size_t x, size_t y, size_t bit);
//ppm_cavas_t * ppm_cavas_create (size_t x, size_t y, size_t bit);
#define ppm_cavas_create(x,y,bit) ppm_cavas_resize (NULL, (x), (y), (bit))

//size_t ppm_cavas_width (ppm_cavas_t * p);
//size_t ppm_cavas_height (ppm_cavas_t * p);
#define ppm_cavas_width(p) ((p)->xmax)
#define ppm_cavas_height(p) ((p)->ymax)

void ppm_cavas_destroy (ppm_cavas_t * p);
void ppm_cavas_zero (ppm_cavas_t * p);
void ppm_cavas_fill (ppm_cavas_t * p,  uint8_t color[4]);
int ppm_cavas_pixel (ppm_cavas_t * pppm, size_t x, size_t y, uint8_t color[4]);
int ppm_cavas_bitblit (ppm_cavas_t *dest, ppm_cavas_t *src, size_t dx, size_t dy, size_t sx, size_t sy, size_t wx, size_t hy);

int ppm_load (ppm_file_t *fp, const char * filename);
int ppm_create (ppm_file_t *fp, const char * filename, size_t x, size_t y, size_t depth);
int ppm_close (ppm_file_t *fp);
int ppm_bitblit_from (ppm_file_t *fp_dest, ppm_cavas_t *src, size_t dx, size_t dy, size_t sx, size_t sy, size_t wx, size_t hy);
int ppm_bitblit_to (ppm_cavas_t *dest, ppm_file_t *fp_src, size_t dx, size_t dy, size_t sx, size_t sy, size_t wx, size_t hy);

int bdf2c_fontpic_init (const char * filename, size_t num, size_t wx, size_t hy, const char * msg);
void bdf2c_fontpic_set_name(const char * name);
void bdf2c_fontpic_clear();
void bdf2c_fontpic_add (uint8_t *bitmap, size_t width, size_t height, int encoding, char flag_shifted);

#endif // _PPM_HDR_H
