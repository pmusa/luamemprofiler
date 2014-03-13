-- Author: Pablo Musa
-- Creation Date: jun 20 2011
-- Last Modification: aug 09 2011

local lmp = require"luamemprofiler"
lmp.start(...)

function eval(str)
  local tab = {}
  local f = loadstring("local x=...; return "..str)
  for i=0,10 do
    a,b = pcall(f,i)
    if not a then
      return a,b
    else
      tab[i] = b
    end
  end
  return tab
end

for k,v in pairs(eval("10*x^4 - 5*x^3 + 7*x^2 - 3*x + 5")) do ; end

lmp.stop()

