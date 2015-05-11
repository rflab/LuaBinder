# lua_binder

Luaのc++バインダです。
前はluabindを使っていたのですが長らくサポートされておらず必要な機能だけ作ることにしました。
luabindが非常に難解だったので、xxluaを参考にしました。

## 機能
関数バインドとクラスのバインドができます。
VC++＆Lua5.3.0でビルド確認、gccのビルドは確認していません。

## 使い方
files/src/test.cppが全てです。

	// 関数バインド
	lua->def("func1", func);
	lua->def("func2", func2);
	lua->def("func3", (int(*)(int))    overload_func);
	lua->def("func4", (int(*)(string)) overload_func);

	// クラスバインド
	lua->def_class<Test>("Test")->
		def("func1", &Test::func).
		def("func2", &Test::func2).
		def("func3", (int(Test::*)(int))    &Test::overload_func).
		def("func4", (int(Test::*)(string)) &Test::overload_func);
