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
* File: ./include/flowvr/render/font.h                            *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_FONT_H
#define FLOWVR_RENDER_FONT_H

#include "chunkrenderwriter.h"

namespace flowvr
{

namespace render
{

class FontData;

class Font
{
public:
  ChunkRenderWriter* scene;
  ModuleAPI* flowvr;

  ID idTexture;
  ID idIBuffer;
  ID idVShader;
  ID idVShaderAlign;
  ID idVShaderFixed;
  ID idPShader;

  float textHeight;
  float lineGap;

  Font(ChunkRenderWriter* _scene, ModuleAPI* _flowvr);
  ~Font();

  /// Load a font from a file. Must be called at least once.
  ChunkTexture* load(const char* fontname="fonts/default.txf", float res=64.0f);

  /// Set text height and interline gap
  void setSize(float textheight, float linegap=0.0f);

  /// Text style.
  enum Style
  {
    Normal, //< Standard text display
    Aligned, //< Text is always aligned to the screen
    Fixed, //< Text position is fixed on screen
  };

  /// Create a new text primitive.
  ChunkVertexBuffer* addText(ID idPrim, ID idVB, const std::string& text="",
			     float halign = 0.0f, float valign = 0.0f,
			     Style style = Normal);

  /// Change the text string.
  ChunkVertexBuffer* setText(ID idVB, const std::string& text,
			     float halign = 0.0f, float valign = 0.0f);

protected:

  FontData* data;

};

} // namespace render

} // namespace flowvr

#endif

