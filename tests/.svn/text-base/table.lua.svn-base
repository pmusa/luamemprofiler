-- Author: Pablo Musa
-- Creation Date: jun 20 2011
-- Last Modification: aug 09 2011

local lmp = require"luamemprofiler"
lmp.start(...)
local t = {}
lmp.stop()

lmp.start(...)
local t = {10}
lmp.stop()

lmp.start(...)
local t = {10, 20, 30}
lmp.stop()

lmp.start(...)
local t = {}
for i=10,200,10 do
  table.insert(t, i)
end
lmp.stop()

lmp.start(...)
local t = {}
for i=10,320,10 do
  table.insert(t, i)
end
lmp.stop()

lmp.start(...)
local t = {}
for i=10,330,10 do
  table.insert(t, i)
end
lmp.stop()

