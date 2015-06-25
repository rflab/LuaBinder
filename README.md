# lua_binder

Luaのc++バインダです。
以前はluabindを使っていたのですが、長らくサポートされておらず必要な機能だけ作ることにしました。
luabindとxxluaを参考にしています。

## 使い方
files/src/test.cpp 参照のこと

```cpp
// test.cpp
#include "luabinder.hpp"

     : (中略)

// lua_State作成
LuaBinder lua;

// 関数バインド
lua.def("func1", f1);
lua.def("func2", (void(*)(int))    f2); // オーバーロード
lua.def("func3", (void(*)(string)) f2); // オーバーロード

// クラスバインド
lua.def_class<Base>("Base")->
	def("new", rf::LuaBinder::constructor<Base()>()).
	def("mem1", &Base::m1).
	def("mem2", (void(Base::*)(int))    &Base::m2). // オーバーロード
	def("mem3", (void(Base::*)(string)) &Base::m2); // オーバーロード

// 派生クラスバインド
lua.def_class<Derived>("Derived", "Base")-> // 基底クラスは登録済みの名前で指定
	def("new", rf::LuaBinder::constructor<Derived(int)>()). // 引数ありコンストラクタ
	def("mem1", &Derived::m1).
	def("mem4", &Derived::m2); // オーバーライド

// luaのファイルを実行
lua.dofile("test.lua");

// luaの文を実行
lua.dostring("function func(n) print(n) end");

// lua関数をcppからコール
lua->call_function<void>("func_lua", 10)
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
	return n*2
end
```

## サンプルのビルド方法

Lua5.3.0＆VC++12で動作確認済み
gcc (Ubuntu 4.9.2-10ubuntu13) 4.9.2も一応対応してたましたが、長らくビルド確認してません。

* VisualStudio2013の場合は、visual_studio_solution/luabinder.slnを開いてF5
* gccの場合はfiles/srcでmake build
