#!/bin/sh

export PATH=./install/bin:$PATH
export LD_LIBRARY_PATH=./install/components:$LD_LIBRARY_PATH

flowvr -x -L \
       -Pproducerconsumerregular/producer:nb=2 \
       --complib install/components/libproducerconsumerregular.comp.so \
       ProducerConsumerRegular
