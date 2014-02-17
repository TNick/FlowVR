all:
	mkdir -p BUILD ; cd BUILD ; cmake .. -DCMAKE_INSTALL_PREFIX=../portutils-bin; make all
	cd ..

install:
	mkdir -p BUILD ; cd BUILD ; cmake .. -DCMAKE_INSTALL_PREFIX=../portutils-bin; make install
	cd ..

clean:
	rm -rf BUILD