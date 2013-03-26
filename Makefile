.PHONY: all

all:
	cd src;make VERSION=$(VERSION)

install: all
	cd src;make install

clean:
	cd src;make clean
