/*
**
** Author: Pablo Musa
** Creation Date: mar 27 2011
** Last Modification: aug 22 2011
** See Copyright Notice in COPYRIGHT
**
** See vmemory.h for module overview
**
*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <lualib.h>
#include <math.h>
#include <stdint.h>


#include "graphic.h"
#include "vmemory.h"
#include "lmp_struct.h"
#include "lmp.h"


#define WINDOW_TITLE "luamemprofiler v1.0"
#define ICON_PATH "logo.bmp"

/* screen state */
#define LMP_PAUSE 0
#define LMP_EXEC 1
#define LMP_FINISH -1
#define LMP_ZOOM_OUT 0
#define LMP_ZOOM_IN 1

#define MIN_mb_SIZE 1
#define MIN_mb_WIDTH 800
#define MIN_mb_HEIGHT 600

#define BOX_XINI 10  /* MINIMUM = 10 */
#define BOX_YINI 10  /* MINIMUM = 10 */
#define BOX_XEND (mb_width + BOX_XINI)
#define BOX_YEND (mb_height + BOX_YINI)

#define RTCOLUMN_WIDTH 150  /* right column width */
#define BTROW_HEIGHT 70  /* bottom row height */

#define BASE_SPACE 10  /* space for separating text from the box */
#define BOX_BORDER 3   /* memory box border width */

#define LMP_FLINE 0  /* bottom row starts writing in first line (0) */
#define LMP_ON 1
#define LMP_OFF 0


/* filter type menu - positions to draw, bool toggled, draw Color, and text */
struct menuitem {
  int x;
  int y;
  int toggle;
  Color color;
  const char *name;
};
typedef struct menuitem LMP_Menuitem;


/* GLOBAL VARIABLES */
/* one block list for each filter type */
extern lmp_Block *lmp_string;
extern lmp_Block *lmp_function;
extern lmp_Block *lmp_userdata;
extern lmp_Block *lmp_thread;
extern lmp_Block *lmp_table;
extern lmp_Block *lmp_other;
extern lmp_Block *lmp_all;

/* one menu for each filter type */
static LMP_Menuitem mi_string;
static LMP_Menuitem mi_function;
static LMP_Menuitem mi_userdata;
static LMP_Menuitem mi_thread;
static LMP_Menuitem mi_table;
static LMP_Menuitem mi_other;

/* STATIC GLOBAL VARIABLES */
static Screen *screen;
static uintptr_t laddress;  /* first address of the program */
static uintptr_t baseaddr;  /* base address of the memory box */
static int state = LMP_PAUSE;  /* luamemprofiler state (paused or executing) */
static int zoom = LMP_ZOOM_OUT;  /* zoom state (in or out) */

static int sc_width;  /* sc = screen */
static int sc_height;
static int mb_width;  /* mb = memory box */
static int mb_height;
static int BYTES_PER_PIXEL = 4;
static int BLOCK_HEIGHT = 2;

static int statesy;  /* used to update the display with state and zoom */

/* STATIC FUNCTIONS */ 
static void drawbtrow();
static void toggleall();
static void drawstates();
static void drawmembox();
static void checkevent();
static void clearmembox(); 
static void untoggleall();
static int istoggled(size_t luatype);
static void drawcallstack(int fline);
static void drawrtcolumn(float memused);
static int setcanvassize (float memused);
static void inverttoggle(LMP_Menuitem *mi);
static void drawmenuitem(LMP_Menuitem *item);
static void drawreport(const char *text, int line);
static void drawmemblock(int addr, size_t luatype, size_t size);
static void writeblockinfo(void *ptr,size_t luatype,size_t size,int alloctype);
static void calcmemdata(void *ptr, size_t size, int *reladdr, size_t *relsize);
static void initmenuitem(LMP_Menuitem *mi, int x, int y, int toggle,
                                           Color color, const char* name); 

/* GLOBAL FUNCTIONS */
void vm_start(int lowestaddress, float memused) {
  /* if memused is very low uses default value and returns it */
  memused = setcanvassize (memused);
  screen = gr_newscreen(sc_width, sc_height, ICON_PATH, WINDOW_TITLE);
  laddress = lowestaddress;
  baseaddr = laddress;

  gr_drawbackground(screen, LMP_VM_BACKGROUND_CL);
  drawmembox();
  drawrtcolumn(memused);
  drawbtrow();

  checkevent();
}

void vm_stop() {
  char dummy;
  state = LMP_FINISH;
  drawreport("Execution finished. The report is in the Terminal.", LMP_FLINE);
  drawstates();
  printf("Press Enter To Finish!");
  scanf("%c", &dummy);

  gr_destroyscreen(screen);
}

void vm_newmemop(int memop, void *ptr, size_t luatype, size_t size) {
  int p;
  size_t mb_size;
  calcmemdata(ptr, size, &p, &mb_size);  /* calculate relative address */
  if (p > 0) {  /* check if is a valid address */
    /* draw full block breaking lines if necessary */
    if (istoggled(luatype))
      drawmemblock(p, luatype, mb_size);
    if (state == LMP_PAUSE) {
      /* write in 'bottom row' block information(parameters) and call stack */
      writeblockinfo(ptr, luatype, size, memop);
    }
    /* check if there is any valid event and treat it */
    checkevent();
  }
  /* DEBUG else { possible print if block is smaller than baseaddress
    printf("Lower Pointer ptr=%d laddr=%d  p=%d\n", (int) ptr, laddress, p);
  } */
}

/* STATIC FUNCTIONS */

/* uses baseaddress to calculate the memory box position of a block */
static void calcmemdata(void *ptr, size_t size, int *reladdr, size_t *relsize) {
  *reladdr = ((uintptr_t) ptr - baseaddr) / BYTES_PER_PIXEL;
  *relsize = (size / BYTES_PER_PIXEL);
  if (*relsize == 0) {
    *relsize = 1;
  }
}

/* draw specified text in report area (botton row) */
static void drawreport(const char *text, int line) {
  int x = BOX_XINI;
  int y = BOX_YEND + BASE_SPACE + (line * gr_gettextheight(screen));

  /* if line does not fit screen size, omit line */
  if (y > sc_height - gr_gettextheight(screen)) {
    return;
  }

  /* if line is first line, new message -> clear bottom row */ 
  if (line == LMP_FLINE) {
    gr_setdrawcolor(screen, LMP_VM_BACKGROUND_CL);
    gr_drawblock(screen, BOX_XINI, BOX_XEND, BOX_YEND+BASE_SPACE, BTROW_HEIGHT);
  }
  
  /* validate text size */
  if (strlen(text) > (mb_width / gr_gettextwidth(screen))) {
    gr_drawtext(screen, "Sorry, but the text is too large.", x, y);
  } else {
    gr_drawtext(screen, text, x, y);
  }
}

/* draw up to 3 levels of the call stack */
static void drawcallstack(int fline) {
  char textbuff[80] = "";
  int res, i = 0;
  lua_Debug ld;
  lua_State *L = (lua_State *) laddress;
  res = lua_getstack(L, i, &ld);
  while (res == 1 && i < (BTROW_HEIGHT/gr_gettextwidth(screen))) {
    int r;
    r = lua_getinfo(L, "lnS", &ld);
    if (r != 0) {
      int line = LMP_FLINE + i + fline;  /* start in fline */
      if (strcmp(ld.what, "main") == 0) {  /* main program execution */
        sprintf(textbuff, "Main - line:%d", ld.currentline);
        drawreport(textbuff, line);
        break;
      }
      if (strcmp(ld.what, "Lua") == 0) {  /* some function execution */
        sprintf(textbuff, "Lua - file:'%s' func:'%s' field:'%s' line:%d",
                           ld.short_src, ld.name, ld.namewhat, ld.currentline);
        drawreport(textbuff, line);
      }
      if (strcmp(ld.what, "C") == 0) {  /* some C function execution */
        sprintf(textbuff, "C - func'%s' field:'%s'", ld.name, ld.namewhat);
        drawreport(textbuff, line);
      }
    } else {
      printf("luamemprofiler internal error: vmemory -> debuginfo -> invalid what.\n");
      exit(EXIT_FAILURE);
    }
    i++;
    res = lua_getstack(L, i, &ld);
  }
}

/* draw blocks in the correct place (recursive calls for big blocks) */
static void drawmemblock(int addr, size_t luatype, size_t size) {
  int x, y;
  x = (addr % mb_width) + BOX_XINI;
  y = ((addr / mb_width) * BLOCK_HEIGHT) + BOX_YINI;

  /* just draw into valid areas */
  if (y >= BOX_YINI && (y + BLOCK_HEIGHT) <= BOX_YEND) { 
    if (x + size > BOX_XEND){ /* break block to fit membox */
      size_t extrasize = size - (BOX_XEND + 1 - x);  /* +1 -> XEND is valid */
      size = size - extrasize;

      /* recursive call for breaking one block in different lines */
      drawmemblock(addr + (size - 1), luatype, extrasize);  /* -1 -> XEND */
    }

    switch(luatype) {
      case LUA_TSTRING:
        gr_setdrawcolor(screen, LMP_VM_STRING_CL);
        break;
      case LUA_TFUNCTION:
        gr_setdrawcolor(screen, LMP_VM_FUNCTION_CL);
        break;
      case LUA_TUSERDATA:
        gr_setdrawcolor(screen, LMP_VM_USERDATA_CL);
        break;
      case LUA_TTHREAD:
        gr_setdrawcolor(screen, LMP_VM_THREAD_CL);
        break;
      case LUA_TTABLE:
        gr_setdrawcolor(screen, LMP_VM_TABLE_CL);
        break;
      case LUA_TFREE:
        gr_setdrawcolor(screen, LMP_VM_FREE_CL);
        break;
      default:
        gr_setdrawcolor(screen, LMP_VM_OTHER_CL);
    }

    /* block length is size, so draw from x to "size-1" */
    gr_drawblock(screen, x, x + size - 1, y, BLOCK_HEIGHT);
  }
  /* DEBUG else { possible use when all blocks have to be drawn 
       printf("draw block (%d, %d) addr = %d\n", x, y, addr); } */
}

/* 
** traverse a filter list drawing all blocks. 'block' is the first block in the
** list and 'fnextblock' is a function that returns the next block in the list
** or NULL
*/
void drawblocklist(lmp_Block *block, lmp_Block* (*fnextblock) (lmp_Block*)) {
  int p;
  size_t mb_size;
  while (block != NULL) {  /* list is not empty */
    calcmemdata(block->ptr, st_getsize(block), &p, &mb_size);
    if (p > 0) {
      int luatype = istoggled(block->luatype) ? block->luatype : LUA_TFREE;
      drawmemblock(p, luatype, mb_size);
    }
    block = fnextblock(block);
  }
}

/* 
** redraw blocks using bigger size and new baseaddress. The new base address is
** calculated using 'y' coordinate. All blocks above this 'y' (y included) are
** redrawn.
*/
static void zoomin(int x, int y) {
  int p;
  size_t mb_size;
  lmp_Block *block;

  zoom = LMP_ZOOM_IN;  /* change zoom state */
  drawstates();        /* update display with new state */
  clearmembox();       /* clear memory box for new blocks in zoom mode */

  /* calculates baseaddress using old baseaddress and 'y' */
  baseaddr = (((y - BOX_YINI) / BLOCK_HEIGHT) * (mb_width)
                                          * BYTES_PER_PIXEL) + baseaddr;
  BYTES_PER_PIXEL = BYTES_PER_PIXEL / 2;  /* width 2x bigger */
  BLOCK_HEIGHT = BLOCK_HEIGHT * 2;        /* height 2x bigger */
  for(block = lmp_all; block != NULL; block = st_getnextall(block)) {
    /* calculates new block values in memry box (relative address and size) */
    p = ((uintptr_t) block->ptr - baseaddr) / BYTES_PER_PIXEL;
    mb_size = (block->size / BYTES_PER_PIXEL);
    if (mb_size == 0) {
      mb_size = 1;
    }
    /* check if block is inside zoom */
    if (istoggled(block->luatype) && p >= 0 && 
             p <= (mb_width * BYTES_PER_PIXEL) * (mb_height / BLOCK_HEIGHT)) {
        drawmemblock(p, block->luatype, mb_size);
    }
  }
}

/* redraw blocks using smaller size and old baseaddress. */
static void zoomout() {
  zoom = LMP_ZOOM_OUT;  /* change zoom state */
  drawstates();         /* update display with new state */
  clearmembox();        /* clear memory box for new blocks without zoom mode */

  BYTES_PER_PIXEL = BYTES_PER_PIXEL * 2;  /* width 2x smaller */
  BLOCK_HEIGHT = BLOCK_HEIGHT / 2;        /* height 2x smaller */

  baseaddr = laddress;  /* restore baseaddress */
  drawblocklist(lmp_all, st_getnextall);  /* draw all blocks */
}

static void checkevent() {
  int eventtype;
  LMP_Event event;

  if (state == LMP_EXEC) {  /* normal execution - only accepts pause command */
    eventtype = gr_getevent(screen, &event);  /* gets an event if exists */
    if (eventtype == LMP_EVENT_KEY && event.kevent.key == ' ') {  /* pause */
      state = LMP_PAUSE;
      drawstates();  /* update display with new state */
      drawreport("Press: 'space' to resume execution; 'n' to resume until next memory operation;", LMP_FLINE);
      drawreport("'c' to clear the memory box; 's,f,u,h,t,o' to redraw blocks of specific type;", LMP_FLINE + 1);
      drawreport("'a' to redraw all blocks; left-click for zoom in and right-click for zoom out.", LMP_FLINE + 2);
    }
  }

  while (state != LMP_EXEC) {  /* execution is paused or finished */
    lmp_Block* (*fnextblock) (lmp_Block*) = st_getnexttype;
    lmp_Block *block = NULL;

    eventtype = gr_waitevent(screen, &event);  /* wait for a valid event */
    if (eventtype == LMP_EVENT_KEY) {
      switch (event.kevent.key) {
        case ' ':  /* space key - continue - resumes normal execution */
          state = LMP_EXEC;
          drawstates();
          drawreport("Press 'space' to Pause execution.", LMP_FLINE);
          return;
        case 'n':  /* next - execute next memory operation */
          return;
        case 's':  /* draw filter type - set correct list and toggle */
          inverttoggle(&mi_string);
          block = lmp_string;
          break;
        case 'f':
          inverttoggle(&mi_function);
          block = lmp_function;
          break;
        case 'u':
          inverttoggle(&mi_userdata);
          block = lmp_userdata;
          break;
        case 'h':
          inverttoggle(&mi_thread);
          block = lmp_thread;
          break;
        case 't':
          inverttoggle(&mi_table);
          block = lmp_table;
          break;
        case 'o':
          inverttoggle(&mi_other);
          block = lmp_other;
          break;
        case 'a':  /* draw all blocks */
          toggleall();
          block = lmp_all;
          fnextblock = st_getnextall;
          break;
        case 'c':  /* erase all blocks from memory box */
          untoggleall();
          clearmembox();
      }
      drawblocklist(block, fnextblock);
    } else if (eventtype == LMP_EVENT_MOUSE) {
      if (event.mevent.b == LEFT_BUTTON && zoom == LMP_ZOOM_OUT) {
        zoomin(event.mevent.x, event.mevent.y);
      } else if (event.mevent.b == RIGHT_BUTTON && zoom == LMP_ZOOM_IN) {
        zoomout();
      }
    }
  }
}

/*
** write in bottom row the memory operation and the block info.
** block info = allocation type, block (address, type and size) and call stack
*/
static void writeblockinfo(void *ptr, size_t luatype, size_t size, int alloctype) {
  char textbuff[60];
  char ltype[9];
  char atype[8];
  switch(luatype) {
    case LUA_TSTRING:
      strcpy(ltype, "String");
      break;
    case LUA_TFUNCTION:
      strcpy(ltype, "Function");
      break;
    case LUA_TUSERDATA:
      strcpy(ltype, "Userdata");
      break;
    case LUA_TTHREAD:
      strcpy(ltype, "Thread");
      break;
    case LUA_TTABLE:
      strcpy(ltype, "Table");
      break;
    default:
      strcpy(ltype, "Other");
      break;
  }
  switch(alloctype) {
    case LMP_VM_FREE:
      strcpy(atype, "Free");
      break;
    case LMP_VM_MALLOC:
      strcpy(atype, "Malloc");
      break;
    case LMP_VM_REALLOC:
      strcpy(atype, "Realloc");
      break;
  }

  sprintf(textbuff, "%s | addr = %p | type = %s | size = %luB", atype, 
                                      ptr, ltype, (unsigned long) size);
  drawreport(textbuff, LMP_FLINE);
  drawcallstack(LMP_FLINE + 1);
}

/* draw border lines and clear memory box */
static void drawmembox() {
  int i;

  gr_setdrawcolor(screen, BLACK);
  for (i = 1; i <= BOX_BORDER; i++) {
    int x1 = BOX_XINI - i, x2 = BOX_XEND + i;
    int y1 = BOX_YINI - i, y2 = BOX_YEND + i;
    gr_drawline(screen, x1, y1, x2, y1);
    gr_drawline(screen, x1, y2, x2, y2);
    gr_drawline(screen, x1, y1, x1, y2);
    gr_drawline(screen, x2, y1, x2, y2);
  }
  clearmembox();
}

/* draw one item of the right column */
static void drawmenuitem(LMP_Menuitem *item) {
  int offset = gr_gettextwidth(screen) * strlen(item->name) + 3;
  int bx = item->x + offset;
  int by = item->y + 4;
  gr_settextcolor(screen, BLACK);
  gr_drawtext(screen, item->name, item->x, item->y);
  if (item->toggle) {
    gr_setdrawcolor(screen, item->color);
    gr_drawblock(screen, bx, bx + 10, by, 10);
  } else {
    gr_setdrawcolor(screen, LMP_VM_BACKGROUND_CL);
    gr_drawblock(screen, bx, bx + 10, by, 10);
    
    gr_setdrawcolor(screen, item->color);
    gr_drawline(screen, bx, by, bx + 10, by);
    gr_drawline(screen, bx, by, bx, by + 10);
    gr_drawline(screen, bx, by+ 10, bx + 10, by + 10);
    gr_drawline(screen, bx + 10, by, bx + 10, by + 10);
  }
}

/* write a line division(black) and a label(red) in (x,y) coordinate */
static int drawl(const char *text, int x, int y) {
  int text_width = strlen(text) * gr_gettextwidth(screen);
  int center_offset = (RTCOLUMN_WIDTH - (text_width + BASE_SPACE))/2;

  gr_setdrawcolor(screen, BLACK);
  gr_drawline(screen, x, y, sc_width - BASE_SPACE, y);
  y = y + 10;

  gr_settextcolor(screen, RED);
  gr_drawtext(screen, text, x + center_offset, y);
  return y;
}

/* write initial message in the bottom row */
static void drawbtrow() {
  gr_settextcolor(screen, BLACK);
  drawreport("Welcome to the luamemprofiler library. Press 'space' to run the program", LMP_FLINE);
  drawreport("normally or 'n' to execute the program until next memory operation.", LMP_FLINE + 1);
}

/* draw right column, including initial states (paused, zoom in) */
static void drawrtcolumn(float memused) {
  int x = BOX_XEND + BOX_BORDER + BASE_SPACE;
  int y = BOX_YINI;
  int offset = 30;
  char textbuff[15];

  /* draw basic information */
  y = drawl("BASIC INFO", x, y);
  y = y + offset;

  gr_settextcolor(screen, BLACK);
  gr_drawtext(screen, "Memory Size", x, y);
  y = y + gr_gettextwidth(screen) + 5;
  sprintf(textbuff, "%.1fMb", memused);
  gr_drawtext(screen, textbuff, x, y);
  y = y + offset;

  gr_drawtext(screen, "Granularity", x, y);
  y = y + gr_gettextwidth(screen) + 5;
  sprintf(textbuff, "1x2 px = %dB", BYTES_PER_PIXEL);
  gr_drawtext(screen, textbuff, x, y);
  y = y + offset;

  /* draw key menu and labels */
  y = drawl("COMMANDS", x, y);
  y = y + offset;

  gr_settextcolor(screen, BLACK);
  initmenuitem(&mi_string, x, y, LMP_ON, LMP_VM_STRING_CL, "s - String");
  drawmenuitem(&mi_string);
  y = y + offset;
  initmenuitem(&mi_function, x, y, LMP_ON, LMP_VM_FUNCTION_CL, "f - Function");
  drawmenuitem(&mi_function);
  y = y + offset;
  initmenuitem(&mi_userdata, x, y, LMP_ON, LMP_VM_USERDATA_CL, "u - Userdata");
  drawmenuitem(&mi_userdata);
  y = y + offset;
  initmenuitem(&mi_thread, x, y, LMP_ON, LMP_VM_THREAD_CL, "h - Thread");
  drawmenuitem(&mi_thread);
  y = y + offset;
  initmenuitem(&mi_table, x, y, LMP_ON, LMP_VM_TABLE_CL, "t - Table");
  drawmenuitem(&mi_table);
  y = y + offset;
  initmenuitem(&mi_other, x, y, LMP_ON, LMP_VM_OTHER_CL, "o - Other");
  drawmenuitem(&mi_other);
  y = y + offset;
  gr_drawtext(screen, "a - All", x, y);
  y = y + offset;
  gr_drawtext(screen, "c - Clear", x, y);
  y = y + offset;
  gr_drawtext(screen, "n - Next", x, y);
  y = y + offset;

  /* draw luamemprofiler state and zoom state */
  y = drawl("STATE", x, y);
  y = y + offset;

  statesy = y; /* set where to redraw states */
  gr_settextcolor(screen, BLACK);
  gr_drawtext(screen, "lmp: PAUSED", x, y);
  y = y + offset;
  gr_drawtext(screen, "zoom: OUT", x, y);
}

/* update states (pause x execution || [zoom] in x out */
static void drawstates() {
  int x = BOX_XEND + BOX_BORDER + BASE_SPACE;
  int y = statesy;
  int offset = 30;

  /* erase old text */
  gr_setdrawcolor(screen, LMP_VM_BACKGROUND_CL);
  gr_drawblock(screen, x, x + RTCOLUMN_WIDTH, y, 50);

  /* write new text */
  gr_settextcolor(screen, BLACK);
  if(state == LMP_PAUSE) {
    gr_drawtext(screen, "lmp: PAUSED", x, y);
  } else if (state == LMP_EXEC) {
    gr_drawtext(screen, "lmp: EXECUTING", x, y);
  } else if (state == LMP_FINISH) {
    gr_drawtext(screen, "lmp: FINISHED", x, y);
  }
  y = y + offset;

  if(zoom == LMP_ZOOM_OUT) {
    gr_drawtext(screen, "zoom: OUT", x, y);
  } else if(zoom == LMP_ZOOM_IN) {
    gr_drawtext(screen, "zoom: IN", x, y);
  }
 
}

/* calculates and sets screen and memory box width and height */
static int setcanvassize (float memused) {
  if (memused <= (float) MIN_mb_SIZE) {
    mb_width = MIN_mb_WIDTH;
    mb_height = MIN_mb_HEIGHT;
    memused = MIN_mb_SIZE;
  } else  {  /* MAX MEM FOR 800 x 600 resolution */
    int side;
    if (memused > 1) {
      BYTES_PER_PIXEL = ((int) memused) * 4;
    }
    /* (1Mb * BLOCK_HEIGHT / BYTES_PER_PIXEL / PROPORTION(4:3)) */
    side = (int) sqrt(memused*1000000*BLOCK_HEIGHT/BYTES_PER_PIXEL/(4*3))+1;
    mb_width = side * 4;
    mb_height = side * 3;
  }
  sc_width = BOX_XINI + mb_width + BOX_BORDER + BASE_SPACE + RTCOLUMN_WIDTH;
  sc_height = BOX_YINI + mb_height + BOX_BORDER + BASE_SPACE + BTROW_HEIGHT;
  return memused;
}

/* paint all memory box with defined color (erase drawn blocks) */
static void clearmembox() {
  gr_setdrawcolor(screen, LMP_VM_MEMBOX_CL);
  gr_drawblock(screen, BOX_XINI, BOX_XEND, BOX_YINI, BOX_YEND - BOX_YINI); 
}

static int istoggled(size_t luatype) {
  switch(luatype) {
    case LUA_TSTRING:
      return mi_string.toggle;
    case LUA_TFUNCTION:
      return mi_function.toggle;
    case LUA_TUSERDATA:
      return mi_userdata.toggle;
    case LUA_TTHREAD:
      return mi_thread.toggle;
    case LUA_TTABLE:
      return mi_table.toggle;
    default:
      return mi_other.toggle;
  }
}

static void toggleall() {
  mi_string.toggle = LMP_ON;
  drawmenuitem(&mi_string);
  mi_function.toggle = LMP_ON;
  drawmenuitem(&mi_function);
  mi_userdata.toggle = LMP_ON;
  drawmenuitem(&mi_userdata);
  mi_thread.toggle = LMP_ON;
  drawmenuitem(&mi_thread);
  mi_table.toggle = LMP_ON;
  drawmenuitem(&mi_table);
  mi_other.toggle = LMP_ON;
  drawmenuitem(&mi_other);
}

static void untoggleall() {
  mi_string.toggle = LMP_OFF;
  drawmenuitem(&mi_string);
  mi_function.toggle = LMP_OFF;
  drawmenuitem(&mi_function);
  mi_userdata.toggle = LMP_OFF;
  drawmenuitem(&mi_userdata);
  mi_thread.toggle = LMP_OFF;
  drawmenuitem(&mi_thread);
  mi_table.toggle = LMP_OFF;
  drawmenuitem(&mi_table);
  mi_other.toggle = LMP_OFF;
  drawmenuitem(&mi_other);
}

static void initmenuitem(LMP_Menuitem *mi, int x, int y, int toggle,
                                           Color color, const char* name) {
  mi->x = x; mi->y = y; mi->toggle = toggle;
  mi->color = color; mi->name = name;
}

/* change toggle settings and redraw menu item */
static void inverttoggle(LMP_Menuitem *mi) {
  mi->toggle = !mi->toggle;
  drawmenuitem(mi);
}
