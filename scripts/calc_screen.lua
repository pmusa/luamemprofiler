local bytes_per_pixel = {4, 8, 12, 16, 32}

function num_of_pixels(w, h)
  return w * (h/2)
end

function num_of_bytes(nop, bpp)
  return bpp * nop
end

function resolution(bpp, memused)
  local side = math.sqrt(memused * 1000000 * 2 / bpp / (4*3))+ 1;
  local w = side * 4;
  local h = side * 3;
  return w, h
end

local op = 1
while op do
print("Digite 1 para numero de bytes, 2 para resolução, 3 para bytes per pixel")
op = tonumber(io.read())

if op == 1 then
  print("Entre com a largura:")
  local w = tonumber(io.read())

  print("Entre com a altura:")
  local h = tonumber(io.read())

  for v=4,32,4 do
    local nop = num_of_pixels(w,h)
    local nob = num_of_bytes(nop, v)
    print(string.format("%d bytes %d x %d | %d | %d", v, w, h, nop, nob))
  end
elseif op == 2 then
  print("Entre com a memoria maxima usada:")
  local mmu = tonumber(io.read())

  for v=4,32,4 do
    local w, h = resolution(v, mmu)
    print(string.format("%d bytes %d x %d", v, w, h))
  end
elseif op == 3 then
  print("Entre com a memoria maxima usada:")
  local mmu = tonumber(io.read())

  local nop = num_of_pixels(800, 600)
  print(string.format("%d bytes %d x %d", mmu*1000000/nop, 800, 600))
else
  op = nil
end
end
