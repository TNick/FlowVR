#!/bin/sh

export FLOWVR_PORTFILE_PATH=./config
export PATH=./install/bin:$PATH
export LD_LIBRARY_PATH=./install/components:./install/plugs:$LD_LIBRARY_PATH

flowvr -x -L --complib install/components/libproducerconsumerportutils.comp.so ProducerConsumerPortUtils
