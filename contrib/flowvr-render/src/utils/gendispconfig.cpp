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
* File: ./src/utils/gendispconfig.cpp                             *
*                                                                 *
* Contacts:                                                       *
*                                                                 *
******************************************************************/
#ifndef NODEVIL
#include <IL/il.h>
#endif
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <errno.h>
#include <math.h>

int main(int argc, char**argv)
{
  if (argc!=8)
  {
    std::cout << "Usage: flowvr-gendispconfig NX NY RESX RESY BX BY gamma\n";
    return 1;
  }

  int NX = atoi(argv[1]);
  int NY = atoi(argv[2]);
  int RX = atoi(argv[3]);
  int RY = atoi(argv[4]);
  int BX = atoi(argv[5]);
  int BY = atoi(argv[6]);
  double gamma = atof(argv[7]);

  double rampX[BX];
  double rampY[BY];

  ilInit();

  ILuint imgid; ilGenImages(1,&imgid);
  ilBindImage(imgid);

  for (int x = 0; x < BX; x++)
    rampX[x] = pow(double(x+1)/double(BX+1), gamma);

  for (int y = 0; y < BY; y++)
    rampY[y] = pow(double(y+1)/double(BY+1), gamma);

  unsigned char* data = new unsigned char[RX*RY];

  char fname[256];

  for (int ix = 0; ix < NX; ix++)
    for (int iy = 0; iy < NY; iy++)
    {

#ifndef NODEVIL
      // image
      unsigned char* dest=data;
      for (int y=0;y<RY;y++)
	for (int x=0;x<RX;x++)
	{
	  double v=1;
	  if (ix>0 && x<BX)        v*=rampX[x];
	  if (ix<NX-1 && x>=RX-BX) v*=rampX[RX-1-x];
	  if (iy<NY-1 && y<BY)        v*=rampY[y];
	  if (iy>0 && y>=RY-BY) v*=rampY[RY-1-y];
	  *dest = (unsigned char)(255*v);
	  ++dest;
	}
      ilTexImage(RX,RY,1,1,IL_LUMINANCE,IL_UNSIGNED_BYTE,data);
      sprintf(fname,"proj_%dx%d_mask.png",iy,ix);
      unlink(fname);
      ilSaveImage(fname);
#endif

      // homography

      sprintf(fname,"proj_%dx%d_homography.txt",iy,ix);
      unlink(fname);
      std::ofstream o;
      o.open(fname);

      o << 1.0 <<' '<< 0.0 <<' '<< ((double)ix*(RX-BX)) <<' ';
      o << 0.0 <<' '<< 1.0 <<' '<< ((double)iy*(RY-BY)) <<' ';
      o << 0.0 <<' '<< 0.0 <<' '<< 1                    << std::endl;

      o.close();
    }

  // display config
  {
    sprintf(fname,"displays_%dx%d.conf",NY,NX);
    unlink(fname);
    std::ofstream o;
    o.open(fname);

    o << "eye_separation 0\n";
    o << "eye_focal 1\n";
    //    char* pwd = get_current_dir_name(); Not POSIX compatible
    int pwdsize = 256;
    char* pwd = (char*)malloc(pwdsize);
    while (!getcwd(pwd,pwdsize) && errno==ERANGE)
    {
      pwdsize*=2;
      free(pwd);
      pwd=(char*)malloc(pwdsize);
    }
    o << "left_path " << pwd << '\n';
    o << "right_path " << pwd << '\n';
    free(pwd);
    o << "proj_size "<<RX<<' '<<RY<<'\n';
    o << "screen_border 0 0 "<<(BX+(RX-BX)*NX)<<' '<<(BY+(RY-BY)*NY)<<'\n';
#ifndef NODEVIL
    o << "correction mask\n";
#else
    o << "correction none\n";
#endif

    int i=1;
    for (int iy = 0; iy < NY; iy++)
      for (int ix = 0; ix < NX; ix++)
      {
	o << "visu"<<i<<" 0 0 "<<RX<<' '<<RY<<" L"<<iy<<'x'<<ix<<'\n';
	i++;
      }
    o.close();
  }

  delete[] data;
  return 0;
}
