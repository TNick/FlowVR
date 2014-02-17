/******* COPYRIGHT ************************************************
 *                                                                 *
 *                         vtk-flowvr                              *
 *          Distributed rendering for VTK using FlowVR Render      *
 *                                                                 *
 *-----------------------------------------------------------      *
 * COPYRIGHT (C) 2003-2011                by                       *
 * INRIA                                                           *
 * ALL RIGHTS RESERVED.                                            *
 *                                                                 *
 * This source is covered by the GNU GPL, please refer to the      *
 * COPYING file for further information.                           *
 *                                                                 *
 *----------------------------------------------------------       *
 *                                                                 *
 *  Original Contributors:                                         *
 *    Jeremie Allard,                                              *
 *    Thomas Arcila.                                               *
 *                                                                 *
 *******************************************************************
 *                                                                 *
 * File: .//FlowVRRender/vtkFlowVRPolyDataMapper.cxx
 *                                                                 *
 * Contacts:
 *                                                                 *
 ******************************************************************/
#include "vtkFlowVRPolyDataMapper.h"
#include "vtkObjectFactory.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkCommand.h"
#include "vtkDataArray.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLRenderer.h"
#include "vtkPlane.h"
#include "vtkPlaneCollection.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolygon.h"
#include "vtkProperty.h"
#include "vtkTimerLog.h"
#include "vtkTriangle.h"

#include "vtkFlowVRRenderer.h"
#include "vtkFlowVRRenderWindow.h"

#include <ftl/vec.h>
#include <flowvr/module.h>
#include <flowvr/render/chunkrenderwriter.h>

#include <sstream>
#include <fstream>

//----------------------------------------------------------------------------

class vtkFlowVRRenderPrimitiveData
{
public:
	vtkFlowVRRenderPrimitiveData(flowvr::ID vShaderId, flowvr::ID pShaderId )
	: PointData(vShaderId, pShaderId)
	, CellData(vShaderId, pShaderId)
	{}

	class ResInfo
	{
	public:
		vtkTimeStamp UpdateTime;
		flowvr::ID Index;
		bool Active;
		ResInfo() : Index(0), Active(false)
		{}
		void Release(flowvr::render::ChunkWriter* scene, flowvr::render::ResourceType restype)
		{
			if (scene!=NULL && Index!=0)
			{
				scene->delResource(Index, restype);
				Index = 0;
			}
		}
		void SetState(bool active, flowvr::render::ChunkWriter* scene, vtkFlowVRRenderer* fren)
		{
			if (active)
			{
				if (!Index)
				{
					Index = fren->GenerateID();
				}
			}
			Active = active;
		}

	};

	class PrimInfo
	{
	public:
		flowvr::ID Index;
		bool Active;
		flowvr::ID VBPositions;
		flowvr::ID VBColors;
		flowvr::ID VBNormals;
		flowvr::ID VBTCoords;
		flowvr::ID IB;
		vtkTimeStamp PropTime;
		vtkTimeStamp TransformTime;

		flowvr::ID m_vShaderId,
		    m_pShaderId;

		PrimInfo(flowvr::ID vShaderId, flowvr::ID pShaderId)
		: Index(0), Active(true), VBPositions(0), VBColors(0), VBNormals(0), VBTCoords(0), IB(0)
		, m_vShaderId(vShaderId)
		, m_pShaderId(pShaderId)
		{}


		void Release(flowvr::render::ChunkWriter* scene)
		{
			if (scene!=NULL && Index!=0)
			{
				scene->delPrimitive(Index);
				Index = 0;
			}
		}


		void SetState(bool active, flowvr::render::ChunkWriter* scene, vtkFlowVRRenderer* fren)
		{
			if (active)
			{
				if (!Index)
				{
					Index = fren->GenerateID();
					scene->addPrimitive(Index, scene->generateName("VTK"));
					// Set shaders parameters

					if( m_vShaderId >= 0 )
						scene->addParamID(Index, flowvr::render::ChunkPrimParam::VSHADER,"",m_vShaderId);

					if( m_pShaderId >= 0 )
						scene->addParamID(Index, flowvr::render::ChunkPrimParam::PSHADER,"",m_pShaderId);


					scene->addParamEnum(Index, flowvr::render::ChunkPrimParam::PARAMVSHADER, "ModelViewProj", flowvr::render::ChunkPrimParam::ModelViewProjection);
					scene->addParamEnum(Index, flowvr::render::ChunkPrimParam::PARAMVSHADER, "ModelView", flowvr::render::ChunkPrimParam::ModelView);
					scene->addParamEnum(Index, flowvr::render::ChunkPrimParam::PARAMVSHADER, "ModelViewIT", flowvr::render::ChunkPrimParam::ModelView_InvTrans);
					flowvr::render::Vec3f light(0,0,1); light.normalize();
					// scene->addParam(Index, flowvr::render::ChunkPrimParam::PARAMVSHADER, "color0", flowvr::render::Vec4f(1,1,1,1));
					scene->addParam(Index, flowvr::render::ChunkPrimParam::PARAMPSHADER, "lightdir", light);
				}
				if (!Active)
				{
					scene->addParam(Index, flowvr::render::ChunkPrimParam::VISIBLE, "", true);
					Active = true;
				}
			}
			else
			{
				if (Index && Active)
				{
					scene->addParam(Index, flowvr::render::ChunkPrimParam::VISIBLE, "", false);
					Active = false;
				}
			}
		}

		void SetPosition(flowvr::render::ChunkWriter* scene, flowvr::ID buffer, int numdata=0)
		{
			if (buffer!=VBPositions)
			{
				if (buffer)
				{
					scene->addParamID(Index, flowvr::render::ChunkPrimParam::VBUFFER_ID,"position", buffer);
					if (numdata>0)
						scene->addParam(Index, flowvr::render::ChunkPrimParam::VBUFFER_NUMDATA,"position", numdata);
				}
				else
				{
					scene->delParam(Index, flowvr::render::ChunkPrimParam::VBUFFER_ID,"position");
				}
				VBPositions = buffer;
			}
		}

		void SetColor(flowvr::render::ChunkWriter* scene, flowvr::ID buffer, int numdata=0)
		{
			if (buffer!=VBColors)
			{
				if (buffer)
				{
					scene->addParamID(Index, flowvr::render::ChunkPrimParam::VBUFFER_ID,"color0", buffer);
					if (numdata>0)
						scene->addParam(Index, flowvr::render::ChunkPrimParam::VBUFFER_NUMDATA,"color0", numdata);
				}
				else
				{
					scene->delParam(Index, flowvr::render::ChunkPrimParam::VBUFFER_ID,"color0");
				}
				VBColors = buffer;
			}
		}

		void SetNormal(flowvr::render::ChunkWriter* scene, flowvr::ID buffer, int numdata=0)
		{
			if (buffer!=VBNormals)
			{
				if (buffer)
				{
					scene->addParamID(Index, flowvr::render::ChunkPrimParam::VBUFFER_ID,"normal", buffer);
					if (numdata>0)
					{
						scene->addParam(Index, flowvr::render::ChunkPrimParam::VBUFFER_NUMDATA,"normal", numdata);
					}
				}
				else
				{
					scene->delParam(Index, flowvr::render::ChunkPrimParam::VBUFFER_ID,"normal");
				}
				VBNormals = buffer;
			}
		}

		void SetTCoord(flowvr::render::ChunkWriter* scene, flowvr::ID buffer, int numdata=0)
		{
			if (buffer!=VBTCoords)
			{
				if (buffer)
				{
					scene->addParamID(Index, flowvr::render::ChunkPrimParam::VBUFFER_ID,"texcoord", buffer);
					if (numdata>0)
						scene->addParam(Index, flowvr::render::ChunkPrimParam::VBUFFER_NUMDATA,"texcoord", numdata);
				}
				else
				{
					scene->delParam(Index, flowvr::render::ChunkPrimParam::VBUFFER_ID,"texcoord");
				}
				VBTCoords = buffer;
			}
		}

		void SetIndex(flowvr::render::ChunkWriter* scene, flowvr::ID buffer, int first=0, int nbi=-1)
		{
			if (buffer!=IB)
			{
				if (buffer)
				{
					scene->addParamID(Index, flowvr::render::ChunkPrimParam::IBUFFER_ID,"", buffer);
				}
				else
				{
					scene->delParam(Index, flowvr::render::ChunkPrimParam::IBUFFER_ID,"");
				}
				IB = buffer;
			}
			if (buffer)
			{
				if (first>0)
					scene->addParam(Index, flowvr::render::ChunkPrimParam::IBUFFER_I0,"", first);
				if (nbi!=-1)
					scene->addParam(Index, flowvr::render::ChunkPrimParam::IBUFFER_NBI,"", nbi);
			}
		}

		void SetProperties(vtkProperty* prop, flowvr::render::ChunkWriter* scene)
		{
			if (prop!=NULL && Index && prop->GetMTime() > PropTime)
			{
				double color[3];
				prop->GetColor(color);
				flowvr::render::Vec4f c;
				c[0] = color[0];
				c[1] = color[1];
				c[2] = color[2];
				c[3] = prop->GetOpacity();
				scene->addParam(Index, flowvr::render::ChunkPrimParam::PARAMVSHADER,"color0",c);
				if (c[3]<1.0f)
				{
					std::cout << "SetProperties() -- alpha < 1.0" << std::endl;
					scene->addParam(Index, flowvr::render::ChunkPrimParam::PARAMOPENGL,"DepthWrite",false);
					scene->addParam(Index, flowvr::render::ChunkPrimParam::PARAMOPENGL,"Blend",true);
					scene->addParam(Index, flowvr::render::ChunkPrimParam::ORDER,"",1);
				}
				else
				{
					scene->addParam(Index, flowvr::render::ChunkPrimParam::PARAMOPENGL,"DepthWrite",true);
					scene->addParam(Index, flowvr::render::ChunkPrimParam::PARAMOPENGL,"Blend",false);
					scene->addParam(Index, flowvr::render::ChunkPrimParam::ORDER,"",0);
					std::cout << "SetProperties() -- alpha == 1.0" << std::endl;
				}

				if(prop->GetBackfaceCulling())
				{
					scene->addParam(Index, flowvr::render::ChunkPrimParam::PARAMOPENGL, "CullFace", 1 );
				}
				else if(prop->GetFrontfaceCulling())
				{
					scene->addParam(Index, flowvr::render::ChunkPrimParam::PARAMOPENGL, "CullFace", -1);
				}
				else
					scene->addParam(Index, flowvr::render::ChunkPrimParam::PARAMOPENGL, "CullFace",  0);

				PropTime.Modified();
			}
		}

		void SetTransform(vtkActor* act, flowvr::render::ChunkWriter* scene)
		{
			if (act!=NULL && Index && act->GetMTime() > TransformTime)
			{
				flowvr::render::Mat4x4d matrix;
				act->GetMatrix((double*)&matrix);
				scene->addParam(Index, flowvr::render::ChunkPrimParam::TRANSFORM,"",matrix);
				TransformTime.Modified();
			}
		}

	};

	// Two cases: separate unmodified points data or recomputed merged data (for objects with cell data)

	struct PointDataType
	{ // Unmodified points data
		PointDataType( flowvr::ID vShaderId, flowvr::ID pShaderId )
		: PPoints(vShaderId, pShaderId)
		, PLines(vShaderId, pShaderId)
		, PTriangles(vShaderId, pShaderId)
		{}

		ResInfo VBColors;
		ResInfo VBNormals;
		ResInfo VBTCoords;
		ResInfo VBPositions;

		ResInfo IBPoints;
		ResInfo IBLines;
		ResInfo IBTriangles;

		PrimInfo PPoints;
		PrimInfo PLines;
		PrimInfo PTriangles;
	}PointData;

	struct CellDataType
	{ // Recomputed points data
		CellDataType( flowvr::ID vShaderId, flowvr::ID pShaderId )
		: PPoints(vShaderId, pShaderId)
		, PLines(vShaderId, pShaderId)
		, PTriangles(vShaderId, pShaderId)
		{

		}

		ResInfo VBAll;

		static ResInfo IBPoints;
		static ResInfo IBLines;
		static ResInfo IBTriangles;

		PrimInfo PPoints;
		PrimInfo PLines;
		PrimInfo PTriangles;
	} CellData;

	void Release(flowvr::render::ChunkWriter* scene)
	{
		PointData.VBColors.Release(scene,flowvr::render::RES_VERTEXBUFFER);
		PointData.VBNormals.Release(scene,flowvr::render::RES_VERTEXBUFFER);
		PointData.VBTCoords.Release(scene,flowvr::render::RES_VERTEXBUFFER);
		PointData.VBPositions.Release(scene,flowvr::render::RES_VERTEXBUFFER);
		PointData.IBPoints.Release(scene,flowvr::render::RES_INDEXBUFFER);
		PointData.IBLines.Release(scene,flowvr::render::RES_INDEXBUFFER);
		PointData.IBTriangles.Release(scene,flowvr::render::RES_INDEXBUFFER);
		PointData.PPoints.Release(scene);
		PointData.PLines.Release(scene);
		PointData.PTriangles.Release(scene);
		CellData.VBAll.Release(scene,flowvr::render::RES_VERTEXBUFFER);
		CellData.PPoints.Release(scene);
		CellData.PLines.Release(scene);
		CellData.PTriangles.Release(scene);
	}

};

vtkFlowVRRenderPrimitiveData::ResInfo
		vtkFlowVRRenderPrimitiveData::CellDataType::IBPoints;
vtkFlowVRRenderPrimitiveData::ResInfo
		vtkFlowVRRenderPrimitiveData::CellDataType::IBLines;
vtkFlowVRRenderPrimitiveData::ResInfo
		vtkFlowVRRenderPrimitiveData::CellDataType::IBTriangles;

//----------------------------------------------------------------------------

vtkCxxRevisionMacro(vtkFlowVRPolyDataMapper, "$Revision: 6250 $");
vtkStandardNewMacro( vtkFlowVRPolyDataMapper);

vtkFlowVRPolyDataMapper::vtkFlowVRPolyDataMapper() :
	Gen(0)
, m_vShaderId(-1)
, m_pShaderId(-1)
, CurrentPiece(-1)
{
}

vtkFlowVRPolyDataMapper::~vtkFlowVRPolyDataMapper()
{
	if (this->LastWindow)
	{
		this->ReleaseGraphicsResources(this->LastWindow);
		this->LastWindow = NULL;
	}
	for (unsigned int i = 0; i < Primitives.size(); i++)
		delete Primitives[i];
	Primitives.clear();
}

void vtkFlowVRPolyDataMapper::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}

// Description:
// This calls RenderPiece (in a for loop if streaming is necessary).
void vtkFlowVRPolyDataMapper::Render(vtkRenderer *ren, vtkActor *act)
{
	this->CurrentPiece = -1;
	this->Superclass::Render(ren, act);
}

static flowvr::render::Type::Type vtkFlowVRType(int type)
{
	switch (type)
	{
	case VTK_BIT:
		return flowvr::render::Type::Bool;
		break;
	case VTK_CHAR:
		return flowvr::render::Type::Byte;
		break;
	case VTK_UNSIGNED_CHAR:
		return flowvr::render::Type::Byte;
		break;
	case VTK_SHORT:
		return flowvr::render::Type::Short;
		break;
	case VTK_UNSIGNED_SHORT:
		return flowvr::render::Type::Short;
		break;
	case VTK_INT:
		return flowvr::render::Type::Int;
		break;
	case VTK_UNSIGNED_INT:
		return flowvr::render::Type::Int;
		break;
	case VTK_LONG:
		return flowvr::render::Type::Long;
		break;
	case VTK_UNSIGNED_LONG:
		return flowvr::render::Type::Long;
		break;
	case VTK_FLOAT:
		return flowvr::render::Type::Float;
		break;
	case VTK_DOUBLE:
		return flowvr::render::Type::Double;
		break;
	default:
		return flowvr::render::Type::Null;
	}
}

static int countCellPts(vtkCellArray* cells, int add = 0, int mult = 1)
{
	int nbc = cells->GetNumberOfCells();
	vtkIdType* data = cells->GetPointer();
	int n = 0;
	for (int c = 0; c < nbc; c++)
	{
		int np = *data;
		n += np;
		data += 1 + np;
	}
	return mult * n + add * nbc;
}

//
// Receives from Actor -> maps data to primitives
//
void vtkFlowVRPolyDataMapper::RenderPiece(vtkRenderer *ren, vtkActor *act)
{
	++this->CurrentPiece;
	if (!ren->IsA("vtkFlowVRRenderer"))
	{
		vtkErrorMacro(<< "Renderer must be a vtkFlowVRRenderer");
		return;
	}

	vtkFlowVRRenderer* fren = static_cast<vtkFlowVRRenderer*> (ren);

	//
	// make sure that we've been properly initialized
	//
	if (ren->GetRenderWindow()->CheckAbortStatus())
	{
		return;
	}

	vtkPolyData *input = this->GetInput();

	if (input == NULL)
	{
		vtkErrorMacro(<< "No input!");
		return;
	}
	else
	{
		this->InvokeEvent(vtkCommand::StartEvent, NULL);
		input->Update();
		this->InvokeEvent(vtkCommand::EndEvent, NULL);
	}

	flowvr::render::ChunkWriter* scene = fren->GetSceneWriter();

	if( m_vShaderId == -1 )
	{
		m_vShaderId = fren->GenerateID();
		// upload the shaders to the remote cgengine, by putting
		// a value on the output port right now.
		if(scene->loadVertexShader ( m_vShaderId, "shaders/polydata_v.cg" ))
			std::cout << "loaded vertex shader from: shaders/polydata_v.cg" << std::endl;
		else
			std::cerr << "failed to vertex pixel shader [shaders/polydata_v.c]." << std::endl;
	}

	if ( m_pShaderId == -1 )
	{
		m_pShaderId = fren->GenerateID();
		// upload the shaders to the remote cgengine, by putting
		// a value on the output port right now.
		if(scene->loadPixelShader ( m_pShaderId, "shaders/polydata_p.cg" ))
			std::cout << "loaded pixel shader from: shaders/polydata_p.cg" << std::endl;
		else
			std::cerr << "failed to load pixel shader [shaders/polydata_p.c]." << std::endl;
	}



	while (CurrentPiece >= Primitives.size())
		Primitives.push_back(new vtkFlowVRRenderPrimitiveData( m_vShaderId, m_pShaderId ) );

	vtkFlowVRRenderPrimitiveData* prims = Primitives[CurrentPiece];

	int rep, interpolation;
	float tran;
	vtkProperty *prop;

	// get the property
	prop = act->GetProperty();

	// get the transparency
	tran = prop->GetOpacity();

	// get the representation (e.g., surface / wireframe / points)
	rep = prop->GetRepresentation();

	// get the shading interpolation
	interpolation = prop->GetInterpolation();

	// if the primitives are invisible then get out of here
	if (tran <= 0.0)
	{
		return;
	}

	vtkIdType numPts = input->GetNumberOfPoints();

	if (numPts == 0)
	{
		vtkDebugMacro(<< "No points!");
		return;
	}

	if (this->LookupTable == NULL)
	{
		this->CreateDefaultLookupTable();
	}

	// make sure our window is current
	ren->GetRenderWindow()->MakeCurrent();

	//
	//TODO: Clip Planes
	//

	int cellScalars = 0;
	if ((this->ScalarMode == VTK_SCALAR_MODE_USE_CELL_DATA
		|| this->ScalarMode == VTK_SCALAR_MODE_USE_CELL_FIELD_DATA
		|| (input->GetPointData()->GetScalars() == NULL
		&& MapScalars(tran) != NULL))
		&& this->ScalarMode != VTK_SCALAR_MODE_USE_POINT_FIELD_DATA)
	{
		cellScalars = 1;
	}

	int cellNormals = (input->GetCellData()->GetNormals() ? 1:0);

	// count the number of points, lines and triangles

	int nb_pts = 0;
	int nb_lns = 0;
	int nb_trs = 0;

	nb_pts = countCellPts(input->GetVerts());

	switch (rep)
	{
	case VTK_POINTS:
		nb_pts += countCellPts(input->GetLines());
		nb_pts += countCellPts(input->GetPolys());
		nb_pts += countCellPts(input->GetStrips());
		break;
	case VTK_WIREFRAME:
		nb_lns += countCellPts(input->GetLines(), -1);
		nb_lns += countCellPts(input->GetPolys());
		nb_lns += countCellPts(input->GetStrips(), -3, 2);
		break;
	default:
		nb_lns += countCellPts(input->GetLines(),  -1);
		nb_trs += countCellPts(input->GetPolys(),  -2);
		nb_trs += countCellPts(input->GetStrips(), -2);
	}

	if (nb_pts == 0 && nb_lns == 0 && nb_trs == 0)
	{
//		std::cout << "(nb_pts == 0 && nb_lns == 0 && nb_trs == 0)" << std::endl;
		prims->PointData.PPoints.SetState(false, scene, fren);
		prims->PointData.PLines.SetState(false, scene, fren);
		prims->PointData.PTriangles.SetState(false, scene, fren);

		prims->CellData.PPoints.SetState(false, scene, fren);
		prims->CellData.PLines.SetState(false, scene, fren);
		prims->CellData.PTriangles.SetState(false, scene, fren);
	}
	else if (!cellScalars && !cellNormals)
	{ // indexed primitives
		prims->PointData.PPoints.SetState(nb_pts > 0, scene, fren);
		prims->PointData.PLines.SetState(nb_lns > 0, scene, fren);
		prims->PointData.PTriangles.SetState(nb_trs > 0, scene, fren);

		prims->CellData.PPoints.SetState(false, scene, fren);
		prims->CellData.PLines.SetState(false, scene, fren);
		prims->CellData.PTriangles.SetState(false, scene, fren);

		// COLORS UPDATE

		if (this->GetMTime() > prims->PointData.VBColors.UpdateTime
				|| input->GetMTime() > prims->PointData.VBColors.UpdateTime
				|| ren->GetRenderWindow() != this->LastWindow)
		{
			vtkUnsignedCharArray* data = this->MapScalars(tran);
			vtkFlowVRRenderPrimitiveData::ResInfo& info = prims->PointData.VBColors;
			if (data != NULL)
			{
//				std::cout << "adding colors [" << info.Index << "] "
//						  << this->GetMTime()
//						  << " ; "
//						  << input->GetMTime()
//						  << " ; "
//						  << prims->PointData.VBColors.UpdateTime
//						  << std::endl;
				info.SetState(true, scene, fren);
				int nc = data->GetNumberOfComponents();
				int np = data->GetNumberOfTuples();

				int dataType[1] = { ftl::Type::Vec4f };
				flowvr::render::ChunkVertexBuffer* vb = scene->addVertexBuffer(info.Index, np, 1, dataType, flowvr::render::BBox());
				vb->gen = Gen;

				ftl::Vec4f *f = (ftl::Vec4f*)vb->data();
				unsigned char *c;
				for( vtkIdType n=0; n < np; ++n )
				{
					c    = (unsigned char*)data->GetPointer(n*nc);
					f[n] = ftl::Vec4f( c[0]/255.0f, c[1]/255.0f, c[2]/255.0f, c[3] / 255.0f );
				}

				info.UpdateTime.Modified();
//
//				std::cout << " ** DONE" << std::endl;
//
//				info.SetState(true, scene, fren);
//				int nc = data->GetNumberOfComponents();
//				int np = data->GetNumberOfTuples();
//
//				std::cout << nc/4 << std::endl;
//
//				int dataType[1] = { flowvr::render::Type::vector(flowvr::render::Type::Int,nc/4) };
//				flowvr::render::ChunkVertexBuffer* vb = scene->addVertexBuffer(info.Index,np,1,dataType, flowvr::render::BBox());
//				vb->gen = Gen;
//				memcpy(vb->data(),data->GetVoidPointer(0),vb->dataSize());
//				info.UpdateTime.Modified();

			}
			else
			{
				info.SetState(false, scene, fren);
			}
		}

		// NORMALS UPDATE
		if (input->GetMTime() > prims->PointData.VBNormals.UpdateTime
				|| ren->GetRenderWindow() != this->LastWindow)
		{

			vtkDataArray* data = input->GetPointData()->GetNormals();
			vtkFlowVRRenderPrimitiveData::ResInfo& info = prims->PointData.VBNormals;
			if (data != NULL)
			{
				info.SetState(true, scene, fren);
				int nc = data->GetNumberOfComponents();
				int np = data->GetNumberOfTuples();

				std::cout << "Normals Update [" << data->GetDataType() << "]" << std::endl
						  << nc << std::endl
						  << np << std::endl;

				flowvr::render::Type::Type t = vtkFlowVRType(data->GetDataType());
				int dataType[1] = { flowvr::render::Type::vector(t, nc) };
				flowvr::render::ChunkVertexBuffer* vb = scene->addVertexBuffer(info.Index, np, 1, dataType, flowvr::render::BBox());
				vb->gen = Gen;
				memcpy(vb->data(), data->GetVoidPointer(0), vb->dataSize());
				info.UpdateTime.Modified();

				std::cout << " ** DONE" << std::endl;
			}
			else
			{
				info.SetState(false, scene, fren);
			}
		}

		// TEXTURE COORDS UPDATE
		if (input->GetMTime() > prims->PointData.VBTCoords.UpdateTime
				|| ren->GetRenderWindow() != this->LastWindow)
		{
			vtkDataArray* data = input->GetPointData()->GetTCoords();
			vtkFlowVRRenderPrimitiveData::ResInfo& info = prims->PointData.VBTCoords;
			if (data != NULL)
			{
				info.SetState(true, scene, fren);
				int nc = data->GetNumberOfComponents();
				int np = data->GetNumberOfTuples();
				flowvr::render::Type::Type t = vtkFlowVRType(data->GetDataType());
				int dataType[1] = { flowvr::render::Type::vector(t, nc) };
				flowvr::render::ChunkVertexBuffer* vb = scene->addVertexBuffer(info.Index, np, 1, dataType, flowvr::render::BBox());
				vb->gen = Gen;
				memcpy(vb->data(), data->GetVoidPointer(0), vb->dataSize());
				info.UpdateTime.Modified();
			}
			else
			{
				info.SetState(false, scene, fren);
			}
		}

		// POSITIONS UPDATE
		if (input->GetMTime() > prims->PointData.VBPositions.UpdateTime
				|| ren->GetRenderWindow() != this->LastWindow)
		{
			vtkPoints* data = input->GetPoints();
			vtkFlowVRRenderPrimitiveData::ResInfo& info = prims->PointData.VBPositions;
			if (data != NULL)
			{
				info.SetState(true, scene, fren);
				int nc = 3;
				int np = data->GetNumberOfPoints();
				flowvr::render::Type::Type t = vtkFlowVRType(data->GetDataType());
				int dataType[1] = { flowvr::render::Type::vector(t, nc) };
				const double* bounds = data->GetBounds();
				flowvr::render::ChunkVertexBuffer* vb = scene->addVertexBuffer(
						info.Index, np, 1, dataType, flowvr::render::BBox(
								flowvr::render::Vec3f(bounds[0], bounds[2],
										bounds[4]), flowvr::render::Vec3f(
										bounds[1], bounds[3], bounds[5])));
				vb->gen = Gen;
				memcpy(vb->data(), data->GetData()->GetVoidPointer(0),
						vb->dataSize());
				info.UpdateTime.Modified();
			}
			else
			{
				info.SetState(false, scene, fren);
			}
		}

		// INDEX BUFFERS
		int* index_pts = NULL;
		int* index_lns = NULL;
		int* index_trs = NULL;

		if (nb_pts > 0)
		{
			vtkFlowVRRenderPrimitiveData::ResInfo& info =
					prims->PointData.IBPoints;
			info.SetState(true, scene, fren);
			if (this->GetMTime() > info.UpdateTime || input->GetMTime()
					> info.UpdateTime || ren->GetRenderWindow()
					!= this->LastWindow)
			{
				flowvr::render::ChunkIndexBuffer* ib = scene->addIndexBuffer(
						info.Index, nb_pts, flowvr::render::Type::Int,
						flowvr::render::ChunkIndexBuffer::Point);
				ib->gen = Gen;
				index_pts = (int*) ib->data();
				info.UpdateTime.Modified();
			}
		}

		if (nb_lns > 0)
		{
			vtkFlowVRRenderPrimitiveData::ResInfo& info =
					prims->PointData.IBLines;
			info.SetState(true, scene, fren);
			if (this->GetMTime() > info.UpdateTime || input->GetMTime()
					> info.UpdateTime || ren->GetRenderWindow()
					!= this->LastWindow)
			{
				flowvr::render::ChunkIndexBuffer* ib = scene->addIndexBuffer(
						info.Index, nb_lns * 2, flowvr::render::Type::Int,
						flowvr::render::ChunkIndexBuffer::Line);
				ib->gen = Gen;
				index_lns = (int*) ib->data();
				info.UpdateTime.Modified();
			}
		}

		if (nb_trs > 0)
		{
			vtkFlowVRRenderPrimitiveData::ResInfo& info =
					prims->PointData.IBTriangles;
			info.SetState(true, scene, fren);
			if (this->GetMTime() > info.UpdateTime || input->GetMTime()
					> info.UpdateTime || ren->GetRenderWindow()
					!= this->LastWindow)
			{
				flowvr::render::ChunkIndexBuffer* ib = scene->addIndexBuffer(
						info.Index, nb_trs * 3, flowvr::render::Type::Int,
						flowvr::render::ChunkIndexBuffer::Triangle);
				ib->gen = Gen;
				index_trs = (int*) ib->data();
				info.UpdateTime.Modified();
			}
		}
		vtkCellArray* cells;
		const vtkIdType* inptr;
		int c, n, p[3];

		// First the points
		cells = input->GetVerts();
		inptr = cells->GetPointer();
		if (index_pts)
		{
			for (c = 0; c < cells->GetNumberOfCells(); c++)
			{
				n = *(inptr++);
				while (n-- > 0)
				{
					p[0] = *(inptr++);
					*(index_pts++) = (p[0]);
				}
			}
		}

		// Lines
		cells = input->GetLines();
		inptr = cells->GetPointer();
		if (rep == VTK_POINTS)
		{
			if (index_pts)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					while (n-- > 0)
					{
						p[0] = *(inptr++);
						*(index_pts++) = (p[0]);
					}
				}
			}
		}
		else
		{
			if (index_lns)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					if (n-- > 0)
					{
						p[0] = *(inptr++);
						while (n-- > 0)
						{
							p[1] = *(inptr++);
							*(index_lns++) = (p[0]);
							*(index_lns++) = (p[1]);
							p[0] = p[1];
						}
					}
				}
			}
		}

		// Polygons
		cells = input->GetPolys();
		inptr = cells->GetPointer();
		if (rep == VTK_POINTS)
		{
			if (index_pts)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					while (n-- > 0)
					{
						p[0] = *(inptr++);
						*(index_pts++) = (p[0]);
					}
				}
			}
		}
		else if (rep == VTK_WIREFRAME)
		{
			if (index_lns)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					if (n-- > 0)
					{
						p[0] = *(inptr++);
						while (n-- > 0)
						{
							p[1] = *(inptr++);
							*(index_lns++) = (p[0]);
							*(index_lns++) = (p[1]);
							p[0] = p[1];
						}
					}
				}
			}
		}
		else
		{
			if (index_trs)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					if (n-- > 0)
					{
						p[0] = *(inptr++);
						if (n-- > 0)
						{
							p[1] = *(inptr++);
							while (n-- > 0)
							{
								p[2] = *(inptr++);
								*(index_trs++) = (p[0]);
								*(index_trs++) = (p[1]);
								*(index_trs++) = (p[2]);
								p[1] = p[2];
							}
						}
					}
				}
			}
		}

		// Triangles Strips
		cells = input->GetStrips();
		inptr = cells->GetPointer();
		if (rep == VTK_POINTS)
		{
			if (index_pts)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					while (n-- > 0)
					{
						p[0] = *(inptr++);
						*(index_pts++) = (p[0]);
					}
				}
			}
		}
		else if (rep == VTK_WIREFRAME)
		{
			if (index_lns)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					if (n-- > 0)
					{
						p[0] = *(inptr++);
						if (n-- > 0)
						{
							p[1] = *(inptr++);
							*(index_lns++) = (p[0]);
							*(index_lns++) = (p[1]);
							while (n-- > 0)
							{
								p[2] = *(inptr++);
								*(index_lns++) = (p[1]);
								*(index_lns++) = (p[2]);
								*(index_lns++) = (p[2]);
								*(index_lns++) = (p[0]);
								p[0] = p[1];
								p[1] = p[2];
							}
						}
					}
				}
			}
		}
		else
		{
			if (index_trs)
			{
				int np;
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					if (n-- > 0)
					{
						p[0] = *(inptr++);
						if (n-- > 0)
						{
							p[1] = *(inptr++);
							np = 2;
							while (n-- > 0)
							{
								p[np] = *(inptr++);
								*(index_trs++) = (p[0]);
								*(index_trs++) = (p[1]);
								*(index_trs++) = (p[2]);
								np = (np & 1) + 1;
								p[0] = p[np];
							}
						}
					}
				}
			}
		}

		// PRIMITIVES

		if (nb_pts > 0)
		{
			vtkFlowVRRenderPrimitiveData::PrimInfo& prim =
					prims->PointData.PPoints;
			prim.SetPosition(
					scene,
					prims->PointData.VBPositions.Active ? prims->PointData.VBPositions.Index
							: 0);
			prim.SetColor(
					scene,
					prims->PointData.VBColors.Active ? prims->PointData.VBColors.Index
							: 0);
			prim.SetNormal(
					scene,
					prims->PointData.VBNormals.Active ? prims->PointData.VBNormals.Index
							: 0);
			prim.SetTCoord(
					scene,
					prims->PointData.VBTCoords.Active ? prims->PointData.VBTCoords.Index
							: 0);
			prim.SetIndex(scene, prims->PointData.IBPoints.Index);
			prim.SetProperties(prop, scene);
			prim.SetTransform(act, scene);
		}

		if (nb_lns > 0)
		{
			vtkFlowVRRenderPrimitiveData::PrimInfo& prim =
					prims->PointData.PLines;
			prim.SetPosition(
					scene,
					prims->PointData.VBPositions.Active ? prims->PointData.VBPositions.Index
							: 0);
			prim.SetColor(
					scene,
					prims->PointData.VBColors.Active ? prims->PointData.VBColors.Index
							: 0);
			prim.SetNormal(
					scene,
					prims->PointData.VBNormals.Active ? prims->PointData.VBNormals.Index
							: 0);
			prim.SetTCoord(
					scene,
					prims->PointData.VBTCoords.Active ? prims->PointData.VBTCoords.Index
							: 0);
			prim.SetIndex(scene, prims->PointData.IBLines.Index);
			prim.SetProperties(prop, scene);
			prim.SetTransform(act, scene);
		}

		if (nb_trs > 0)
		{
			vtkFlowVRRenderPrimitiveData::PrimInfo& prim =
					prims->PointData.PTriangles;
			prim.SetPosition(
					scene,
					prims->PointData.VBPositions.Active ? prims->PointData.VBPositions.Index
							: 0);
			prim.SetColor(
					scene,
					prims->PointData.VBColors.Active ? prims->PointData.VBColors.Index
							: 0);
			prim.SetNormal(
					scene,
					prims->PointData.VBNormals.Active ? prims->PointData.VBNormals.Index
							: 0);
			prim.SetTCoord(
					scene,
					prims->PointData.VBTCoords.Active ? prims->PointData.VBTCoords.Index
							: 0);
			prim.SetIndex(scene, prims->PointData.IBTriangles.Index);
			prim.SetProperties(prop, scene);
			prim.SetTransform(act, scene);
		}
	}
	else
	{

//		std::cout << "else---" << std::endl;

		// Some attribute is changing on each cell: must separate all points

		prims->CellData.PPoints.SetState(nb_pts > 0, scene, fren);
		prims->CellData.PLines.SetState(nb_lns > 0, scene, fren);
		prims->CellData.PTriangles.SetState(nb_trs > 0, scene, fren);

		prims->PointData.PPoints.SetState(false, scene, fren);
		prims->PointData.PLines.SetState(false, scene, fren);
		prims->PointData.PTriangles.SetState(false, scene, fren);

		int ipts = 0;
		int ilns = ipts + nb_pts;
		int itrs = ilns + 2 * nb_lns;
		int vtotal = itrs + 3 * nb_trs;

		vtkDataArray* vbdata[4];
		vbdata[0] = input->GetPoints()->GetData();
		vbdata[1] = MapScalars(tran);
		if (cellNormals)
			vbdata[2] = input->GetCellData()->GetNormals();
		else
			vbdata[2] = input->GetPointData()->GetNormals();
		vbdata[3] = input->GetPointData()->GetTCoords();

		if (this->GetMTime() > prims->CellData.VBAll.UpdateTime
				|| input->GetMTime() > prims->CellData.VBAll.UpdateTime
				|| ren->GetRenderWindow() != this->LastWindow)
		{

			int dataTypes[4];
			for (int i = 0; i < 4; i++)
			{
				if (vbdata[i] == NULL)
					dataTypes[i] = flowvr::render::Type::Null;
				else
					dataTypes[i] = flowvr::render::Type::vector(vtkFlowVRType(
							vbdata[i]->GetDataType()),
							vbdata[i]->GetNumberOfComponents());
			}

			vtkFlowVRRenderPrimitiveData::ResInfo& info = prims->CellData.VBAll;
			info.SetState(true, scene, fren);
			const double* bounds = input->GetPoints()->GetBounds();
			flowvr::render::ChunkVertexBuffer* vb = scene->addVertexBuffer(
					info.Index, vtotal, 4, dataTypes, flowvr::render::BBox(
							flowvr::render::Vec3f(bounds[0], bounds[2],
									bounds[4]), flowvr::render::Vec3f(
									bounds[1], bounds[3], bounds[5])));
			vb->gen = Gen;

			unsigned char* outptr = (unsigned char*) vb->data();
			int cellnum = 0;
			vtkCellArray* cells;
			const vtkIdType* inptr;
			const char* pptr;
			const char* pptr2;
			int c, n, p[3];

#define WPOINT(p) \
      { \
	pptr = (const char*)vbdata[0]->GetVoidPointer(p); \
	pptr2 = pptr+flowvr::render::Type::size(dataTypes[0]); \
        while (pptr<pptr2) \
	  *(outptr++) = *(pptr++); \
	if (vbdata[1]!=NULL) \
	{ \
	  if (!cellScalars) \
	    pptr = (const char*)vbdata[1]->GetVoidPointer(p); \
	  else \
	    pptr = (const char*)vbdata[1]->GetVoidPointer(cellnum); \
	  pptr2 = pptr+flowvr::render::Type::size(dataTypes[1]); \
	  while (pptr<pptr2) \
	    *(outptr++) = *(pptr++); \
	} \
	if (vbdata[2]!=NULL) \
	{ \
	  if (!cellNormals) \
	    pptr = (const char*)vbdata[2]->GetVoidPointer(p); \
	  else \
	    pptr = (const char*)vbdata[2]->GetVoidPointer(cellnum); \
	  pptr2 = pptr+flowvr::render::Type::size(dataTypes[2]); \
	  while (pptr<pptr2) \
	    *(outptr++) = *(pptr++); \
	} \
	if (vbdata[3] != NULL) \
	{ \
	  pptr = (const char*)vbdata[3]->GetVoidPointer(p); \
	  pptr2 = pptr+flowvr::render::Type::size(dataTypes[3]); \
	  while (pptr<pptr2) \
	    *(outptr++) = *(pptr++); \
	} \
      }

			// First the points
			cells = input->GetVerts();
			inptr = cells->GetPointer();
			for (c = 0; c < cells->GetNumberOfCells(); c++)
			{
				n = *(inptr++);
				while (n-- > 0)
				{
					p[0] = *(inptr++);
					WPOINT(p[0]);
				}
				++cellnum;
			}

			// Lines
			cells = input->GetVerts();
			inptr = cells->GetPointer();
			if (rep == VTK_POINTS)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					while (n-- > 0)
					{
						p[0] = *(inptr++);
						WPOINT(p[0]);
					}
					++cellnum;
				}
			}
			else
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					if (n-- > 0)
					{
						p[0] = *(inptr++);
						while (n-- > 0)
						{
							p[1] = *(inptr++);
							WPOINT(p[0]);
							WPOINT(p[1]);
							p[0] = p[1];
						}
					}
					++cellnum;
				}
			}

			// Polygons
			cells = input->GetPolys();
			inptr = cells->GetPointer();
			if (rep == VTK_POINTS)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					while (n-- > 0)
					{
						p[0] = *(inptr++);
						WPOINT(p[0]);
					}
					++cellnum;
				}
			}
			else if (rep == VTK_WIREFRAME)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					if (n-- > 0)
					{
						p[0] = *(inptr++);
						while (n-- > 0)
						{
							p[1] = *(inptr++);
							WPOINT(p[0]);
							WPOINT(p[1]);
							p[0] = p[1];
						}
					}
					++cellnum;
				}
			}
			else
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					if (n-- > 0)
					{
						p[0] = *(inptr++);
						if (n-- > 0)
						{
							p[1] = *(inptr++);
							while (n-- > 0)
							{
								p[2] = *(inptr++);
								WPOINT(p[0]);
								WPOINT(p[1]);
								WPOINT(p[2]);
								p[1] = p[2];
							}
						}
					}
					++cellnum;
				}
			}

			// Triangles Strips
			cells = input->GetStrips();
			inptr = cells->GetPointer();
			if (rep == VTK_POINTS)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					while (n-- > 0)
					{
						p[0] = *(inptr++);
						WPOINT(p[0]);
					}
					++cellnum;
				}
			}
			else if (rep == VTK_WIREFRAME)
			{
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					if (n-- > 0)
					{
						p[0] = *(inptr++);
						if (n-- > 0)
						{
							p[1] = *(inptr++);
							WPOINT(p[0]);
							WPOINT(p[1]);
							while (n-- > 0)
							{
								p[2] = *(inptr++);
								WPOINT(p[1]);
								WPOINT(p[2]);
								WPOINT(p[2]);
								WPOINT(p[0]);
								p[0] = p[1];
								p[1] = p[2];
							}
						}
					}
					++cellnum;
				}
			}
			else
			{
				int np;
				for (c = 0; c < cells->GetNumberOfCells(); c++)
				{
					n = *(inptr++);
					if (n-- > 0)
					{
						p[0] = *(inptr++);
						if (n-- > 0)
						{
							p[1] = *(inptr++);
							np = 2;
							while (n-- > 0)
							{
								p[np] = *(inptr++);
								WPOINT(p[0]);
								WPOINT(p[1]);
								WPOINT(p[2]);
								np = (np & 1) + 1;
								p[0] = p[np];
							}
						}
					}
					++cellnum;
				}
			}
			info.UpdateTime.Modified();
		}
		// Now the index buffers are very easy :)

		if (nb_pts > 0)
		{
			vtkFlowVRRenderPrimitiveData::ResInfo& info =
					prims->CellData.IBPoints;
			if (!info.Index)
			{
				info.Index = fren->GenerateID();
				if (!info.Index)
				{
					vtkErrorMacro(<< "Failed to generate an ID!");
					return;
				}
				flowvr::render::ChunkIndexBuffer* ib = scene->addIndexBuffer(
						info.Index, -1, flowvr::render::Type::Null,
						flowvr::render::ChunkIndexBuffer::Point);
			}
			info.UpdateTime.Modified();
		}

		if (nb_lns > 0)
		{
			vtkFlowVRRenderPrimitiveData::ResInfo& info =
					prims->CellData.IBLines;
			if (!info.Index)
			{
				info.Index = fren->GenerateID();
				if (!info.Index)
				{
					vtkErrorMacro(<< "Failed to generate an ID!");
					return;
				}
				flowvr::render::ChunkIndexBuffer* ib = scene->addIndexBuffer(
						info.Index, -1, flowvr::render::Type::Null,
						flowvr::render::ChunkIndexBuffer::Line);
			}
			info.UpdateTime.Modified();
		}

		if (nb_trs > 0)
		{
			vtkFlowVRRenderPrimitiveData::ResInfo& info =
					prims->CellData.IBTriangles;
			if (!info.Index)
			{
				info.Index = fren->GenerateID();
				if (!info.Index)
				{
					vtkErrorMacro(<< "Failed to generate an ID!");
					return;
				}
				flowvr::render::ChunkIndexBuffer* ib = scene->addIndexBuffer(
						info.Index, -1, flowvr::render::Type::Null,
						flowvr::render::ChunkIndexBuffer::Triangle);
			}
			info.UpdateTime.Modified();
		}

		// PRIMITIVES
		flowvr::ID VB = prims->CellData.VBAll.Index;
		if (nb_pts > 0)
		{
			vtkFlowVRRenderPrimitiveData::PrimInfo& prim =
					prims->CellData.PPoints;
			prim.SetPosition(scene, vbdata[0] ? VB : 0, 0);
			prim.SetColor(scene, vbdata[1] ? VB : 0, 1);
			prim.SetNormal(scene, vbdata[2] ? VB : 0, 2);
			prim.SetTCoord(scene, vbdata[3] ? VB : 0, 3);
			prim.SetIndex(scene, prims->CellData.IBPoints.Index, ipts, nb_pts);
			prim.SetProperties(prop, scene);
			prim.SetTransform(act, scene);
		}

		if (nb_lns > 0)
		{
			vtkFlowVRRenderPrimitiveData::PrimInfo& prim =
					prims->CellData.PLines;
			prim.SetPosition(scene, vbdata[0] ? VB : 0, 0);
			prim.SetColor(scene, vbdata[1] ? VB : 0, 1);
			prim.SetNormal(scene, vbdata[2] ? VB : 0, 2);
			prim.SetTCoord(scene, vbdata[3] ? VB : 0, 3);
			prim.SetIndex(scene, prims->CellData.IBLines.Index, ilns, 2
					* nb_lns);
			prim.SetProperties(prop, scene);
			prim.SetTransform(act, scene);
		}

		if (nb_trs > 0)
		{
			vtkFlowVRRenderPrimitiveData::PrimInfo& prim =
					prims->CellData.PTriangles;
			prim.SetPosition(scene, vbdata[0] ? VB : 0, 0);
			prim.SetColor(scene, vbdata[1] ? VB : 0, 1);
			prim.SetNormal(scene, vbdata[2] ? VB : 0, 2);
			prim.SetTCoord(scene, vbdata[3] ? VB : 0, 3);
			prim.SetIndex(scene, prims->CellData.IBTriangles.Index, itrs, 3
					* nb_trs);
			prim.SetProperties(prop, scene);
			prim.SetTransform(act, scene);
		}

	}
	this->LastWindow = ren->GetRenderWindow();
	++this->Gen;
}

// Description:
// Release any graphics resources that are being consumed by this PolyDataMapper.
// The parameter window could be used to determine which graphic
// resources to release. Using the same PolyDataMapper object in multiple
// render windows is NOT currently supported. 
void vtkFlowVRPolyDataMapper::ReleaseGraphicsResources(vtkWindow *win)
{
	if (win != NULL && win->IsA("vtkFlowVRRenderWindow"))
	{
		flowvr::render::ChunkWriter* scene =
				static_cast<vtkFlowVRRenderWindow*> (win)->GetSceneWriter();

		for (unsigned int i = 0; i < Primitives.size(); i++)
			Primitives[i]->Release(scene);

		Primitives.clear();

		if(m_vShaderId >= 0)
		{
			scene->delVertexShader(m_vShaderId);
			m_vShaderId = -1;
		}

		if(m_pShaderId)
		{
			scene->delPixelShader(m_pShaderId);
			m_pShaderId = -1;
		}

		LastWindow = NULL;
		Modified();
	}
}

