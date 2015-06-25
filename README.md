# lua_binder

Luaのc++バインダです。
前はluabindを使っていたのですが長らくサポートされておらず必要な機能だけ作ることにしました。
luabindが非常に難解だったので、xxluaを参考にしました。

## ビルド・インストール
とりあえずLua5.3.0＆VC++12＆gcc (Ubuntu 4.9.2-10ubuntu13) 4.9.2でビルド確認済み
* gccの場合はfiles/srcでmake build
* VisualStudio2013の場合は、visual_studio_solution/luabinder.slnを開いてF5


## 使い方
files/src/test.cppが全てです。

```cpp
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
