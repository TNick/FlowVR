/******* COPYRIGHT ************************************************
*                                                                 *
*                         FlowVR Render                           *
*                   Parallel Rendering Modules                    *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
*  ALL RIGHTS RESERVED.                                           *
*                                                                 *
* This source is covered by the GNU GPL, please refer to the      *
* COPYING file for further information.                           *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Clement Menier.                                              *
*                                                                 * 
*******************************************************************
*                                                                 *
* File: ./src/utils/genftfont.cpp                                 *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */

/* Modified for FreeType 2 by Jeremie Allard. */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <iostream>

// FreeType Headers

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TRUETYPE_IDS_H

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#include <flowvr/render/texfont.h>

using namespace flowvr::render;

typedef struct {
  short width;
  short height;
  short xoffset;
  short yoffset;
  short advance;
  unsigned char *bitmap;
} PerGlyphInfo, *PerGlyphInfoPtr;

int format = TXF_FORMAT_BYTE;
int gap = 3;

FT_Library library;
FT_Face face;

void getMetric(int c, TexGlyphInfo * tgi)
{
  int glyph_index = FT_Get_Char_Index( face, c );

  tgi->c = c;
  if (glyph_index==0)
  {
    tgi->width = 0;
    tgi->height = 0;
    tgi->xoffset = 0;
    tgi->yoffset = 0;
    tgi->dummy = 0;
    tgi->advance = 0;
  }
  else
  {
    FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER );
    tgi->width = face->glyph->bitmap.width;
    tgi->height =  face->glyph->bitmap.rows;
    tgi->xoffset = face->glyph->bitmap_left;
    tgi->yoffset = face->glyph->bitmap_top - tgi->height;
    tgi->dummy = 0;
    tgi->advance = face->glyph->advance.x>>6;
  }
}

int
glyphCompare(const void *a, const void *b)
{
  unsigned char *c1 = (unsigned char *) a;
  unsigned char *c2 = (unsigned char *) b;
  TexGlyphInfo tgi1;
  TexGlyphInfo tgi2;

  getMetric(*c1, &tgi1);
  getMetric(*c2, &tgi2);
  return tgi2.height - tgi1.height;
}

void
placeGlyph(int c, unsigned char *texarea, int stride, int x, int y)
{
  FT_Bitmap *bitmap;
  int i;

  TexGlyphInfo tgi;
  getMetric(c, &tgi);

  bitmap = &(face->glyph->bitmap);
  if (bitmap->buffer)
  {
    for (i = 0; i < bitmap->rows; i++)
    {
      memcpy(texarea+stride*(y+i)+x, bitmap->buffer + bitmap->pitch*(bitmap->rows-i-1), bitmap->width);
    }
  }
}

char *
nodupstring(char *s)
{
  int len, i, p;
  char *newc;

  len = strlen(s);
  newc = (char *) malloc(len + 1);
  p = 0;
  for (i = 0; i < len; i++) {
    if (!strchr(newc, s[i])) {
      newc[p] = s[i];
      p++;
      newc[p] = '\0'; // = (char*)realloc(newc, p + 1);
    }
  }
  newc = (char*)realloc(newc, p + 1);
  return newc;
}

int
main(int argc, char *argv[])
{
  int texw, texh;
  unsigned char *texarea, *texbitmap;
  FILE *file;
  int len, stride;
  unsigned char *glist;
  int width, height, size;
  int px, py, maxheight;
  TexGlyphInfo tgi;
  int usageError = 0;
  const char *fontname;
  const char *filename;
  int endianness;
  int i, j;

  texw = texh = 256;
  //glist = (unsigned char*)" ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijmklmnopqrstuvwxyz?.;,!*:\"/+@#$%^&()";
  glist = (unsigned char*)malloc (255-32+1);
  for (i=32;i<255;i++)
    glist[i-32] = (unsigned char)i;
  glist[255-32]='\0';
  fontname = "arial.ttf";
  filename = "default.txf";
  size = 24;

  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-w")) {
      i++;
      texw = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-h")) {
      i++;
      texh = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-s")) {
      i++;
      size = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-gap")) {
      i++;
      gap = atoi(argv[i]);
    } else if (!strcmp(argv[i], "-byte")) {
      format = TXF_FORMAT_BYTE;
    } else if (!strcmp(argv[i], "-bitmap")) {
      format = TXF_FORMAT_BITMAP;
    } else if (!strcmp(argv[i], "-glist")) {
      i++;
      glist = (unsigned char *) argv[i];
    } else if (!strcmp(argv[i], "-fn")) {
      i++;
      fontname = argv[i];
    } else if (!strcmp(argv[i], "-file")) {
      i++;
      filename = argv[i];
    } else {
      usageError = 1;
    }
  }

  if (usageError) {
    putchar('\n');
    printf("usage: texfontgen [options]\n");
    printf(" -w #          textureWidth (def=%d)\n", texw);
    printf(" -h #          textureHeight (def=%d)\n", texh);
    printf(" -h #          character size (def=%d)\n", size);
    printf(" -gap #        gap between glyphs (def=%d)\n", gap);
    printf(" -bitmap       use a bitmap encoding (aliased, more compact)\n");
    printf(" -byte         use a byte encoding (default)\n");
    printf(" -glist ABC    glyph list (def=%s)\n", glist);
    printf(" -fn name      font filename (def=%s)\n", fontname);
    printf(" -file name    output file for textured font (def=%s)\n", fontname);
    putchar('\n');
    exit(1);
  }
  texarea = (unsigned char*) calloc(texw * texh, sizeof(unsigned char));
  glist = (unsigned char *) nodupstring((char *) glist);

  int error;
  if ((error=FT_Init_FreeType( &library )))
  { std::cerr << "FreeType Init failed."<<std::endl; return 1; }

  if ((error=FT_New_Face(library, fontname, 0, &face)))
  {
    std::cerr << "FreeType New Face "<<fontname<<" failed."<<std::endl;
    return 1;
  }

  if ((error=FT_Set_Pixel_Sizes(face, 0, size)))
  {
    std::cerr << "FreeType Set Pixel Sizes "<<size<<" failed."<<std::endl;
    return 1;
  }

  FT_Select_Charmap( face, FT_ENCODING_ADOBE_LATIN_1 );

  int max_ascent = face->size->metrics.ascender>>6;
  int max_descent = -face->size->metrics.descender>>6;

  std::cout << "FACE max_ascent="<<max_ascent<<" max_descent="<<max_descent<<" height="<<face->size->metrics.height*(1.0/64)<<std::endl;
  
  len = strlen((char *) glist);
  qsort(glist, len, sizeof(unsigned char), glyphCompare);

  file = fopen(filename, "wb");
  fwrite("\377txf", 1, 4, file);
  endianness = 0x12345678;
  assert(sizeof(int) == 4);  /* Ensure external file format size. */
  fwrite(&endianness, sizeof(int), 1, file);
  fwrite(&format, sizeof(int), 1, file);
  fwrite(&texw, sizeof(int), 1, file);
  fwrite(&texh, sizeof(int), 1, file);
  fwrite(&max_ascent, sizeof(int), 1, file);
  fwrite(&max_descent, sizeof(int), 1, file);
  fwrite(&len, sizeof(int), 1, file);

  px = gap;
  py = gap;
  maxheight = 0;
  for (i = 0; i < len; i++) {
    if (glist[i] != 0) {  /* If not already processed... */

      /* Try to find a character from the glist that will fit on the
         remaining space on the current row. */

      int foundWidthFit = 0;
      int c = i;

      getMetric(glist[i], &tgi);
      width = tgi.width;
      height = tgi.height;
      if (height > 0 && width > 0) {
        for (j = i; j < len;) {
	  if (glist[j] != 0) {
	    if (height > 0 && width > 0) {
	      if (px + width + gap < texw) {
		foundWidthFit = 1;
		if (j != i) {
		  i--;  /* Step back so i loop increment leaves us at same character. */
		}
		break;
	      }
	    }
	  }
          j++;
          getMetric(glist[j], &tgi);
          width = tgi.width;
          height = tgi.height;
        }

        /* If a fit was found, use that character; otherwise, advance a line
           in  the texture. */
        if (foundWidthFit) {
          if (height > maxheight) {
            maxheight = height;
          }
          c = j;
        } else {
	  printf("\n");
          getMetric(glist[i], &tgi);
          width = tgi.width;
          height = tgi.height;

          py += maxheight + gap;
          px = gap;
          maxheight = height;
          if (py + height + gap >= texh) {
            printf("Overflowed texture space.\n");
            exit(1);
          }
          c = i;
        }

	printf("'%c' ",glist[c]);


        /* Place the glyph in the texture image. */
        placeGlyph(glist[c], texarea, texw, px, py);

        /* Assign glyph's texture coordinate. */
        tgi.x = px;
        tgi.y = py;

	/* Advance by glyph width, remaining in the current line. */
        px += width + gap;
      } else {
	/* No texture image; assign invalid bogus texture coordinates. */
        tgi.x = -1;
        tgi.y = -1;
      }
      glist[c] = 0;     /* Mark processed; don't process again. */
      assert(sizeof(tgi) == 12);  /* Ensure external file format size. */
      fwrite(&tgi, sizeof(tgi), 1, file);
    }
  }
  printf("\n");

  switch (format) {
  case TXF_FORMAT_BYTE:
    fwrite(texarea, texw * texh, 1, file);
    break;
  case TXF_FORMAT_BITMAP:
    stride = (texw + 7) >> 3;
    texbitmap = (unsigned char *) calloc(stride * texh, 1);
    for (i = 0; i < texh; i++) {
      for (j = 0; j < texw; j++) {
        if (texarea[i * texw + j] >= 128) {
          texbitmap[i * stride + (j >> 3)] |= 1 << (j & 7);
        }
      }
    }
    fwrite(texbitmap, stride * texh, 1, file);
    free(texbitmap);
    break;
  default:
    printf("Unknown texture font format.\n");
    exit(1);
  }
  free(texarea);
  fclose(file);
  return 0;
}
