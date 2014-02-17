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
* File: ./src/viewers/spheres.cpp                                 *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#include <flowvr/module.h>
#include <flowvr/id.h>
#include <flowvr/render/chunk.h>
#include <flowvr/render/mesh.h>
#include <flowvr/render/chunkrenderwriter.h>
#include <ftl/vec.h>
#include <ftl/quat.h>

#include <math.h>
#include <stdlib.h>

double drand()
{
  return (double)rand()/(double)RAND_MAX;
}

using namespace flowvr::render;
using namespace flowvr;
using namespace std;


struct Object
{
  Object( ID _id )
  : id( _id )
  , pos( Vec3f(0,0,0) )
  , rotAxis( Vec3f( 0,1,0 ) )
  , depl( Vec3f(0,0,0) )
  , r(1.0f)
  {}

  Object()
  : id(~0)
  {}

  ID id;
  Vec3f pos;
  Vec3f rotAxis;
  Vec3f depl;
  float r;
};

map<string,flowvr::ID> sharedVS;
map<string,flowvr::ID> sharedPS;
map<string,flowvr::ID> sharedT;
map<string,flowvr::ID> sharedNM;

flowvr::ID loadSharedTexture(const string& filename, ModuleAPI *module, ChunkRenderWriter &scene )
{
    flowvr::ID& id = sharedT[filename];
    if (!id)
    {
        id = module->generateID();
        ChunkTexture* t = scene.loadTexture(id, filename.c_str());
        if (t)
            cout << "FlowVR Render: loaded " << t->nx << "x" << t->ny << "x" << ftl::Type::nx(t->pixelType)*(ftl::Type::elemSize(t->pixelType)==0?1:8*ftl::Type::elemSize(t->pixelType)) << " texture " << filename << endl;
        else
            cerr << "ERROR FlowVR Render: loading texture "<<filename<<" failed."<<endl;
    }
    //else cout << "FlowVR Render: Reusing texture " << filename << endl;
    return id;
}

flowvr::ID loadSharedNormalMap(const string& filename, ModuleAPI *module, ChunkRenderWriter &scene )
{
    flowvr::ID& id = sharedNM[filename];
    if (!id)
    {
        id = module->generateID();
        ChunkTexture* t = scene.loadTextureNormalMap(id, filename.c_str());
        if (t)
            cout << "FlowVR Render: loaded " << t->nx << "x" << t->ny << "x" << ftl::Type::nx(t->pixelType)*(ftl::Type::elemSize(t->pixelType)==0?1:8*ftl::Type::elemSize(t->pixelType)) << " normal map " << filename << endl;
        else
            cerr << "ERROR FlowVR Render: loading normal map "<<filename<<" failed."<<endl;
    }
    //else cout << "FlowVR Render: Reusing normal map " << filename << endl;
    return id;
}

flowvr::ID loadSharedVertexShader(const string& filename, ModuleAPI *module , ChunkRenderWriter &scene, const string& predefines="")
{
    flowvr::ID& id = sharedVS[filename + "\n" + predefines];
    if (!id)
    {
        id = module->generateID();
        scene.loadVertexShader(id, filename, predefines.empty() ? NULL : predefines.c_str());
    }
    return id;
}

flowvr::ID loadSharedPixelShader(const string& filename, ModuleAPI *module , ChunkRenderWriter &scene, const string& predefines="")
{
    flowvr::ID& id = sharedPS[filename + "\n" + predefines];
    if (!id)
    {
        id = module->generateID();
        scene.loadPixelShader(id, filename, predefines.empty() ? NULL : predefines.c_str());
    }
    return id;
}



bool loadObj( const char *filename, ModuleAPI *module, ChunkRenderWriter &scene, list<Object> &objects )
{
    Mesh obj;

	if (!obj.load(filename))
	{
		cerr << "ERROR: Failed to read " << filename << endl;
		return false;
	}

	obj.optimize();
	obj.calcNormals(true);

	ID idVB = module->generateID();
	ID idIB = module->generateID();

	obj.writeMesh(&scene, idIB, idVB);

	int nprims = obj.nbmatg();

	bool useMat = (obj.getAttrib(Mesh::MESH_MATERIALS) and (nprims > 0));

	if (nprims == 0)
		nprims = 1;

	for (int p = 0; p < nprims; p++)
	{
		ID idP = module->generateID();

		Object o;
		o.r       = 1.0f;
		o.pos     =  Vec3f(5, 8, 5);
		o.rotAxis = Vec3f(drand(), drand(), drand());
		o.rotAxis.normalize();
		o.depl    = Vec3f(drand() * 0.2 - 0.1, drand() * 0.2 - 0.1, -drand()* 0.1);
		o.id      = idP;

		scene.addPrimitive(idP, filename);
		objects.push_back(o);

		Mesh::Material* m = (useMat ? obj.mat_groups[p].mat : NULL);

		string texture;
		if (m && !m->map_diffuse.empty())
			texture = m->map_diffuse;
		else if (m && !m->map_ambient.empty())
			texture = m->map_ambient;
		else if (m && !m->map_specular.empty())
			texture = m->map_specular;

		string texbump;
		if (m)
			texbump = m->map_bump;

		ID idTex = 0;
		ID idTexBump = 0;
		ID idVS = 0;
		ID idPS = 0;

		ftl::Vec4f ambient(0.3f, 0.3f, 0.3f, 1.0f);
		ftl::Vec3f diffuse(0.6f, 0.6f, 0.6f);
		ftl::Vec4f specular(1.0f, 1.0f, 1.0f, 32.0f);
		if (m != NULL)
		{
			ambient = ftl::Vec4f(m->ambient * 0.5, m->alpha);
			diffuse = ftl::Vec3f(m->diffuse);
			specular = ftl::Vec4f(m->specular, m->shininess);
		}

		bool hasSpecular = (specular[3] > 0.001f);

		string predefs;

		if (hasSpecular)
			predefs += "#define SPECULAR 1\n";

		if (!texture.empty())
			idTex = loadSharedTexture(texture, module, scene);

		if (!texbump.empty())
			idTexBump = loadSharedNormalMap(texbump, module, scene);

		if (!idTex)
		{
			idVS = loadSharedVertexShader("shaders/obj_v.cg", module, scene, predefs);
			idPS = loadSharedPixelShader("shaders/obj_mat_p.cg", module, scene, predefs);
		}
		else if (!idTexBump)
		{
			idVS = loadSharedVertexShader("shaders/obj_color_texture_v.cg", module, scene, predefs);
			idPS = loadSharedPixelShader("shaders/obj_mat_color_p.cg", module, scene, predefs);
		}
		else
		{
			idVS = loadSharedVertexShader("shaders/obj_color_tangent_v.cg",module, scene, predefs);
			idPS = loadSharedPixelShader("shaders/obj_mat_color_tangent_p.cg",module, scene, predefs);
		}

		scene.addParamID(idP, ChunkPrimParam::VSHADER, "", idVS);
		scene.addParamID(idP, ChunkPrimParam::PSHADER, "", idPS);

		obj.writeParams(&scene, idP, idIB, idVB);

		if (nprims > 1)
		{
			scene.addParamID(idP, ChunkPrimParam::IBUFFER_I0, "", obj.mat_groups[p].f0 * 3);
			scene.addParamID(idP, ChunkPrimParam::IBUFFER_NBI, "", obj.mat_groups[p].nbf * 3);
		}
		if (idTex)
			scene.addParamID(idP, ChunkPrimParam::TEXTURE, "texcolor", idTex);
		if (idTexBump)
			scene.addParamID(idP, ChunkPrimParam::TEXTURE, "normalmap", idTexBump);

		scene.addParam(idP, ChunkPrimParam::PARAMOPENGL, "CullFace", 1);

		ftl::Vec4f color(diffuse, 1);

		scene.addParam(idP, ChunkPrimParam::PARAMPSHADER, "mat_ambient", ambient);
		scene.addParam(idP, ChunkPrimParam::PARAMPSHADER, "mat_diffuse", diffuse);

		if (hasSpecular)
			scene.addParam(idP, ChunkPrimParam::PARAMPSHADER, "mat_specular", specular);
	}

	return true;
}

int main(int argc, char** argv)
{

	SceneOutputPort pOut("scene");
	vector<flowvr::Port*> ports;
	ports.push_back(&pOut);

	flowvr::ModuleAPI* module = flowvr::initModule(ports);
	if (module == NULL)
		return 1;

	ChunkRenderWriter scene;

	ID idT  = module->generateID();
	ID idVB = module->generateID();
	ID idIB = module->generateID();
	ID idVS = module->generateID();
	ID idPS = module->generateID();

	list<Object> objs;

//	string filename = "images/sphere1.png";
	if(loadObj( "models/sphere.obj", module, scene, objs ) == false)
		return 2;

	scene.addParam(objs.front().id, ChunkPrimParam::TRANSFORM_POSITION, "",objs.front().pos);

//		scene.loadVertexShader(idVS, "shaders/sphere_v.cg");
//		scene.loadPixelShader(idPS, "shaders/sphere_p.cg");
//
//		Object o;
//		o.id = module->generateID();
//		o.r = 1.0f;
//		o.rotAxis = Vec3f(0, 0, 1);
//		o.rotAxis.normalize();
//		o.pos = Vec3f(5, 8, 5);
//		scene.addDefaultPrimitive(o.id, "Sphere", idT, idVB, idIB, idVS, idPS);
//		scene.addParam(o.id, ChunkPrimParam::TRANSFORM_POSITION, "", o.pos);
//		scene.addParam(o.id, ChunkPrimParam::PARAMVSHADER, "mapscale", Vec2f(2,1));
//		objs.push_back(o);
//
//		if (!scene.loadTexture(idT, filename))
//			scene.addDefaultTexture(idT);
//
//		scene.addMeshSphere(idVB, idIB);
//	}

	// Add other objects in scene

	ID cubeVB = module->generateID();
	ID cubeIB = module->generateID();
	scene.addMeshCube(cubeVB, cubeIB);
	ID cube = module->generateID();
	scene.addDefaultPrimitive(cube, "Floor", 0, cubeVB, cubeIB, 0, 0, Vec4f(0.5, 0.5, 0.5, 0.5));
	scene.addParam(cube, ChunkPrimParam::PARAMOPENGL, "DepthWrite", false);
	scene.addParam(cube, ChunkPrimParam::PARAMOPENGL, "Blend", true);
	scene.addParam(cube, ChunkPrimParam::ORDER, "", 1);
	scene.addParam(cube, ChunkPrimParam::TRANSFORM_SCALE, "", Vec3f(5, 5, 0.2));
	scene.addParam(cube, ChunkPrimParam::TRANSFORM_POSITION, "", Vec3f(5, 5,-0.1));

	scene.put(&pOut);

	int period = 100;
	float gravity = 0.001;

	int objnum = 1;
	char objname[32];

	int it = 1;
	while (module->wait())
	{
		if (period && (it % period == 0))
		{
			objs.front().r *= 0.99;
			scene.addParam(objs.front().id, ChunkPrimParam::TRANSFORM_SCALE,"", objs.front().r);
			Object o;
			o.id = module->generateID();
			o.r = objs.front().r;
			o.pos = objs.front().pos;
			o.rotAxis = Vec3f(drand(), drand(), drand());
			o.rotAxis.normalize();
			o.depl = Vec3f(drand() * 0.2 - 0.1, drand() * 0.2 - 0.1, -drand()* 0.1);

			snprintf(objname, sizeof(objname), "Sphere%d", objnum);
			++objnum;
			scene.addPrimitive(o.id, objname, objs.front().id);
			scene.addParam(o.id, ChunkPrimParam::PARAMVSHADER, "color", Vec4f(1,0,0,1) ); //Vec4f(drand(), drand(), drand(), 0.6f));
			objs.push_back(o);
		}

		// Build data
		for (list<Object>::iterator i = objs.begin(); i != objs.end(); ++i)
		{
			if (i != objs.begin())
			{
				i->pos += i->depl;
				i->depl[2] -= gravity;
				for (int c = 0; c < 3; c++)
				{
					if ((i->pos[c] + i->r >= 10 && i->depl[c] > 0) || (i->pos[c] - i->r <= 0 && i->depl[c] < 0))
						i->depl[c] = -0.95 * i->depl[c];
				}
				scene.addParam(i->id, ChunkPrimParam::TRANSFORM_POSITION, "",i->pos);
			}
			scene.rotatePrimitive(i->id, it, i->rotAxis);
		}

		// Send message
		scene.put(&pOut);

		++it;
	}

  module->close();

  return 0;
}
