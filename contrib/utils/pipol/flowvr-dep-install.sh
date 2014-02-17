##################################
# Install  FlowVR dependencies   #
##################################

#!/bin/bash



usage() {
  cat << EOT
Usage: flowvr-dep-install
       Probe OS and install dependencies (should be executed as root)
       Work with Debian, Ubuntu and Mac OS X. Modify the script to fit your needs. 
EOT
  exit 0
}



dependencies() {

#GET the OS name
    OS=`uname`
    
    echo "Install FlowVR dependencies on $OS"
    
# Linux
    if [[ $OS == 'Linux' ]]; then

        CurrentDir=$PWD 
        echo "Install Debian/Ubuntu packages"  
	sudo  aptitude update
        sudo  aptitude install -y build-essential \
              doxygen freeglut3 freeglut3-dev libdevil \
              libdevil-dev libdevil1c2 libglew1.5 libglew1.5-dev \
              libglut3 libglut3-dev libfreetype6 libfreetype6-dev \
              libxml-dom-perl nvidia-cg-toolkit libxml2-dev libxml2 \
              libxslt1.1 libxslt1-dev libgraphviz4 libgraphviz-dev \
              graphviz libqt4-core libqt4-dev libqt4-qt3support \
              libxi6 libxi-dev libxmu6 libopenmpi-dev libopenmpi1 \
              libglui-dev wget libcegui-mk2-dev openssh-server \
              unzip mesa-common-dev subversion libbz2-dev
        

#install de cmake

	VERSION=`cmake --version`
	echo "version de cmake disponible: $VERSION"

	if [ "$VERSION" = "cmake version 2.8.0" ]
	then
        	echo "No need to re-install cmake"
	else
        	cd $CurrentDir
		mkdir cmake
	        cd cmake
	        wget http://www.cmake.org/files/v2.8/cmake-2.8.0.tar.gz
	        tar -xzf ./cmake-2.8.0.tar.gz
        	cd cmake-2.8.0
	        #cmake .
        	./configure

	        sudo make install
	fi
        
    #VRPN
        echo "VRPN"
	cd $CurrentDir
        wget ftp://ftp.cs.unc.edu/pub/packages/GRIP/vrpn/vrpn_07_28.zip
        mkdir vrpn_07_28
        cd vrpn_07_28
        unzip ../vrpn_07_28.zip
        cd vrpn
	mkdir build
        cd build
        sudo cmake .. -DGLUT_Xi_LIBRARY:FILEPATH=/usr/lib/libXi.so -DGLUT_Xmu_LIBRARY:FILEPATH=/usr/lib/libXmu.so.6
        sudo make install 
    fi
    
#MAC:
    if [[ $OS == 'Darwin' ]]; then

    # set environment
    export PATH=$PATH:/opt/local/bin
        
    # Dawrin port
    
        if !(which /opt/local/bin/port &>/dev/null) then
            echo "Error: Install Darwin port first (http://darwinports.com/)"
            exit 0;
        fi
        
    echo "Install multiple flowvr dependencies through  darwin port"

    # wget
        sudo port install wget
        
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
diff -ur ../REF/devil-1.7.8/src-ILU/ilur/ilur.c ./src-ILU/ilur/ilur.c
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
        chmod a+x  configure
        ./configure  LDFLAGS='-L/opt/local/lib'  CPPFLAGS='-I/opt/local/include/'  CFLAGS='-I/opt/local/include/'  --prefix=/usr/local/ --enable-opengl=yes --enable-ILU=yes --enable-ILUT=yes --enable-x86_64=yes --enable-allegro=no  --without-x 
        make
	sudo chmod a+x build-aux/install-sh
        sudo make install
        
    # GLEW
        sudo port install glew
        
    # graphviz
        sudo port install graphviz
        
    # MPI
        sudo port install openmpi
        
    # Doxygen
        sudo port install doxygen
        
    # cmake
        sudo port install cmake

    # glui
        sudo port install glui

        
    #VRPN
        sudo port install vrpn
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
        echo "Nvidia CG"
        wget http://developer.download.nvidia.com/cg/Cg_2.2/Cg-2.2_February2010.dmg
        hdiutil mount Cg-2.2_February2010.dmg 
        sudo  tar --gzip --extract -f /Volumes/Cg-2.2.0017/Cg-2.2.0017.app/Contents/Resources/Installer\ Items/NVIDIA_Cg.tgz   -C /
        hdiutil unmount /Volumes/Cg-2.2.0017
        
    #QT
        echo "QT4"

        # Take a lot of time: prefer install from  Nokia .dmg
        #sudo /opt/local/bin/port install qt4-mac

        wget http://get.qt.nokia.com/qt/source/qt-mac-cocoa-opensource-4.6.3.dmg
        hdiutil mount qt-mac-cocoa-opensource-4.6.3.dmg 
        sudo  installer  -pkg /Volumes/Qt\ 4.6.3/Qt.mpkg -target /
        hdiutil unmount /Volumes/Qt\ 4.6.3
        
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

