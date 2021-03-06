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
#include <fstream>
#include <iostream>

#include <flowvr/module.h>
#include <flowvr/render/chunk.h>
#include <flowvr/render/chunkrenderwriter.h>
#include <ftl/vec.h>
#include <ftl/quat.h>
#include <flowvr/utils/cmdline.h>

#include <flowvr/render/mesh.h>

using namespace flowvr::render;


flowvr::utils::FlagOption optNoIn("noin",'n',"do not use input ports");
flowvr::utils::FlagOption optFlip("flip",'f',"flip mesh");
flowvr::utils::FlagOption optClose("close",'c',"close mesh");
flowvr::utils::Option<ID> id("id", 'I' , "Force the first ID of the created primitives", flowvr::ID(0), false);


float scale=1;
flowvr::utils::Option<float> optScale("scale",'s',"scale the mesh [scale = 1]",&scale);

std::string matrixFile;
flowvr::utils::Option<std::string> optMatrixFile("matrixFile",'m',"file for matrix initialisation",&matrixFile);

ftl::Mat4x4f matrixTransformation;

flowvr::ModuleAPI* module = NULL;
flowvr::render::ChunkRenderWriter scene;

std::map<std::string,flowvr::ID> sharedVS;
std::map<std::string,flowvr::ID> sharedPS;
std::map<std::string,flowvr::ID> sharedT;
std::map<std::string,flowvr::ID> sharedNM;

flowvr::ID loadSharedTexture(const std::string& filename)
{
    flowvr::ID& id = sharedT[filename];
    if (!id)
    {
        id = module->generateID();
        flowvr::render::ChunkTexture* t = scene.loadTexture(id, filename.c_str());
        if (t)
            std::cout << "FlowVR Render: loaded " << t->nx << "x" << t->ny << "x" << ftl::Type::nx(t->pixelType)*(ftl::Type::elemSize(t->pixelType)==0?1:8*ftl::Type::elemSize(t->pixelType)) << " texture " << filename << std::endl;
        else
            std::cerr << "ERROR FlowVR Render: loading texture "<<filename<<" failed."<<std::endl;
    }
    //else std::cout << "FlowVR Render: Reusing texture " << filename << std::endl;
    return id;
}

flowvr::ID loadSharedNormalMap(const std::string& filename)
{
    flowvr::ID& id = sharedNM[filename];
    if (!id)
    {
        id = module->generateID();
        flowvr::render::ChunkTexture* t = scene.loadTextureNormalMap(id, filename.c_str());
        if (t)
            std::cout << "FlowVR Render: loaded " << t->nx << "x" << t->ny << "x" << ftl::Type::nx(t->pixelType)*(ftl::Type::elemSize(t->pixelType)==0?1:8*ftl::Type::elemSize(t->pixelType)) << " normal map " << filename << std::endl;
        else
            std::cerr << "ERROR FlowVR Render: loading normal map "<<filename<<" failed."<<std::endl;
    }
    //else std::cout << "FlowVR Render: Reusing normal map " << filename << std::endl;
    return id;
}

flowvr::ID loadSharedVertexShader(const std::string& filename, const std::string& predefines="")
{
    flowvr::ID& id = sharedVS[filename + "\n" + predefines];
    if (!id)
    {
        id = module->generateID();
        scene.loadVertexShader(id, filename, predefines.empty() ? NULL : predefines.c_str());
    }
    return id;
}

flowvr::ID loadSharedPixelShader(const std::string& filename, const std::string& predefines="")
{
    flowvr::ID& id = sharedPS[filename + "\n" + predefines];
    if (!id)
    {
        id = module->generateID();
        scene.loadPixelShader(id, filename, predefines.empty() ? NULL : predefines.c_str());
    }
    return id;
}



int main(int argc, char** argv)
{
    flowvr::utils::CmdLine cmd(std::string("Mesh viewer. Usage: ") + std::string(argv[0]) + std::string(" [options] mesh.obj [texture.png]" ));
    bool error=false;
    if (!cmd.parse(argc,argv,&error))
    {
    	if(error)
    		std::cerr << "pasing error on cmdline!" << std::endl;

    	std::cout << cmd.help();

        return error?1:0;
    }

    if (cmd.args.size()<1) // || cmd.args.size()>2)
    {
        std::cerr << cmd.help();
        return 1;
    }

    SceneOutputPort pOut("scene");
    flowvr::InputPort pInMatrix("matrix");

    std::vector<flowvr::Port*> ports;
    ports.push_back(&pOut);
    if (!optNoIn)
        ports.push_back(&pInMatrix);

    module = flowvr::initModule(ports);
    if (module == NULL) return 1;

    matrixTransformation.identity();

    //Get the initialization matrix by file
    if(!matrixFile.empty())
    {
        std::ifstream fMatrix(matrixFile.c_str());

        if (fMatrix.bad() || !fMatrix.is_open())
        {
            std::cerr << "Could not open Matrix file: " << matrixFile << "\n";
            return false;
        }
        if (fMatrix.eof())
        {
            std::cerr << "Problem with Matrix file: " << matrixFile << "\n";
            return false;
        }
        fMatrix >> matrixTransformation;
        //Safety in case you didn't fill the last line of the matrix
        matrixTransformation(3,3)=1.0;
    }

        std::vector<ID> prims;
        for (int onum = 0; onum < cmd.args.size(); onum++)
        {

            Mesh obj;

            if (!obj.load(cmd.args[onum].c_str()))
            {
                std::cerr << "ERROR: Failed to read "<<cmd.args[onum]<<std::endl;
                continue;
            }
            std::cout << "Mesh bbox="<<obj.calcBBox()<<std::endl;

            if (optFlip)
                obj.calcFlip();
            if (optClose)
            {
                std::cout << "Mesh is "<<(obj.isClosed()?"":"NOT ")<<"closed."<<std::endl;
                std::cout << "Closing mesh..."<<std::endl;
                obj.close();
                std::cout << "Mesh closed."<<std::endl;
                std::cout << "Mesh is "<<(obj.isClosed()?"":"NOT ")<<"closed."<<std::endl;
            }

            obj.optimize();

            obj.calcNormals(true);

            ID idVB = module->generateID();
            ID idIB = module->generateID();

            obj.writeMesh(&scene, idIB, idVB);

            int nprims = obj.nbmatg();

            bool useMat = (obj.getAttrib(Mesh::MESH_MATERIALS) && (nprims > 0));

            if (nprims == 0) nprims = 1;

            for (int p=0; p<nprims; p++)
            {

                ID idP;
                if (id)
                {
                    idP = id;
                    id = id+1;
                }
                else
                {
                    idP = module->generateID();
                }
                scene.addPrimitive(idP,cmd.args[onum]);


                Mesh::Material* m = (useMat ? obj.mat_groups[p].mat : NULL);

                std::string texture;
                if (m && !m->map_diffuse.empty())
                	texture = m->map_diffuse;
                else if (m && !m->map_ambient.empty())
                    texture = m->map_ambient;
                else if (m && !m->map_specular.empty())
                    texture = m->map_specular;

                std::string texbump;
                if (m)
                	texbump = m->map_bump;

                ID idTex = 0;
                ID idTexBump = 0;
                ID idVS = 0;
                ID idPS = 0;

                ftl::Vec4f ambient  ( 0.3f, 0.3f, 0.3f, 1.0f );
                ftl::Vec3f diffuse  ( 0.6f, 0.6f, 0.6f );
                ftl::Vec4f specular ( 1.0f, 1.0f, 1.0f, 32.0f );
                if (m != NULL)
                {
                    ambient  = ftl::Vec4f( m->ambient * 0.5, m->alpha );
                    diffuse  = ftl::Vec3f( m->diffuse );
                    specular = ftl::Vec4f( m->specular, m->shininess );
                }

                bool hasSpecular = (specular[3] > 0.001f);

                std::string predefs;

                if (hasSpecular)
                {
                    predefs += "#define SPECULAR 1\n";
                }

                if (!texture.empty())
                {
                    idTex = loadSharedTexture(texture);
                }

                if (!texbump.empty())
                {
                    idTexBump = loadSharedNormalMap(texbump);
                }

                if (!idTex)
                {
                    idVS = loadSharedVertexShader("shaders/obj_v.cg",predefs);
                    idPS = loadSharedPixelShader("shaders/obj_mat_p.cg",predefs);
                }
                else if (!idTexBump)
                {
                    idVS = loadSharedVertexShader("shaders/obj_color_v.cg",predefs);
                    idPS = loadSharedPixelShader("shaders/obj_mat_color_p.cg",predefs);
                }
                else
                {
                    idVS = loadSharedVertexShader("shaders/obj_color_tangent_v.cg",predefs);
                    idPS = loadSharedPixelShader("shaders/obj_mat_color_tangent_p.cg",predefs);
                }

                scene.addParamID(idP, ChunkPrimParam::VSHADER,"",idVS);
                scene.addParamID(idP, ChunkPrimParam::PSHADER,"",idPS);

                obj.writeParams(&scene, idP, idIB, idVB);

                if (nprims > 1)
                {
                    scene.addParamID(idP, ChunkPrimParam::IBUFFER_I0,"",obj.mat_groups[p].f0*3);
                    scene.addParamID(idP, ChunkPrimParam::IBUFFER_NBI,"",obj.mat_groups[p].nbf*3);
                }
                if (idTex)
                    scene.addParamID(idP, ChunkPrimParam::TEXTURE,"texcolor",idTex);
                if (idTexBump)
                    scene.addParamID(idP, ChunkPrimParam::TEXTURE,"normalmap",idTexBump);

                scene.addParam(idP, ChunkPrimParam::PARAMOPENGL, "CullFace", 1);

                ftl::Vec4f color ( diffuse, 1 );

                scene.addParam(idP, flowvr::render::ChunkPrimParam::PARAMPSHADER, "mat_ambient", ambient);
                scene.addParam(idP, flowvr::render::ChunkPrimParam::PARAMPSHADER, "mat_diffuse", diffuse);
                //Scale the mesh
                if(optScale) {
                    scene.addParam(idP, flowvr::render::ChunkPrimParam::TRANSFORM_SCALE,"",matrixScale(scale));
                }	

                if (hasSpecular)
                    scene.addParam(idP, flowvr::render::ChunkPrimParam::PARAMPSHADER, "mat_specular", specular);

                prims.push_back(idP);

            }
        }

        std::cout << prims.size()<<" primitives created."<<std::endl;
        if (id)
            std::cout << "Next available ID: 0x"<<std::hex<<id<<std::dec<<std::endl;

        scene.put(&pOut);

        int it=-1;
        while (module->wait())
        {
            ++it;
            //std::cout << it << std::endl;

            if (!optNoIn)
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
        			}
                }
            }

			for (unsigned int p=0; p<prims.size(); p++)
					scene.addParam(prims[p],ChunkPrimParam::TRANSFORM,"",matrixTransformation*matrixScale(scale));

            scene.put(&pOut);

            if(optNoIn || !pInMatrix.isConnected())
                break;
        }

        module->close();
        return 0;
    }
