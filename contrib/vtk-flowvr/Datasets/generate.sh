#!/bin/sh -e
if [ -z "${VTK_FLOWVR_DATA_ROOT}" ]; then
  echo "VTK_FLOWVR_DATA_ROOT must be exported before running this script"
  echo "please refere to the documentation."
  exit 1
fi

DOWNLOADED=0
if [ -f "${VTK_FLOWVR_DATA_ROOT}/CT-Head.pvm" -a -f "${VTK_FLOWVR_DATA_ROOT}/XMasTree-LO.pvm" -a -f "${VTK_FLOWVR_DATA_ROOT}/VIEWER-2.2.zip" ]; then
  DOWNLOADED=1
fi

if [ ${DOWNLOADED} -eq 0 ]; then
  WGET="$(which wget)"
  if [ -x "${WGET}" ]; then
    OUTPUT=-O
  else
    WGET="$(which curl)"
    if [ -x "${WGET}" ]; then
      OUTPUT=-o
    else 
      echo "I can't find any download program (looked for curl and wget)."
      echo "Please download the following files, put them in ${VTK_FLOWVR_DATA_ROOT} and rerun this script"
      echo "http://www9.cs.fau.de/Persons/Roettger/library/CT-Head.pvm"
      echo "http://www9.cs.fau.de/Persons/Roettger/library/XMasTree-LO.pvm"
      echo "http://www.stereofx.org/download/VIEWER-2.2.zip"
      exit 0
    fi
  fi
fi

echo "Switching to ${VTK_FLOWVR_DATA_ROOT}..."
cd "${VTK_FLOWVR_DATA_ROOT}"
if [ ${DOWNLOADED} -eq 0 ]; then
  ${WGET} "http://www9.cs.fau.de/Persons/Roettger/library/CT-Head.pvm" ${OUTPUT} CT-Head.pvm
  ${WGET} "http://www9.cs.fau.de/Persons/Roettger/library/XMasTree-LO.pvm" ${OUTPUT} XMasTree-LO.pvm
  ${WGET} "http://www.stereofx.org/download/vvv/VIEWER-2.2.zip" ${OUTPUT} VIEWER-2.2.zip
fi

echo Building pvm2pvm and pvm2raw tools...
[ ! -d viewer ] && unzip VIEWER-2.2.zip

cd "${VTK_FLOWVR_DATA_ROOT}"/viewer

if ! grep -q ^.PHONY Makefile; then
  sed 's,^tools:,.PHONY:tools\ntools:,' -i Makefile
fi

ARCH="$(uname -m 2> /dev/null)"


if [ "$ARCH" = "x86_64" ]; then
  sed 's,i686,x86_64,g' -i build.sh
fi

HOSTTYPE=${ARCH}

if ! HOSTTYPE=${ARCH} ./build.sh all; then
  echo "There were some errors while building V^3. Can't continue. See http://www9.cs.fau.de/Persons/Roettger for more informations."
  pwd
  exit 1
fi

cd ..

PVM2PVM=./viewer/tools/pvm2pvm
PVM2RAW=./viewer/tools/pvm2raw
echo Converting XMas example...
${PVM2PVM} XMasTree-LO.pvm XMasTree-LO-8.pvm
${PVM2RAW} XMasTree-LO-8.pvm XMasTree-LO-8.pvm.raw

echo Converting Skull example...
${PVM2PVM} CT-Head.pvm CT-Head-8.pvm
${PVM2RAW} CT-Head-8.pvm CT-Head-8.pvm.raw

echo Checking generated files...
cat << EOF >> MD5SUM
581ec852c0a1cab95af767e6fdcd07ca  CT-Head-8.pvm.raw
2fb68a23d31a197ece68363a1a960eca  XMasTree-LO-8.pvm.raw
EOF
md5sum -c MD5SUM
