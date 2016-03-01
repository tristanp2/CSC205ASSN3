CC=g++
CFLAGS=-Wall
LIN_DIR=./lib_linux
OSX_DIR=./lib_osx

all:	

osx: 
	$(CC) -o LSViewer -Wall -g  *.cpp -framework SDL2 -L${OSX_DIR} -I. -lSDL2_gfx 
linux:
	$(CC) -o LSViewer -Wall -g  *.cpp `sdl2-config --cflags --libs` -L${LIN_DIR} -lSDL2_gfx 
