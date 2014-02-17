/******* COPYRIGHT ************************************************
*                                                                 *
*                         FlowVR Render                           *
*                   Parallel Rendering Library                    *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
*  ALL RIGHTS RESERVED.                                           *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING-LIB file for further information.                       *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Clement Menier.                                              *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: ./src/librender/texfont.cpp                               *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */

/* Modified by Jeremie Allard to remove all OpenGL dependencies */

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <flowvr/render/texfont.h>

namespace flowvr
{

namespace render
{

/* byte swap a 32-bit value */
#define SWAPL(x, n) { \
                 n = ((char *) (x))[0];\
                 ((char *) (x))[0] = ((char *) (x))[3];\
                 ((char *) (x))[3] = n;\
                 n = ((char *) (x))[1];\
                 ((char *) (x))[1] = ((char *) (x))[2];\
                 ((char *) (x))[2] = n; }

/* byte swap a short */
#define SWAPS(x, n) { \
                 n = ((char *) (x))[0];\
                 ((char *) (x))[0] = ((char *) (x))[1];\
                 ((char *) (x))[1] = n; }

TexGlyphVertexInfo *
txfGetTCVI(TexFont * txf, int c)
{
  TexGlyphVertexInfo *tgvi;

  /* Automatically substitute uppercase letters with lowercase if not
     uppercase available (and vice versa). */
  if ((c >= txf->min_glyph) && (c < txf->min_glyph + txf->range)) {
    tgvi = txf->lut[c - txf->min_glyph];
    if (tgvi) {
      return tgvi;
    }
    if (islower(c)) {
      c = toupper(c);
      if ((c >= txf->min_glyph) && (c < txf->min_glyph + txf->range)) {
        return txf->lut[c - txf->min_glyph];
      }
    }
    if (isupper(c)) {
      c = tolower(c);
      if ((c >= txf->min_glyph) && (c < txf->min_glyph + txf->range)) {
        return txf->lut[c - txf->min_glyph];
      }
    }
  }
  fprintf(stderr, "texfont: tried to access unavailable font character \"%c\" (%d)\n",
    isprint(c) ? c : ' ', c);
  //abort();
  return txf->lut[' ' - txf->min_glyph];
  /* NOTREACHED */
}

static char *lastError;

char *
txfErrorString(void)
{
  return lastError;
}

TexFont *
txfLoadFont(const char *filename)
{
  TexFont *txf;
  FILE *file;
  float w, h, xstep, ystep;
  char fileid[4], tmp;
  unsigned char *texbitmap;
  int min_glyph, max_glyph;
  int endianness, swap, format, stride, width, height;
  int i, j, got;

  txf = NULL;
  file = fopen(filename, "rb");
  if (file == NULL) {
    lastError = (char*) "file open failed.";
    goto error;
  }
  txf = (TexFont *) malloc(sizeof(TexFont));
  if (txf == NULL) {
    lastError = (char*) "out of memory.";
    goto error;
  }
  /* For easy cleanup in error case. */
  txf->tgi = NULL;
  txf->tgvi = NULL;
  txf->lut = NULL;
  txf->teximage = NULL;

  got = fread(fileid, 1, 4, file);
  if (got != 4 || strncmp(fileid, "\377txf", 4)) {
    lastError = (char*) "not a texture font file.";
    goto error;
  }
  assert(sizeof(int) == 4);  /* Ensure external file format size. */
  got = fread(&endianness, sizeof(int), 1, file);
  if (got == 1 && endianness == 0x12345678) {
    swap = 0;
  } else if (got == 1 && endianness == 0x78563412) {
    swap = 1;
  } else {
    lastError = (char*) "not a texture font file.";
    goto error;
  }
#define EXPECT(n) if (got != n) { lastError = (char*) "premature end of file."; goto error; }
  got = fread(&format, sizeof(int), 1, file);
  EXPECT(1);
  got = fread(&txf->tex_width, sizeof(int), 1, file);
  EXPECT(1);
  got = fread(&txf->tex_height, sizeof(int), 1, file);
  EXPECT(1);
  got = fread(&txf->max_ascent, sizeof(int), 1, file);
  EXPECT(1);
  got = fread(&txf->max_descent, sizeof(int), 1, file);
  EXPECT(1);
  got = fread(&txf->num_glyphs, sizeof(int), 1, file);
  EXPECT(1);

  if (swap) {
    SWAPL(&format, tmp);
    SWAPL(&txf->tex_width, tmp);
    SWAPL(&txf->tex_height, tmp);
    SWAPL(&txf->max_ascent, tmp);
    SWAPL(&txf->max_descent, tmp);
    SWAPL(&txf->num_glyphs, tmp);
  }
  txf->tgi = (TexGlyphInfo *) malloc(txf->num_glyphs * sizeof(TexGlyphInfo));
  if (txf->tgi == NULL) {
    lastError = (char*) "out of memory.";
    goto error;
  }
  assert(sizeof(TexGlyphInfo) == 12);  /* Ensure external file format size. */
  got = fread(txf->tgi, sizeof(TexGlyphInfo), txf->num_glyphs, file);
  EXPECT(txf->num_glyphs);

  if (swap) {
    for (i = 0; i < txf->num_glyphs; i++) {
      SWAPS(&txf->tgi[i].c, tmp);
      SWAPS(&txf->tgi[i].x, tmp);
      SWAPS(&txf->tgi[i].y, tmp);
    }
  }
  txf->tgvi = (TexGlyphVertexInfo *)
    malloc(txf->num_glyphs * sizeof(TexGlyphVertexInfo));
  if (txf->tgvi == NULL) {
    lastError = (char*) "out of memory.";
    goto error;
  }
  w = txf->tex_width;
  h = txf->tex_height;
  xstep = 0.5 / w;
  ystep = 0.5 / h;

  {
    int x0 = 0, x1 = 0, y0 = 0, y1 = 0;
    
    for (i = 0; i < txf->num_glyphs; i++) {
      TexGlyphInfo *tgi;
      
      tgi = &txf->tgi[i];
      txf->tgvi[i].t0[0] = tgi->x / w - xstep;
      txf->tgvi[i].t0[1] = tgi->y / h - ystep;
      txf->tgvi[i].v0[0] = tgi->xoffset;
      txf->tgvi[i].v0[1] = tgi->yoffset;
      txf->tgvi[i].t1[0] = (tgi->x + tgi->width) / w + xstep;
      txf->tgvi[i].t1[1] = tgi->y / h - ystep;
      txf->tgvi[i].v1[0] = tgi->xoffset + tgi->width;
      txf->tgvi[i].v1[1] = tgi->yoffset;
      txf->tgvi[i].t2[0] = (tgi->x + tgi->width) / w + xstep;
      txf->tgvi[i].t2[1] = (tgi->y + tgi->height) / h + ystep;
      txf->tgvi[i].v2[0] = tgi->xoffset + tgi->width;
      txf->tgvi[i].v2[1] = tgi->yoffset + tgi->height;
      txf->tgvi[i].t3[0] = tgi->x / w - xstep;
      txf->tgvi[i].t3[1] = (tgi->y + tgi->height) / h + ystep;
      txf->tgvi[i].v3[0] = tgi->xoffset;
      txf->tgvi[i].v3[1] = tgi->yoffset + tgi->height;
      txf->tgvi[i].advance = tgi->advance;
      
      if (i==0)
      {
	x0 = tgi->xoffset; x1 = tgi->xoffset+tgi->width;
	y0 = tgi->yoffset; y1 = tgi->yoffset+tgi->height;
      }
      else
      {
	if (tgi->xoffset < x0) x0 = tgi->xoffset;
	if (tgi->xoffset+tgi->width > x1) x1 = tgi->xoffset+tgi->width;
	if (tgi->yoffset < y0) y0 = tgi->yoffset;
	if (tgi->yoffset+tgi->height > y1) y1 = tgi->yoffset+tgi->height;
      }

    }
    
  }

  //fprintf(stderr,"Font x0=%d x1=%d y0=%d y1=%d max_acsent=%d max_descent=%d\n",x0,x1,y0,y1,txf->max_ascent,txf->max_descent);

  min_glyph = txf->tgi[0].c;
  max_glyph = txf->tgi[0].c;
  for (i = 1; i < txf->num_glyphs; i++) {
    if (txf->tgi[i].c < min_glyph) {
      min_glyph = txf->tgi[i].c;
    }
    if (txf->tgi[i].c > max_glyph) {
      max_glyph = txf->tgi[i].c;
    }
  }
  txf->min_glyph = min_glyph;
  txf->range = max_glyph - min_glyph + 1;

  txf->lut = (TexGlyphVertexInfo **)
    calloc(txf->range, sizeof(TexGlyphVertexInfo *));
  if (txf->lut == NULL) {
    lastError = (char*) "out of memory.";
    goto error;
  }
  for (i = 0; i < txf->num_glyphs; i++) {
    //printf("TEXFONT: Found char '%c'\n",txf->tgi[i].c);
    txf->lut[txf->tgi[i].c - txf->min_glyph] = &txf->tgvi[i];
  }

  switch (format) {
  case TXF_FORMAT_BYTE:
    txf->teximage = (unsigned char *)
      malloc(txf->tex_width * txf->tex_height);
    if (txf->teximage == NULL) {
      lastError = (char*) "out of memory.";
      goto error;
    }
    got = fread(txf->teximage, 1, txf->tex_width * txf->tex_height, file);
    EXPECT(txf->tex_width * txf->tex_height);
    break;
  case TXF_FORMAT_BITMAP:
    width = txf->tex_width;
    height = txf->tex_height;
    stride = (width + 7) >> 3;
    texbitmap = (unsigned char *) malloc(stride * height);
    if (texbitmap == NULL) {
      lastError = (char*) "out of memory.";
      goto error;
    }
    got = fread(texbitmap, 1, stride * height, file);
    EXPECT(stride * height);
    txf->teximage = (unsigned char *) calloc(width * height, 1);
    if (txf->teximage == NULL) {
      lastError = (char*) "out of memory.";
      goto error;
    }
    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        if (texbitmap[i * stride + (j >> 3)] & (1 << (j & 7))) {
          txf->teximage[i * width + j] = 255;
        }
      }
    }
    free(texbitmap);
    break;
  }

  fclose(file);
  return txf;

error:

  if (txf) {
    if (txf->tgi)
      free(txf->tgi);
    if (txf->tgvi)
      free(txf->tgvi);
    if (txf->lut)
      free(txf->lut);
    if (txf->teximage)
      free(txf->teximage);
    free(txf);
  }
  if (file)
    fclose(file);
  return NULL;
}

void
txfUnloadFont(
  TexFont * txf)
{
  if (txf->teximage) {
    free(txf->teximage);
  }
  free(txf->tgi);
  free(txf->tgvi);
  free(txf->lut);
  free(txf);
}

void
txfGetStringMetrics(
  TexFont * txf,
  const char *string,
  int len,
  int *width,
  int *max_ascent,
  int *max_descent)
{
  TexGlyphVertexInfo *tgvi;
  int w, i;

  w = 0;
  for (i = 0; i < len; i++) {
    if (string[i] == 27) {
      switch (string[i + 1]) {
      case 'M':
        i += 4;
        break;
      case 'T':
        i += 7;
        break;
      case 'L':
        i += 7;
        break;
      case 'F':
        i += 13;
        break;
      }
    } else {
      tgvi = txfGetTCVI(txf, string[i]);
      w += (int)tgvi->advance;
    }
  }
  *width = w;
  *max_ascent = txf->max_ascent;
  *max_descent = txf->max_descent;
}

int
txfInFont(TexFont * txf, int c)
{
  /* NOTE: No uppercase/lowercase substitution. */
  if ((c >= txf->min_glyph) && (c < txf->min_glyph + txf->range)) {
    if (txf->lut[c - txf->min_glyph]) {
      return 1;
    }
  }
  return 0;
}


} // namespace render

} // namespace flowvr
