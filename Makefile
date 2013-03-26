.PHONY: all

all:
	cd src;make VERSION=$(VERSION)

clean:
	cd src;make clean
