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
* File: ./include/flowvr/render/texfont.h                         *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */

/* Modified by Jeremie Allard to remove all OpenGL dependencies */

#ifndef FLOWVR_RENDER_TEXFONT_H
#define FLOWVR_RENDER_TEXFONT_H

namespace flowvr
{

namespace render
{

enum { TXF_FORMAT_BYTE	 = 0 };
enum { TXF_FORMAT_BITMAP = 1 };

typedef struct {
  unsigned short c;       /* Potentially support 16-bit glyphs. */
  unsigned char width;
  unsigned char height;
  signed char xoffset;
  signed char yoffset;
  signed char advance;
  char dummy;           /* Space holder for alignment reasons. */
  short x;
  short y;
} TexGlyphInfo;

typedef struct {
  float t0[2];
  short v0[2];
  float t1[2];
  short v1[2];
  float t2[2];
  short v2[2];
  float t3[2];
  short v3[2];
  float advance;
} TexGlyphVertexInfo;

typedef struct {
  unsigned int texobj;
  int tex_width;
  int tex_height;
  int max_ascent;
  int max_descent;
  int num_glyphs;
  int min_glyph;
  int range;
  unsigned char *teximage;
  TexGlyphInfo *tgi;
  TexGlyphVertexInfo *tgvi;
  TexGlyphVertexInfo **lut;
} TexFont;

extern char *txfErrorString(void);

extern TexFont *txfLoadFont(
  const char *filename);

extern void txfUnloadFont(
  TexFont * txf);

extern void txfGetMetrics(
  TexFont * txf,
  int *max_width,
  int *max_advance,
  int *max_ascent,
  int *max_descent);

extern void txfGetStringMetrics(
  TexFont * txf,
  const char *string,
  int len,
  int *width,
  int *max_ascent,
  int *max_descent);

#if 0
extern GLuint txfEstablishTexture(
  TexFont * txf,
  GLuint texobj,
  GLboolean setupMipmaps);

extern void txfBindFontTexture(
  TexFont * txf);
#endif

#if 0
extern void txfRenderGlyph(
  TexFont * txf,
  int c);

extern void txfRenderString(
  TexFont * txf,
  const char *string,
  int len);

extern void txfRenderFancyString(
  TexFont * txf,
  const char *string,
  int len);
#endif

extern TexGlyphVertexInfo *
txfGetTCVI(TexFont * txf, int c);

} // namespace render

} // namespace flowvr

#endif /* __TEXFONT_H__ */
