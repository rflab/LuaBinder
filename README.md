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

    LuaBinder lua;
    	
    // 関数バインド
    lua.def("func1", func);
    lua.def("func2", func2);
    lua.def("func3", (int(*)(int))    overload_func);
    lua.def("func4", (int(*)(string)) overload_func);

    // クラスバインド
    lua.def_class<Test>("Test")->
    	def("func1", &Test::func).
    	def("func2", &Test::func2).
    	def("func3", (int(Test::*)(int))    &Test::overload_func).
    	def("func4", (int(Test::*)(string)) &Test::overload_func);

    lua.dofile("test.lua");
