-- Author: Pablo Musa
-- Creation Date: jun 20 2011
-- Last Modification: aug 09 2011

local lmp = require"luamemprofiler"
lmp.start(...)

local function aux_cicle (ntab, tab, aux_tab)
  aux_tab[ntab] = 1
  for k,v in pairs(ntab) do
    if v == tab then
      return true
    end
    if type(v) == "table" and not aux_tab[v] then
      return aux_cicle(v, tab, aux_tab)
    end
  end
  return false
end

function cicle(tab)
  if tab then
    return aux_cicle(tab, tab, {})
  else
    return false
  end
end

a = {x = {}};  a.x[4] = a
cicle(a)
a = {x = {}};  a.x[4] = a.x
cicle(a)
a = {x = {}};  
a.x[4] = a.x
a.x[5] = x
b = {c = {a}}
a.x = b
cicle(a)

lmp.stop()
lmp.start(...)

local function aux_cicle2(ntab, tab, aux_tab)
  aux_tab[ntab] = 1
  for k,v in pairs(ntab) do
    if v == tab then
      return true 
    end
    if type(v) == "table" and not aux_tab[v] then
      local tmp = aux_cicle2(v, tab, aux_tab, tmp)
      if tmp then
        if type(tmp) == "table" then
          table.insert(tmp, k)
        else
          tmp = {k} 
        end
      end
      return tmp
    end
  end
  return false
end

function cicle2(tab)
   if tab then
    local res = {}
    local tmp = aux_cicle2(tab, tab, {})
    if tmp then
    local i = #tmp
    for _,v in ipairs(tmp) do
      res[i] = v
      i = i - 1;
    end
    return res
    end
  end
  return false
end

a = {x = {}};  a.x[4] = a
cicle2(a)
a = {x = {}};  a.x[4] = a.x
cicle2(a)
a = {x = {}};  
a.x[4] = a.x
a.x[5] = x
b = {c = {a}}
a.x = b
cicle2(a)

lmp.stop()
