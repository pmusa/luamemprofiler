/*
**
** Author: Pablo Musa
** Creation Date: mar 27 2011
** Last Modification: aug 22 2011
** See Copyright Notice in COPYRIGHT
**
** This is the header file of the drawing module. There is no graphic tool
** attached to our implementation. One can choose any graphic library to
** implement the graphic module. But one must implement all functions of
** this file and respect their contracts.
** If you are going to implement the graphic module using another graphic
** tool, please pay attention to the functions definitions.
**
*/


#ifndef LMP_GRAPHIC_H
#define LMP_GRAPHIC_H


#define LMP_EVENT_EMPTY 0
#define LMP_EVENT_KEY 1
#define LMP_EVENT_MOUSE 2
#define LEFT_BUTTON 1
#define RIGHT_BUTTON 3

#define FONT_SIZE 16

#define mkColor(r,g,b)	((r) | ((g) << 8) | ((b) << 16))

#define getRed(c)		((c) & 0xFF)
#define getGreen(c)		(((c) & 0xFF00) >> 8)
#define getBlue(c)		(((c) & 0xFF0000) >> 16)

#define DARKBLUE      mkColor(0, 0, 139)
#define DARKRED	      mkColor(139, 0, 0)
#define DARKGREEN     mkColor(0, 100, 0)
#define DARKORANGE    mkColor(255, 140, 0)
#define DARKMAGENTA   mkColor(139, 0, 139)
#define DIMGRAY       mkColor(105, 105, 105)
#define LTWHITE	      mkColor(240, 240, 240)
#define LTGRAY        mkColor(211, 211, 211)
#define WHITE         mkColor(255, 255, 255)
#define BLACK         mkColor(0, 0, 0)
#define RED           mkColor(255, 0, 0)


struct kevent {
  int key;
};

struct mevent {
  int x, y, b;
};

union LMP_event {
  struct kevent kevent;
  struct mevent mevent;
};
typedef union LMP_event LMP_Event;

/*
** There are some available colors predefined in the beginning of this file.
** Use the macros: get(Red, Green, Blue) to map from Color to R, G or B.
*/
typedef long Color;

typedef void Screen;
typedef void Font;

/*
** Create a new window with the specified width, height, icon and title.
** This function must load a base font if the graphic tool does not have
** one by default. This font will be used by other functions.
** Returns the screen pointer.
*/
Screen *gr_newscreen (int width,int height,const char *icon,const char *title);

/*
** Destroy the window referenced by the screen pointer.
** This function must unload the font if any font was loaded in gr_newscreen.
*/
void gr_destroyscreen (Screen *screen);

/*
** Draw a line in the window from (x0,y0) to (x1, y1).
*/
void gr_drawline (Screen *screen, int x0 , int y0 , int x1, int y1 );

/*
** Draw an horizontal block from (x0,y) to x1 with blockheight.
*/
void gr_drawblock (Screen *screen, int x0 , int x1 , int y, int blockheight);

/*
** Write the specified text beginning at (x,y).
*/
void gr_drawtext (Screen *screen, const char *text, int x, int y);

/*
** Paint all window with the specified color.
** See Color typedef (beginning of this file) for more details about Color.
*/
void gr_drawbackground (Screen *screen, Color color);

/*
** Set the specified color for any following draw.
** See Color typedef (beginning of this file) for more details about Color.
*/
void gr_setdrawcolor(Screen *screen, Color color);

/*
** Set the specified color for any following text.
** See Color typedef (beginning of this file) for more details about Color.
*/
void gr_settextcolor (Screen *screen, Color color);

/*
** Return the width of one text character in pixels.
*/
int gr_gettextwidth(Screen *screen);

/*
** Return the height of one text character in pixels.
*/
int gr_gettextheight(Screen *screen);

/*
** Search for a keyboard or mouse event until queue is empty.
** Return an int containing the event type (mouse, keyboard or empty).
** This function removes all events from the top of the queue that are not
** keydown or mousebuttondown events.
*/
int gr_getevent(Screen *screen, LMP_Event *event);

/*
** Wait until a keyboard or mouse event occur.
** Returns an int containing the event type (mouse, keyboard or empty).
** This function discards all events from the top of the queue that are not
** keydown or mousebuttondown events.
*/
int gr_waitevent(Screen *screen, LMP_Event *event);

#endif
