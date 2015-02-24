local lmprof = require"luamemprofiler"

local LOOP = 10000000
local START = 1
local START_DIFF = LOOP/10
local STOP = 0
local STOP_DIFF = LOOP/20

for i=1,LOOP do
  if i == START then
    lmprof.start()
    STOP = START + STOP_DIFF
    START = START + START_DIFF
    collectgarbage()
  elseif i == STOP then
    lmprof.stop()
  end
end
