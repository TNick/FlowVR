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
* File: ./src/utils/simpletxf.cpp                                 *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/

/* Copyright (c) Mark J. Kilgard, 1997. */

/* This program is freely distributable without licensing fees  and is
   provided without guarantee or warrantee expressed or  implied. This
   program is -not- in the public domain. */

/* X compile line: cc -o simpletxf simpletxf.c texfont.c -lglut -lGLU -lGL -lXmu -lXext -lX11 -lm */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <flowvr/render/texfont.h>
#include <flowvr/utils/filepath.h>


flowvr::utils::FilePath datapath("FLOWVR_DATA_PATH");


using namespace flowvr::render;

// GL dependant bits from texfont.c

#ifndef GL_VERSION_1_1
#if defined(GL_EXT_texture_object) && defined(GL_EXT_texture)
#define glGenTextures glGenTexturesEXT
#define glBindTexture glBindTextureEXT
#ifndef GL_INTENSITY4
#define GL_INTENSITY4 GL_INTENSITY4_EXT
#endif
#else
#define USE_DISPLAY_LISTS
#endif
#endif

GLuint
txfEstablishTexture(
  TexFont * txf,
  GLuint texobj,
  GLboolean setupMipmaps)
{
  if (txf->texobj == 0) {
    if (texobj == 0) {
#if !defined(USE_DISPLAY_LISTS)
      glGenTextures(1, (GLuint *) &txf->texobj);
#else
      txf->texobj = glGenLists(1);
#endif
    } else {
      txf->texobj = texobj;
    }
  }
#if !defined(USE_DISPLAY_LISTS)
  glBindTexture(GL_TEXTURE_2D, txf->texobj);
#else
  glNewList(txf->texobj, GL_COMPILE);
#endif
  /* Use GL_INTENSITY4 as internal texture format since we want to use as
     little texture memory as possible. */
  if (setupMipmaps) {
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_INTENSITY4,
		      txf->tex_width, txf->tex_height,
		      GL_LUMINANCE, GL_UNSIGNED_BYTE, txf->teximage);
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_INTENSITY4,
		 txf->tex_width, txf->tex_height, 0,
		 GL_LUMINANCE, GL_UNSIGNED_BYTE, txf->teximage);
  }

#if defined(USE_DISPLAY_LISTS)
  glEndList();
  glCallList(txf->texobj);
#endif
  return txf->texobj;
}

void
txfBindFontTexture(
  TexFont * txf)
{
#if !defined(USE_DISPLAY_LISTS)
  glBindTexture(GL_TEXTURE_2D, txf->texobj);
#else
  glCallList(txf->texobj);
#endif
}

void
txfRenderGlyph(TexFont * txf, int c)
{
  TexGlyphVertexInfo *tgvi;

  tgvi = txfGetTCVI(txf, c);
  glBegin(GL_QUADS);
  glTexCoord2fv(tgvi->t0);
  glVertex2sv(tgvi->v0);
  glTexCoord2fv(tgvi->t1);
  glVertex2sv(tgvi->v1);
  glTexCoord2fv(tgvi->t2);
  glVertex2sv(tgvi->v2);
  glTexCoord2fv(tgvi->t3);
  glVertex2sv(tgvi->v3);
  glEnd();
  glTranslatef(tgvi->advance, 0.0, 0.0);
}

void
txfRenderString(
  TexFont * txf,
  char *string,
  int len)
{
  int i;

  for (i = 0; i < len; i++) {
    txfRenderGlyph(txf, string[i]);
  }
}

enum {
  MONO, TOP_BOTTOM, LEFT_RIGHT, FOUR
};

void
txfRenderFancyString(
  TexFont * txf,
  char *string,
  int len)
{
  TexGlyphVertexInfo *tgvi;
  GLubyte c[4][3];
  int mode = MONO;
  int i;

  for (i = 0; i < len; i++) {
    if (string[i] == 27) {
      switch (string[i + 1]) {
      case 'M':
        mode = MONO;
        glColor3ubv((GLubyte *) & string[i + 2]);
        i += 4;
        break;
      case 'T':
        mode = TOP_BOTTOM;
        memcpy(c, &string[i + 2], 6);
        i += 7;
        break;
      case 'L':
        mode = LEFT_RIGHT;
        memcpy(c, &string[i + 2], 6);
        i += 7;
        break;
      case 'F':
        mode = FOUR;
        memcpy(c, &string[i + 2], 12);
        i += 13;
        break;
      }
    } else {
      switch (mode) {
      case MONO:
        txfRenderGlyph(txf, string[i]);
        break;
      case TOP_BOTTOM:
        tgvi = txfGetTCVI(txf, string[i]);
        glBegin(GL_QUADS);
        glColor3ubv(c[0]);
        glTexCoord2fv(tgvi->t0);
        glVertex2sv(tgvi->v0);
        glTexCoord2fv(tgvi->t1);
        glVertex2sv(tgvi->v1);
        glColor3ubv(c[1]);
        glTexCoord2fv(tgvi->t2);
        glVertex2sv(tgvi->v2);
        glTexCoord2fv(tgvi->t3);
        glVertex2sv(tgvi->v3);
        glEnd();
        glTranslatef(tgvi->advance, 0.0, 0.0);
        break;
      case LEFT_RIGHT:
        tgvi = txfGetTCVI(txf, string[i]);
        glBegin(GL_QUADS);
        glColor3ubv(c[0]);
        glTexCoord2fv(tgvi->t0);
        glVertex2sv(tgvi->v0);
        glColor3ubv(c[1]);
        glTexCoord2fv(tgvi->t1);
        glVertex2sv(tgvi->v1);
        glColor3ubv(c[1]);
        glTexCoord2fv(tgvi->t2);
        glVertex2sv(tgvi->v2);
        glColor3ubv(c[0]);
        glTexCoord2fv(tgvi->t3);
        glVertex2sv(tgvi->v3);
        glEnd();
        glTranslatef(tgvi->advance, 0.0, 0.0);
        break;
      case FOUR:
        tgvi = txfGetTCVI(txf, string[i]);
        glBegin(GL_QUADS);
        glColor3ubv(c[0]);
        glTexCoord2fv(tgvi->t0);
        glVertex2sv(tgvi->v0);
        glColor3ubv(c[1]);
        glTexCoord2fv(tgvi->t1);
        glVertex2sv(tgvi->v1);
        glColor3ubv(c[2]);
        glTexCoord2fv(tgvi->t2);
        glVertex2sv(tgvi->v2);
        glColor3ubv(c[3]);
        glTexCoord2fv(tgvi->t3);
        glVertex2sv(tgvi->v3);
        glEnd();
        glTranslatef(tgvi->advance, 0.0, 0.0);
        break;
      }
    }
  }
}

int doubleBuffer = 1;
std::string filename = "fonts/default.txf";
TexFont *txf;
GLfloat angle = 20;

void
idle(void)
{
  angle += 0.01;
  glutPostRedisplay();
}

void 
visible(int vis)
{
  if (vis == GLUT_VISIBLE)
    glutIdleFunc(idle);
  else
    glutIdleFunc(NULL);
}

void
display(void)
{
  char *str;

  /* Clear the color buffer. */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();

  glRotatef(angle, 0, 0, 1);
  glTranslatef(-2.0, 0.0, 0.0);
  glScalef(1 / 60.0, 1 / 60.0, 1 / 60.0);

  glPushMatrix();
  glColor3f(0.0, 0.0, 1.0);
  str = (char*) "OpenGL is";
  txfRenderString(txf, str, strlen(str));
  glPopMatrix();

  glPushMatrix();
  glColor3f(1.0, 0.0, 0.0);
  glTranslatef(0.0, -60.0, 0.0);
  str = (char*) "the best.";
  txfRenderString(txf, str, strlen(str));
  glPopMatrix();

  glPopMatrix();

  /* Swap the buffers if necessary. */
  if (doubleBuffer) {
    glutSwapBuffers();
  }
}

int minifyMenu;

void
minifySelect(int value)
{
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, value);
  glutPostRedisplay();
}

int alphaMenu;

void
alphaSelect(int value)
{
  switch (value) {
  case GL_ALPHA_TEST:
    glDisable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 0.5);
    break;
  case GL_BLEND:
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    break;
  case GL_NONE:
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    break;
  }
}

void
mainSelect(int value)
{
  if (value == 666) {
    exit(0);
  }
}

int
main(int argc, char **argv)
{
  int i;

  glutInit(&argc, argv);
  for (i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-sb")) {
      doubleBuffer = 0;
    } else {
      filename = argv[i];
    }
  }
  if (filename.empty()) {
    fprintf(stderr, "usage: show [GLUT-options] [-sb] txf-file\n");
    exit(1);
  }
  datapath.findFile(filename);
  txf = txfLoadFont(filename.c_str());
  if (txf == NULL) {
    fprintf(stderr, "Problem loading %s\n", filename.c_str());
    exit(1);
  }

  if (doubleBuffer) {
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  } else {
    glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
  }
  glutInitWindowSize(300, 300);
  glutCreateWindow("texfont");
  glutDisplayFunc(display);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, 1.0, 0.1, 20.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 5.0,
    0.0, 0.0, 0.0,
    0.0, 1.0, 0.0);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  alphaSelect(GL_ALPHA_TEST);
  minifySelect(GL_NEAREST);

  txfEstablishTexture(txf, 0, GL_TRUE);

  glutVisibilityFunc(visible);

  minifyMenu = glutCreateMenu(minifySelect);
  glutAddMenuEntry("Nearest", GL_NEAREST);
  glutAddMenuEntry("Linear", GL_LINEAR);
  glutAddMenuEntry("Nearest mipmap nearest", GL_NEAREST_MIPMAP_NEAREST);
  glutAddMenuEntry("Linear mipmap nearest", GL_LINEAR_MIPMAP_NEAREST);
  glutAddMenuEntry("Nearest mipmap linear", GL_NEAREST_MIPMAP_LINEAR);
  glutAddMenuEntry("Linear mipmap linear", GL_LINEAR_MIPMAP_LINEAR);

  alphaMenu = glutCreateMenu(alphaSelect);
  glutAddMenuEntry("Alpha testing", GL_ALPHA_TEST);
  glutAddMenuEntry("Alpha blending", GL_BLEND);
  glutAddMenuEntry("Nothing", GL_NONE);

  glutCreateMenu(mainSelect);
  glutAddSubMenu("Filtering", minifyMenu);
  glutAddSubMenu("Alpha", alphaMenu);
  glutAddMenuEntry("Quit", 666);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
