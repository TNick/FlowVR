##################################
# script compilation auto FlowVR pour MAC #
##################################

#!/bin/bash


#PIPOL esn amd64-linux-ubuntu-hardy.dd none 2:00 --user
#PIPOL esn i386-linux-ubuntu-lucid.dd.gz none 2:00 --user
#PIPOL esn x86_64_mac-mac-osx-server-snow-leopard.dd.gz none 2:00 --user
#PIPOL esn amd64-linux-debian-lenny.dd.gz none 2:00 --user


#GET the OS name
OS=`uname`

# OS specific working directory 
BASEDIR=/pipol
DIR=$BASEDIR/$OS


cd $BASEDIR
# Clean any leftover  from previous install
rm -fr $OS

# Install dependencies (en mode auto)
mkdir  $OS
cd $OS


echo "Install dependencies"
echo "calling flowvr-dep-install.sh "

#svn co --username pipol --password pipol https://wiki-flowvr.imag.fr/svn/flowvr-suite/branches/dev/  $BASEDIR/install_dep/ --depth empty
#cd  $BASEDIR/install_dep/
#svn up flowvr-dep-install.sh

svn co --username pipol --password pipol https://wiki-flowvr.imag.fr/svn/flowvr-suite/branches/scripts_and_utils/ $BASEDIR/install_dep/
cd  $BASEDIR/install_dep/
sudo chmod 777 ./flowvr-dep-install.sh
./flowvr-dep-install.sh

echo "Flowvr Trunk...."
cd $BASEDIR
#check out  'trunk'
svn co --username pipol --password pipol https://wiki-flowvr.imag.fr/svn/flowvr-suite/trunk/flowvr-suite $BASEDIR/flowvr-suite


#cmake flowvr-suite
cd flowvr-suite
PWD1=$PWD;
mkdir build
cd build

if [[ $OS == 'Darwin' ]]; then

export PATH=$PATH:/opt/local/bin

cmake ./..  \
             -DCMAKE_INSTALL_PREFIX:PATH=./../install-suite\
             -DCMAKE_BUILD_TYPE:STRING=Release\
	     -DBUILD_FVMODULEAPI:BOOL=OFF\
             -DBUILD_FLOWVR_RENDER:BOOL=ON\
             -DBUILD_FLOWVR_GLGRAPH:BOOL=ON\
             -DBUILD_FLOWVR_RENDER_MPLAYER:BOOL=OFF\
             -DBUILD_FLOWVR_VRPN:BOOL=OFF\
             -DBUILD_VTK_FLOWVR:BOOL=OFF

ctest -D Nightly
fi

if [[ $OS == 'Linux' ]]; then
cmake ./.. -DCMAKE_INSTALL_PREFIX:PATH=/pipol/flowvr-suite/install-suite  -DBUILD_FVMODULEAPI:BOOL=OFF  -DBUILD_FLOWVR_RENDER:BOOL=ON  -DBUILD_FLOWVR_GLGRAPH:BOOL=ON  -DBUILD_FLOWVR_RENDER_MPLAYER:BOOL=OFF -DBUILD_FLOWVR_VRPN:BOOL=ON -DBUILD_VTK_FLOWVR:BOOL=OFF -DCG_COMPILER:PATH=/home/flowvr/avanel/dep_64/usr/bin/cgc -DGLUT_Xi_LIBRARY:FILEPATH=/usr/lib/libXi.so -DGLUT_Xmu_LIBRARY:FILEPATH=/usr/lib/libXmu.so.6


ctest -D NightlyStart
ctest -D NightlyConfigure
ctest -D NightlyBuild
ctest -D NightlySubmit

make install
fi

. ./../install-suite/bin/flowvr-suite-config.sh


#examples compilation 

	# simple script to configure, compile and install the example tictac
	cd /pipol/flowvr-suite/install-suite/share/flowvr/examples/tictac
	mkdir build;
	cd build;
	cmake -DCMAKE_VERBOSE_MAKEFILE=1 -DBUILD_MODULES=1  -DCMAKE_INSTALL_PREFIX:PATH=$PWD/../ -DGLUT_Xmu_LIBRARY:FILEPATH=/usr/lib/libXmu.so.6 $PWD/../;
	ctest -D Experimental

	# now primes example :
        cd /pipol/flowvr-suite/install-suite/share/flowvr/examples/primes
	mkdir build;
        cd build;
        cmake -DCMAKE_VERBOSE_MAKEFILE=1 -DBUILD_MODULES=1  -DCMAKE_INSTALL_PREFIX:PATH=$PWD/../ -DGLUT_Xmu_LIBRARY:FILEPATH=/usr/lib/libXmu.so.6 $PWD/../;
        ctest -D Experimental

	# and fluid :
	cd /pipol/flowvr-suite/install-suite/share/flowvr/examples/fluid
        mkdir build;
        cd build;
        cmake -DCMAKE_VERBOSE_MAKEFILE=1 -DBUILD_MODULES=1  -DCMAKE_INSTALL_PREFIX:PATH=$PWD/../ -DGLUT_Xmu_LIBRARY:FILEPATH=/usr/lib/libXmu.so.6 $PWD/../;
        ctest -D Experimental
        

cd $DIR



echo "Flowvr Dev...."

#check out FlowVR  'dev'
cd $BASEDIR
svn co --username pipol --password pipol https://wiki-flowvr.imag.fr/svn/flowvr-suite/branches/dev $BASEDIR/flowvr-suite-dev


#cmake flowvr-dev
cd flowvr-suite-dev
PWD1=$PWD;
mkdir build
cd build
if [[ $OS == 'Darwin' ]]; then

cmake  $PWD1 \
             -DCMAKE_INSTALL_PREFIX:PATH=$PWD1/install-suite\
             -DCMAKE_BUILD_TYPE:STRING=Release\
	     -DBUILD_FVMODULEAPI:BOOL=OFF\
             -DBUILD_FLOWVR_RENDER:BOOL=ON\
             -DBUILD_FLOWVR_GLGRAPH:BOOL=ON\
             -DBUILD_FLOWVR_RENDER_MPLAYER:BOOL=OFF\
             -DBUILD_FLOWVR_VRPN:BOOL=OFF\
             -DBUILD_VTK_FLOWVR:BOOL=OFF

ctest -D Experimental
fi

if [[ $OS == 'Linux' ]]; then
cmake ./.. -DCMAKE_INSTALL_PREFIX:PATH=$PWD1/install-suite -DCMAKE_BUILD_TYPE:STRING=Release -DBUILD_FVMODULEAPI:BOOL=OFF -DBUILD_FLOWVR_RENDER:BOOL=ON -DBUILD_FLOWVR_GLGRAPH:BOOL=ON -DBUILD_FLOWVR_RENDER_MPLAYER:BOOL=OFF -DBUILD_FLOWVR_VRPN:BOOL=ON -DBUILD_VTK_FLOWVR:BOOL=OFF -DCG_COMPILER:PATH=/home/flowvr/avanel/dep_64/usr/bin/cgc -DGLUT_Xi_LIBRARY:FILEPATH=/usr/lib/libXi.so.6 -DGLUT_Xmu_LIBRARY:FILEPATH=/usr/lib/libXmu.so.6

ctest -D ExperimentalStart
ctest -D ExperimentalConfigure
ctest -D ExperimentalBuild
ctest -D ExperimentalSubmit
fi

cd $DIR


echo "Done!"




