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
* File: ./src/viewers/obj.cpp                                     *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include <flowvr/module.h>
#include <flowvr/render/chunk.h>
#include <flowvr/render/chunkrenderwriter.h>
#include <ftl/vec.h>
#include <ftl/quat.h>
#include <flowvr/render/bbox.h>

using namespace flowvr::render;

struct Vertex //__attribute__((packed))
{
  Vec3f p;
  Vec2f t;
  Vec3f n;
};

struct Face //__attribute__((packed))
{
  int p[3];
  bool needNormal;
};

const char* skip(const char* c)
{
  while (*c==' ' || *c=='\t') ++c;
  return c;
}

class ObjReader
{
public:

  std::vector<Vertex> points;
  std::vector<Face> faces;

protected:
  std::vector<Vec3f> t_v;
  std::vector<Vec2f> t_vt;
  std::vector<Vec3f> t_vn;

  std::vector<int> t_v_p0; // points using this position
  std::vector<int> t_p_prev; // previous point at the same position

  int merge;

  bool readPoint(const char** c, int& index)
  {
    bool needNormal = false;
    Vertex v;
    int i;
    int iv = strtol(skip(*c),(char**)c,10); if (iv<0) iv += t_v.size();
    if ((unsigned)iv<t_v.size())
      v.p = t_v[iv];
    else iv = 0;
    if (**c=='/')
    {
      ++(*c);
      i = strtol(*c,(char**)c,10); if (i<0) i += t_vt.size();
      if ((unsigned)i<t_vt.size())
	v.t = t_vt[i];
    }
    //else std::cout << *c << "\n";
    if (**c=='/')
    {
      ++(*c);
      i = strtol(*c,(char**)c,10); if (i<0) i += t_vn.size();
      if ((unsigned)i<t_vn.size())
	v.n = t_vn[i];
    }
    else needNormal = true;

    // Find first equal point
    i = t_v_p0[iv];
    while (i>=0)
    {
      if (points[i].t == v.t && points[i].n == v.n)
      { // found a match
	++merge;
	index = i;
	return needNormal;
      }
      // else find next point
      i = t_p_prev[i];
    }
    // no match : new point
    i = points.size();
    points.push_back(v);
    t_p_prev.push_back(t_v_p0[iv]);
    t_v_p0[iv] = i;
    index = i;
    return needNormal;
  }

public:

  bool read(const char* filename)
  {
    FILE* fp = fopen(filename,"r");
    if (fp==NULL) return false;
    
    Vec3f v3;
    Vec2f v2;
    Face f;

    //Vec2f tmin,tmax;

    merge = 0;

    //    char * line = NULL;
    //size_t len = 0;
    //    ssize_t read;
    char line[5000];

    t_v.clear(); t_v.push_back(v3);
    t_v_p0.clear(); t_v_p0.push_back(-1);
    t_vt.clear(); t_vt.push_back(v2);
    t_vn.clear(); t_vn.push_back(v3);

    // Not Posix compatible
    //    while ((read = getline(&line, &len, fp)) != -1)
    line [sizeof(line)-1]='\0';
    while (fgets(line, sizeof(line), fp) != NULL)
      {
      const char* c = line;
      switch (*c)
      {
      case '#':
	if (!strncmp(line,"#Vertex Count ",strlen("#Vertex Count ")))
	{
	  int n = atoi(line+strlen("#Vertex Count "));
	  std::cout << "Allocating "<<n<<" vertices."<<std::endl;
	  t_v.reserve(n+1);
	  t_v_p0.reserve(n+1);
	}
	else if (!strncmp(line,"#UV Vertex Count ",strlen("#UV Vertex Count ")))
	  t_vt.reserve(atoi(line+strlen("#UV Vertex Count "))+1);
	else if (!strncmp(line,"#Normal Vertex Count ",strlen("#Normal Vertex Count ")))
	  t_vn.reserve(atoi(line+strlen("#Normal Vertex Count "))+1);
	else if (!strncmp(line,"#Face Count ",strlen("#Face Count ")))
	{
	  int n = atoi(line+strlen("#Face Count "));
	  faces.reserve(2*n);
	  points.reserve(4*n);
	  t_p_prev.reserve(4*n);
	}
	break;
      case 'v':
	++c;
	switch(*c)
	{
	case ' ': // position
	  v3.x() = strtof(skip(c),(char**)&c);
	  v3.y() = strtof(skip(c),(char**)&c);
	  v3.z() = strtof(skip(c),(char**)&c);
	  t_v.push_back(v3);
	  t_v_p0.push_back(-t_v.size());
	  break;
	case 't': // texture
	  ++c;
	  v2.x() = strtof(skip(c),(char**)&c); //+0.5f/4096;
	  v2.y() = strtof(skip(c),(char**)&c); //+0.5f/4096;
// 	  if (t_vt.empty())
// 	  {
// 	    tmin = v2;
// 	    tmax = v2;
// 	  }
// 	  else
// 	  {
// 	    if (tmin.x()>v2.x()) tmin.x()=v2.x(); else if (tmax.x()<v2.x()) tmax.x()=v2.x();
// 	    if (tmin.y()>v2.y()) tmin.y()=v2.y(); else if (tmax.y()<v2.y()) tmax.y()=v2.y();
// 	  }
	  t_vt.push_back(v2);
	  break;
	case 'n': // normal
	  ++c;
	  v3.x() = strtof(skip(c),(char**)&c);
	  v3.y() = strtof(skip(c),(char**)&c);
	  v3.y() = strtof(skip(c),(char**)&c);
	  t_vn.push_back(v3);
	  break;
	}
	break;
      case 'f':
	++c;
	switch(*c)
	{
	case ' ': // face
	{
	  f.needNormal = readPoint(&c,f.p[0]);
	  f.needNormal = readPoint(&c,f.p[1]);
	  c = skip(c);
	  while (*c != '\n' && *c!='\0')
	  {
	    f.needNormal = readPoint(&c,f.p[2]);
	    faces.push_back(f);
	    f.p[1] = f.p[2];
	    c = skip(c);
	  }
	}
	break;
	}
	break;
      }
    }
//    if (line)
//      free(line);
    fclose(fp);
    std::cout<<t_v.size()-1<<" positions, "<<t_vt.size()-1<<" texcoords, "<<t_vn.size()-1<<" normals"<<std::endl;
    //std::cout<<"Texture mapping <"<<tmin<<">-<"<<tmax<<">"<<std::endl;
    std::cout<<points.size()<<" final points, "<<faces.size()<<" triangles, "<<merge<<" points merged"<<std::endl;

    // Computing normals

    for (unsigned int i=0;i<faces.size();i++)
    {
      if (faces[i].needNormal)
      {
	Vec3f n = cross(points[faces[i].p[1]].p-points[faces[i].p[0]].p,points[faces[i].p[2]].p-points[faces[i].p[0]].p);
	n.normalize();
	points[faces[i].p[0]].n += n;
	points[faces[i].p[1]].n += n;
	points[faces[i].p[2]].n += n;
      }
    }

    for (unsigned int i0=0;i0<t_v.size();i0++)
    {
      if (t_v_p0[i0]>=0)
      {
	Vec3f n;
	int i = t_v_p0[i0];
	do
	{
	  n+=points[i].n;
	  i = t_p_prev[i];
	}
	while (i>=0);
	n.normalize();
	i = t_v_p0[i0];
	do
	{
	  points[i].n=n;
	  i = t_p_prev[i];
	}
	while (i>=0);
      }
    }

    std::cout << "Normals computed."<<std::endl;

    return true;
  }
};

int main(int argc, char** argv)
{
  if (argc<2)
  {
    std::cout << "Usage: "<<argv[0]<<" mesh.obj [texture.png]"<<std::endl;
    return 0;
  }

  ObjReader obj;

  if (!obj.read(argv[1]))
  {
    std::cerr << "Failed to read "<<argv[1]<<std::endl;
    return 1;
  }

  SceneOutputPort pOut("scene");
  flowvr::InputPort pInMatrix("matrix");

  std::vector<flowvr::Port*> ports;
  ports.push_back(&pOut);
  ports.push_back(&pInMatrix);
  
  flowvr::ModuleAPI* module = flowvr::initModule(ports);
  if (module == NULL) return 1;

  ChunkRenderWriter scene;

  ID idP = module->generateID();
  ID idVB = module->generateID();
  ID idIB = module->generateID();
  ID idVS = module->generateID();
  ID idPS = module->generateID();

  int dataType[3] = { Type::Vec3f, Type::Vec2f, Type::Vec3f };

  BBox bb;
  for (unsigned int i=0;i<obj.points.size();i++)
  {
    bb+=obj.points[i].p;
  }

  ChunkVertexBuffer* vb = scene.addVertexBuffer(idVB, obj.points.size(), 3, dataType, bb);
  Vertex* v = (Vertex*) vb->data();
  for (unsigned int i=0;i<obj.points.size();i++)
    v[i] = obj.points[i];

  if (obj.points.size()<=256)
  {
    ChunkIndexBuffer* ib = scene.addIndexBuffer(idIB, obj.faces.size()*3, Type::Byte, ChunkIndexBuffer::Triangle);
    Vec<3,char>* f = (Vec<3,char>*)ib->data();
    for (unsigned int i=0;i<obj.faces.size();i++)
    {
      f[i][0] = obj.faces[i].p[0];
      f[i][1] = obj.faces[i].p[1];
      f[i][2] = obj.faces[i].p[2];
    }
  }
  else if (obj.points.size()<=65536)
  {
    ChunkIndexBuffer* ib = scene.addIndexBuffer(idIB, obj.faces.size()*3, Type::Short, ChunkIndexBuffer::Triangle);
    Vec<3,short> *f = (Vec<3,short>*)ib->data();
    for (unsigned int i=0;i<obj.faces.size();i++)
    {
      f[i][0] = obj.faces[i].p[0];
      f[i][1] = obj.faces[i].p[1];
      f[i][2] = obj.faces[i].p[2];
    }
  }
  else
  {
    ChunkIndexBuffer* ib = scene.addIndexBuffer(idIB, obj.faces.size()*3, Type::Int, ChunkIndexBuffer::Triangle);
    Vec<3,int> *f = (Vec<3,int>*)ib->data();
    for (unsigned int i=0;i<obj.faces.size();i++)
    {
      f[i][0] = obj.faces[i].p[0];
      f[i][1] = obj.faces[i].p[1];
      f[i][2] = obj.faces[i].p[2];
    }
  }

  ID idTexColor = 0;

  if (argc<3)
  {
    scene.loadVertexShader(idVS, "shaders/obj_v.cg");
    scene.loadPixelShader(idPS, "shaders/obj_p.cg");
  }
  else
  {
    idTexColor = module->generateID();
    scene.loadTexture(idTexColor,argv[2]);
    scene.loadVertexShader(idVS, "shaders/obj_color_v.cg");
    scene.loadPixelShader(idPS, "shaders/obj_color_p.cg");
  }

  scene.addPrimitive(idP,argv[0]);
  scene.addParamID(idP, ChunkPrimParam::VSHADER,"",idVS);
  scene.addParamID(idP, ChunkPrimParam::PSHADER,"",idPS);
  scene.addParamID(idP, ChunkPrimParam::VBUFFER_ID,"position",idVB);
  scene.addParamID(idP, ChunkPrimParam::VBUFFER_ID,"texcoord0",idVB);
  scene.addParam(idP, ChunkPrimParam::VBUFFER_NUMDATA,"texcoord0",1);
  scene.addParamID(idP, ChunkPrimParam::VBUFFER_ID,"normal",idVB);
  scene.addParam(idP, ChunkPrimParam::VBUFFER_NUMDATA,"normal",2);
  scene.addParamID(idP, ChunkPrimParam::IBUFFER_ID,"",idIB);
  scene.addParamEnum(idP, ChunkPrimParam::PARAMVSHADER, "Proj", ChunkPrimParam::Projection);
  scene.addParamEnum(idP, ChunkPrimParam::PARAMVSHADER, "ModelViewProj", ChunkPrimParam::ModelViewProjection);
  scene.addParamEnum(idP, ChunkPrimParam::PARAMVSHADER, "ModelViewIT", ChunkPrimParam::ModelView_InvTrans);
  scene.addParamEnum(idP, ChunkPrimParam::PARAMVSHADER, "ModelViewT", ChunkPrimParam::ModelView_Trans);
  if (idTexColor)
    scene.addParamID(idP, ChunkPrimParam::TEXTURE,"texcolor",idTexColor);

  Vec3f light(1,1,2); light.normalize();
  scene.addParam(idP, ChunkPrimParam::PARAMVSHADER, "lightdir", light);
  scene.addParam(idP, ChunkPrimParam::PARAMPSHADER, "lightdir", light);

  scene.put(&pOut);
ftl::Mat4x4f matrixTransformation;
matrixTransformation.identity();

  while (module->wait())
  {
    if (pInMatrix.isConnected())
    {
      flowvr::Message m;
      module->get(&pInMatrix, m);
      if(m.data.getSize()==sizeof(ftl::Mat4x4f) || m.data.getSize()==sizeof(ftl::Mat4x4d)){
              				if(m.data.getSize()==sizeof(ftl::Mat4x4f))
              					matrixTransformation = *(m.data.getRead<ftl::Mat4x4f>(0));
              				else
              					matrixTransformation = *(m.data.getRead<ftl::Mat4x4d>(0));
      scene.addParam(idP,ChunkPrimParam::TRANSFORM,"",*(m.data.getRead<Mat4x4f>(0)));
      }
    } else break;

    scene.put(&pOut);
  }

  module->close();
  return 0;
}
