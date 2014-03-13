luamemprofiler
==============

A Memory Profiler for the Lua language

* Author: Pablo Musa
* Creation Date: jun 25 2011
* Last Modification: aug 22 2011
* See Copyright Notice in LICENSE

luamemprofiler is a library for the Lua language (version 5.2 or greater)
that collects information about the memory usage of a program throughout its
execution.
The memory profile of the program using our library can be shown graphically in
real time, detailing the different data types being allocated, reallocated or
released.
Our library also provides other useful information, such as the maximum amount
of memory allocated, the amount of each memory operation (malloc, free,
realloc) and the maximum number of each allocated object (function, string,
table, etc.).

Read INSTALL to see the library dependencies.

Use the following command to load the library into a local variable:
local lmp = require"luamemprofiler"

*
* luamemprofiler API
*
-- starts the memory monitor.
-- if the optional parameter is used, the library starts the graphical display
-- and enables different inspection features.
-- optional parameter must be a number.
lmp.start([estimated_memory_use_in_MB])

-- stops the memory monitor.
-- prints a log on the standard output.
-- if the graphical display was used it is then destroyed.
lmp.stop()

*
* luamemprofiler graphical display functionalities
*
During library execution WITH GRAPHICAL DISPLAY, there are two main states
of the luamemprofiler library: PAUSED and EXECUTING.
EXECUTING:
one can see the blocks painted in the screen representing each memory
operation. The following option is enabled:
space - stops program execution and changes the library state to PAUSED.

PAUSED:
one can use keyboard and mouse events to explore memory usage. The following
options are enabled:

space - changes the library state to EXECUTING and resume execution.

n - program executes until next memory operation. The library prints in the
window bottom: the memory operation (malloc, free or realloc), the block
address, the block type (table, string, etc.), the block size and the call
stack (3 levels most).

c - clear the memory box. Note this key must be pressed before one can start
filtering blocks by type. Otherwise the other commands just overdraw blocks.

a - toggle all types and redraw all blocks.

s - String: toggle/untoggle type and draw/erase type blocks.

f - Function: toggle/untoggle type and draw/erase type blocks.

u - Userdata: toggle/untoggle type and draw/erase type blocks.

h - Thread: toggle/untoggle type and draw/erase type blocks.

t - Table: toggle/untoggle type and draw/erase type blocks.

o - Other: toggle/untoggle type and draw/erase type blocks. (these blocks are
internal structures that are not well typed by Lua. For example, table contents
have this type).

Mouse1 - Zoom IN. All blocks that follows 'y' coordinate clicked are redrawn
with bigger size and new baseaddress. One can then opt to use all the keys
enabled (PAUSED and EXECUTING mode).

Mouse2 - Zoom OUT. All blocks are redrawn using the default size and the
default baseaddress.

*
* Some Considerations
*
Calling the start function twice raises an error.

The start can be used in any part of the program, so can the stop function if
it comes after a start call.
The library monitors only the interval between start and stop. Memory operations
done before the start call, or after the stop call are not counted.

Blocks that the address do not fit in the memory box are not painted.

