# Lua Binder

[日本語](https://github.com/rflab/stream_reader/blob/master/README_detail.md)

This is Simple Lua / C++ binder.
The implementation is in only one hpp file（files/src/luabinder.hpp）

## How to use
Please see also example .. files/src/test.cpp

```cpp
// test.cpp

// include (this project)
#include "luabinder.hpp"

     : (中略)

// Create lua_State
rf::wrapper::LuaBinder lua;

// function bindding
lua.def("func1", f1);
lua.def("func2", (void(*)(int))    f2); // function overloading
lua.def("func3", (void(*)(string)) f2); // function overloading

// class bindding
lua.def_class<Base>("Base")->
	def("new", rf::wrapper::LuaBinder::constructor<Base()>()).
	def("mem1", &Base::m1).
	def("mem2", (void(Base::*)(int))    &Base::m2). // member function overloading
	def("mem3", (void(Base::*)(string)) &Base::m2); // member function overloading

// subclass binding
lua.def_class<Derived>("Derived", "Base")-> // specify base class by registered name, in this case "Base"
	def("new", rf::wrapper::LuaBinder::constructor<Derived(int)>()). // added constructor with int argument
	def("mem1", &Derived::m1).
	def("mem4", &Derived::m2); // override

// execute lua file
lua.dofile("test.lua");

// execute lua string
lua.dostring("function func(n) print(n) end");

// call lua function from c++ code
lua.call_function<void>("func_lua", 10)
```

```lua
-- test.lua

----<function>------
print(func1())
func2(1000)
func3("hoge")

------<base>--------
c1 = Base:new()
print(c1:mem1())
c1:mem2(2000)
c1:mem3("foo")

------<derived>-----
c2 = Derived:new(3000)
print(c2:mem1())
c2:mem2(4000)
c2:mem3("bar")
c2:mem4(c1)

------<release>-----
c1 = nil
c2 = nil
collectgarbage("collect")

-----<def func>-----
function func_lua(n)
	print(n)
end
```

## How to build sample project
build check is in Lua5.3.0＆VC++12, sometimes gcc (Ubuntu 4.9.2-10ubuntu13) 4.9.2 also.

* VisualStudio2013 --> open visual_studio_solution/luabinder.sln then "F5"
* gcc --> cd files/src; make build

## others
luabind and xxlua has been reference to implement this project.
