#
# Author: Pablo Musa
# Creation Date: mar 27 2011
# Last Modification: aug 22 2011
# See Copyright Notice in COPYRIGHT
# 
# luamemprofiler - A Memory Profiler for the Lua language
#

SDL_CFLAGS = -I/usr/include/SDL -D_GNU_SOURCE=1 -D_REENTRANT
SDL_LDFLAGS = -L/usr/lib -lSDL -lSDL_ttf
LUA_DIR = /home/pablomusa/aulas/2011.1/PFP/lua5.2/include

CC = gcc

# compilation should generate Dynamic-Link Libraries
CFLAGS = -g -Wall -O0 -ansi -pedantic
LFLAGS = -I$(LUA_DIR) -fpic -shared

all: luamemprofiler.so

luamemprofiler.so: graphic.o lmp_struct.o vmemory.o lmp.o luamemprofiler.o
	$(CC) $(CFLAGS) $(LFLAGS) $(SDL_CFLAGS) $(SDL_LDFLAGS) graphic.o lmp_struct.o vmemory.o lmp.o luamemprofiler.o -o luamemprofiler.so

luamemprofiler.o: luamemprofiler.c lmp.h
	$(CC) $(CFLAGS) $(LFLAGS) -c luamemprofiler.c

lmp.o: lmp.c lmp.h lmp_struct.h vmemory.h
	$(CC) $(CFLAGS) -c lmp.c

lmp_struct.o: lmp_struct.c lmp_struct.h
	$(CC) $(CFLAGS) -c lmp_struct.c

vmemory.o: vmemory.c lmp.h vmemory.h graphic.h
	$(CC) $(CFLAGS) -c vmemory.c

graphic.o: gsdl.c graphic.h
	$(CC) $(CFLAGS) $(SDL_CFLAGS) $(SDL_LDFLAGS) -c gsdl.c -o graphic.o

clean:
	rm *.o

test:
	./run.sh

