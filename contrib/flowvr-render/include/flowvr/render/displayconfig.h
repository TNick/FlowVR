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
* File: ./include/flowvr/render/displayconfig.h                   *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef FLOWVR_RENDER_DISPLAY_CONFIG_H
#define FLOWVR_RENDER_DISPLAY_CONFIG_H

#include <vector>
#include <string>

namespace flowvr
{

namespace render
{

struct DisplayConfig
{
  // Global config

  float eye_separation;
  float eye_focal;
  std::string left_path;
  std::string right_path;
  float proj_nx;
  float proj_ny;
  float screen_x0;
  float screen_y0;
  float screen_nx;
  float screen_ny;

  // Local config
  int win_x0;
  int win_y0;
  int win_nx;
  int win_ny;
  struct View
  {
    enum { Left, Right } eye;
    std::string filename;
    double proj[4][4];
    int texture_mask;
    int texture_add;
    int texture_mult;
    View()
    {
      eye = Left;
      for (int i=0;i<4;i++)
	for (int j=0;j<4;j++)
	  proj[i][j] = 0;
      for (int i=0;i<4;i++)
	proj[i][i] = 1;
      texture_mask = 0;
      texture_add = 0;
      texture_mult = 0;
    }
  };
  std::vector<View> views;

  enum Correction { NoCorrection, Mask, Lumi, WMask, WLumi, BLumi, NbCorrection } correction;

  DisplayConfig()
  {
    init();
  }

  void init();

  bool load(std::string filename, int rank=0);

  bool loadProj();

  int getNbView()
  {
    if (views.empty()) return 1;
    else return views.size();
  }

  void loadViewProj(int view);

  void getViewBBox(int view, float& x0, float &y0, float &x1, float &y1);

  float getViewEyeShift(int view)
  {
    if ((unsigned)view<views.size())
      return (views[view].eye==View::Left)?eye_separation:-eye_separation;
    else
      return 0;
  }

  void applyMasks(int view, bool white = false);

  void applyLumi(int view, bool white = false, bool black = false);

  void applyCorrection(int view);

};

} // namespace render

} // namespace flowvr

#endif
