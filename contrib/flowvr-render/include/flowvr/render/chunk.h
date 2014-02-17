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
* File: ./include/flowvr/render/chunk.h                           *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_CHUNK_H
#define FLOWVR_RENDER_CHUNK_H

#include <flowvr/message.h>
#include <flowvr/id.h>

#include <ftl/type.h>
#include <ftl/chunk.h>

namespace flowvr
{

namespace render
{

using namespace ftl;
using flowvr::ID;

/// This assures that the messages have the shortest size possible and no compiler alignment
#if defined _MSC_VER
# pragma pack(push)
# pragma pack (1)
#else
# pragma pack(push,1)
#endif

/// Just the struct to define types for rendering chunks
struct ChunkRender : public Chunk
{

  enum Type
  {
    PRIM_ADD   = 0x01,
    PRIM_DEL   = 0x02,
    PRIM_PARAM = 0x03,
    RES_ADD    = 0x04,
    RES_DEL    = 0x05,
    RES_UPDATE = 0x06,
  };

  /// Print chunk info to stdout
  void print() const;

};


enum
{
  ID_CAMERA         = 0x00000000,
};

enum
{
	TP_CAMERA = 0,
	TP_OBJECT = 1,
	TP_TEXTURE = 2,
	TP_RENDER_TEXTURE_BASE = 3,
	TP_RENDER_TEXTURE00 = TP_RENDER_TEXTURE_BASE,
	TP_RENDER_TEXTURE01,
	TP_RENDER_TEXTURE02,
	TP_RENDER_TEXTURE03,
	TP_RENDER_TEXTURE04,
	TP_RENDER_TEXTURE05,
	TP_RENDER_TEXTURE06,
	TP_RENDER_TEXTURE07,
	TP_RENDER_TEXTURE08,
	TP_RENDER_TEXTURE09,
	TP_RENDER_TEXTURE10,
	TP_RENDER_TEXTURE11,
	TP_RENDER_TEXTURE12,
	TP_RENDER_TEXTURE13,
	TP_RENDER_TEXTURE14,
	TP_RENDER_TEXTURE15,
	TP_RENDER_TEXTURE16,
	TP_RENDER_TEXTURE17,
	TP_RENDER_TEXTURE18,
	TP_RENDER_TEXTURE19,
	TP_RENDER_TEXTURE20,
	TP_RENDER_TEXTURE21,
	TP_RENDER_TEXTURE22,
	TP_RENDER_TEXTURE23,
	TP_RENDER_TEXTURE24,
	TP_RENDER_TEXTURE25,
	TP_RENDER_TEXTURE26,
	TP_RENDER_TEXTURE27,
	TP_RENDER_TEXTURE28,
	TP_RENDER_TEXTURE29,
	TP_RENDER_TEXTURE30,
	TP_RENDER_TEXTURE31,
	TP_RENDER_TEXTURE_MAX = TP_RENDER_TEXTURE31
};

enum
{
	TG_00 = 1<<0,
	TG_01 = 1<<1,
	TG_02 = 1<<2,
	TG_03 = 1<<3,
	TG_04 = 1<<4,
	TG_05 = 1<<5,
	TG_06 = 1<<6,
	TG_07 = 1<<7,
	TG_08 = 1<<8,
	TG_09 = 1<<9,
	TG_10 = 1<<10,
	TG_11 = 1<<11,
	TG_12 = 1<<12,
	TG_13 = 1<<13,
	TG_14 = 1<<14,
	TG_15 = 1<<15,
	TG_16 = 1<<16,
	TG_17 = 1<<17,
	TG_18 = 1<<18,
	TG_19 = 1<<19,
	TG_20 = 1<<20,
	TG_21 = 1<<21,
	TG_22 = 1<<22,
	TG_23 = 1<<23,
	TG_24 = 1<<24,
	TG_25 = 1<<25,
	TG_26 = 1<<26,
	TG_27 = 1<<27,
	TG_28 = 1<<28,
	TG_29 = 1<<29,
	TG_30 = 1<<30,
	TG_31 = 1<<31,
};



namespace TargetGroups
{
	inline int tg2TxTp( unsigned int tg )
	{
		if( tg == 0 )
			return -1; // error
		return tg + TP_RENDER_TEXTURE_BASE - 1;
	}

	inline unsigned int txTp2Tg( int TxTp )
	{
		if( TxTp < TP_RENDER_TEXTURE_BASE )
			return ~0; // error
		return TxTp - TP_RENDER_TEXTURE_BASE + 1; // add a +1 here to skim the normal frame-buffer
	}

	inline unsigned int txTp2FBO( int TxTp )
	{
		unsigned int n = txTp2Tg( TxTp ) + 1;
		if( n > TP_RENDER_TEXTURE_MAX )
			return ~0; // error
		return n;
	}

	inline unsigned int FBO2txTp( int fbo )
	{
		return TP_RENDER_TEXTURE_BASE + fbo;
	}

	inline unsigned int FBO2Tg( int fbo )
	{
		if( fbo > 32 )
			return ~0;
		return fbo + 1; // FBOs start at 1 ;)
	}

	inline unsigned int tg2FBO( int tg )
	{
		if( tg == 0 )
			return ~0;
		return tg - 1; // FBOs start at 1
	}
}


struct ChunkPrimAdd : public ChunkRender
{

  enum { TYPE = PRIM_ADD };
  ID id;   ///< ID of the new primitive.
  ID from; ///< ID of an existing primitive to copy (0 for none).
  short    tp;   ///< ID type of this primitive
  unsigned int targetGroups;

  /// Print chunk info to stdout
  void print() const;

  ChunkPrimAdd()
  : tp( TP_OBJECT )
  , targetGroups(1<<0)
  {}
};

struct ChunkPrimDel : public ChunkRender
{
  enum { TYPE = ChunkRender::PRIM_DEL };
  ID id; ///< ID of the primitive to remove.
  /// Print chunk info to stdout
  void print() const;
};


// OpenGL parameters enum values
enum ParamBlendMode
{
  BLEND_STD=0,    /**< glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD); */
  BLEND_ADD,      /**< glBlendFunc(GL_SRC_ALPHA, GL_ONE);  glBlendEquation(GL_FUNC_ADD); */
  BLEND_SUB,      /**< glBlendFunc(GL_SRC_ALPHA, GL_ONE);  glBlendEquation(GL_FUNC_REVERSE_SUBTRACT); */
  BLEND_MULT,     /**< glBlendFunc(GL_DST_COLOR, GL_ZERO); glBlendEquation(GL_FUNC_ADD); */
  BLEND_PREMULT,  /**< glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); glBlendEquation(GL_FUNC_ADD); */
  BLEND_ONE,      /**< glBlendFunc(GL_ONE, GL_ONE); glBlendEquation(GL_FUNC_ADD); */
  BLEND_MIN,      /**< glBlendFunc(GL_ONE, GL_ONE); glBlendEquation(GL_MIN); */
  BLEND_MAX,      /**< glBlendFunc(GL_ONE, GL_ONE); glBlendEquation(GL_MAX); */
};

struct ChunkPrimParam : public ChunkRender
{
  enum { TYPE = PRIM_PARAM };
  ID id; ///< ID of the primitive to modify.
  int param; ///< Parameter to modify (see Param enum).
  int dataType; ///< Type of data (see Type enum).
  char name[0]; ///< For parameters in string maps: name of the value to modify. Must end with 1 to 4 NULL chars (to align to 4 bytes).

  void* data()
  {
    int namelen = 4;
    while (name[namelen-1])
      namelen+=4;
    return (void*)(name+namelen);
  }

  const void* data() const
  {
    int namelen = 4;
    while (name[namelen-1])
      namelen+=4;
    return (const void*)(name+namelen);
  }

  int dataSize() const
  {
    return size+sizeof(Chunk)-((const char*)(data())-(const char*)(this));
  }

  enum Param
  {
    VISIBLE,
    ORDER,
    VSHADER,
    PSHADER,
    VBUFFER_ID,
    VBUFFER_NUMDATA,
    VBUFFER_V0,
    NBV,
    IBUFFER_ID,
    IBUFFER_I0,
    IBUFFER_NBI,
    TEXTURE,
    PARAMVSHADER,
    PARAMPSHADER,
    PARAMOPENGL,
    TRANSFORM,
    TRANSFORM_POSITION,
    TRANSFORM_ROTATION,
    TRANSFORM_SCALE,
    NAME,
    PARAMDISPLAY,
    PROJECTION,
    TARGETGROUP,
    TRANSFORM_OVERRIDE,
  };


  class TG
  {
  public:
  	TG( unsigned int _n )
  	: n(_n) {}

  	unsigned int n;

  	operator unsigned int() const { return (1<<n); }
  };


  enum StateMatrix
  {
    ModelView = 0, ModelView_Trans, ModelView_Inv, ModelView_InvTrans,
    Projection, Projection_Trans, Projection_Inv, Projection_InvTrans,
    ModelViewProjection, ModelViewProjection_Trans, ModelViewProjection_Inv, ModelViewProjection_InvTrans,
    Model, Model_Trans, Model_Inv, Model_InvTrans,
    View, View_Trans, View_Inv, View_InvTrans,
    ViewProjection, ViewProjection_Trans, ViewProjection_Inv, ViewProjection_InvTrans,
  };

  /// Print chunk info to stdout
  void print() const;
};

enum ResourceType
{
  RES_TEXTURE = 0,
  RES_VERTEXBUFFER = 1,
  RES_INDEXBUFFER = 2,
  RES_VERTEXSHADER = 4,
  RES_PIXELSHADER = 6,
};

struct ChunkResource : public ChunkRender
{
  enum { TYPE = RES_ADD };
  ID id; ///< ID of the new resource.
  short resType; ///< Type of resource (see ResourceType enum).
  short level; ///< Level of detail (-1 for all levels). Level 0 is the most detailed
  int gen; ///< Generation number. New data <-> Higher gen. Use -1 for static resources.
  bool is3D() const
  {
    return type&1;
  }
  void print() const;
};

class BBox;

struct ChunkResource3D : public ChunkResource
{
  float bbox[2][3]; ///< Bounding Box
  void print() const;
  void getBBox(BBox& bb) const;
  void setBBox(const BBox& bb);
};

struct ChunkTexture : public ChunkResource
{
  short imageType; ///< See ImageType enum.
  short nx,ny,nz; ///< Size of this texture. ny==0 means a 1D texture and nz==0 means a 2D texture, else a 3D texture
  int pixelType; ///< Type of each pixel (see Type enum). For compressed formats, this is the type of each compressed blocs (i.e. 4x4 pixels in DXT formats)
  int lineSize; ///< Size of each line in bytes. For compressed formats, this is the size of each line of compressed blocs (i.e. 4 lines in DXT formats)
  int tp;

  enum ImageType
  {
    Unknown=0,
    BW,
    Grayscale,
    RGB, RGBA=RGB,
    RGB32F_ARB, RGBA32F_ARB=RGB32F_ARB,
    BGR, BGRA=BGR,
    YUV,
    YUYV,
    Depth,
    MASK_TYPE = 0x00ff,
      
    // Compressed texture types
    MASK_BLOC_LOG2 = 0x00e0,
    SHIFT_BLOC_LOG2 = 5,
    BLOC_2x2   = 1 << SHIFT_BLOC_LOG2,
    BLOC_4x4   = 2 << SHIFT_BLOC_LOG2,
    BLOC_8x8   = 3 << SHIFT_BLOC_LOG2,
    BLOC_16x16 = 4 << SHIFT_BLOC_LOG2,
    RGB_DXT1 = BLOC_4x4+1, RGBA_DXT1, RGBA_DXT3, RGBA_DXT5, YCOCG_DXT5,

    // Flags
    CUBEMAP = 0x0100,
    CLAMP = 0x0200,
    NEAREST = 0x0400,
    RECT = 0x0800,
    ARRAY = 0x1000,
  };

  const void* data() const
  {
    return (const void*)(this+1);
  }

  void* data()
  {
    return (void*)(this+1);
  }

  int dataSize() const
  {
    return size+sizeof(Chunk)-((const char*)(data())-(const char*)(this));
  }

  void print() const;
};

struct ChunkVertexBuffer : public ChunkResource3D
{
  //short nbv; ///< Number of Vertices
  short nbData; ///< Number of data streams
  short vertexSize; ///< Total size of each vertex
  int dataType[0]; ///< Type of each stream (see Type enum).

  const void* data() const
  {
    return (const void*)(dataType+nbData);
  }

  void* data()
  {
    return (void*)(dataType+nbData);
  }

  void* dataNum(int num, int v0=0)
  {
    if (num >= nbData)
    	return NULL;

    int offset = vertexSize*v0;

    for (int i=0;i<num;i++)
      offset += ftl::Type::size(dataType[i]);

    return ((char*)data())+offset;
  }

  const void* dataNum(int num, int v0=0) const
  {
    if (num >= nbData)
    	return NULL;

    int offset = vertexSize*v0;

    for (int i=0;i<num;i++)
      offset += ftl::Type::size(dataType[i]);

    return ((char*)data())+offset;
  }

  int dataSize() const
  {
    return size+sizeof(Chunk)-((const char*)(data())-(const char*)(this));
  }
  void print() const;
};

struct ChunkIndexBuffer : public ChunkResource
{
  int dataType; ///< Type of each index (see Type enum). Only allow values: Null Byte Short Int
  int primitive; ///< Indexed primitive type (see PrimType enum).
  int restart; ///< If non-null: special index value meaning start a new primitive (to render several strips for example).

  enum PrimType
  { // Must correspond to GL constants GL_*
    Point           = 0x0000,
    Line            = 0x0001,
    LineLoop        = 0x0002,
    LineStrip       = 0x0003,
    Triangle        = 0x0004,
    TriangleStrip   = 0x0005,
    TriangleFan     = 0x0006,
    Quad            = 0x0007,
    QuadStrip       = 0x0008,
    Polygon         = 0x0009,
    Tetrahedron     = 0x0010, ///< One day graphics cards will use tetrahedrons instead of mere triangles ;)
  };

  const void* data() const
  {
    return (const void*)(this+1);
  }

  void* data()
  {
    return (void*)(this+1);
  }

  const void* data(int i0) const
  {
    int size = ftl::Type::size(dataType);
    return ((const char*)(this+1))+size*i0;
  }

  void* data(int i0)
  {
    int size = ftl::Type::size(dataType);
    return ((char*)(this+1))+size*i0;
  }

  int dataSize() const
  {
    return size+sizeof(Chunk)-((const char*)(data())-(const char*)(this));
  }

  void print() const;
};

enum ShaderLanguage
{
  SHADER_ALL=0,
  SHADER_HLSL,
  SHADER_CG,
  SHADER_GLSL,
  SHADER_ARB_PROGRAM,
};

struct ChunkShader : public ChunkResource
{
  short language;

  const char* data() const
  {
    return (const char*)(this+1);
  }

  int dataSize() const
  {
    return size+sizeof(Chunk)-((const char*)(data())-(const char*)(this));
  }
  void print() const;
};

struct ChunkVertexShader : public ChunkShader
{
};

struct ChunkPixelShader : public ChunkShader
{
};

struct ChunkResDel : public ChunkRender
{
  enum { TYPE = ChunkRender::RES_DEL };
  ID id; ///< ID of the primitive to remove.
  short resType; ///< Type of resource (see ResourceType enum).
  short level; ///< Level of detail (-1 for all levels).
  int gen; ///< Maximum generation number to remove (-1 for all generations).
  void print() const;
};

struct ChunkResUpdate : public ChunkRender
{
  enum { TYPE = ChunkRender::RES_UPDATE };
  ID id; ///< ID of the primitive to modify
  short resType; ///< Type of resource (see ResourceType enum).
  short level; ///< Level of detail (-1 for all levels).
  int gen; ///< Maximum generation number to remove (-1 for all generations).
  bool is3D() const
  {
    return type&1;
  }
  void print() const;
};

struct ChunkRes3DUpdate : public ChunkResUpdate
{
  float bbox[2][3]; ///< Bounding Box
  void print() const;
  void getBBox(BBox& bb) const;
  void setBBox(const BBox& bb);
};

struct ChunkTextureUpdate : public ChunkResUpdate
{
  short imageType; ///< See ImageType enum.
  short nx,ny,nz; ///< Size of this texture. ny==0 means a 1D texture and nz==0 means a 2D texture, else a 3D texture
  int pixelType; ///< Type of each pixel (see Type enum). For compressed formats, this is the type of each compressed blocs (i.e. 4x4 pixels in DXT formats)
  int lineSize; ///< Size of each update line in bytes. For compressed formats, this is the size of each line of compressed blocs (i.e. 4 lines in DXT formats)

  short ux0,uy0,uz0; ///< Top-left corner of update area
  short unx,uny,unz; ///< Size of update area

  enum ImageType
  {
    Unknown=0,
    BW,
    Grayscale,
    RGB, RGBA=RGB,
    RGB32F_ARB, RGBA32F_ARB=RGB32F_ARB,
    BGR, BGRA=BGR,
    YUV,
    YUYV,
    Depth,
    MASK_TYPE = 0x00ff,
    
    // Compressed texture types
    MASK_BLOC_LOG2 = 0x00e0,
    SHIFT_BLOC_LOG2 = 5,
    BLOC_2x2   = 1 << SHIFT_BLOC_LOG2,
    BLOC_4x4   = 2 << SHIFT_BLOC_LOG2,
    BLOC_8x8   = 3 << SHIFT_BLOC_LOG2,
    BLOC_16x16 = 4 << SHIFT_BLOC_LOG2,
    RGB_DXT1 = BLOC_4x4+1, RGBA_DXT1, RGBA_DXT3, RGBA_DXT5, YCOCG_DXT5,
    
    // Flags
    CUBEMAP = 0x0100,
    CLAMP = 0x0200,
    NEAREST = 0x0400,
    RECT = 0x0800,
    ARRAY = 0x1000,
  };

  const void* data() const
  {
    return (const void*)(this+1);
  }

  void* data()
  {
    return (void*)(this+1);
  }

  int dataSize() const
  {
    return size+sizeof(Chunk)-((const char*)(data())-(const char*)(this));
  }

  void print() const;
};

struct ChunkVertexBufferUpdate : public ChunkRes3DUpdate
{
  int uoffset; ///< Offset of updated area (in bytes).
  //short nbv; ///< Number of Vertices
  short nbData; ///< Number of data streams
  short vertexSize; ///< Total size of each vertex
  int dataType[0]; ///< Type of each stream (see Type enum).

  const void* data() const
  {
    return (const void*)(dataType+nbData);
  }

  void* data()
  {
    return (void*)(dataType+nbData);
  }

  int dataSize() const
  {
    return size+sizeof(ftl::Chunk)-((const char*)(data())-(const char*)(this));
  }
  void print() const;
};

struct ChunkIndexBufferUpdate : public ChunkResUpdate
{
  int uoffset; ///< Offset of updated area (in bytes).
  int dataType; ///< Type of each index (see Type enum). Only allow values: Null Byte Short Int
  int primitive; ///< Indexed primitive type (see PrimType enum).
  int restart; ///< If non-null: special index value meaning start a new primitive (to render several strips for example).

  const void* data() const
  {
    return (const void*)(this+1);
  }

  void* data()
  {
    return (void*)(this+1);
  }

  int dataSize() const
  {
    return size+sizeof(Chunk)-((const char*)(data())-(const char*)(this));
  }

  void print() const;
};

#pragma pack(pop)

} // namespace render

} // namespace flowvr

#endif
