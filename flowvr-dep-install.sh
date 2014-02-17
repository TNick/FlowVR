##################################
# Install  FlowVR dependencies   #
##################################

#!/bin/bash




usage() {
  cat << EOT
Usage: flowvr-dep-install
       Probe OS and install dependencies (should be executed as root)
       Works with Debian, Ubuntu and Mac OS X. Modify the script to fit your needs. 
EOT
  exit 0
}



dependencies() {

    #GET the OS name
    OS=`uname`
    
    echo "Operating System: $OS"
    
    # Linux
    if [[ $OS == 'Linux' ]]; then
        
        echo ""
        echo "Debian/Ubuntu packages  - contrib repository required."
        echo ""
	sudo  aptitude update

	echo ""
	echo "Installing core FlowVR dependencies. Make sure you thoroughly check aptitude's solution before accepting it."
	echo ""
	# FlowVR core dependencies
	sudo aptitude install build-essential gcc g++ make cmake cmake-curses-gui python2.6 python2.6-dev python-dev swig freeglut3 freeglut3-dev libxi-dev libxmu-dev openssh-server openssh-client

       	echo ""
       	echo "Installing flowvr-glgraph dependencies."
       	echo ""
        # FlowVR-glgraph dependencies (Qt4, graphviz)
        sudo aptitude install libxml2-dev libxml2 libxslt1.1 libxslt1-dev libgraphviz4 libgraphviz-dev graphviz libqt4-core libqt4-dev libqt4-qt3support
        
        # Misc dependencies
        #sudo aptitude install doxygen
        
        # VTK-FlowVR dependencies
        #sudo aptitude install libvtk5-dev python-vtk tcl-vtk
        
        # MPI (used by fluidmpi, mpi daemon)
        #sudo aptitude libopenmpi-dev libopenmpi1 mpi-default-bin
        
        #VRPN (used by contrib/flowvr-vrpn)
        #echo "VRPN 07_30 - Needs CMake >=  2.8.3"
        #wget ftp://ftp.cs.unc.edu/pub/packages/GRIP/vrpn/vrpn_07_30.zip
        #mkdir vrpn_07_30
        #cd vrpn_07_30
        #unzip ../vrpn_07_30.zip
        
        #cd vrpn
        #mkdir build
        #cd build
        #cmake ..
        #sudo make install
        
    fi
    
    #MAC:
    if [[ $OS == 'Darwin' ]]; then
        
        # Darwin port
        if !(which port &>/dev/null) then
            echo "Error: Install Darwin port first (http://darwinports.com/)"
            exit 0;
        fi
        
        echo "Install multiple flowvr dependencies through  darwin port"

        # wget
        sudo port install wget

        # cmake
        sudo port install cmake


        # python
        sudo port install python27

       
        # Devil
        # First install image format handling library  for textures (install other formats  as needed)
        sudo port install jpeg tiff  libpng
        
        # Now install devil

        # Devil available through port not compiled with opengl support 
        #sudo port install libdevil
        wget http://downloads.sourceforge.net/openil/DevIL-1.7.8.zip
        unzip DevIL-1.7.8.zip
        cd  devil-1.7.8 
                echo " 
--- ../REF/devil-1.7.8/src-ILU/ilur/ilur.c      2009-03-25 13:30:40.000000000 +0100
+++ ./src-ILU/ilur/ilur.c       2010-11-23 20:04:58.000000000 +0100
@@ -1,6 +1,7 @@
 #include <string.h>
 #include <stdio.h>
-#include <malloc.h>
+//#include <malloc.h>
+#include <stdlib.h>
 
 #include <IL/il.h>
 #include <IL/ilu.h>
" | patch src-ILU/ilur/ilur.c  
        echo "
--- ../REF/devil-1.7.8/src-IL/src/il_icon.c     2009-03-25 13:30:42.000000000 +0100
+++ ./src-IL/src/il_icon.c      2011-03-03 10:31:34.000000000 +0100
@@ -525,7 +525,7 @@
 
 	// Expand low-bit-depth grayscale images to 8 bits
 	if (ico_color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
-		png_set_gray_1_2_4_to_8(ico_png_ptr);
+		png_set_expand_gray_1_2_4_to_8(ico_png_ptr);
 	}
 
 	// Expand RGB images with transparency to full alpha channels
"  | patch src-IL/src/il_icon.c 
        echo "
--- ../REF/devil-1.7.8/src-IL/src/il_png.c      2009-03-25 13:30:42.000000000 +0100
+++ ./src-IL/src/il_png.c       2011-03-03 10:31:54.000000000 +0100
@@ -278,7 +278,7 @@
 
 	// Expand low-bit-depth grayscale images to 8 bits
 	if (png_color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
-		png_set_gray_1_2_4_to_8(png_ptr);
+		png_set_expand_gray_1_2_4_to_8(png_ptr);
 	}
 
 	// Expand RGB images with transparency to full alpha channels
"  | patch src-IL/src/il_png.c 
        chmod a+x  configure
        ./configure  LDFLAGS='-L/opt/local/lib'  CPPFLAGS='-I/opt/local/include/'  CFLAGS='-I/opt/local/include/'  --prefix=/usr/local/ --enable-opengl=yes --enable-ILU=yes --enable-ILUT=yes --enable-x86_64=yes --enable-allegro=no  --without-x 
        make
        chmod a+x build-aux/install-sh 
        sudo make install
        
    # GLEW
        sudo port install glew
        
    # graphviz
        sudo port install graphviz-devel
        
    # MPI
        sudo port install openmpi
        
    # Doxygen
        sudo port install doxygen
        
        
    # swig
        sudo port install swig-python

    # glui
        sudo port install glui

        
    #VRPN
        #sudo port install vrpn
        # echo "VRPN"
        # wget ftp://ftp.cs.unc.edu/pub/packages/GRIP/vrpn/vrpn_07_26.zip
        # mkdir vrpn_07_26
        # cd vrpn_07_26
        # unzip ../vrpn_07_26.zip
        # cd quat
        # sudo make install HW_OS=universal_macosx 
        # cd ../vrpn
        # sudo  make install HW_OS=universal_macosx


    #CG
        #echo "Nvidia CG"
        #wget http://developer.download.nvidia.com/cg/Cg_3.0/Cg-3.0_February2011.dmg
        #hdiutil mount Cg-3.0_February2011.dmg 
        #sudo  tar --gzip --extract -f /Volumes/Cg-3.0.0016/Cg-3.0.0016.app/Contents/Resources/Installer\ Items/NVIDIA_Cg.tgz  -C /
        #hdiutil unmount /Volumes/Cg-3.0.0016
        
    #QT
        echo "QT4"

        # Takes a lot of time: prefer install from  Nokia .dmg
        #sudo port install qt4-mac

        
        wget http://releases.qt-project.org/qt4/source/qt-mac-opensource-4.8.4.dmg
        hdiutil mount qt-mac-opensource-4.8.4.dmg
        sudo  installer  -pkg /Volumes/Qt\ 4.8.4/Qt.mpkg -target /
        hdiutil unmount /Volumes/Qt\ 4.8.4
        
    fi

    echo "FlowVR dependencies installed. You can now compile and install FlowVR"
}


#main

finish=0

while [[ $finish != 1 ]]; do
    if [[ $1 == "-h" ]] ||  [[ $1 == "--help" ]]  ||  [[ $1 == "-help" ]]; then
        usage;
        exit 0;
    else
        dependencies;
        finish=1
    fi
done

