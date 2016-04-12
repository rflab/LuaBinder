-- file-local value
local n = 1

-- file-local function (closure)
local function OnTick()
  print("one", n)
  n = n + 1
end

-- OnLoad code (this will be executed when dofile)
table.insert(g_script_table, OnTick)
print("One Loaded")

