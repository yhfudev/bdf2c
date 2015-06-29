/**
 * @file    ppmhdr.c
 * @brief   PPM file format
 * @author  Yunhui Fu (yhfudev@gmail.com)
 * @version 1.0
 * @date    2015-06-19
 * @copyright GPL/BSD
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>

#include <assert.h>

#include "ppmhdr.h"
#include "fontdraw.h"

/**
 * @brief resize the cavas
 */
// bit: ignored
ppm_cavas_t *
ppm_cavas_resize (ppm_cavas_t * pppm, size_t x, size_t y, size_t bit)
{
    fontdata_t * fntdefault = (fontdata_t *)g_font5x8_0;
    size_t sz = x * y * 4;
    char flg_newmem = 1;

    if (NULL != pppm) {
        if (pppm->buffer_size >= sz) {
            flg_newmem = 0;
        }
        fntdefault = (fontdata_t *)pppm->fontptr;
    }
    if (flg_newmem) {
        pppm = realloc (pppm, sz + sizeof (*pppm));
        if (NULL == pppm) {
            return NULL;
        }
    } else {
        sz = pppm->buffer_size;
    }
    pppm->xmax = x;
    pppm->ymax = y;
    pppm->bit  = bit = 8;
    pppm->buffer_size = sz;
    pppm->fontptr = fntdefault;
    return pppm;
}

/**
 * @brief destroy the cavas
 */
void
ppm_cavas_destroy(ppm_cavas_t * p)
{
    free (p);
}

/**
 * @brief clear the cavas
 */
void
ppm_cavas_zero (ppm_cavas_t * p)
{
    size_t sz;
    sz = p->xmax * p->ymax * 4;
    memset (p->buffer, 0, sz);
}

/**
 * @brief fill the cavas with color
 */
void
ppm_cavas_fill (ppm_cavas_t * pppm,  uint8_t color[4])
{
    uint8_t *pend;
    uint8_t *p;
    size_t sz;
    //uint32_t val = (((uint32_t)color[3]) << 24) | (((uint32_t)color[2]) << 16) | (((uint32_t)color[1]) << 8) | ((uint32_t)color[3]);

    sz = pppm->xmax * pppm->ymax * 4;
    //memset (p->buffer, val, sz);
    p = pppm->buffer;
    pend = p + sz;
    for (; p < pend; p += 4) {
        p[0] = color[0];
        p[1] = color[1];
        p[2] = color[2];
        p[3] = color[3];
    }
}

#define PPM_CAVAS_BUFF_OFFSET(ppm,x,y) (((y)*(ppm)->xmax) + (x))

/**
 * @brief put a pixel in the cavas
 */
// val: 0-A, 1-R, 2-G, 3-B
int
ppm_cavas_pixel (ppm_cavas_t * pppm, size_t x, size_t y, uint8_t color[4])
{
    uint8_t *p;
    assert (NULL != pppm);
    if ((x >= pppm->xmax) || (y >= pppm->ymax)) {
        fprintf (stderr, "Warning: out of range: max(%zu,%zu) <= in(%zu,%zu)\n", pppm->xmax, pppm->ymax, x, y);
        if (x > 100000000 || y > 10000000) {
            fprintf (stderr, "Error in data\n");
        }
        return -1;
    }
    p = (uint8_t *)pppm->buffer;
    p += (PPM_CAVAS_BUFF_OFFSET(pppm, x, y) * 4);
    p[0] = color[0];
    p[1] = color[1];
    p[2] = color[2];
    p[3] = color[3];
    return 0;
}

/**
 * @brief transfer a block(cavas) to the cavas
 */
int
ppm_cavas_bitblit (ppm_cavas_t *dest, ppm_cavas_t *src, size_t dx, size_t dy, size_t sx, size_t sy, size_t wx, size_t hy)
{
    size_t i;
    size_t j;
    uint8_t *p1;
    uint8_t *p2;
    if (dx >= dest->xmax) {
        fprintf (stderr, "Warning: out of range: dest max(%zu,%zu) <= dx(%zu,%zu)\n", dest->xmax, dest->ymax, dx, dy);
        return -1;
    }
    if (dy >= dest->ymax) {
        fprintf (stderr, "Warning: out of range: dest max(%zu,%zu) <= dy(%zu,%zu)\n", dest->xmax, dest->ymax, dx, dy);
        return -1;
    }
    if (sx >= src->xmax) {
        fprintf (stderr, "Warning: out of range: src max(%zu,%zu) <= sx(%zu,%zu)\n", src->xmax, src->ymax, sx, sy);
        return -1;
    }
    if (sy >= src->ymax) {
        fprintf (stderr, "Warning: out of range: src max(%zu,%zu) <= sy(%zu,%zu)\n", src->xmax, src->ymax, sx, sy);
        return -1;
    }
    if (sx + wx > src->xmax) {
        wx = src->xmax - sx;
    }
    if (sy + hy > src->ymax) {
        hy = src->ymax - sy;
    }
    if (dx + wx > dest->xmax) {
        wx = dest->xmax - dx;
    }
    if (dy + hy > dest->ymax) {
        hy = dest->ymax - dy;
    }
    if ((wx <= 0) || (hy <= 0)) {
        fprintf (stderr, "Warning: area(%zu,%zu) = 0\n", wx, hy);
        return 0;
    }
    // TODO: consider overlap image
    for (i = 0; i < hy; i ++) {
        p1 = src->buffer;
        p2 = dest->buffer;
        p1 += (PPM_CAVAS_BUFF_OFFSET(src, sx, sy + i) * 4);
        p2 += (PPM_CAVAS_BUFF_OFFSET(dest, dx, dy + i) * 4);
        for (j = 0; j < wx; j ++) {
            p2[0] = p1[0];
            p2[1] = p1[1];
            p2[2] = p1[2];
            p2[3] = p1[3];
            p1 += 4;
            p2 += 4;
        }
    }
    return 0;
}

/**
 * @brief transfer a block(cavas) to ppm file
 */
int
ppm_bitblit_from (ppm_file_t *fp_dest, ppm_cavas_t *src, size_t dx, size_t dy, size_t sx, size_t sy, size_t wx, size_t hy)
{
    size_t i;
    size_t j;
    uint8_t *p1;
    off_t p2;
    if (dx >= fp_dest->xmax) {
        return -1;
    }
    if (dy >= fp_dest->ymax) {
        return -1;
    }
    if (sx >= src->xmax) {
        return -1;
    }
    if (sy >= src->ymax) {
        return -1;
    }
    if (sx + wx > src->xmax) {
        wx = src->xmax - sx;
    }
    if (sy + hy > src->ymax) {
        hy = src->ymax - sy;
    }
    if (dx + wx > fp_dest->xmax) {
        wx = fp_dest->xmax - dx;
    }
    if (dy + hy > fp_dest->ymax) {
        hy = fp_dest->ymax - dy;
    }
    if ((wx <= 0) || (hy <= 0)) {
        return 0;
    }
    for (i = 0; i < hy; i ++) {
        p1 = src->buffer;
        p2 = fp_dest->off_data;
        p1 += (PPM_CAVAS_BUFF_OFFSET(src, sx, sy + i) * 4);
        p2 += (PPM_CAVAS_BUFF_OFFSET(fp_dest, dx, dy + i) * 3);
        fseek(fp_dest->fp, p2, SEEK_SET);
        for (j = 0; j < wx; j ++) {
            fwrite (&(p1[1]), 3, 1, fp_dest->fp);
            p1 += 4;
            //off += 3;
        }
    }
    return 0;
}

/**
 * @brief transfer a block(PPM file) to the cavas
 */
int
ppm_bitblit_to (ppm_cavas_t *dest, ppm_file_t *fp_src, size_t dx, size_t dy, size_t sx, size_t sy, size_t wx, size_t hy)
{
    size_t i;
    size_t j;
    off_t p1;
    uint8_t *p2;
    if (dx >= dest->xmax) {
        return -1;
    }
    if (dy >= dest->ymax) {
        return -1;
    }
    if (sx >= fp_src->xmax) {
        return -1;
    }
    if (sy >= fp_src->ymax) {
        return -1;
    }
    if (sx + wx > fp_src->xmax) {
        wx = fp_src->xmax - sx;
    }
    if (sy + hy > fp_src->ymax) {
        hy = fp_src->ymax - sy;
    }
    if (dx + wx > dest->xmax) {
        wx = dest->xmax - dx;
    }
    if (dy + hy > dest->ymax) {
        hy = dest->ymax - dy;
    }
    if ((wx <= 0) || (hy <= 0)) {
        return 0;
    }
    for (i = 0; i < hy; i ++) {
        p1 = fp_src->off_data;
        p2 = dest->buffer;
        p1 += (PPM_CAVAS_BUFF_OFFSET(fp_src, sx, sy + i) * 3);
        p2 += (PPM_CAVAS_BUFF_OFFSET(dest, dx, dy + i) * 4);
        fseek(fp_src->fp, p1, SEEK_SET);
        for (j = 0; j < wx; j ++) {
            fread (&(p2[1]), 3, 1, fp_src->fp);
            p2[0] = 0;
            p2 += 4;
            //off += 3;
        }
    }
    return 0;
}

/**
 * @brief load a PPM file
 */
int
ppm_load (ppm_file_t *fp, const char * filename)
{
    char buf[300];
    size_t x, y, d;
    fp->fp = fopen(filename, "rb+");
    if(fp->fp == NULL) {
        // does not exist
        return -1;
    }

    fscanf (fp->fp, "%s\n", buf);
    fscanf (fp->fp, "%lu%lu%lu\n", &x, &y, &d);
    fp->xmax = x;
    fp->ymax = y;
    fp->depth = d;
    return 0;
}

/**
 * @brief create a new PPM file
 */
int
ppm_create (ppm_file_t *fp, const char * filename, size_t x, size_t y, size_t depth)
{
    assert (NULL != fp);
    if (fp == NULL) {
        return -1;
    }
    fp->fp = fopen (filename, "wb");
    if (NULL == fp->fp) {
        // error
        printf ("Error in create file: %s\n", filename);
        perror ("create file");
        return -1;
    }
    fp->xmax = x;
    fp->ymax = y;
    fp->depth = depth = 255;
    fprintf(fp->fp, "P6\n");
    fprintf(fp->fp, "%lu %lu %lu\n", x, y, depth);
    fp->off_data = ftell (fp->fp);
    ftruncate (fileno(fp->fp), fp->off_data + x * y * 3 );
    return 0;
}

/**
 * @brief close a PPM file
 */
int
ppm_close (ppm_file_t *fp)
{
    fclose (fp->fp);
    memset (fp, 0, sizeof(*fp));
    return 0;
}

void
DumpCharacter2Cavas(ppm_cavas_t * out, size_t offx, size_t offy, uint8_t *bitmap, size_t width, size_t height, uint8_t c_dot[4], uint8_t c_background[4])
{
    size_t x;
    size_t y;
    size_t c;
    size_t mask;
    int i;

    //assert (width <= out->xmax);
    //assert (height <= out->ymax);
    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; x += 8) {
            c = bitmap[y * ((width + 7) / 8) + x / 8];
            //printf("%d = %d\n", y * ((width+7)/8) + x/8, c);
            mask = 0x80;
            for (i = 0; (i < 8) && (x + i < width); i ++) {
                if (c & mask) {
                    ppm_cavas_pixel (out, offx + x + i, offy + y, c_dot);
                } else {
                    ppm_cavas_pixel (out, offx + x + i, offy + y, c_background);
                }
                mask >>= 1;
            }
        }
    }
}

// the margin
#define PPM_OFF_X 50
#define PPM_OFF_Y 50

// the boarder of item
#define PPM_FNT_BORDER 1

ppm_file_t fd;
ppm_cavas_t * chbuf = NULL;
size_t current_num = 0;
size_t pic_xnum = 0;
size_t pic_ynum = 0;
size_t char_wx = 0;
size_t char_hy = 0;
const char * fntname = "";
//fontdata_t * curfont = NULL;

void bdf2c_fontpic_set_name(const char * msg)
{
    printf ("set font name: %s\n", msg);
    fntname = msg;
}

// wx,hy: the width and height of a char
int
bdf2c_fontpic_init (const char * filename, size_t num, size_t wx, size_t hy, const char * msg)
{
    size_t width;
    size_t height;
    size_t nextnum;
    size_t bit;

#if 1
// debug
if (num <= 256) {
    num = 256;
} else if (num < 65536) {
    num = 65536;
}
#endif

    bit = 0;
    for (nextnum = 1; nextnum < num; nextnum <<= 1) {
        bit ++;
    }
    bit = (bit + 1) / 2;
    width = 0x1 << bit;
    height = (num + width - 1) / width;
    assert (width * height >= num);
    pic_xnum = width;
    pic_ynum = height;
    char_wx = wx;
    char_hy = hy;

    bdf2c_fontpic_set_name(msg);

    fprintf (stderr, "bit=%zu, num=%zu, w=%zu, h=%zu, wx=%zu, hy=%zu\n", bit, num, width, height, wx, hy);
    fprintf (stderr, "ppm.xmax=%zu, ppm.ymax=%zu\n", (wx + 2 * PPM_FNT_BORDER) * width + PPM_OFF_X * 2, (hy + 2 * PPM_FNT_BORDER) * height + PPM_OFF_Y * 2);

    if (0 != ppm_create (&fd, filename, (wx + 2 * PPM_FNT_BORDER) * width + PPM_OFF_X * 2, (hy + 2 * PPM_FNT_BORDER) * height + PPM_OFF_Y * 2, 255)) {
        fprintf (stderr, "error in create ppm file out.ppm\n");
        return -1;
    }

    // +2 lines around box
    chbuf = ppm_cavas_create (wx+2*PPM_FNT_BORDER, hy+2*PPM_FNT_BORDER, 8);
    if (NULL == chbuf) {
        goto fntpic4ppm_error;
    }
    current_num = 0;
    return 0;

fntpic4ppm_error:
    if (NULL != chbuf) {
        ppm_cavas_destroy (chbuf);
    }
    chbuf = NULL;
    ppm_close (&fd);
    return -1;
}

static uint8_t bitmap_hznone[] = {
    0xff, 0xff,
    0xc0, 0x03,
    0xa0, 0x05,
    0x90, 0x09,
    0x88, 0x11,
    0x84, 0x21,
    0x82, 0x41,
    0x81, 0x81,
    0x81, 0x81,
    0x82, 0x41,
    0x84, 0x21,
    0x88, 0x11,
    0x90, 0x09,
    0xa0, 0x05,
    0xc0, 0x03,
    0xff, 0xff,
};

static uint8_t color_shift[4] = {0, 255, 0, 0};
static uint8_t color_none[4] = {0, 255, 0, 0};
static uint8_t color_line[4] = {0, 190, 190, 190};
static uint8_t color_dot[4] = {0,   0,   0, 255};
static uint8_t color_background[4] = {0, 255, 255, 255};;

int
bdf2c_fontpic_draw_metric ()
{
    size_t i;
    ppm_cavas_t * pcavas = NULL;
    unsigned char buf1[1000];
    int cnt;

    // x axis
    pcavas = ppm_cavas_create ((char_wx + 2*PPM_FNT_BORDER) * pic_xnum + PPM_OFF_X * 2, PPM_OFF_Y, 8);
    if (NULL == pcavas) {
        printf ("error in malloc memory\n");
        return -1;
    }

    // bottom
    ppm_cavas_fill (pcavas, color_background);
    // description
    sprintf ((char *)buf1, "By ppmhdr, Yunhui Fu <yhfudev@gmail.com>");
    ppm_cavas_drawstring (pcavas, PPM_OFF_X + 1, PPM_OFF_X - ppm_cavas_getfont_height(pcavas) - 5, color_shift, color_background, buf1);
    sprintf ((char *)buf1, "Font %lux%lu, %s", char_wx, char_hy, fntname);
    //sprintf ((char *)buf1, "SWpqstgy");
    ppm_cavas_drawstring (pcavas, PPM_OFF_X + 1, PPM_OFF_X - ppm_cavas_getfont_height(pcavas) - 1 - ppm_cavas_getfont_height(pcavas) - 5, color_shift, color_background, buf1);
    // draw line:
    for (i = 0; i < (char_wx + 2*PPM_FNT_BORDER) * pic_xnum; i ++) {
        ppm_cavas_pixel (pcavas, i + PPM_OFF_X, 0, color_dot);
    }
    // draw dot
    cnt = 0;
    for (i = 0; i < (char_wx + 2*PPM_FNT_BORDER) * pic_xnum; i += (char_wx + 2*PPM_FNT_BORDER)) {
        ppm_cavas_pixel (pcavas, char_wx / 2 + PPM_FNT_BORDER + i + PPM_OFF_X, 1, color_shift);
        ppm_cavas_pixel (pcavas, char_wx / 2 + PPM_FNT_BORDER + i + PPM_OFF_X, 2, color_shift);
        sprintf ((char *)buf1, "%02X", cnt);
        cnt ++;
        ppm_cavas_drawstring (pcavas
            , char_wx/2 + PPM_FNT_BORDER + i + PPM_OFF_X - ppm_cavas_getfont_width(pcavas)
            , 4
            , color_shift, color_background, buf1);
    }
    ppm_bitblit_from (&fd, pcavas, 0, PPM_OFF_Y + (char_hy + 2*PPM_FNT_BORDER) * pic_ynum, 0, 0, pcavas->xmax, pcavas->ymax);

    // top
    ppm_cavas_fill (pcavas, color_background);
    // draw line:
    for (i = 0; i < (char_wx + 2*PPM_FNT_BORDER) * pic_xnum; i ++) {
        ppm_cavas_pixel (pcavas, i + PPM_OFF_X, PPM_OFF_Y - 1, color_dot);
    }
    cnt = 0;
    // draw dot and mark
    for (i = 0; i < (char_wx + 2*PPM_FNT_BORDER) * pic_xnum; i += (char_wx + 2*PPM_FNT_BORDER)) {
        ppm_cavas_pixel (pcavas, char_wx / 2 + PPM_FNT_BORDER + i + PPM_OFF_X, PPM_OFF_Y - 2, color_shift);
        ppm_cavas_pixel (pcavas, char_wx / 2 + PPM_FNT_BORDER + i + PPM_OFF_X, PPM_OFF_Y - 3, color_shift);
        sprintf ((char *)buf1, "%02X", cnt);
        cnt ++;
        ppm_cavas_drawstring (pcavas
            , char_wx / 2 + PPM_FNT_BORDER + i + PPM_OFF_X - ppm_cavas_getfont_width(pcavas)
            , PPM_OFF_Y - 3 - ppm_cavas_getfont_height(pcavas) - 1
            , color_shift, color_background, buf1);
    }
    ppm_bitblit_from (&fd, pcavas, 0, 0, 0, 0, pcavas->xmax, pcavas->ymax);

    // y axis
    pcavas = ppm_cavas_resize (pcavas, PPM_OFF_X, (char_hy + 2*PPM_FNT_BORDER) * pic_ynum + PPM_OFF_Y * 2, 8);
    if (NULL == pcavas) {
        printf ("error in malloc memory 2\n");
        return -1;
    }

    // right
    // draw line:
    ppm_cavas_fill (pcavas, color_background);
    for (i = 0; i < (char_hy + 2*PPM_FNT_BORDER) * pic_ynum; i ++) {
        ppm_cavas_pixel (pcavas, 0, i + PPM_OFF_Y, color_dot);
    }
    // draw dot and mark
    cnt = 0;
    for (i = 0; i < (char_hy + 2*PPM_FNT_BORDER) * pic_ynum; i += (char_hy + 2*PPM_FNT_BORDER)) {
        ppm_cavas_pixel (pcavas, 1, char_hy / 2 + PPM_FNT_BORDER + i + PPM_OFF_Y, color_shift);
        ppm_cavas_pixel (pcavas, 2, char_hy / 2 + PPM_FNT_BORDER + i + PPM_OFF_Y, color_shift);
        sprintf ((char *)buf1, "%02X", cnt);
        cnt ++;
        ppm_cavas_drawstring (pcavas
            , 4
            , char_hy / 2 + PPM_FNT_BORDER + i + PPM_OFF_Y - ppm_cavas_getfont_height(pcavas)/2 - 1
            , color_shift, color_background, buf1);
    }
    ppm_bitblit_from (&fd, pcavas, PPM_OFF_X + (char_wx + 2*PPM_FNT_BORDER) * pic_xnum, 0, 0, 0, pcavas->xmax, pcavas->ymax);

    // left
    // draw line:
    ppm_cavas_fill (pcavas, color_background);
    for (i = 0; i < (char_hy + 2*PPM_FNT_BORDER) * pic_ynum; i ++) {
        ppm_cavas_pixel (pcavas, PPM_OFF_X - 1, i + PPM_OFF_Y, color_dot);
    }
    // draw dot
    cnt = 0;
    for (i = 0; i < (char_hy + 2*PPM_FNT_BORDER) * pic_ynum; i += (char_hy + 2*PPM_FNT_BORDER)) {
        ppm_cavas_pixel (pcavas, PPM_OFF_X - 2, char_hy / 2 + PPM_FNT_BORDER + i + PPM_OFF_Y, color_shift);
        ppm_cavas_pixel (pcavas, PPM_OFF_X - 3, char_hy / 2 + PPM_FNT_BORDER + i + PPM_OFF_Y, color_shift);
        sprintf ((char *)buf1, "%02X", cnt);
        cnt ++;
        ppm_cavas_drawstring (pcavas
            , PPM_OFF_X - 3 - (ppm_cavas_getfont_width(pcavas) + 1) * 2
            , char_hy / 2 + PPM_FNT_BORDER + i + PPM_OFF_Y - ppm_cavas_getfont_height(pcavas)/2 - 1
            , color_shift, color_background, buf1);

    }
    ppm_bitblit_from (&fd, pcavas, 0, 0, 0, 0, pcavas->xmax, pcavas->ymax);

    ppm_cavas_destroy (pcavas);
    return 0;
}

void
bdf2c_fontpic_clear()
{
    if (chbuf) {
        bdf2c_fontpic_draw_metric();
        ppm_cavas_destroy (chbuf);
        chbuf = NULL;
        ppm_close (&fd);
    }
}

void
bdf2c_fontpic_add_cavas (ppm_cavas_t * pchbuf, size_t width, size_t height, int encoding, char flag_shifted)
{
    size_t x;
    size_t y;
    current_num = encoding;
    x = current_num % pic_xnum;
    y = current_num / pic_xnum;
    x *= (char_wx + 2 * PPM_FNT_BORDER);
    y *= (char_hy + 2 * PPM_FNT_BORDER);
    x += PPM_OFF_X;
    y += PPM_OFF_Y;

#define UNUSED_VARIABLE(a) ((void)(a))
#if (PPM_FNT_BORDER > 0)
{
    uint8_t * color1;
    size_t i;
    size_t j;

    // lines around box
    color1 = color_line;
    if (flag_shifted) {
        color1 = color_shift;
    }

    for (j = 0; j < width + 2*PPM_FNT_BORDER; j ++) {
        for (i = 0; i < PPM_FNT_BORDER; i ++) {
            ppm_cavas_pixel (pchbuf, j, 0 + i, color1);
            ppm_cavas_pixel (pchbuf, j, height + 2*PPM_FNT_BORDER - 1 - i, color1);
        }
    }
    for (i = 0; i < height + 2*PPM_FNT_BORDER; i ++) {
        for (j = 0; j < PPM_FNT_BORDER; j ++) {
            ppm_cavas_pixel (pchbuf, 0 + j, i, color1);
            ppm_cavas_pixel (pchbuf, width + 2*PPM_FNT_BORDER - 1 - j, i, color1);
        }
    }
}
#else
    UNUSED_VARIABLE(flag_shifted);
    UNUSED_VARIABLE(color_line);
#endif
    ppm_bitblit_from (&fd, pchbuf, x, y, 0, 0, width + 2*PPM_FNT_BORDER, height + 2*PPM_FNT_BORDER);
    current_num ++;
}

// bitmap == NULL, place a holder sign
// flag_shifted, 1=if there's a BBX value for x shift > 0 in BDF font file
void
bdf2c_fontpic_add (uint8_t *bitmap, size_t width, size_t height, int encoding, char flag_shifted)
{
    ppm_cavas_fill (chbuf, color_background);
    if (NULL == bitmap) {
        assert (width >= 16);
        assert (height >= 16);
        DumpCharacter2Cavas (chbuf, PPM_FNT_BORDER + (width - 16)/2, PPM_FNT_BORDER + (height - 16)/2, bitmap_hznone, 16, 16, color_none, color_background);
    } else {
        DumpCharacter2Cavas (chbuf, PPM_FNT_BORDER, PPM_FNT_BORDER, bitmap, width, height, color_dot, color_background);
    }
    bdf2c_fontpic_add_cavas (chbuf, width, height, encoding, flag_shifted);
}


void
bdf2c_fontpic_add_fntchar (unsigned char c, char flag_shifted)
{
    ppm_cavas_fill (chbuf, color_background);
    if (0 != ppm_cavas_fontdraw (chbuf, PPM_FNT_BORDER, PPM_FNT_BORDER, color_dot, color_background, c)) {
        size_t width = ppm_cavas_width(chbuf);
        size_t height = ppm_cavas_height(chbuf);
        if (PPM_FNT_BORDER + width / 2 < 8) {
            width = 0;
        } else {
            width = PPM_FNT_BORDER + width / 2 - 8;
        }
        if (PPM_FNT_BORDER + height / 2 < 8) {
            height = 0;
        } else {
            height = PPM_FNT_BORDER + height/2 - 8;
        }
        DumpCharacter2Cavas (chbuf
            , width
            , height
            , bitmap_hznone, 16, 16, color_none, color_background);
    }
    bdf2c_fontpic_add_cavas (chbuf, ppm_cavas_getfont_width(chbuf), ppm_cavas_getfont_height(chbuf), c, flag_shifted);
}

void
bdf2c_fontpic_setfont_data (fontdata_t *fntptr)
{
    //curfont = fntptr;
    ppm_cavas_setfont (chbuf, fntptr);
}

#if DEBUG

char bitmaphz[] = {
    0x00, 0x00,
    0x27, 0xfe,
    0x10, 0x90,
    0x10, 0x90,
    0x03, 0xfc,
    0x02, 0x94,
    0xf2, 0x94,
    0x13, 0x0c,
    0x12, 0x04,
    0x13, 0xfc,
    0x12, 0x04,
    0x12, 0x04,
    0x13, 0xfc,
    0x12, 0x04,
    0x28, 0x00,
    0x47, 0xfe,
};

#define BITSZ_OF(a) (sizeof (a) * 8)
void RotateBitmap(uint8_t *bitmap, int shift, int width, int height)
{
    int y;

    if ((shift <= 0) || (shift >= width)) {
        fprintf(stderr, "Waring: This shift isn't supported: w=%d,h=%d, shift=%2d; ignored!!\n", shift, width, height);
        return;
    }
#if DEBUG
    // debug
    if ((size_t)shift >= BITSZ_OF (uint8_t)) {
        fprintf(stderr, "Waring: This shift: w=%d,h=%d, shift=%2d\n", width, height, shift);
    }
#endif

    uint8_t * p0;
    uint8_t * p1;
    uint8_t * p2;
    uint8_t val;
    size_t byteshift;
    byteshift = shift % BITSZ_OF (uint8_t);

    for (y = 0; y < height; ++y) {
        p0 = &bitmap[y * ((width + BITSZ_OF (uint8_t) - 1) / BITSZ_OF (uint8_t))];
        p2 = p0 + ((width + BITSZ_OF (uint8_t) - 1) / BITSZ_OF (uint8_t)) - 1;
        p1 = p2 + 1 - ((shift + BITSZ_OF (uint8_t) - 1) / BITSZ_OF (uint8_t));
#if DEBUG
if (shift < BITSZ_OF (uint8_t)) {
    assert (p1 == p2);
} else if (shift < BITSZ_OF (uint8_t) * 2) {
    assert (p1 + 1 == p2);
}
#endif
        for (; p2 >= p0; p2 --) {
            val = 0;
            if (p1 >= p0) {
                val = *p1;
                assert (byteshift < BITSZ_OF (uint8_t));
                if (byteshift > 0) {
                    val >>= byteshift;
                    if (p1 > p0) {
                        val |= (*(p1-1) << (BITSZ_OF (uint8_t) - byteshift));
                    }
                }
                p1 --;
            }
            *p2 = val;
        }
    }
}

#define MAX_FONT_ITEMS 100
// 65536
void
test_ppm2(int shift)
{
    int i;
    // test RotateBitmap
    char buf[sizeof(bitmaphz)];
    memmove (buf, bitmaphz, sizeof (bitmaphz));
    RotateBitmap(buf, shift, 16, 16);

    bdf2c_fontpic_init ("test2.ppm", MAX_FONT_ITEMS, 16, 16, "tstfont");
    for (i = 0; i < MAX_FONT_ITEMS; i ++) {
        bdf2c_fontpic_add (buf, 16, 16, i, 1);
    }
    bdf2c_fontpic_clear ();
}


// 256
void
test_ppm3(int shift)
{
    int i;

#define GENERATE_FONTDATA_TEST(val) \
    bdf2c_fontpic_init ("test-" #val ".ppm", fontdata_get_charnumber(val), fontdata_get_width(val), fontdata_get_height(val), #val); \
    bdf2c_fontpic_setfont_data (val); \
    for (i = 0; i < 256; i ++) { \
        bdf2c_fontpic_add_fntchar (i, 0); \
    } \
    bdf2c_fontpic_clear ();

    /*
    bdf2c_fontpic_init ("test-g_fontdata_6x8_small.ppm", fontdata_get_charnumber(g_fontdata_6x8_small), fontdata_get_width(g_fontdata_6x8_small), fontdata_get_height(g_fontdata_6x8_small), "g_fontdata_6x8_small");
    bdf2c_fontpic_setfont_data (g_fontdata_6x8_small);
    for (i = 0; i < 256; i ++) {
        bdf2c_fontpic_add_fntchar (i, 0);
    }
    bdf2c_fontpic_clear ();
    */

    GENERATE_FONTDATA_TEST (g_font5x8_0);
    //GENERATE_FONTDATA_TEST (g_font6x8_0);
    //GENERATE_FONTDATA_TEST (g_fontdata_4x6_tiny);
    //GENERATE_FONTDATA_TEST (g_fontdata_8x16);

    //GENERATE_FONTDATA_TEST (g_fontdata_16x24digits);
    //GENERATE_FONTDATA_TEST (g_fontdata_big_digits);
    //GENERATE_FONTDATA_TEST (g_fontdata_medium_digits);
    //GENERATE_FONTDATA_TEST (g_font5x8_1);
    //GENERATE_FONTDATA_TEST (g_font5x8_2);
    //GENERATE_FONTDATA_TEST (g_fontdata_6x8_small);
}


void
test_set_hz(ppm_cavas_t *pcavas, size_t offx, size_t offy)
{
    unsigned char buf1[100];
    DumpCharacter2Cavas (pcavas, offx, offy, bitmaphz, 16, 16, color_dot, color_background);

    sprintf ((char *)buf1, "ABCabc0123: %02X", 391);
    ppm_cavas_drawstring (pcavas, offx + 16, offy, color_shift, color_background, buf1);
}

#define MAXX 1024
#define MAXY 768
void
test_ppm1()
{
    int i;
    int j;
    char val[4] = {0, 132, 149, 12};
    ppm_cavas_t * cavas;
    ppm_cavas_t * buffer;
    ppm_file_t * pfile = &fd;

    if (0 != ppm_create (pfile, "test1.ppm", MAXX, MAXY, 255)) {
        printf ("error in create ppm file tmp.ppm\n");
        return;
    }
    buffer = ppm_cavas_create (MAXX, MAXY/4, 8);

    // fill the small block
    cavas = ppm_cavas_create (MAXX/4, MAXY/4, 8);
    val[0] = 0; val[1] = 0; val[2] = 255; val[3] = 0;
    ppm_cavas_fill (cavas, val);

    val[0] = 0; val[1] = 132; val[2] = 149; val[3] = 12;
    for (i = 2; i < cavas->ymax - 2; i ++) {
        for (j = 2; j < cavas->xmax - 2; j ++) {
            ppm_cavas_pixel (cavas, j, i, val);
        }
    }
    val[0] = 0; val[1] = 255; val[2] = 0; val[3] = 0;
    for (j = 1; j < cavas->xmax - 1; j ++) {
        ppm_cavas_pixel (cavas, j, 1, val);
        ppm_cavas_pixel (cavas, j, cavas->ymax - 2, val);
    }
    for (i = 1; i < cavas->ymax - 1; i ++) {
        ppm_cavas_pixel (cavas, 1, i, val);
        ppm_cavas_pixel (cavas, cavas->xmax - 2, i, val);
    }
    test_set_hz (cavas, 5, 5);

#if 0
    ppm_bitblit_from (&pfile, cavas, (MAXX - cavas->xmax)/2, (MAXY - cavas->ymax)/2, 0, 0, cavas->xmax, cavas->ymax);
#else
    // fill buffer
    for (j = 0; j < MAXX; j += cavas->xmax) {
        ppm_cavas_bitblit (buffer, cavas, j, 0, 0, 0, cavas->xmax, cavas->ymax);
    }
    for (i = 0; i < MAXY; i += cavas->ymax) {
        ppm_bitblit_from (pfile, buffer, 0, i, 0, 0, buffer->xmax, buffer->ymax);
    }
#endif
    ppm_cavas_destroy (cavas);
    ppm_cavas_destroy (buffer);
    ppm_close (pfile);
}

int
main(int argc, char * argv[])
{
    test_ppm1();
    if (argc < 2) {
        test_ppm2(1);
        test_ppm3(1);
    } else {
        test_ppm2(atoi(argv[1]));
        test_ppm3(atoi(argv[1]));
    }
    return 0;
}
#endif
