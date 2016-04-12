-- load classes
package.path = "sample2/scripts/?.lua"
require("one")
require("two")
require("three")

-- Instantiation
local obj_table = {}
table.insert(obj_table, one:new())
table.insert(obj_table, two:new())
table.insert(obj_table, two:new())
table.insert(obj_table, three:new(100))
table.insert(obj_table, three:new(200))
table.insert(obj_table, three:new(300))

-- tick function will be called from C++
local count = 0
function OnTick()
	count = count + 1
	print("----"..count.."----")

	-- call each OnTick function
	for i, v in ipairs(obj_table) do
		v:OnTick()
	end
end
