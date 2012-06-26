all: qread

qread: objpath.c msgpack.c objpath.h
	gcc objpath.c msgpack.c -o qread -lmsgpack -g
