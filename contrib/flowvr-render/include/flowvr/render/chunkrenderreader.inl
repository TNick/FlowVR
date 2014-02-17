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
* File: ./include/flowvr/render/chunkrenderreader.inl             *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/


#if !defined(CHUNKREADER_INL)
#define CHUNKREADER_INL

#include <ftl/convert.h>

namespace flowvr
{

namespace render
{

#define ERRMSG(msg) (std::cerr << __FILE__ << ":" << __LINE__ << "(" << __PRETTY_FUNCTION__ <<"): " << msg << std::endl, false)

template <class UserData>
bool ChunkRenderReader<UserData>::init()
{
  // Auto add the camera primitive
//  Primitive* camera = primitives.add(ID_CAMERA);
//  if (camera == NULL)
//    return ERRMSG("Camera creation failed.");
//  camera->name = "camera";
//
//  if (!addPrimitive(camera, NULL))
//	  return false;
//
//  Primitive* display = primitives.add(ID_DISPLAY);
//  if (display == NULL)
//    return ERRMSG("Display creation failed.");
//  display->name = "display";
//  if (!addPrimitive(display, NULL))
//	  return false;
  return true;
}

template <class UserData>
bool ChunkRenderReader<UserData>::process(flowvr::Message msg, int iter)
{
  if (iter<0) ++iteration;
  else iteration = iter;
  ftl::ChunkIterator it = chunkBegin(msg);
  ftl::ChunkIterator end = chunkEnd(msg);
  bool res = true;
  while (it != end)
  {
    if (!processChunk((ftl::MsgChunk<Chunk>)it))
    {
      std::cout << "Failed chunk: ";
      ((const ChunkRender*)&*it)->print();
      std::cout << std::endl;
      res = false;
    }
    it++;
  }
  return res;
}

template <class UserData>
void ChunkRenderReader<UserData>::updateBBox()
{
  BBox bb = primitives.calcBBox();
  sceneBBox += bb;
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<Chunk>& data)
{
  if (data.getSize()==0)
	  return ERRMSG("Empty chunk");

  switch (data->type)
  {
	  case ChunkRender::PRIM_ADD:
	  {
		ftl::MsgChunk<ChunkPrimAdd> c(data);
		return processChunk(c);
	  }
	  case ChunkRender::PRIM_DEL:
	  {
		ftl::MsgChunk<ChunkPrimDel> c(data);
		return processChunk(c);
	  }
	  case ChunkRender::PRIM_PARAM:
	  {
		ftl::MsgChunk<ChunkPrimParam> c(data);
		return processChunk(c);
	  }
	  case ChunkRender::RES_ADD:
	  {
		ftl::MsgChunk<ChunkResource> c(data);
		return processChunk(c);
	  }
	  case ChunkRender::RES_UPDATE:
	  {
		ftl::MsgChunk<ChunkResUpdate> c(data);
		return processChunk(c);
	  }
	  case ChunkRender::RES_DEL:
	  {
		ftl::MsgChunk<ChunkResDel> c(data);
		return processChunk(c);
	  }
	  default:
		return ERRMSG("Unknown chunk type "<<data->type);
  }
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkPrimAdd>& data)
{
  Primitive* prim;
  Primitive* model = NULL;

  if (data->from)
    model = primitives.get(data->from);

  if (model != NULL)
    prim = primitives.add(data->id, model, model->order, data->tp, data->targetGroups);
  else
    prim = primitives.add(data->id, NULL, 0, data->tp, data->targetGroups);

  if (prim == NULL)
    return ERRMSG("Primitive "<<std::hex<<data->id<<std::dec<<" creation failed");

  primitives.iteration = iteration;

  if (prim->vShader!=NULL)
    prim->vShader->users.push_back(prim);

  if (prim->pShader!=NULL)
    prim->pShader->users.push_back(prim);

  if (prim->iBuffer.buffer!=NULL)
    prim->iBuffer.buffer->users.push_back(prim);

  for (typename std::map<std::string,typename Primitive::VBData>::iterator
	 it = prim->vBuffer.begin(); it != prim->vBuffer.end(); it++)
  {
    if (it->second.buffer!=NULL)
      it->second.buffer->users.push_back(prim);
  }

  for (typename std::map<std::string,typename Primitive::TData>::iterator
	 it = prim->textures.begin(); it != prim->textures.end(); it++)
  {
    if (it->second.texture!=NULL)
      it->second.texture->users.push_back(prim);
  }

  prim->iteration = iteration;


  return addPrimitive(prim, model);
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkPrimDel>& data)
{
  Primitive* prim = primitives.get(data->id);
  if (prim == NULL)
    return ERRMSG("Primitive suppression failed");

  bool res = delPrimitive(prim);

  primitives.iteration = iteration;

  if (prim->vShader!=NULL)
    prim->vShader->users.remove(prim);
  if (prim->pShader!=NULL)
    prim->pShader->users.remove(prim);

  if (prim->iBuffer.buffer!=NULL)
    prim->iBuffer.buffer->users.remove(prim);

  for (typename std::map<std::string,typename Primitive::VBData>::iterator
	 it = prim->vBuffer.begin(); it != prim->vBuffer.end(); it++)
  {
    if (it->second.buffer!=NULL)
      it->second.buffer->users.remove(prim);
  }

  for (typename std::map<std::string,typename Primitive::TData>::iterator
	 it = prim->textures.begin(); it != prim->textures.end(); it++)
  {
    if (it->second.texture!=NULL)
      it->second.texture->users.remove(prim);
  }

  res &= primitives.del(data->id);
  return res;
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkPrimParam>& data)
{
  Primitive* prim = primitives.get(data->id);
  if (prim == NULL)
  {
    return ERRMSG("Unknown primitive in parameter change" );
  }

  if (data->dataType == (int)Type::Delete)
    return resetParam(prim, data->param, data->name);
  else
  {
    TypedArray val(data->dataType,data->data(),data->dataSize());
    return setParam(prim, data->param, data->name, val);
  }
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkResource>& data)
{
  if (data->id == 0)
	  return ERRMSG("Resources must have a non-null ID");

  switch (data->resType)
  {
  case RES_TEXTURE:
  {
    ftl::MsgChunk<ChunkTexture> c(data);
    return processChunk(c);
  }
  case RES_VERTEXBUFFER:
  {
    ftl::MsgChunk<ChunkVertexBuffer> c(data);
    return processChunk(c);
  }
  case RES_INDEXBUFFER:
  {
    ftl::MsgChunk<ChunkIndexBuffer> c(data);
    return processChunk(c);
  }
  case RES_VERTEXSHADER:
  {
    ftl::MsgChunk<ChunkVertexShader> c(data);
    return processChunk(c);
  }
  case RES_PIXELSHADER:
  {
    ftl::MsgChunk<ChunkPixelShader> c(data);
    return processChunk(c);
  }
  default:
    return ERRMSG("Unknown resource type "<<data->resType);
  }
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkResUpdate>& data)
{
  if (data->id == 0)
	  return ERRMSG("Ressources must have a non-null ID");

  switch (data->resType)
  {
	  case RES_TEXTURE:
	  {
		ftl::MsgChunk<ChunkTextureUpdate> c(data);
		return processChunk(c);
	  }
	  case RES_VERTEXBUFFER:
	  {
		ftl::MsgChunk<ChunkVertexBufferUpdate> c(data);
		return processChunk(c);
	  }
	  case RES_INDEXBUFFER:
	  {
		ftl::MsgChunk<ChunkIndexBufferUpdate> c(data);
		return processChunk(c);
	  }
	  case RES_VERTEXSHADER:
		return ERRMSG("Unsupported resource update chunk for Vertex Shaders");
	  case RES_PIXELSHADER:
		return ERRMSG("Unsupported resource update chunk for Pixel Shaders");
	  default:
		return ERRMSG("Unknown resource type "<<data->resType);
  }
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkTexture>& data)
{
  int nbDim;
  if (data->ny == 0)
	  nbDim = 1;
  else if (data->nz == 0)
	  nbDim = 2;
  else
	  nbDim = 3;

  Texture* dest = textures[data->id];
  bool create = !dest->created;
  if (create)
  {
    dest->created = true;
    dest->nbDim = nbDim;
  }
  else if (nbDim != dest->nbDim)
  {
    return ERRMSG("Incoherent number of dimensions in texture.");
  }
  dest->iteration = iteration;
  if (storeTexture(dest,data))
    dest->add(data);
  return addTexture(dest,data,create);
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkTextureUpdate>& data)
{
  int nbDim;
  if (data->ny == 0)
	  nbDim = 1;
  else if (data->nz == 0)
	  nbDim = 2;
  else
	  nbDim = 3;

  Texture* dest = textures.get(data->id);
  if (dest == NULL)
  {
    return ERRMSG("Texture update on unknown ID "<<std::hex<<data->id<<std::dec);
  }
  else if (nbDim != dest->nbDim)
  {
    return ERRMSG("Incoherent number of dimensions in texture.");
  }
  dest->iteration = iteration;
  if (storeTexture(dest,(const ChunkTexture*)(const ChunkTextureUpdate*)data))
    dest->add(data);
  return updateTexture(dest,data);
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkVertexBuffer>& data)
{
  VertexBuffer* dest = vBuffers[data->id];
  bool create = !dest->created;
  if (create)
  {
    dest->created = true;
  }
  dest->iteration = iteration;
  if (storeVertexBuffer(dest,data))
    dest->add(data);

  dest->updateBBox(data);

  std::for_each( dest->users.begin(), dest->users.end(), std::mem_fun( &Primitive::updateBBox ) );

  return addVertexBuffer(dest,data,create);
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkVertexBufferUpdate>& data)
{
  VertexBuffer* dest = vBuffers.get(data->id);
  if (dest == NULL)
  {
    return ERRMSG("Vertex Buffer update on unknown ID "<<std::hex<<data->id<<std::dec);
  }
  dest->iteration = iteration;

  if (storeVertexBuffer(dest,(const ChunkVertexBuffer*)(const ChunkVertexBufferUpdate*)data))
    dest->add(data);

  return updateVertexBuffer(dest,data);
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkIndexBuffer>& data)
{
  IndexBuffer* dest = iBuffers[data->id];
  bool create = !dest->created;
  if (create)
  {
    dest->created = true;
  }
  dest->iteration = iteration;
  if (storeIndexBuffer(dest,data))
    dest->add(data);

  return addIndexBuffer(dest,data,create);
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkIndexBufferUpdate>& data)
{
  IndexBuffer* dest = iBuffers.get(data->id);
  if (dest == NULL)
  {
    return ERRMSG("Index Buffer update on unknown ID "<<std::hex<<data->id<<std::dec);
  }
  dest->iteration = iteration;
  if (storeIndexBuffer(dest,(const ChunkIndexBuffer*)(const ChunkIndexBufferUpdate*)data))
    dest->add(data);

  return updateIndexBuffer(dest,data);
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkVertexShader>& data)
{
  if (!supportVertexShader((ShaderLanguage)data->language))
	  return ERRMSG("Unsupported Vertex Shader language.");

  VertexShader* dest = vShaders[data->id];
  bool create = !dest->created;
  if (create)
  {
    dest->created = true;
  }
  dest->iteration = iteration;

  if (storeVertexShader(dest,data))
    dest->add(data);

  return addVertexShader(dest,data,create);
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkPixelShader>& data)
{
  if (!supportPixelShader((ShaderLanguage)data->language))
	  return ERRMSG("Unsupported Pixel Shader language.");

  PixelShader* dest = pShaders[data->id];
  bool create = !dest->created;
  if (create)
  {
    dest->created = true;
  }
  dest->iteration = iteration;

  if (storePixelShader(dest,data))
    dest->add(data);

  return addPixelShader(dest,data,create);
}

template <class UserData>
bool ChunkRenderReader<UserData>::processChunk(const ftl::MsgChunk<ChunkResDel>& data)
{
  if (data->id == 0)
	  return ERRMSG("Resources must have a non-null ID.");

  switch (data->resType)
  {
  case RES_TEXTURE:
  {
    Texture* dest = textures.get(data->id);
    if (dest == NULL || !dest->created) return ERRMSG("Texture not found.");
    dest->del(data->level, data->gen);
    if (dest->empty())
    {
      if (!delTexture(dest))
    	  return false;

      // @todo what is the below here for?
      ID id = dest->id;
      std::list<Primitive*> users = dest->users;
      *dest = Texture();
      dest->id    = id;
      dest->users = users;
      return true;
    }
    break;
  }
  case RES_VERTEXBUFFER:
  {
    VertexBuffer* dest = vBuffers.get(data->id);
    if (dest == NULL || !dest->created) return ERRMSG("Vertex Buffer not found.");
    dest->del(data->level, data->gen);
    if (dest->empty())
    {
      if (!delVertexBuffer(dest)) return false;
      // @todo what is the below here for?
      ID id = dest->id;
      std::list<Primitive*> users = dest->users;
      *dest = VertexBuffer();
      dest->id = id;
      dest->users = users;
      return true;
    }
    break;
  }
  case RES_INDEXBUFFER:
  {
    IndexBuffer* dest = iBuffers.get(data->id);
    if (dest == NULL || !dest->created) return ERRMSG("Index Buffer not found.");
    dest->del(data->level, data->gen);
    if (dest->empty())
    {
      if (!delIndexBuffer(dest))
    	  return false;
      // @todo what is the below here for?
      ID id = dest->id;
      std::list<Primitive*> users = dest->users;
      *dest = IndexBuffer();
      dest->id = id;
      dest->users = users;
      return true;
    }
    break;
  }
  case RES_VERTEXSHADER:
  {
    VertexShader* dest = vShaders.get(data->id);
    if (dest == NULL || !dest->created) return ERRMSG("Vertex Shader not found.");
    dest->del(data->level, data->gen);
    if (dest->empty())
    {
      if (!delVertexShader(dest))
    	  return false;
      // @todo what is the below here for?
      ID id = dest->id;
      std::list<Primitive*> users = dest->users;
      *dest = VertexShader();
      dest->id = id;
      dest->users = users;
      return true;
    }
    break;
  }
  case RES_PIXELSHADER:
  {
    PixelShader* dest = pShaders.get(data->id);
    if (dest == NULL || !dest->created) return ERRMSG("Pixel Shader not found.");
    dest->del(data->level, data->gen);
    if (dest->empty())
    {
      if (!delPixelShader(dest))
    	  return false;
      // @todo what is the below here for?
      ID id = dest->id;
      std::list<Primitive*> users = dest->users;
      *dest = PixelShader();
      dest->id = id;
      dest->users = users;
      return true;
    }
    break;
  }
  default:
    return ERRMSG("Unknown resource type " << data->resType);
  }
  return true;
}

template <class UserData>
bool ChunkRenderReader<UserData>::setParam(Primitive* prim,
		                                        int param,
		                                        const char* name,
		                                        const TypedArray& val)
{
  switch (param)
  {
  case ChunkPrimParam::VISIBLE:
    if (!val.assign(prim->visible))
    	return ERRMSG("Incorrect VISIBLE parameter type in primitive "<<prim->name);
    prim->iteration = iteration;
    break;
  case ChunkPrimParam::ORDER:
    int newOrder;
    if (!val.assign(newOrder))
    	return ERRMSG("Incorrect ORDER parameter type in primitive "<<prim->name);
    if (!primitives.move(prim->id,newOrder))
    	return ERRMSG("Primitive order change failed in primitive "<<prim->name);
    prim->iteration = iteration;
    break;
  case ChunkPrimParam::VSHADER:
  {
    ID id;
    if (!val.assign(id))
    	return ERRMSG("Incorrect VSHADER parameter type in primitive "<<prim->name);
    VertexShader* res = vShaders[id];
    if (prim->vShader != res)
    {
      if (prim->vShader!=NULL)
    	  prim->vShader->users.remove(prim);
      if (res!=NULL)
    	  res->users.push_back(prim);
      prim->vShader = res;
      prim->iteration = iteration;
      if (!setVShader(prim,res))
    	  return false;
    }
    break;
  }
  case ChunkPrimParam::PSHADER:
  {
    ID id;
    if (!val.assign(id)) return ERRMSG("Incorrect PSHADER parameter type in primitive "<<prim->name);
    PixelShader* res = pShaders.get(id);
    if (prim->pShader != res)
    {
      if (prim->pShader!=NULL)
	prim->pShader->users.remove(prim);
      if (res!=NULL)
	res->users.push_back(prim);
      prim->pShader = res;
      prim->iteration = iteration;
      if (!setPShader(prim,res)) return false;
    }
    break;
  }
  case ChunkPrimParam::VBUFFER_ID:
  {
    if (name == NULL || name[0]=='\0')
    	return ERRMSG("Vertex buffers must have a name in primitive "<<prim->name);
    ID id;
    if (!val.assign(id))
    	return ERRMSG("Incorrect VBUFFER_ID parameter type in primitive "<<prim->name);

    VertexBuffer* res = vBuffers[id];
    bool create = (prim->vBuffer.find(name) == prim->vBuffer.end());
    typename Primitive::VBData* dest = &prim->vBuffer[name];
    if (create || dest->buffer != res)
    {
      if (dest->buffer!=NULL)
    	  dest->buffer->users.remove(prim);
      if (res!=NULL)
    	  res->users.push_back(prim);
      dest->buffer = res;
      prim->iteration = iteration;
      if (!setVBuffer(prim, name, dest, res, create))
    	  return false;

      prim->updateBBox();
    }
    break;
  }
  case ChunkPrimParam::VBUFFER_NUMDATA:
  {
    if (name == NULL || name[0]=='\0')
    	return ERRMSG("Vertex buffers must have a name in primitive "<<prim->name);

    // see if we have already a chunk named name
    bool create = (prim->vBuffer.find(name) == prim->vBuffer.end());

    // will implicitly create a TBData in prim->vBuffer, named <name>
    typename Primitive::VBData* dest = &prim->vBuffer[name];
    if (!val.assign(dest->numData))
    	return ERRMSG("Incorrect VBUFFER_NUMDATA parameter type in primitive "<<prim->name);
    prim->iteration = iteration;

    if (create) // @todo check this... why only on create?
      if (!setVBuffer(prim, name, dest, dest->buffer, create))
    	  return false;
    break;
  }
  case ChunkPrimParam::VBUFFER_V0:
  {
    if (name == NULL || name[0]=='\0')
    	return ERRMSG("Vertex buffers must have a name in primitive "<<prim->name);
    bool create = (prim->vBuffer.find(name) == prim->vBuffer.end());
    typename Primitive::VBData* dest = &prim->vBuffer[name];
    if (!val.assign(dest->v0))
    	return ERRMSG("Incorrect VBUFFER_V0 parameter type in primitive "<<prim->name);
    prim->iteration = iteration;
    if (create)
      if (!setVBuffer(prim, name, dest, dest->buffer, create)) return false;
    break;
  }
  case ChunkPrimParam::NBV:
    if (!val.assign(prim->nbv))
    	return ERRMSG("Incorrect NBV parameter type in primitive "<<prim->name);
    break;
  case ChunkPrimParam::IBUFFER_ID:
  {
    ID id;
    if (!val.assign(id)) return ERRMSG("Incorrect IBUFFER_ID parameter type in primitive "<<prim->name);
    IndexBuffer* res = iBuffers[id];
    if (prim->iBuffer.buffer != res)
    {
      if (prim->iBuffer.buffer!=NULL)
	prim->iBuffer.buffer->users.remove(prim);
      if (res!=NULL)
	res->users.push_back(prim);
      prim->iBuffer.buffer = res;
      prim->iteration = iteration;
    }
    break;
  }
  case ChunkPrimParam::IBUFFER_I0:
    if (!val.assign(prim->iBuffer.i0))
    	return ERRMSG("Incorrect IBUFFER_I0 parameter type in primitive "<<prim->name);
    prim->iteration = iteration;
    break;
  case ChunkPrimParam::IBUFFER_NBI:
    if (!val.assign(prim->iBuffer.nbi))
    	return ERRMSG("Incorrect IBUFFER_NBI parameter type in primitive "<<prim->name);
    prim->iteration = iteration;
    break;
  case ChunkPrimParam::TEXTURE:
  {
    if (name == NULL || name[0]=='\0')
    	return ERRMSG("Textures must have a name in primitive "<<prim->name);
    ID id;
    if (!val.assign(id)) // claim texture id from val arg array
    	return false;

    // this will create an empty Texture with id 0 if id was not assigned.
    if( textures.get(id) == NULL )
    {
    	std::cerr << "no texture with id [" << id << "] found."
    	          << std::endl;
    }

    Texture* res = textures[id];
    bool create = (prim->textures.find(name) == prim->textures.end());
    typename Primitive::TData* dest = &prim->textures[name];
    if (create || dest->texture != res)
    {
    	if (dest->texture != NULL)
    		dest->texture->users.remove(prim);
		if (res != NULL)
			res->users.push_back(prim);
		dest->texture = res;
		prim->iteration = iteration;
		if (!setTexture(prim, name, dest, res, create))
		{
			std::cerr << "failed to set texture."
			          << std::endl;
			return false;
		}
    }
    break;
  }
  case ChunkPrimParam::PARAMVSHADER:
  {
    if (name == NULL || name[0]=='\0')
    	return ERRMSG("Vertex shader parameters must have a name in primitive "<<prim->name);
    bool create = (prim->paramVShader.find(name) == prim->paramVShader.end());
    typename Primitive::ParamVShader* dest = &prim->paramVShader[name];
    *dest = val;
    prim->iteration = iteration;
    if (!setParamVShader(prim, name, dest, val, create))
    	return false;
    break;
  }
  case ChunkPrimParam::PARAMPSHADER:
  {
    if (name == NULL || name[0]=='\0')
    	return ERRMSG("Pixel shader parameters must have a name in primitive "<<prim->name);
    bool create = (prim->paramPShader.find(name) == prim->paramPShader.end());
    typename Primitive::ParamPShader* dest = &prim->paramPShader[name];
    *dest = val;
    prim->iteration = iteration;
    if (!setParamPShader(prim, name, dest, val, create))
    	return false;
    break;
  }
  case ChunkPrimParam::PARAMOPENGL:
  {
    if (name == NULL || name[0]=='\0')
    	return ERRMSG("OpenGL parameters must have a name in primitive "<<prim->name);
    bool create = (prim->paramOpenGL.find(name) == prim->paramOpenGL.end());
    typename Primitive::ParamOpenGL* dest = &prim->paramOpenGL[name];
    *dest = val;
    prim->iteration = iteration;
    if (!setParamOpenGL(prim, name, dest, val, create))
    	return false;
    break;
  }
  case ChunkPrimParam::TRANSFORM:
    if (!val.assign(prim->xform.mat))
    	return ERRMSG("Incorrect TRANSFORM parameter type in primitive "<<prim->name);
    prim->updateAABB();
    prim->xform.iteration = iteration;
    break;
  case ChunkPrimParam::TRANSFORM_POSITION:
    if (!val.assign(prim->xform.position))
    	return ERRMSG("Incorrect TRANSFORM_POSITION parameter type in primitive "<<prim->name);
    prim->xform.update();
    prim->updateAABB();
    prim->xform.iteration = iteration;
    break;
  case ChunkPrimParam::TRANSFORM_ROTATION:
    if (!val.assign(prim->xform.rotation))
    	return ERRMSG("Incorrect TRANSFORM_ROTATION parameter type in primitive "<<prim->name);
    prim->xform.update();
    prim->updateAABB();
    prim->xform.iteration = iteration;
    break;
  case ChunkPrimParam::TRANSFORM_SCALE:
    if (!val.assign(prim->xform.scale))
    	return ERRMSG("Incorrect TRANSFORM_SCALE parameter type in primitive "<<prim->name);
    prim->xform.update();
    prim->updateAABB();
    prim->xform.iteration = iteration;
    break;
  case ChunkPrimParam::PROJECTION:
	if (!val.assign(prim->proj.mat))
		return ERRMSG("Incorrect PROJECTION parameter type in primitive "<<prim->name);
	prim->proj.iteration = iteration;
	break;
  case ChunkPrimParam::NAME:
    if (!val.assign(prim->name))
    	return ERRMSG("Incorrect NAME parameter type in primitive "<<prim->name);
//    else
//    	std::cout << "setting name of prim[" << prim->id << "] to [" << prim->name << "]" << std::endl;
    prim->iteration = iteration;
    break;
  case ChunkPrimParam::PARAMDISPLAY:
  {
    if (name == NULL || name[0]=='\0')
    	return ERRMSG("Display parameters must have a name in primitive " << prim->name);

    bool create = (prim->paramDisplay.find(name) == prim->paramDisplay.end());
    typename Primitive::ParamDisplay* dest = &prim->paramDisplay[name];

    *dest = val;
    prim->iteration = iteration;
    if (!setParamDisplay(prim, name, dest, val, create))
    	return false;
    break;
  }
  case ChunkPrimParam::TARGETGROUP:
  {
	  unsigned int lstTgtGrp = prim->targetGroups;
	  if(!val.assign( prim->targetGroups ) )
		  return ERRMSG("Could not convert target groups");
	  prim->lastTargetGroups = lstTgtGrp;

	  return updateTargetGroups( prim );
  }
  case ChunkPrimParam::TRANSFORM_OVERRIDE:
  {
	    if (!val.assign(prim->xform.override))
	    	return ERRMSG("Incorrect TRANSFORM_SCALE parameter type in primitive "<<prim->name);
	    prim->xform.iteration = iteration;
	  break;
  }
  default:
    return false;
  }
  return true;
}

template <class UserData>
bool ChunkRenderReader<UserData>::resetParam(Primitive* prim, int param, const char* name)
{
  switch (param)
  {
  case ChunkPrimParam::VISIBLE:
    prim->visible = true;
    prim->iteration = iteration;
    break;
  case ChunkPrimParam::ORDER:
    if (!primitives.move(prim->id,0)) return false;
    prim->iteration = iteration;
    break;
  case ChunkPrimParam::VSHADER:
    if (prim->vShader != NULL)
    {
      if (prim->vShader!=NULL)
	prim->vShader->users.remove(prim);
      prim->vShader = NULL;
      prim->iteration = iteration;
      if (!setVShader(prim, NULL)) return false;
    }
    break;
  case ChunkPrimParam::PSHADER:
    if (prim->pShader != NULL)
    {
      if (prim->pShader!=NULL)
	prim->pShader->users.remove(prim);
      prim->pShader = NULL;
      prim->iteration = iteration;
      if (!setPShader(prim,NULL)) return false;
    }
    break;
  case ChunkPrimParam::VBUFFER_ID:
    prim->vBuffer[name].buffer = NULL;
    if (!prim->vBuffer[name].modified())
    {
      typename Primitive::VBData* dest = &prim->vBuffer[name];
      if (!resetVBuffer(prim,name,dest)) return false;
      if (dest->buffer!=NULL)
	dest->buffer->users.remove(prim);
      prim->iteration = iteration;
      prim->vBuffer.erase(name);
    }
    break;
  case ChunkPrimParam::VBUFFER_NUMDATA:
    prim->vBuffer[name].numData = 0;
    prim->iteration = iteration;
    if (!prim->vBuffer[name].modified())
      prim->vBuffer.erase(name);
    break;
  case ChunkPrimParam::VBUFFER_V0:
    prim->vBuffer[name].v0 = 0;
    prim->iteration = iteration;
    if (!prim->vBuffer[name].modified())
      prim->vBuffer.erase(name);
    break;
  case ChunkPrimParam::NBV:
    prim->nbv = -1;
    prim->iteration = iteration;
    break;
  case ChunkPrimParam::IBUFFER_ID:
    if (prim->iBuffer.buffer!=NULL)
      prim->iBuffer.buffer->users.remove(prim);
    prim->iBuffer.buffer = NULL;
    prim->iteration = iteration;
    break;
  case ChunkPrimParam::IBUFFER_I0:
    prim->iBuffer.i0 = 0;
    prim->iteration = iteration;
    break;
  case ChunkPrimParam::IBUFFER_NBI:
    prim->iBuffer.nbi = -1;
    prim->iteration = iteration;
    break;
  case ChunkPrimParam::TEXTURE:
    if (prim->textures.find(name) != prim->textures.end())
    {
      typename Primitive::TData* dest = &prim->textures[name];
      if (!resetTexture(prim,name,dest)) return false;
      if (dest->texture!=NULL)
	dest->texture->users.remove(prim);
      prim->textures.erase(name);
      prim->iteration = iteration;
    }
    break;
  case ChunkPrimParam::PARAMVSHADER:
    if (prim->paramVShader.find(name) != prim->paramVShader.end())
    {
      if (!resetParamVShader(prim,name,&prim->paramVShader[name])) return false;
      prim->paramVShader.erase(name);
      prim->iteration = iteration;
    }
    break;
  case ChunkPrimParam::PARAMPSHADER:
    if (prim->paramPShader.find(name) != prim->paramPShader.end())
    {
      if (!resetParamPShader(prim,name,&prim->paramPShader[name])) return false;
      prim->paramPShader.erase(name);
      prim->iteration = iteration;
    }
    break;
  case ChunkPrimParam::PARAMOPENGL:
    if (prim->paramOpenGL.find(name) != prim->paramOpenGL.end())
    {
      if (!resetParamOpenGL(prim,name,&prim->paramOpenGL[name])) return false;
      prim->paramOpenGL.erase(name);
      prim->iteration = iteration;
    }
    break;
  case ChunkPrimParam::TRANSFORM:
    prim->xform.mat.identity();
    prim->updateAABB();
    prim->xform.iteration = iteration;
    //if (prim == camera) cameraMoved();
    break;
  case ChunkPrimParam::PROJECTION:
    prim->proj.mat.identity();
    prim->proj.iteration = iteration;
    //if (prim == camera) cameraMoved();
    break;
  case ChunkPrimParam::TRANSFORM_POSITION:
    prim->xform.position.clear();
    prim->xform.update();
    prim->updateAABB();
    prim->xform.iteration = iteration;
    //if (prim == camera) cameraMoved();
    break;
  case ChunkPrimParam::TRANSFORM_ROTATION:
    prim->xform.rotation.clear();
    prim->xform.update();
    prim->updateAABB();
    prim->xform.iteration = iteration;
    //if (prim == camera) cameraMoved();
    break;
  case ChunkPrimParam::TRANSFORM_SCALE:
    prim->xform.scale = Vec3f(1,1,1);
    prim->xform.update();
    prim->updateAABB();
    prim->xform.iteration = iteration;
    //if (prim == camera) cameraMoved();
    break;
  case ChunkPrimParam::PARAMDISPLAY:
    if (prim->paramDisplay.find(name) != prim->paramDisplay.end())
    {
      if (!resetParamDisplay(prim,name,&prim->paramDisplay[name])) return false;
      prim->paramDisplay.erase(name);
      prim->iteration = iteration;
    }
  case ChunkPrimParam::TARGETGROUP:
  {
	  prim->targetGroups = 1;
	  updateTargetGroups(prim);
	  break;
  }
    break;
  default:
    return false;
  }
  return true;
}

#undef ERRMSG

} // namespace render

} // namespace flowvr

#endif // CHUNKREADER_INL
