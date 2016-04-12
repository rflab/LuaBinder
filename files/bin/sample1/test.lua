-- prepare script table
g_script_table = {}

-- load script
dofile("sample1/scripts/one.lua")
dofile("sample1/scripts/two.lua")
dofile("sample1/scripts/three.lua")

-- tick function will be called from C++
function OnTick()
	-- call each OnTick function
	for i, v in ipairs(g_script_table) do
		v()
	end
end
