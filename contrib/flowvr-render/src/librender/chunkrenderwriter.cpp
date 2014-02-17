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
 * File: ./src/librender/chunkwriter.cpp                           *
 *                                                                 *
 * Contacts:                                                       *
 *                                                                 *
 ******************************************************************/
#include <flowvr/render/chunkrenderwriter.h>

#include <string>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <iostream>


#include <flowvr/render/noise.h>



#define PI 3.14159265358979323846264338327

#ifndef NODEVIL
#include <IL/il.h>
#include <IL/ilu.h>
#if IL_VERSION >= 175
typedef void ILvoid; 
static ILboolean IsInit;
#else
extern ILboolean IsInit;
#endif
#endif

namespace flowvr
{

	namespace render
	{
		flowvr::utils::FilePath ChunkRenderWriter::path("FLOWVR_DATA_PATH");

		ChunkTexture* ChunkRenderWriter::addTexture(flowvr::ID id, int imageType, int pixelType, int nx, int ny, int nz, int tp)
		{
		  int b = 1; /*<< ((imageType&ChunkTexture::MASK_BLOC_LOG2) >> ChunkTexture::SHIFT_BLOC_LOG2);*/
		  int lineSize = ((nx+b-1)/b) * Type::size(pixelType);
		  ChunkTexture* texture = addChunk<ChunkTexture>(lineSize*(ny==0?1:(ny+b-1)/b)*(nz==0?1:(imageType&ChunkTexture::ARRAY)?nz:(nz+b-1)/b));
					texture->id = id;
					texture->resType = RES_TEXTURE;
					texture->level = -1;
					texture->gen = -1;
					texture->imageType = imageType;
					texture->nx = nx;
					texture->ny = ny;
					texture->nz = nz;
					texture->pixelType = pixelType;
					texture->tp = tp;

		  texture->lineSize = lineSize;
		  return texture;
		}

		ChunkVertexBuffer* ChunkRenderWriter::addVertexBuffer(flowvr::ID id, int nbp, int nbData, int* dataType, const BBox& bbox)
		{
			int vertexSize = 0;
			for (int i=0;i<nbData;i++)
				vertexSize += Type::size(dataType[i]);
			ChunkVertexBuffer* vb = addChunk<ChunkVertexBuffer>(nbData*sizeof(int)+nbp*vertexSize);
			vb->id = id;
			vb->resType = RES_VERTEXBUFFER;
			vb->level = -1;
			vb->gen = -1;
			vb->bbox[0][0] = bbox.a[0]; vb->bbox[0][1] = bbox.a[1]; vb->bbox[0][2] = bbox.a[2];
			vb->bbox[1][0] = bbox.b[0]; vb->bbox[1][1] = bbox.b[1]; vb->bbox[1][2] = bbox.b[2];
			vb->nbData = nbData;
			vb->vertexSize = vertexSize;
			for (int i=0;i<nbData;i++)
				vb->dataType[i] = dataType[i];
			return vb;
		}

		ChunkIndexBuffer* ChunkRenderWriter::addIndexBuffer(flowvr::ID id, int nbi, int dataType, int primitive)
		{
			ChunkIndexBuffer* ib = addChunk<ChunkIndexBuffer>(nbi*Type::size(dataType));
			ib->id = id;
			ib->resType = RES_INDEXBUFFER;
			ib->level = -1;
			ib->gen = -1;
			ib->dataType = dataType;
			ib->primitive = primitive;
			ib->restart = 0;
			return ib;
		}

		ChunkVertexShader* ChunkRenderWriter::addVertexShader(flowvr::ID id, int language, const std::string& code)
		{
			ChunkVertexShader* vs = addChunk<ChunkVertexShader>(code.length());
			vs->id = id;
			vs->resType = RES_VERTEXSHADER;
			vs->level = -1;
			vs->gen = -1;
			vs->language = language;
			memcpy((void*)vs->data(),code.c_str(),code.length());
			return vs;
		}

		ChunkPixelShader* ChunkRenderWriter::addPixelShader(flowvr::ID id, int language, const std::string& code)
		{
			ChunkPixelShader* ps = addChunk<ChunkPixelShader>(code.length());
			ps->id = id;
			ps->resType = RES_PIXELSHADER;
			ps->level = -1;
			ps->gen = -1;
			ps->language = language;
			memcpy((void*)ps->data(),code.c_str(),code.length());
			return ps;
		}

		ChunkTexture* ChunkRenderWriter::addDefaultTexture(ID id, int nx, int ny)
		{
			ChunkTexture* texture = addTexture(id, ChunkTexture::Grayscale, Type::Byte, nx, ny);
			if (texture == NULL) return NULL;
			unsigned char* data = (unsigned char*) texture->data();
			for (int y=0;y<ny;y++)
				for (int x=0;x<nx;x++, data++)
					data[0] = ((x&0x10)^(y&0x10))?178:25;
			return texture;
		}


		/// Create a 2D noise texture
		ChunkTexture* ChunkRenderWriter::addNoise2DTexture(ID id, int size, float scale)
		{
			Noise2D noise((int)((size-1)/scale+1));

			ChunkTexture* texture = addTexture(id, ChunkTexture::Grayscale, Type::Byte, size, size);
			unsigned char* data = (unsigned char*) texture->data();
			for (int y=0;y<size;y++)
				for (int x=0;x<size;x++, data++)
				{
					float fx = (x / scale) - 0.5f;
					float fy = (y / scale) - 0.5f;
					float n = noise.cubic(fx, fy);
					*data = (unsigned char)(n*255);
				}
			return texture;
		}

		/// Create a 3D noise texture
		ChunkTexture* ChunkRenderWriter::addNoise3DTexture(ID id, int size, float scale)
		{
			Noise3D noise((int)((size-1)/scale+1));

			ChunkTexture* texture = addTexture(id, ChunkTexture::Grayscale, Type::Byte, size, size, size);
			unsigned char* data = (unsigned char*) texture->data();
			for (int z=0;z<size;z++)
				for (int y=0;y<size;y++)
					for (int x=0;x<size;x++, data++)
					{
						float fx = (x / scale) - 0.5f;
						float fy = (y / scale) - 0.5f;
						float fz = (z / scale) - 0.5f;
						float n = noise.cubic(fx, fy, fz);
						*data = (unsigned char)(n*255);
					}
			return texture;
		}

		ChunkVertexBufferUpdate* ChunkRenderWriter::updateVertexBuffer(flowvr::ID id, int p0, int nbp, int nbData, int* dataType, const BBox& bbox)
		{
			int vertexSize = 0;
			for (int i=0;i<nbData;i++)
				vertexSize += Type::size(dataType[i]);
			ChunkVertexBufferUpdate* vb = addChunk<ChunkVertexBufferUpdate>(nbData*sizeof(int)+nbp*vertexSize);
			vb->id = id;
			vb->resType = RES_VERTEXBUFFER;
			vb->level = -1;
			vb->gen = -1;
			vb->bbox[0][0] = bbox.a[0]; vb->bbox[0][1] = bbox.a[1]; vb->bbox[0][2] = bbox.a[2];
			vb->bbox[1][0] = bbox.b[0]; vb->bbox[1][1] = bbox.b[1]; vb->bbox[1][2] = bbox.b[2];
			vb->uoffset = vertexSize * p0;
			vb->nbData = nbData;
			vb->vertexSize = vertexSize;
			for (int i=0;i<nbData;i++)
				vb->dataType[i] = dataType[i];
			return vb;
		}

		ChunkIndexBufferUpdate* ChunkRenderWriter::updateIndexBuffer(flowvr::ID id, int i0, int nbi, int dataType, int primitive)
		{
			ChunkIndexBufferUpdate* ib = addChunk<ChunkIndexBufferUpdate>(nbi*Type::size(dataType));
			ib->id = id;
			ib->resType = RES_INDEXBUFFER;
			ib->level = -1;
			ib->gen = -1;
			ib->uoffset = i0*Type::size(dataType);
			ib->dataType = dataType;
			ib->primitive = primitive;
			ib->restart = 0;
			return ib;
		}

		static int doCompressTextures()
		{
		  const char* var = getenv("FLOWVR_RENDER_COMPRESS_TEXTURES");
		  if (var && *var) return atoi(var);
		  else return 0;
		}

		ChunkTextureUpdate* ChunkRenderWriter::updateTexture(ID id, int imageType, int pixelType, int nx, int ny, int nz, int ux0, int uy0, int uz0, int unx, int uny, int unz)
		{
		  int b = 1;/* << ((imageType&ChunkTexture::MASK_BLOC_LOG2) >> ChunkTexture::SHIFT_BLOC_LOG2);*/
		  int lineSize = ((unx+b-1)/b) * Type::size(pixelType);
		  ChunkTextureUpdate* texture = addChunk<ChunkTextureUpdate>(lineSize*(uny==0?1:(uny+b-1)/b)*(unz==0?1:(imageType&ChunkTexture::ARRAY)?unz:(unz+b-1)/b));
			texture->id = id;
			texture->resType = RES_TEXTURE;
			texture->level = -1;
			texture->gen = -1;
			texture->imageType = imageType;
			texture->nx = nx;
			texture->ny = ny;
			texture->nz = nz;
			texture->ux0 = ux0;
			texture->uy0 = uy0;
			texture->uz0 = uz0;
			texture->unx = unx;
			texture->uny = uny;
			texture->unz = unz;
			texture->pixelType = pixelType;
		  texture->lineSize = lineSize;
		  return texture;
		}


		ChunkTexture* ChunkRenderWriter::loadTexture(ID id, std::string filename)
		{
		#ifndef NODEVIL
			if (!path.findFile(filename))
				return NULL;

		  if (!IsInit)
		  {
			ilInit();
			iluInit();
		  }

		  ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
		  ilEnable(IL_ORIGIN_SET);
		  ilEnable(IL_CONV_PAL);
		#ifdef IL_DXTC_FORMAT
		  ilSetInteger(IL_KEEP_DXTC_DATA, IL_TRUE);
		#endif
		  ILuint imgid; ilGenImages(1,&imgid);
		  ilBindImage(imgid);

		  if (!ilLoadImage((char*)filename.c_str()))
			return NULL;

		  int nx = ilGetInteger(IL_IMAGE_WIDTH);
		  int ny = ilGetInteger(IL_IMAGE_HEIGHT);
		  int nz = ilGetInteger(IL_NUM_IMAGES);

					int format = ilGetInteger(IL_IMAGE_FORMAT);
					int type = ilGetInteger(IL_IMAGE_TYPE);

		  int imgtype;
		  int pixtype;

		  bool compressed = false;


		#if defined(IL_DXTC_FORMAT)
		  // ilImageToDxtcData does not seem to exist in the 1.6 API of DevIL
		  // as an intermediate hack, we check for the version to be bigger
		  // than 1.7
		  #if IL_VERSION >= 178
		  int  dxtc   = ilGetInteger(IL_DXTC_DATA_FORMAT);
		  if (dxtc == IL_DXT_NO_COMP)
		  { // check if we should compress all images
			static int compress = doCompressTextures();
			if (compress)
			{
			  if (format == IL_RGBA || format == IL_BGRA)
				dxtc = IL_DXT5;
			  else
				dxtc = IL_DXT1;
			  int size0 = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
			  ilImageToDxtcData(dxtc);
			  format = ilGetInteger(IL_IMAGE_FORMAT);
			  type = ilGetInteger(IL_IMAGE_TYPE);
			  dxtc = ilGetInteger(IL_DXTC_DATA_FORMAT);
			  int size1 = ilGetDXTCData(NULL, 0, dxtc);
			  std::cout << "Compressed " << nx << "x" << ny << " image from " << size0 << "B to " << size1 << "B" << std::endl;
			}
		  }
		  #endif // if IL_VERSION >= 178


		#endif // if defined(IL_DXTC_FORMAT)

		  if (!compressed)
		  {
			int nc = 0;
			switch(format)
			{
			case IL_LUMINANCE: imgtype = ChunkTexture::Grayscale; nc=1; break;
			case IL_RGB: imgtype = ChunkTexture::RGB; nc=3; break;
			case IL_RGBA: imgtype = ChunkTexture::RGBA; nc=4; break;
			case IL_BGR: imgtype = ChunkTexture::BGR; nc=3; break;
			case IL_BGRA: imgtype = ChunkTexture::BGRA; nc=4; break;
			default:
			  return NULL;
			}

			switch(type)
			{
			case IL_UNSIGNED_BYTE:
			case IL_BYTE:
			  pixtype = Type::vector(Type::Byte, nc);
			  break;
			case IL_UNSIGNED_SHORT:
			case IL_SHORT:
			  pixtype = Type::vector(Type::Short, nc);
			  break;
			case IL_UNSIGNED_INT:
			case IL_INT:
			  pixtype = Type::vector(Type::Int, nc);
			  break;
			case IL_FLOAT:
			  pixtype = Type::vector(Type::Float, nc);
			  break;
			case IL_DOUBLE:
			  pixtype = Type::vector(Type::Double, nc);
			  break;
			default:
			  return NULL;
			}
		  }
		  if (nz<=1)
		  {
			nz = 0;
			if (nx==1 || ny==1)
			{
			  if (nx == 1) nx = ny;
			  ny = 0;
			}
		  }

		  ChunkTexture* texture = addTexture(id, imgtype, pixtype, nx, ny, nz);

		#ifdef IL_DXTC_FORMAT
		  // ilGetDXTCData does not seem to exist in the 1.6 API of DevIL
		  // as an intermediate hack, we check for the version to be bigger
		  // than 1.7
		  #if IL_VERSION >= 178
		  if (compressed)
			//memcpy(texture->data(),ilGetDxtcData(),texture->dataSize());
			ilGetDXTCData(texture->data(), texture->dataSize(), dxtc);
		  else
		  #endif // IF
		#endif
			memcpy(texture->data(),ilGetData(),texture->dataSize());

		  ilDeleteImages(1,&imgid);

		  return texture;
		#else
		  return NULL;
		#endif // NODEVIL
		}

		/// Load a bump map from a file and convert it to a normal map
		ChunkTexture* ChunkRenderWriter::loadTextureNormalMap(ID id, std::string filename)
		{
		#ifndef NODEVIL
			if (!path.findFile(filename))
				return NULL;

			if (!IsInit)
			{
				ilInit();
				iluInit();
			}

			ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
			ilEnable(IL_ORIGIN_SET);
			ilEnable(IL_CONV_PAL);
			ILuint imgid; ilGenImages(1,&imgid);
			ilBindImage(imgid);

			if (!ilLoadImage((char*)filename.c_str()))
				return NULL;

			int nx = ilGetInteger(IL_IMAGE_WIDTH);
			int ny = ilGetInteger(IL_IMAGE_HEIGHT);
			int nz = 0; //ilGetInteger(IL_NUM_IMAGES);

			int format = ilGetInteger(IL_IMAGE_FORMAT);
			int type = ilGetInteger(IL_IMAGE_TYPE);

			int nc = 0;
			switch(format)
			{
				case IL_LUMINANCE: nc=1; break;
				case IL_RGB: nc=3; break;
				case IL_RGBA: nc=4; break;
				case IL_BGR: nc=3; break;
				case IL_BGRA: nc=4; break;
				default:
						  return NULL;
			}

			std::vector<float> height(nx*ny);

			switch(type)
			{
				case IL_UNSIGNED_BYTE:
				case IL_BYTE:
					{
						const unsigned char* input = (const unsigned char*)ilGetData();
						for (int i=0; i<nx*ny;i++)
							height[i] = input[i*nc]/255.0f;
					}
					break;
				case IL_UNSIGNED_SHORT:
					{
						const unsigned short* input = (const unsigned short*)ilGetData();
						for (int i=0; i<nx*ny;i++)
							height[i] = input[i*nc]/65535.0f;
					}
					break;
				case IL_SHORT:
					{
						const short* input = (const short*)ilGetData();
						for (int i=0; i<nx*ny;i++)
							height[i] = input[i*nc]/32767.0f;
					}
					break;
				case IL_UNSIGNED_INT:
					{
						const unsigned int* input = (const unsigned int*)ilGetData();
						for (int i=0; i<nx*ny;i++)
							height[i] = input[i*nc]/(4294967295.0f);
					}
					break;
				case IL_INT:
					{
						const int* input = (const int*)ilGetData();
						for (int i=0; i<nx*ny;i++)
							height[i] = input[i*nc]/(2147483647.0f);
					}
					break;
				case IL_FLOAT:
					{
						const float* input = (const float*)ilGetData();
						for (int i=0; i<nx*ny;i++)
							height[i] = input[i*nc];
					}
					break;
				case IL_DOUBLE:
					{
						const double* input = (const double*)ilGetData();
						for (int i=0; i<nx*ny;i++)
							height[i] = (float)input[i*nc];
					}
					break;
				default:
					return NULL;
			}

			if (nz<=1)
			{
				nz = 0;
				if (nx==1 || ny==1)
				{
					if (nx == 1) nx = ny;
					ny = 0;
				}
			}

			ilDeleteImages(1,&imgid);

			int imgtype = imgtype = ChunkTexture::BGRA;
			int pixtype = Type::vector(Type::Byte,4);

			ChunkTexture* texture = addTexture(id, imgtype, pixtype, nx, ny, nz);

			Vec4b* dest = (Vec4b*)texture->data();
			int i = 0;
			for (int y=0;y<ny;y++)
			{
				int dy_1 = ((y==0)?ny-1:-1)*nx;
				int dy1 = ((y==ny-1)?-(ny-1):1)*nx;
				for (int x=0;x<nx;x++,i++)
				{
					int dx_1 = ((x==0)?nx-1:-1);
					int dx1 = ((x==nx-1)?-(nx-1):1);

					// prewitt 3x3 filter
					float du = (height[i+dx1 +dy_1]+height[i+dx1 ]+height[i+dx1 +dy1])
						-(height[i+dx_1+dy_1]+height[i+dx_1]+height[i+dx_1+dy1]);
					float dv = (height[i+dy1 +dx_1]+height[i+dy1 ]+height[i+dy1 +dx1])
						-(height[i+dy_1+dx_1]+height[i+dy_1]+height[i+dy_1+dx1]);
					Vec3f n(-du,-dv,1.0f);
					n.normalize();

					dest[i] = Vec4b((unsigned char)((n[2]+1.0f)*127.5f),
							(unsigned char)((n[1]+1.0f)*127.5f),
							(unsigned char)((n[0]+1.0f)*127.5f),
							(unsigned char)(height[i]*255.0f));
				}
			}

			return texture;
		#else
			return NULL;
		#endif // NODEVIL
		}

		ChunkTexture* ChunkRenderWriter::loadTextureCubemap(ID id, std::string filename)
		{
		#ifndef NODEVIL
			if (!path.findFile(filename))
				return NULL;

			if (!IsInit)
			{
				ilInit();
				iluInit();
			}

			ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
			ilEnable(IL_ORIGIN_SET);
			ilEnable(IL_CONV_PAL);
			ILuint imgid[6]; ilGenImages(6,imgid);

			std::string fname[6];

			static const char* facenames[5][6]={
				{"2","4","5","6","1","3"},
				{"px","nx","py","ny","pz","nz"},
				{"PX","NX","PY","NY","PZ","NZ"},
				{"left","right","top","bottom","front","back"},
				{"LEFT","RIGHT","TOP","BOTTOM","FRONT","BACK"}};

			std::string prefix, suffix;

			size_t pext = filename.rfind('.');
			if (pext == std::string::npos) pext = filename.size();
			bool found = false;
			int faceset = 0;
			for (int s=0;s<5 && !found;s++)
			{
				for (int i=0;i<6 && !found;i++)
				{
					int l = strlen(facenames[s][i]);
					if (filename.substr(pext-l,l) == facenames[s][i])
					{
						prefix = filename.substr(0,pext-l);
						suffix = filename.substr(pext);
						found = true;
						faceset = s;
					}
				}
			}

			if (!found) return NULL;

			for (int i=0;i<6;i++)
				fname[i] = prefix + facenames[faceset][i] + suffix;

			ilBindImage(imgid[0]);

			if (!ilLoadImage((char*)fname[0].c_str()))
				return NULL;

			int nx = ilGetInteger(IL_IMAGE_WIDTH);
			int ny = ilGetInteger(IL_IMAGE_HEIGHT);

			int format = ilGetInteger(IL_IMAGE_FORMAT);
			int type = ilGetInteger(IL_IMAGE_TYPE);

			int nz = 1;
			for (nz=1;nz<6;nz++)
			{
				ilBindImage(imgid[nz]);
				if (!ilLoadImage((char*)fname[nz].c_str()))
					break;

				if (ilGetInteger(IL_IMAGE_WIDTH) != nx) break;
				if (ilGetInteger(IL_IMAGE_HEIGHT) != ny) break;
				if (ilGetInteger(IL_IMAGE_FORMAT) != format) break;
				if (ilGetInteger(IL_IMAGE_TYPE) != type) break;
			}

			int imgtype;
			int pixtype;

			int nc = 0;
			switch(format)
			{
				case IL_LUMINANCE: imgtype = ChunkTexture::Grayscale; nc=1; break;
				case IL_RGB: imgtype = ChunkTexture::RGB; nc=3; break;
				case IL_RGBA: imgtype = ChunkTexture::RGBA; nc=4; break;
				case IL_BGR: imgtype = ChunkTexture::BGR; nc=3; break;
				case IL_BGRA: imgtype = ChunkTexture::BGRA; nc=4; break;
				default:
						  return NULL;
			}

			switch(type)
			{
				case IL_UNSIGNED_BYTE:
				case IL_BYTE:
					pixtype = Type::vector(Type::Byte, nc);
					break;
				case IL_UNSIGNED_SHORT:
				case IL_SHORT:
					pixtype = Type::vector(Type::Short, nc);
					break;
				case IL_UNSIGNED_INT:
				case IL_INT:
					pixtype = Type::vector(Type::Int, nc);
					break;
				case IL_FLOAT:
					pixtype = Type::vector(Type::Float, nc);
					break;
				case IL_DOUBLE:
					pixtype = Type::vector(Type::Double, nc);
					break;
				default:
					return NULL;
			}

			ChunkTexture* texture = addTexture(id, imgtype | ChunkTexture::CUBEMAP, pixtype, nx, ny, nz);

			int imgsize = ny * texture->lineSize;

			for (int i=0;i<nz;i++)
			{
				ilBindImage(imgid[i]);
				memcpy(((char*)texture->data())+imgsize*i,ilGetData(),imgsize);
			}

			ilDeleteImages(6,imgid);

			return texture;
		#else
			return NULL;
		#endif // NODEVIL
		}


		static std::string getParentDir(const std::string& filename)
		{
			std::string::size_type pos = filename.find_last_of("/\\");
			if (pos == std::string::npos)
				return ""; // no directory
			else
				return filename.substr(0,pos);
		}

		static std::string getRelativeFile(std::string filename, const std::string& basefile)
		{
			std::cout << filename << std::endl;
			if (filename.empty() || filename[0] == '/') return filename;
			std::string base = getParentDir(basefile);
			// remove any ".."
			while ((filename.substr(0,3)=="../" || filename.substr(0,3)=="..\\") && !base.empty())
			{
				filename = filename.substr(3);
				base = getParentDir(base);
			}
			if (base.empty())
				return filename;
			else if (base[base.length()-1] == '/')
				return base + filename;
			else
				return base + "/" + filename;
		}

		/// Read a shader from a file, with support for '#include' directives
		/// Append the result to the given std::string
		bool ChunkRenderWriter::readShader(std::string& result, const std::string& filename)
		{
			FILE* f = fopen( filename.c_str() , "rt" );
			if (!f) return false;
			while (!feof(f))
			{
				std::string line;
				char buf[1024];
				while (fgets(buf,sizeof(buf), f))
				{
					line += buf;
					if (strlen(buf) < sizeof(buf)-1
							|| buf[sizeof(buf)-1] == '\n'
							|| buf[sizeof(buf)-1] == '\r') break;
				}
				const char* c = line.c_str();
				if (*c == '#')
				{
					++c;
					while (*c == ' ' || *c == '\t')
						++c;
					if (!strncmp(c,"include",7))
					{
						c += 7;
						while (*c == ' ' || *c == '\t') ++c;
						if (*c=='"' || *c=='<') ++c;
						const char* file = c;
						while (*c!='"' && *c!='>') ++c;
						std::string includefile ( file, c);
						std::cout << "Including file "<<includefile<<std::endl;
						// replace the line with the included file
						if (!readShader(result, getRelativeFile(includefile, filename)))
						{
							std::cerr << "ERROR: file "<<includefile<<" included from "<<filename<<" not found.\n";
						}
						continue;
					}
				}
				result += line;
			}
			fclose(f);
			return true;
		}


		ChunkVertexShader* ChunkRenderWriter::loadVertexShader(ID id, std::string filename, const char* predefines)
		{
			if (!path.findFile(filename))
			{
                                std::cerr << "ChunkRenderWriter::loadVertexShader: could not find " << filename << " in FLOWVR_DATA_PATH\n"; 
                                return NULL;
			}
			int language = SHADER_ALL;
			size_t pext = filename.rfind('.');
			if (pext != std::string::npos)
			{
				std::string ext = filename.substr(pext+1);
				std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
				if (ext == "cg") language = SHADER_CG;
				else if (ext == "glsl") language = SHADER_GLSL;
				else if (ext == "hlsl") language = SHADER_HLSL;
			}
			std::string source;
			if (predefines)
			{
				source += predefines;
				source += "\n";
			}
			source += "//file://"+filename+"\n";
			if (!readShader(source, filename)) {
                                std::cerr << "ChunkRenderWriter::loadVertexShader: could not readShader " << filename << "\n"; 
                                return NULL;
                        }
			//std::cout << source << std::endl;
			return addVertexShader(id, language, source);
		}

		ChunkPixelShader* ChunkRenderWriter::loadPixelShader(ID id, std::string filename, const char* predefines)
		{
                        if (!path.findFile(filename)) {
                                std::cerr << "ChunkRenderWriter::loadPixelShader: could not find " << filename << " in FLOWVR_DATA_PATH\n"; 
                                return NULL;                          
                        }


			int language = SHADER_ALL;
			size_t pext = filename.rfind('.');
			if (pext != std::string::npos)
			{
				std::string ext = filename.substr(pext+1);
				std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
				if (ext == "cg") language = SHADER_CG;
				else if (ext == "glsl") language = SHADER_GLSL;
				else if (ext == "hlsl") language = SHADER_HLSL;
			}
			FILE* f = fopen( filename.c_str() , "rt" );
			if (!f) return NULL;
			std::string source;
			if (predefines)
			{
				source += predefines;
				source += "\n";
			}
			source += "//file://"+filename+"\n";
			if (!readShader(source, filename)) {
                                std::cerr << "ChunkRenderWriter::loadPixelShader: could not readShader " << filename << "\n"; 
                          
				return NULL;
                        }

			return addPixelShader(id, language, source);
		}

		void ChunkRenderWriter::addMeshSphere(ID idVB, ID idIB, int nx, int ny)
		{
			int nbp = (nx+1)*(ny+1);

			struct Vertex
			{
				Vec3f pos;
				Vec3f normal;
				Vec2f tex0;
			};

			int dataType[3] = { Type::Vec3f, Type::Vec3f, Type::Vec2f };
			ChunkVertexBuffer* vb = addVertexBuffer(idVB, nbp, 3, dataType);
			Vertex* v = (Vertex*) vb->data();
			for (int y = 0; y<=ny; y++)
				for (int x = 0; x<=nx; x++)
				{
					v->pos = Vec3f(sin(y*PI/ny)*cos(x*2*PI/nx),sin(y*PI/ny)*sin(x*2*PI/nx),cos(y*PI/ny));
					v->normal = v->pos;
					v->tex0 = Vec2f(float(x)/nx, float(y)/ny);
					++v;
				}

			int nbf = nx + 2*nx*(ny-2) + nx;

			if (nbp <= 256)
			{
				ChunkIndexBuffer* ib = addIndexBuffer(idIB, nbf*3, Type::Byte, ChunkIndexBuffer::Triangle);
				unsigned char* ind = (unsigned char*) ib->data();

				for (int x = 0; x<nx; x++)
				{
					ind[0] = x; ind[1] = nx+1+x; ind[2]= nx+1+x+1;
					ind+=3;
				}

				for (int y = 1; y<ny-1; y++)
				{
					int l0 = y*(nx+1);
					int l1 = (y+1)*(nx+1);
					for (int x = 0; x<nx; x++)
					{
						ind[0] = l0+x; ind[1] = l1+x;   ind[2] = l0+x+1;
						ind+=3;
						ind[0] = l1+x; ind[1] = l1+x+1; ind[2] = l0+x+1;
						ind+=3;
					}
				}
				{
					int l0 = (ny-1)*(nx+1);
					int l1 = (ny  )*(nx+1);
					for (int x = 0; x<nx; x++)
					{
						ind[0] = l0+x; ind[1] = l1+x; ind[2]= l0+x+1;
						ind+=3;
					}
				}
			}
			else
			{
				ChunkIndexBuffer* ib = addIndexBuffer(idIB, nbf*3, Type::Short, ChunkIndexBuffer::Triangle);
				unsigned short* ind = (unsigned short*) ib->data();

				for (int x = 0; x<nx; x++)
				{
					ind[0] = x; ind[1] = nx+1+x; ind[2]= nx+1+x+1;
					ind+=3;
				}

				for (int y = 1; y<ny-1; y++)
				{
					int l0 = y*(nx+1);
					int l1 = (y+1)*(nx+1);
					for (int x = 0; x<nx; x++)
					{
						ind[0] = l0+x; ind[1] = l1+x;   ind[2] = l0+x+1;
						ind+=3;
						ind[0] = l1+x; ind[1] = l1+x+1; ind[2] = l0+x+1;
						ind+=3;
					}
				}
				{
					int l0 = (ny-1)*(nx+1);
					int l1 = (ny  )*(nx+1);
					for (int x = 0; x<nx; x++)
					{
						ind[0] = l0+x; ind[1] = l1+x; ind[2]= l0+x+1;
						ind+=3;
					}
				}
			}
		}

		void ChunkRenderWriter::addMeshCube(ID idVB, ID idIB)
		{
			int nbp = 2*3*4;

			struct Vertex
			{
				Vec3f pos;
				Vec3f normal;
				Vec2f tex0;
			};

			int dataType[3] = { Type::Vec3f, Type::Vec3f, Type::Vec2f };
			ChunkVertexBuffer* vb = addVertexBuffer(idVB, nbp, 3, dataType);
			Vertex* v = (Vertex*) vb->data();
			for (int d=0;d<3;d++)
				for (int i=0;i<8;i++)
				{
					float side = (i&4?-1:1);
					v[8*d+i].pos[d      ] = (i&1?-1:1)*side;
					v[8*d+i].pos[(d+1)%3] = (i&2?-1:1)*side;
					v[8*d+i].pos[(d+2)%3] = side;
					v[8*d+i].normal[(d+2)%3] = side;
					v[8*d+i].tex0[0] = i&1?1:0;
					v[8*d+i].tex0[1] = i&2?1:0;
				}

			int nbf = 6;
			ChunkIndexBuffer* ib = addIndexBuffer(idIB, nbf*4, Type::Byte, ChunkIndexBuffer::Quad);
			unsigned char* ind = (unsigned char*) ib->data();

			for (int x = 0; x<nbf; x++)
			{
				ind[4*x+0] = 4*x+0;
				ind[4*x+1] = 4*x+1;
				ind[4*x+2] = 4*x+3;
				ind[4*x+3] = 4*x+2;
			}
		}

		void ChunkRenderWriter::addMeshCylinder(ID idVB, ID idIB, int nx)
		{
			int nbp = (nx+1)*6;

			struct Vertex
			{
				Vec3f pos;
				Vec3f normal;
				Vec2f tex0;
			};

			int dataType[3] = { Type::Vec3f, Type::Vec3f, Type::Vec2f };
			ChunkVertexBuffer* vb = addVertexBuffer(idVB, nbp, 3, dataType);
			Vertex* v = (Vertex*) vb->data();
			for (int y = 0; y<6; y++)
				for (int x = 0; x<=nx; x++)
				{
					static float side[6]={1,1,1,-1,-1,-1};
					static float radius[6]={0,1,1,1,1,0};
					static float texv[6]={0,0.25,0.25,0.75,0.75,1.0};
					static float normy[6]={1,1,0,0,-1,-1};
					float normh = sqrt(1-normy[y]*normy[y]);
					v->pos = Vec3f(radius[y]*cos(x*2*PI/nx),radius[y]*sin(x*2*PI/nx),side[y]);
					v->tex0 = Vec2f(1-float(x)/nx, texv[y]);
					v->normal = Vec3f(normh*cos(x*2*PI/nx),normy[y],normh*sin(x*2*PI/nx));
					++v;
				}

			int nbf = nx + 2*nx + nx;
			ChunkIndexBuffer* ib = addIndexBuffer(idIB, nbf*3, Type::Byte, ChunkIndexBuffer::Triangle);
			unsigned char* ind = (unsigned char*) ib->data();

			for (int x = 0; x<nx; x++)
			{
				ind[0] = x; ind[1] = nx+1+x; ind[2]= nx+1+x+1;
				ind+=3;
			}

			{
				int l0 = 2*(nx+1);
				int l1 = 3*(nx+1);
				for (int x = 0; x<nx; x++)
				{
					ind[0] = l0+x; ind[1] = l1+x;   ind[2] = l0+x+1;
					ind+=3;
					ind[0] = l1+x; ind[1] = l1+x+1; ind[2] = l0+x+1;
					ind+=3;
				}
			}
			{
				int l0 = 4*(nx+1);
				int l1 = 5*(nx+1);
				for (int x = 0; x<nx; x++)
				{
					ind[0] = l0+x; ind[1] = l1+x; ind[2]= l0+x+1;
					ind+=3;
				}
			}
		}

		void ChunkRenderWriter::rotatePrimitive(ID primID, float angle, const Vec3f& axis)
		{
			Quat q;
			q.fromDegreeAngAxis(angle, axis);
			addParam(primID, ChunkPrimParam::TRANSFORM_ROTATION, "", q);
		}


		std::string ChunkRenderWriter::generateName(const std::string prefix)
		{
			return prefix;
		}

		bool ChunkRenderWriter::saveTexture(const ChunkTexture* texture, std::string filename)
		{
#ifndef NODEVIL
			if (!IsInit)
			{
				ilInit();
				iluInit();
			}

			int format = IL_RGB;
			int type = IL_UNSIGNED_BYTE;

			int nc = Type::nx(texture->pixelType);
			switch(texture->imageType&ChunkTexture::MASK_TYPE)
			{
				case ChunkTexture::Grayscale: format = IL_LUMINANCE; break;
				case ChunkTexture::RGB: if (nc<=3) format = IL_RGB; else format = IL_RGBA; break;
				case ChunkTexture::BGR: if (nc<=3) format = IL_BGR; else format = IL_BGRA; break;
				default:
								std::cerr << "SaveTexture: unknown image type 0x"<<std::hex<<texture->imageType<<std::dec<<std::endl;
								return false;
			}

			switch(Type::toSingle(texture->pixelType))
			{
				case Type::Byte: type = IL_UNSIGNED_BYTE; break;
				case Type::Short: type = IL_UNSIGNED_SHORT; break;
				case Type::Int: type = IL_UNSIGNED_INT; break;
				case Type::Float: type = IL_FLOAT; break;
				case Type::Double: type = IL_DOUBLE; break;
				default:
						   std::cerr << "SaveTexture: unknown pixel component type "<<Type::name(Type::toSingle(texture->pixelType))<<std::endl;
						   return false;
			}

			ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
			ilEnable(IL_ORIGIN_SET);
			ilEnable(IL_CONV_PAL);
			ILuint imgid; ilGenImages(1,&imgid);
			ilBindImage(imgid);

			if (!ilTexImage(texture->nx, texture->ny?texture->ny:1, texture->nz?texture->nz:1,
						nc, format, type, (ILvoid*)texture->data()))
			{
				std::cout << "SaveImage: FAILED ilTexImage("<<texture->nx<<", "<<(texture->ny?texture->ny:1)<<", "<<(texture->nz?texture->nz:1)<<", "
					<< nc<<", 0x"<<std::hex<<format<<std::dec<<", 0x"<<std::hex<<type<<std::dec<<", "<<(ILvoid*)texture->data()<<")"<<std::endl;
				ILenum err;
				while ((err=ilGetError())!=IL_NO_ERROR)
					std::cout << "IL_ERROR "<<err<<": "<<iluErrorString(err)<<std::endl;
			}

			unlink(filename.c_str());
			bool ok = ilSaveImage((char*)filename.c_str());
			if (!ok)
			{
				std::cout << "SaveImage: FAILED ilSaveImage("<<filename<<") from ilTexImage("<<texture->nx<<", "<<(texture->ny?texture->ny:1)<<", "<<(texture->nz?texture->nz:1)<<", "
					<< nc<<", 0x"<<std::hex<<format<<std::dec<<", 0x"<<std::hex<<type<<std::dec<<", "<<(ILvoid*)texture->data()<<")"<<std::endl;
				ILenum err;
				while ((err=ilGetError())!=IL_NO_ERROR)
					std::cout << "IL_ERROR "<<err<<": "<<iluErrorString(err)<<std::endl;
			}

			ilDeleteImages(1,&imgid);

			return ok;
#else
			return false;
#endif // NODEVIL
		}

	} // namespace render

} // namespace flowvr
