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
* File: ./include/flowvr/render/chunkwriter.h                     *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_CHUNKWRITER_H
#define FLOWVR_RENDER_CHUNKWRITER_H

#include "chunk.h"
#include "bbox.h"

#include <ftl/type.h>
#include <ftl/quat.h>
#include <ftl/chunk.h>
#include <ftl/chunkwriter.h>

#include <flowvr/utils/filepath.h>

#include <flowvr/moduleapi.h>

namespace flowvr
{

namespace render
{

class SceneOutputPort : public OutputPort
{
public:
  StampInfo stampScratch;
  SceneOutputPort(const std::string &name = std::string("scene"))
  : OutputPort(name), stampScratch("scratch",TypeInt::create())
  {
    stamps->add(&stampScratch);
  }
};


/// Helper class to construct messages containing graphical primitives.
class ChunkRenderWriter : public ftl::ChunkWriter
{
public:

  // Constructor
  ChunkRenderWriter() : ftl::ChunkWriter()
  {
  }

  /// Extern file search path
  static flowvr::utils::FilePath path;

  /// Add a texture chunk. The data part must be written by the caller.
  ChunkTexture* addTexture(ID id, int imageType, int pixelType, int nx, int ny=0, int nz=0, int tp = TP_TEXTURE);

  /// Add a vertex buffer chunk. The data part must be written by the caller.
  ChunkVertexBuffer* addVertexBuffer(ID id, int nbp, int nbData, int* dataType, const BBox& bbox=BBox(Vec3f(-1,-1,-1),Vec3f(1,1,1)));

  /// Add an index buffer chunk. The data part must be written by the caller.
  ChunkIndexBuffer* addIndexBuffer(ID id, int nbi, int dataType, int primitive);

  /// Add a vertex shader.
  ChunkVertexShader* addVertexShader(ID id, int language, const std::string& code);

  /// Add a pixel shader.
  ChunkPixelShader* addPixelShader(ID id, int language, const std::string& code);

  /// Create a texture with default data (checkboard)
  ChunkTexture* addDefaultTexture(ID id, int nx=256, int ny=256);

  /// Create a 2D noise texture
  ChunkTexture* addNoise2DTexture(ID id, int size=64, float scale=4);

  /// Create a 3D noise texture
  ChunkTexture* addNoise3DTexture(ID id, int size=64, float scale=4);

  /// Update a vertex buffer. The data part must be written by the caller.
  /// @param id of the vertex buffer to update (user defines the id)
  /// @param p0 first point to update:
  ///        used to calculate the uoffset in the data, this can be used for partial updates
  ///        of the vertex field, set for 0 if you start your update at the
  ///        beginning, set to n when you start your update at vertex n
  /// @param nbp number of points to allocate for this update
  /// @param nbData number of element in the data types array (given in dataType)
  /// @param dataType an array of data type specifications
  ChunkVertexBufferUpdate* updateVertexBuffer(ID id, int p0, int nbp, int nbData, int* dataType, const BBox& bbox=BBox(Vec3f(-1,-1,-1),Vec3f(1,1,1)));

  /// Update an index buffer. The data part must be written by the caller.
  ChunkIndexBufferUpdate* updateIndexBuffer(ID id, int i0, int nbi, int dataType, int primitive);

  /// Update a texture chunk. The data part must be written by the caller.
  ChunkTextureUpdate* updateTexture(ID id, int imageType, int pixelType, int nx, int ny, int nz, int ux0, int uy0, int uz0, int unx, int uny, int unz);

  /// Delete a resource
  ChunkResDel* delResource(ID id, ResourceType resType, int level=-1, int gen=-1)
  {
    ChunkResDel* p = ftl::ChunkWriter::addChunk<ChunkResDel>();
    p->id = id;
    p->resType = resType;
    p->level = level;
    p->gen = gen;
    return p;
  }

  ChunkResDel* delTexture(ID id, int level=-1, int gen=-1)
  {
    return delResource(id, RES_TEXTURE, level, gen);
  }

  ChunkResDel* delVertexBuffer(ID id, int level=-1, int gen=-1)
  {
    return delResource(id, RES_VERTEXBUFFER, level, gen);
  }

  ChunkResDel* delIndexBuffer(ID id, int level=-1, int gen=-1)
  {
    return delResource(id, RES_INDEXBUFFER, level, gen);
  }

  ChunkResDel* delVertexShader(ID id, int level=-1, int gen=-1)
  {
    return delResource(id, RES_VERTEXSHADER, level, gen);
  }

  ChunkResDel* delPixelShader(ID id, int level=-1, int gen=-1)
  {
    return delResource(id, RES_PIXELSHADER, level, gen);
  }

  /// Load a texture from a file.
  ChunkTexture* loadTexture(ID id, std::string filename);

  /// Load a Cubemap texture from a set file (only the first file's name is necessary).
  ChunkTexture* loadTextureCubemap(ID id, std::string filename);

  /// Load a bump map from a file and convert it to a normal map
  ChunkTexture* loadTextureNormalMap(ID id, std::string filename);

  /// Save a texture to a file (to check procedural textures)
  bool saveTexture(const ChunkTexture* texture, std::string filename);

  /// Read a shader from a file, with support for '#include' directives
  /// Append the result to the given string
  static bool readShader(std::string& result, const std::string& filename);

  /// Load a vertex shader from a file, possibly adding extra defines before the start of the sourcecode.
  ChunkVertexShader* loadVertexShader(ID id, std::string filename, const char* predefines = NULL);

  /// Load a pixel shader from a file, possibly adding extra defines before the start of the sourcecode.
  ChunkPixelShader* loadPixelShader(ID id, std::string filename, const char* predefines = NULL);


  /// Create a Cube mesh.
  void addMeshCube(ID idVB, ID idIB);

  /// Create a Cylinder mesh (main axis is Z).
  void addMeshCylinder(ID idVB, ID idIB, int nx=32);

  /// Create a sphere mesh.
  void addMeshSphere(ID idVB, ID idIB, int nx=32, int ny=16);

  /// Add a primitive's parameter.
  ChunkPrimParam* addParam(ID primID, int param, const char* name, const void* data, int type)
  {
    Type::Type t = (Type::Type)type;
    ChunkPrimParam* p = ftl::ChunkWriter::addChunk<ChunkPrimParam>(padSize(strlen(name)+1) + Type::size(t));
    p->id = primID;
    p->param = param;
    p->dataType = t;
    strcpy(p->name, name);
    memcpy(p->data(), data, Type::size(t));
    return p;
  }

  /// Add a primitive's parameter.
  template <class T>
  ChunkPrimParam* addParam(ID primID, int param, const char* name, const T& data)
  {
    Type::Type t = Type::get(data);
    ChunkPrimParam* p = ftl::ChunkWriter::addChunk<ChunkPrimParam>(padSize(strlen(name)+1) + Type::size(t));
    p->id = primID;
    p->param = param;
    p->dataType = t;
    strcpy(p->name, name);
    *(T*)p->data() = data;
    return p;
  }

  /// Add a primitive's parameter with string value.
  ChunkPrimParam* addParam(ID primID, int param, const char* name, const std::string& data)
  {
    Type::Type t = Type::get(data);
    ChunkPrimParam* p = ftl::ChunkWriter::addChunk<ChunkPrimParam>(padSize(strlen(name)+1) + Type::size(t));
    p->id = primID;
    p->param = param;
    p->dataType = Type::get(data);
    strcpy(p->name, name);
    memcpy((void*)p->data(), data.c_str(), Type::size(t));
    return p;
  }

  /// Add a primitive's parameter with ID value.
  ChunkPrimParam* addParamID(ID primID, int param, const char* name, ID data)
  {
    ChunkPrimParam* p = ftl::ChunkWriter::addChunk<ChunkPrimParam>(padSize(strlen(name)+1) + sizeof(ID));
    p->id = primID;
    p->param = param;
    p->dataType = Type::ID;
    strcpy(p->name, name);
    *(ID*)p->data() = data;
    return p;
  }

  /// Add a primitive's parameter with Enum value.
  ChunkPrimParam* addParamEnum(ID primID, int param, const char* name, int data)
  {
    ChunkPrimParam* p = ftl::ChunkWriter::addChunk<ChunkPrimParam>(padSize(strlen(name)+1) + sizeof(int));
    p->id = primID;
    p->param = param;
    p->dataType = Type::Enum;
    strcpy(p->name, name);
    *(int*)p->data() = data;
    return p;
  }

  /// Reset a primitive's parameter
  ChunkPrimParam* delParam(ID primID, int param, const char* name)
  {
    ChunkPrimParam* p = ftl::ChunkWriter::addChunk<ChunkPrimParam>(padSize(strlen(name)+1));
    p->id = primID;
    p->param = param;
    p->dataType = Type::Delete;
    strcpy(p->name, name);
    return p;
  }

  std::string generateName(const std::string prefix="");

  /// Add a primitive optionally copying its parameters from an existing primitive.
  ChunkPrimAdd* addPrimitive(ID primID, const std::string& name, ID from = 0, short tp = TP_OBJECT, unsigned int targetgroups = (1<<0) )
  {
    ChunkPrimAdd* prim = ftl::ChunkWriter::addChunk<ChunkPrimAdd>(padSize(sizeof( short ) + sizeof( unsigned int )));
    prim->id = primID;
    prim->from = from;
    prim->tp   = tp;
    prim->targetGroups = targetgroups;

    addParam(primID, ChunkPrimParam::NAME,"",name);
    return prim;
  }

  /// Add a primitive and set several parameters to default values.
  void addDefaultPrimitive(ID primID, const std::string& name, ID idTexture, ID idVB, ID idIB, ID idVS=0, ID idPS=0, const Vec4f& color=Vec4f(1,1,1,1))
  {
    addPrimitive(primID, name, 0, TP_OBJECT);
    if (idVS!=0)
    	addParamID(primID, ChunkPrimParam::VSHADER,"",idVS);
    if (idPS!=0)
    	addParamID(primID, ChunkPrimParam::PSHADER,"",idPS);

    addParamID(primID, ChunkPrimParam::VBUFFER_ID, "position", idVB);
    addParamID(primID, ChunkPrimParam::VBUFFER_ID, "normal", idVB);
    addParam(primID, ChunkPrimParam::VBUFFER_NUMDATA, "normal", int(1));
    if (idTexture)
    {
      addParamID(primID, ChunkPrimParam::TEXTURE, "texture", idTexture);
      addParamID(primID, ChunkPrimParam::VBUFFER_ID, "texcoord0", idVB);
      addParam(primID, ChunkPrimParam::VBUFFER_NUMDATA, "texcoord0", int(2));
    }
    addParamID(primID, ChunkPrimParam::IBUFFER_ID, "", idIB);
    addParam(primID, ChunkPrimParam::PARAMVSHADER, "color0", color);
    Vec3f light(1,3,2); light.normalize();
    addParam(primID, ChunkPrimParam::PARAMPSHADER, "lightdir", light);
  }

  /// Translate a primitive to a new position.
  void movePrimitive(ID primID, Vec3f pos)
  {
    addParam(primID, ChunkPrimParam::TRANSFORM_POSITION, "", pos);
  }

  /// Rotate a primitive to a new orientation (angle is in degree).
  void rotatePrimitive(ID primID, float angle, const Vec3f& axis = Vec3f(0,0,1));

  /// Delete a primitive.
  ChunkPrimDel* delPrimitive(ID primID)
  {
    ChunkPrimDel* prim = ftl::ChunkWriter::addChunk<ChunkPrimDel>();
    prim->id = primID;
    return prim;
  }

  flowvr::Message put(OutputPort* port, bool scratch = false)
  {
    flowvr::StampsWrite stamps;
    flowvr::StampInfo *scratchStamp = (*port->stamps)["scratch"];
    if(scratchStamp)
    	stamps.write(*scratchStamp,scratch?1:0);
    return ChunkWriter::put(port, stamps);
  }

};

typedef ChunkRenderWriter ChunkWriter;

} // namespace render

} // namespace flowvr

#endif

