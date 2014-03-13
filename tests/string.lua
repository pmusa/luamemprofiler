-- Author: Pablo Musa
-- Creation Date: jun 20 2011
-- Last Modification: aug 09 2011

local lmp = require"luamemprofiler"

-- pre-defined strings do not use malloc
local a = ""
local b = "t"
local c = "a"
local d = "tamanho8"
local e = "tamanhotamanhotamanhotamanhotamanhotamanhotamanhotamanhotamanho65"

lmp.start(...)
assert((a..b))
lmp.stop()

lmp.start(...)
assert((b..c))
lmp.stop()

lmp.start(...)
assert((b..c..d))
lmp.stop()

lmp.start(...)
assert((b..c..d..e))
lmp.stop()
