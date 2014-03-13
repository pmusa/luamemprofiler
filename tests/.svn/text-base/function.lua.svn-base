-- Author: Pablo Musa
-- Creation Date: jun 20 2011
-- Last Modification: aug 09 2011

local lmp = require"luamemprofiler"

lmp.start(...)
local function x() end
lmp.stop()

lmp.start(...)
local function x() end
local function y() end
lmp.stop()

lmp.start(...)
local function x() print("teste") end
local function y() end
lmp.stop()

lmp.start(...)
local function x() print("teste"); print("funcao");end
local function y() end
lmp.stop()

lmp.start(...)
local function x() print("teste"); end
local function y() x(); end
lmp.stop()

lmp.start(...)
local function x() print("teste"); end
local function y() print("teste"); y(); end
lmp.stop()

lmp.start(...)
local function x() print("teste"); end
local function y() print("teste"); y(); x(); end
lmp.stop()

