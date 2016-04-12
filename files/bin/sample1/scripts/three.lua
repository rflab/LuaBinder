-- file-local value
local n = 3

-- file-local function (closure)
local function OnTick()
	print("three", n)
    n = n + 1
end

-- OnLoad code (this will be executed when dofile)
table.insert(g_script_table, OnTick)
print("Three Loaded")

