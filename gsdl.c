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
#include "graphic.h"

/* GLOBAL VARIABLES */
static TTF_Font *font;
static SDL_Colour fontcolor = {0,0,0};
static Uint32 color;
static int gr_textwidth;
static int gr_textheight;

/* STATIC FUNCTIONS */
static TTF_Font* loadfont(char* file, int ptsize); 

/* GLOBAL FUNCTIONS */
Screen *gr_newscreen(int width,int height,const char *icon,const char *title) {
  int depth = 32;
  SDL_Surface *screen;
  Uint32 vmode = SDL_HWSURFACE | SDL_ANYFORMAT | SDL_RESIZABLE |
                                 SDL_DOUBLEBUF | SDL_HWPALETTE;
  
  if (SDL_Init(SDL_INIT_VIDEO) == -1)
    return NULL;

  SDL_WM_SetIcon(SDL_LoadBMP(icon), NULL);
  screen = SDL_SetVideoMode(width, height, depth, vmode);
  if(!screen){
    printf("Unable to set video mode: %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_WM_SetCaption(title, 0);

  if (TTF_Init() == -1) {
    printf("Unable to initialize SDL_ttf(fonts): %s \n", TTF_GetError());
    exit(EXIT_FAILURE);
  }

  font = loadfont("FreeMono.ttf", FONT_SIZE);

  return (Screen*) screen;
}

void gr_destroyscreen (Screen *screen) {
  TTF_Quit();
  SDL_Quit();
}

/* DRAW FUNCTIONS */

/* Bresenham's line_algorithm */
void gr_drawline (Screen *screen, int x0, int y0, int x1, int y1) {
  int pos, dx, dy, sx, sy, err;
  SDL_Surface *sdl_screen = (SDL_Surface*) screen;

  dx = abs(x1 - x0);
  dy = abs(y1 - y0);

  if (x0 < x1) sx = 1; else sx = -1;
  if (y0 < y1) sy = 1; else sy = -1;

  err = dx-dy;

  while(x0 != x1 || y0 != y1) {
    int pos = (sdl_screen->w * y0) + x0;
    int e2 = err + err;
    ((Uint32 *) sdl_screen->pixels)[pos] = color;
    if (e2 > -dy) {
      err = err - dy;
      x0 = x0 + sx;
    }
    if (e2 < dx) {
      err = err + dx;
      y0 = y0 + sy;
    }
  }
  /* draw last pixel */
  pos = (sdl_screen->w * y0) + x0;
  ((Uint32 *) sdl_screen->pixels)[pos] = color;

  SDL_Flip(sdl_screen);
}

void gr_drawblock (Screen *screen, int x0 , int x1 , int y, int blockheight) {
  SDL_Surface *sdl_screen = (SDL_Surface*) screen;
  int i, pos = (sdl_screen->w * y) + x0;
  int dx = x1 - x0 + 1;  /* +1 -> draw x0 AND x1 */
  int y1 = y + blockheight;  /* draw y but not y1 */

  while(y < y1) {
    for (i = x0; i <= x1; i++, pos++) {
      ((Uint32 *) sdl_screen->pixels)[pos] = color;
    }
    pos = pos + sdl_screen->w - dx;  /* go to the beginning of the next line */
    y++;
  }
  SDL_Flip(screen);
}

void gr_drawtext(Screen *screen, const char *text, int x, int y) {
  SDL_Surface *tsurface = TTF_RenderText_Solid(font, text, fontcolor);
  SDL_Rect rect;
  rect.x = x;
  rect.y = y;

  if(!tsurface) {
    printf("Unable to write text: %s \n", TTF_GetError());
    exit(EXIT_FAILURE);
  } else {
    SDL_BlitSurface(tsurface, NULL, screen, &rect);
    SDL_Flip(screen);
    SDL_FreeSurface(tsurface);
  }
}

void gr_drawbackground(Screen *screen, Color clr) {
  SDL_Surface *sdl_screen = (SDL_Surface*) screen;
  color = SDL_MapRGB(sdl_screen->format, getRed(clr), getGreen(clr),
                                                               getBlue(clr));
  SDL_FillRect(sdl_screen, &sdl_screen->clip_rect, color);
  SDL_Flip(sdl_screen);
}

/* SET FUNCTIONS */
void gr_setdrawcolor(Screen *screen, Color clr) {
  SDL_Surface *sdl_screen = (SDL_Surface*) screen;
  color = SDL_MapRGB(sdl_screen->format, getRed(clr), getGreen(clr),
                                                               getBlue(clr));
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

