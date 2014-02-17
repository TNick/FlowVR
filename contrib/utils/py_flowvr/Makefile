


all: cflowvr_wrap.cxx _cflowvr.so


cflowvr_wrap.cxx: cflowvr.swg
	swig -v -python -c++  -I$(FLOWVR_PREFIX)/include cflowvr.swg 


_cflowvr.so: cflowvr_wrap.cxx
	g++ -shared -o _cflowvr.so \
              -fPIC -g cflowvr_wrap.cxx -I $(FLOWVR_PREFIX)/include -I /usr/include/python2.7 \
              -L $(FLOWVR_PREFIX)/lib -lflowvr-base -lflowvr-mod

test: 
	python -c "import cflowvr"


clean: 
	rm -rf _cflowvr.so cflowvr_wrap.cxx cflowvr.py



