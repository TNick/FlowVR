/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                                                                 *
*                                                                 *
*-----------------------------------------------------------------*
 * COPYRIGHT (C) 2003-2011                by                       *
* INRIA                                                           *
* ALL RIGHTS RESERVED.	                                          *
*                                                                 *
* This source is covered by the GNU LGPL, please refer to the     *
* COPYING file for further information.                           *
*                                                                 *
*-----------------------------------------------------------------*
*                                                                 *
*  Original Contributors:                                         *
*    Jeremie Allard,                                              *
*    Thomas Arcila,                                               *
*    Jean-Denis Lesage.                                           *
*    Clement Menier,                                              *
*    Bruno Raffin                                                 *
*                                                                 *
*******************************************************************
*                                                                 *
*  Contact :                                                      *
*                                                                 *
******************************************************************/

#ifndef VTKFLOWVRPROPERTY_H_
#define VTKFLOWVRPROPERTY_H_

#include <vtkProperty.h>


class vtkFlowVRProperty : public vtkProperty
{
public:
	static vtkFlowVRProperty *New();
protected:
	vtkFlowVRProperty();
};


#endif // VTKFLOWVRPROPERTY_H_
