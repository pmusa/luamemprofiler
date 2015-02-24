/*
**
** Author: Pablo Musa
** Creation Date: mar 27 2011
** Last Modification: aug 22 2011
** See Copyright Notice in COPYRIGHT
**
** This file contains the implementation of the graphic module (graphic.h)
** using the SDL graphic toolkit and the SDL_ttf library.
** See graphic.h for more details about the functions
*/

#include <SDL.h>
#include <SDL_ttf.h>
#include <string.h>
#include "graphic.h"

/* GLOBAL VARIABLES */
static TTF_Font *font;
static SDL_Colour fontcolor = {0,0,0};
static int gr_textwidth;
static int gr_textheight;
static int timer;

/* STATIC FUNCTIONS */
static TTF_Font* loadfont(char* file, int ptsize); 

/* GLOBAL FUNCTIONS */
Screen *gr_newscreen(int width,int height,const char *icon,const char *title) {
  SDL_Window *screen;
  SDL_Renderer *renderer;

  Uint32 vmode = SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_RESIZABLE;
                                            /*SDL_WINDOW_OPENGL;*/
  
  if (SDL_Init(SDL_INIT_VIDEO) == -1)
    return NULL;

  screen = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, vmode);
  if(!screen) {
    printf("Unable to set video mode: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  SDL_SetWindowIcon(screen, SDL_LoadBMP(icon));

  renderer = SDL_CreateRenderer(screen, -1, 0);
  if(!renderer) {
    printf("Unable to set renderer: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  if (TTF_Init() == -1) {
    printf("Unable to initialize SDL_ttf(fonts): %s \n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  font = loadfont("/usr/share/fonts/truetype/freefont/FreeMono.ttf", FONT_SIZE);
  timer = 0;

  return (Screen*) renderer;
}

void gr_destroyscreen (Screen *screen) {
  TTF_Quit();
  SDL_Quit();
}

/* DRAW FUNCTIONS */
void gr_drawline (Screen *screen, int x0, int y0, int x1, int y1) {
  SDL_Renderer *sdl_screen = (SDL_Renderer*) screen;
  SDL_RenderDrawLine(sdl_screen, x0, y0, x1, y1);
  SDL_RenderPresent(sdl_screen);
}

void gr_drawblock (Screen *screen, int x0 , int x1 , int y, int blockheight) {
  Uint32 time = SDL_GetTicks();
  SDL_Renderer *sdl_screen = (SDL_Renderer*) screen;
  SDL_Rect rect;
  rect.x = x0;
  rect.y = y;
  rect.w = x1 - x0 + 1;  /* +1 -> draw x0 AND x1 */
  rect.h = blockheight;
  SDL_RenderFillRect(sdl_screen, &rect);
  if (time - timer > 60) {
    SDL_RenderPresent(sdl_screen);
  }
}

void gr_drawtext(Screen *screen, const char *text, int x, int y) {
  SDL_Renderer *sdl_screen = (SDL_Renderer*) screen;
  SDL_Surface *tsurface;
  SDL_Texture *ttexture;
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;
  rect.w = strlen(text) * gr_textwidth;
  rect.h = gr_textheight;

  tsurface = TTF_RenderText_Solid(font, text, fontcolor);
  if(!tsurface) {
    printf("Unable to create text surface: %s \n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  ttexture = SDL_CreateTextureFromSurface(sdl_screen, tsurface);
  SDL_FreeSurface(tsurface);
  if(!ttexture) {
    printf("Unable to create text texture: %s \n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_RenderCopy(sdl_screen, ttexture, NULL, &rect);
  SDL_RenderPresent(sdl_screen);
}

void gr_drawbackground(Screen *screen, Color clr) {
  SDL_Renderer *sdl_screen = (SDL_Renderer*) screen;
  SDL_SetRenderDrawColor(sdl_screen, getRed(clr), getGreen(clr), getBlue(clr), SDL_ALPHA_OPAQUE);
  SDL_RenderClear(sdl_screen);
  SDL_RenderPresent(sdl_screen);
}

/* SET FUNCTIONS */
void gr_setdrawcolor(Screen *screen, Color clr) {
  SDL_Renderer *sdl_screen = (SDL_Renderer*) screen;
  SDL_SetRenderDrawColor(sdl_screen, getRed(clr), getGreen(clr), getBlue(clr), SDL_ALPHA_OPAQUE);
}

void gr_settextcolor (Screen *screen, Color clr) {
  fontcolor.r = getRed(clr);
  fontcolor.g = getGreen(clr);
  fontcolor.b = getBlue(clr);
}

/* GET FUNCTIONS */
int gr_gettextwidth(Screen *screen) {
  return gr_textwidth;
}

int gr_gettextheight(Screen *screen) {
  return gr_textheight;
}

/* EVENT FUNCTIONS */
int gr_getevent(Screen *screen, LMP_Event *event) {
  int res;
  SDL_Event sdlevent;

  /* search for a keyboard or mouse event until list is empty */
  res = SDL_PollEvent(&sdlevent);
  while (res == 1) {
    if (sdlevent.type == SDL_KEYDOWN) {
      event->kevent.key = sdlevent.key.keysym.sym;
      return LMP_EVENT_KEY;
    } else if (sdlevent.type == SDL_MOUSEBUTTONDOWN) {
      event->mevent.x = sdlevent.button.x;
      event->mevent.y = sdlevent.button.y;
      return LMP_EVENT_MOUSE;
    }
    res = SDL_PollEvent(&sdlevent);
  }

  /* no more events in the list */
  return LMP_EVENT_EMPTY;
}

int gr_waitevent(Screen *screen, LMP_Event *event) {
  int res;
  SDL_Event sdlevent;

  /* wait until a keyboard or mouse event occur. discard other events */
  res = SDL_WaitEvent(&sdlevent);
  while (res == 1) {
    if (sdlevent.type == SDL_KEYDOWN) {
      event->kevent.key = sdlevent.key.keysym.sym;
      return LMP_EVENT_KEY;
    } else if (sdlevent.type == SDL_MOUSEBUTTONDOWN) {
      event->mevent.b = sdlevent.button.button;  /* 1 - Left | 3 - Right */
      event->mevent.x = sdlevent.button.x;
      event->mevent.y = sdlevent.button.y;
      return LMP_EVENT_MOUSE;
    }
    res = SDL_WaitEvent(&sdlevent);
  }

  printf("Unable to wait for an event!\n");
  exit(EXIT_FAILURE);
}

/* STATIC FUNCTIONS */
static TTF_Font* loadfont(char* file, int ptsize) {
  TTF_Font *tmpfont = TTF_OpenFont(file, ptsize);
  if (tmpfont == NULL){
    printf("Unable to load font: %s %s \n", file, TTF_GetError());
    exit(EXIT_FAILURE);
  }
  TTF_SizeText(tmpfont, "0", &gr_textwidth, &gr_textheight);
  return tmpfont;
}

