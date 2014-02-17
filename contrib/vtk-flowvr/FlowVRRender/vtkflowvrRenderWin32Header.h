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
* File: ./FlowVRRender/vtkflowvrRenderWin32Header.h               *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile$

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkflowvrRenderWin32Header - manage Windows system differences
// .SECTION Description
// The vtkflowvrRenderWin32Header captures some system differences between Unix
// and Windows operating systems. 

#ifndef __vtkflowvrRenderWin32Header_h
#define __vtkflowvrRenderWin32Header_h

#include <vtkflowvrConfigure.h>

#if defined(WIN32) && !defined(VTKFLOWVR_STATIC)
#if defined(vtkFlowVRRender_EXPORTS)
#define VTK_FLOWVR_RENDER_EXPORT __declspec( dllexport ) 
#else
#define VTK_FLOWVR_RENDER_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_FLOWVR_RENDER_EXPORT
#endif

#endif
