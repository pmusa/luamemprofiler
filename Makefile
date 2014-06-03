#
# Author: Pablo Musa
# Creation Date: mar 27 2011
# Last Modification: jun 03 2014
# See Copyright Notice in COPYRIGHT
# 
# luamemprofiler - A Memory Profiler for the Lua language
#

CC = gcc
CFLAGS = -g -Wall -ansi -pedantic -fPIC -shared

# used the 'sdl2-config' output + the '-lSDL2_ttf'
SDL_LIBS = -L/home/pmusa/Programs/SDL/lib -Wl,-rpath,/home/pmusa/Programs/SDL/lib -lSDL2 -lSDL2_ttf -lpthread
SDL_CFLAGS = -I/home/pmusa/Programs/SDL/include/SDL2 -D_REENTRANT

LUA_DIR = /usr/include/lua5.2
LUA_CFLAGS = -I$(LUA_DIR)
LUA_LIBS = -llua5.2

all: luamemprofiler.so

luamemprofiler.so: graphic.o lmp_struct.o vmemory.o lmp.o luamemprofiler.o
	$(CC) graphic.o lmp_struct.o vmemory.o lmp.o luamemprofiler.o -o luamemprofiler.so $(CFLAGS) $(SDL_LIBS) $(LUA_LIBS)

luamemprofiler.o: luamemprofiler.c lmp.h
	$(CC) -c luamemprofiler.c $(CFLAGS) $(LUA_CFLAGS)

lmp.o: lmp.c lmp.h lmp_struct.h vmemory.h
	$(CC) -c lmp.c $(CFLAGS) $(LUA_CFLAGS)

lmp_struct.o: lmp_struct.c lmp_struct.h
	$(CC) -c lmp_struct.c $(CFLAGS) $(LUA_CFLAGS)

vmemory.o: vmemory.c lmp.h vmemory.h graphic.h
	$(CC) -c vmemory.c $(CFLAGS) $(LUA_CFLAGS)

graphic.o: gsdl.c graphic.h
	$(CC) -c gsdl.c -o graphic.o $(CFLAGS) $(SDL_CFLAGS)

clean:
	rm *.o

test:
	./run.sh

