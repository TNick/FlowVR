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
* File: ./src/librender/font.cpp                                  *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include <flowvr/render/font.h>
#include <flowvr/render/chunkrenderwriter.h>
#include <flowvr/render/texfont.h>

#include <string>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

namespace flowvr
{

namespace render
{

class FontData
{
public:

  TexFont* txf;

  float scale;
  float yoffset;

  int gen;
  int nbprims;

  FontData() : txf(NULL), scale(0), gen(0), nbprims(0) {}

};

Font::Font(flowvr::render::ChunkRenderWriter* _scene, flowvr::ModuleAPI* _flowvr)
: scene(_scene), flowvr(_flowvr), idTexture(0), idIBuffer(0), idVShader(0), idVShaderAlign(0), idVShaderFixed(0), idPShader(0),
  textHeight(1), lineGap(0),
  data(new FontData())
{
}

Font::~Font()
{
  if (data->txf!=NULL) txfUnloadFont(data->txf);
  delete data;
}

/// Load a font from a file. Must be called at least once.
/// Returns the created texture so that you can save it for debugging
ChunkTexture* Font::load(const char* fontname, float res)
{
  std::string fname = fontname;
  if (!ChunkRenderWriter::path.findFile(fname)) return NULL;
  if (data->txf != NULL)
  {
    txfUnloadFont(data->txf);
  }
  data->txf = txfLoadFont(fname.c_str());
  if (data->txf == NULL)
  {
    std::cerr << "Font "<<fontname<<" load failed: "<<txfErrorString()<<std::endl;
    return NULL;
  }

  data->scale = 1.0 / (data->txf->max_ascent + data->txf->max_descent);
  data->yoffset = data->txf->max_descent;

  bool create = false;

  if (!idTexture)
  {
    idTexture = flowvr->generateID();
    create = true;
  }

  ChunkTexture* texture = scene->addTexture(idTexture, ChunkTexture::Grayscale, Type::Byte, data->txf->tex_width, data->txf->tex_height);
  texture->level = 0;
  if (!create)
    texture->gen = data->gen++;

  memcpy(texture->data(), data->txf->teximage, texture->dataSize());

  return texture;

}

void Font::setSize(float textheight, float linegap)
{
  textHeight = textheight;
  lineGap = linegap;
}

/// Create a new text primitive.
/// Returns the created Vertex Buffer to retrieve the bounding box.
ChunkVertexBuffer* Font::addText(ID idPrim, ID idVB, const std::string& text,
				 float halign, float valign,
				 Style style)
{
  ChunkVertexBuffer* vbuffer = setText(idVB, text, halign, valign);

  char buf[32];
  snprintf(buf,sizeof(buf),"Text%d",data->nbprims++);

  scene->addPrimitive(idPrim,buf);

  switch(style)
  {
  case Aligned:
    scene->addParamID(idPrim, ChunkPrimParam::VSHADER,"",idVShaderAlign);
    scene->addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelView", ChunkPrimParam::ModelView);
    scene->addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "Proj", ChunkPrimParam::Projection);
    break;
  case Fixed:
    scene->addParamID(idPrim, ChunkPrimParam::VSHADER,"",idVShaderFixed);
    scene->addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "Proj", ChunkPrimParam::Projection);
    break;
  default:
    scene->addParamEnum(idPrim, ChunkPrimParam::PARAMVSHADER, "ModelViewProj", ChunkPrimParam::ModelViewProjection);
    scene->addParamID(idPrim, ChunkPrimParam::VSHADER,"",idVShader);
  }
  scene->addParamID(idPrim, ChunkPrimParam::PSHADER,"",idPShader);
  scene->addParamID(idPrim, ChunkPrimParam::VBUFFER_ID, "position", idVB);
  scene->addParamID(idPrim, ChunkPrimParam::TEXTURE, "texture", idTexture);
  scene->addParamID(idPrim, ChunkPrimParam::VBUFFER_ID, "texcoord0", idVB);
  scene->addParam(idPrim, ChunkPrimParam::VBUFFER_NUMDATA, "texcoord0", int(1));

  scene->addParamID(idPrim, ChunkPrimParam::IBUFFER_ID, "", idIBuffer);
  scene->addParam(idPrim, ChunkPrimParam::PARAMVSHADER, "color", Vec3f(0.5f,0.5f,0.5f,1.0f));
  scene->addParam(idPrim, ChunkPrimParam::PARAMOPENGL,"Blend",true);
  scene->addParam(idPrim, ChunkPrimParam::PARAMOPENGL,"AlphaTest",0.1f);
  scene->addParam(idPrim, ChunkPrimParam::ORDER,"",1);

  return vbuffer;
}

static bool isVisible(TexGlyphVertexInfo *tgvi)
{
  return (tgvi!=NULL) && (tgvi->v1[0]>tgvi->v0[0]) && (tgvi->v2[1]>tgvi->v0[1]);
}

/// Change the text string.
/// Returns the created Vertex Buffer to retrieve the bounding box.
ChunkVertexBuffer* Font::setText(ID idVB, const std::string& text,
				 float halign, float valign)
{
  if (!idIBuffer)
  {
    idIBuffer = flowvr->generateID();
    scene->addIndexBuffer(idIBuffer, 0, Type::Null, ChunkIndexBuffer::Quad);
  }
  if (!idVShader)
  {
    idVShader = flowvr->generateID();
    scene->loadVertexShader(idVShader, "shaders/text_v.cg");
  }
  if (!idVShaderAlign)
  {
    idVShaderAlign = flowvr->generateID();
    scene->loadVertexShader(idVShaderAlign, "shaders/text_align_v.cg");
  }
  if (!idVShaderFixed)
  {
    idVShaderFixed = flowvr->generateID();
    scene->loadVertexShader(idVShaderFixed, "shaders/text_fixed_v.cg");
  }
  if (!idPShader)
  {
    idPShader = flowvr->generateID();
    scene->loadPixelShader(idPShader, "shaders/text_p.cg");
  }

  // First find the dimension of the text and the number of visible characters
  float scale = textHeight*data->scale;

  std::vector<float> wline;
  int nbq = 0; // number of quads

  wline.push_back(0);
  for (unsigned int i=0;i<text.length();i++)
  {
    char c = text[i];
    if (c=='\n')
    {
      wline.push_back(0);
    }
    else
    {
      TexGlyphVertexInfo *tgvi = txfGetTCVI(data->txf, (unsigned char)c);
      if (tgvi != NULL)
      {
	wline.back() += tgvi->advance*scale;
	if (isVisible(tgvi)) ++nbq;
      }
    }
  }

  float height = textHeight*wline.size()+lineGap*(wline.size()-1);
  float width = 0;
  for (unsigned int i=0;i<wline.size();i++)
    if (wline[i]>width) width=wline[i];

  struct Vertex
  {
    Vec2f pos;
    Vec2f t;
  };

  int dataType[2] = { Type::Vec2f, Type::Vec2f };
  ChunkVertexBuffer* vbuffer =
    scene->addVertexBuffer(idVB, 4*nbq, 2, dataType,
			   BBox(Vec3f(width*(0.0-halign),height*(0.0-valign),0),
				Vec3f(width*(1.0-halign),height*(1.0-valign),0)));
  vbuffer->gen = data->gen++;
  Vertex* v = (Vertex*) vbuffer->data();

  int q = 0;
  int l = 0;
  bool start = true;
  Vec2f pos(0,height*(1.0-valign)-textHeight);
  for (unsigned int i=0;i<text.length();i++)
  {
    char c = text[i];
    if (c=='\n')
    {
      ++l;
      pos[1] -= textHeight+lineGap;
      start = true;
    }
    else
    {
      if (start)
      {
	pos[0] = wline[l]*(0.0-halign);
	start = false;
      }
      TexGlyphVertexInfo *tgvi = txfGetTCVI(data->txf, (unsigned char)c);
      if (isVisible(tgvi))
      {
	v[4*q+0].pos = pos + Vec2f(tgvi->v0[0], tgvi->v0[1]+data->yoffset)*scale;
	v[4*q+0].t   =       Vec2f(tgvi->t0[0], tgvi->t0[1]);
	v[4*q+1].pos = pos + Vec2f(tgvi->v1[0], tgvi->v1[1]+data->yoffset)*scale;
	v[4*q+1].t   =       Vec2f(tgvi->t1[0], tgvi->t1[1]);
	v[4*q+2].pos = pos + Vec2f(tgvi->v2[0], tgvi->v2[1]+data->yoffset)*scale;
	v[4*q+2].t   =       Vec2f(tgvi->t2[0], tgvi->t2[1]);
	v[4*q+3].pos = pos + Vec2f(tgvi->v3[0], tgvi->v3[1]+data->yoffset)*scale;
	v[4*q+3].t   =       Vec2f(tgvi->t3[0], tgvi->t3[1]);
	++q;
      }
      pos[0]+=tgvi->advance*scale;
    }
  }
  return vbuffer;
}


} // namespace render

} // namespace flowvr

