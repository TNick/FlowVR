#!/bin/bash

DTD=0

if [ $# = 1 ]
then
if [ $1 = "-dtd" ]
then
DTD=1
fi
fi

if [ $DTD = 1 ]
then
echo
echo "      You should have set the environment variable SGML_CATALOG_FILES"
echo "      to find the catalogs (xml and flowvr-parse catalogs)"
echo "      The current value is SGML_CATALOG_FILES=$SGML_CATALOG_FILES"
echo "      It should look something like:"
echo "      SGML_CATALOG_FILES=/path/to/xml.soc:/path/to/flowvr-parse/dtd/flowvr-sgml.cat"
echo
echo 
else
echo "      Use flowvr-module-test.sh -dtd to active dtd validation with nsgmls "
fi


if [ $DTD = 1 ]
then
echo "      DTD Validation of  viz.desc.xml"
nsgmls -s  -wxml viz.desc.xml
echo "      DTD Validation of  vision.desc.xml"
nsgmls -s  -wxml vision.desc.xml
echo "      DTD Validation of  vision.mml.xml"
nsgmls -s  -wxml vizvis.mml.xml
fi

echo "      Flowvr-module Compute run commands (create machine list files viz.machine and vision.machine):"
flowvr-module  -d ./ -mml vizvis.mml.xml -run
echo "      Flowvr-module Compute the module list and put the result in vizvis.net.xml"
flowvr-module  -d ./ -mml vizvis.mml.xml -mod -mo vizvis.net.xml

if [ $DTD = 1 ]
then
echo "      DTD Validation of  vision.net : nsgmls should complain"
nsgmls -s  -wxml vizvis.net.xml
fi

