-- Author: Pablo Musa
-- Creation Date: jun 20 2011
-- Last Modification: aug 09 2011

local lmp = require"luamemprofiler"
lmp.start(...)

function duplic (str)
  local str_tab = {}
  for v in string.gmatch(str, "%w+") do
    if str_tab[v] then
      return v
    else
      str_tab[v] = 1
    end
  end
  return nil
end

local str = [[
This version of the King James Bible was created by taking several
public domain copies and painstakingly comparing them to find all
the differences, and selecting the most common version. Each of the
differences was also compared to printed versions for verification.
]]

duplic("")
duplic("um dois tres dois um")
duplic("um dois tres quatro")
duplic(str)

