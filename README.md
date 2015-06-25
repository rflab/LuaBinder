# lua_binder

Luaのc++バインダです。
前はluabindを使っていたのですが長らくサポートされておらず必要な機能だけ作ることにしました。
luabindとxxluaを参考にしています。

## ビルド方法

Lua5.3.0＆VC++12で動作確認済み
* VisualStudio2013の場合は、visual_studio_solution/luabinder.slnを開いてF5

gcc (Ubuntu 4.9.2-10ubuntu13) 4.9.2も一応対応してたましたが、長らくビルド確認してません。
* gccの場合はfiles/srcでmake build

## 使い方
files/src/test.cppが全て。

```cpp
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
lua.def_class<Derived>("Derived", "Base")-> // 基底クラスはlua内の名前で指定
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
