#
# Author: Pablo Musa
# Creation Date: mar 27 2011
# Last Modification: feb 22 2015
# See Copyright Notice in COPYRIGHT
# 
# luamemprofiler - A Memory Profiler for the Lua language
#
LUA_VERSION=5.3

CC = gcc
#CFLAGS = -g -O0 -Wall -ansi -pedantic -fPIC -shared
CFLAGS = -O2 -Wall -ansi -pedantic -fPIC -shared

# used the 'sdl2-config' output + the '-lSDL2_ttf'
SDL_LIBS = -L/home/pmusa/Programs/SDL/lib -Wl,-rpath,/home/pmusa/Programs/SDL/lib -lSDL2 -lSDL2_ttf -lpthread
SDL_CFLAGS = -I/home/pmusa/Programs/SDL/include/SDL2 -D_REENTRANT

#LUA_DIR = /usr/include/lua${LUA_VERSION}
LUA_DIR = /home/pmusa/Coding/lua/lua-${LUA_VERSION}.0/src
LUA_CFLAGS = -I$(LUA_DIR)
LUA_LIBS = -llua${LUA_VERSION}

all: bin/luamemprofiler.so

install: bin/luamemprofiler.so
	cp bin/luamemprofiler.so /usr/local/lib/lua/${LUA_VERSION}/

bin/luamemprofiler.so: src/graphic.o src/lmp_struct.o src/vmemory.o src/lmp.o src/luamemprofiler.o
	cd src && $(CC) graphic.o lmp_struct.o vmemory.o lmp.o luamemprofiler.o -o luamemprofiler.so $(CFLAGS) $(SDL_LIBS) $(LUA_LIBS) && mkdir -p ../bin && mv luamemprofiler.so ../bin/

src/luamemprofiler.o: src/luamemprofiler.c src/lmp.h
	cd src && $(CC) -c luamemprofiler.c $(CFLAGS) $(LUA_CFLAGS)

src/lmp.o: src/lmp.c src/lmp.h src/vmemory.h src/lmp_struct.c src/lmp_struct.h
	cd src && $(CC) -c lmp.c $(CFLAGS) $(LUA_CFLAGS)

src/vmemory.o: src/vmemory.c src/vmemory.h src/graphic.h src/lmp_struct.c src/lmp_struct.h
	cd src && $(CC) -c vmemory.c $(CFLAGS) $(LUA_CFLAGS)

src/graphic.o: src/gsdl.c src/graphic.h
	cd src && $(CC) -c gsdl.c -o graphic.o $(CFLAGS) $(SDL_CFLAGS)

src/lmp_struct.o: src/lmp_struct.c src/lmp_struct.h
	cd src && $(CC) -c lmp_struct.c $(CFLAGS) $(LUA_CFLAGS)

clean:
	rm src/*.o

test:
	./run.sh

