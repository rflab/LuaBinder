------------------------------------------------
-- for require()
------------------------------------------------
local name = ...           -- module name = require(name)
local _m = {}              -- member function table
local _meta = {__index=_m} -- metatable to link instance and member function table
package.loaded[name] = _m  -- set package to LuaState
_G[name] = _m              -- register to global

------------------------------------------------
-- private function
------------------------------------------------

local function func()
end

------------------------------------------------
-- public function
------------------------------------------------

-- constractor
function _m:new(num)
	local instance = {n = num} -- new instance
	setmetatable(instance, _meta) -- assign metatable to instance
	
	print("new Three")
	
	return instance
end

-- member function
function _m:OnTick()
	print("three", self.n)
	self.n = self.n + 1
end 	

