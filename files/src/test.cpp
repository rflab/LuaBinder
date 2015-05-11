#include <iostream>
#include <memory>
#include <string>

#include "luabinder.hpp"

using namespace std;

// テスト用の関数
void   func()                 { cout << "A" << endl; }
string func2(int i)           { cout << "B" << endl; return string("C"); }
int    overload_func(int i)   { cout << "D " << i << endl; return 100; }
int    overload_func(string s){ cout << "E " << s << endl; return 200; }

// テスト用のクラス
class Test
{
public:
	Test()                        { cout << "constructor" << endl; }
	~Test()                       { cout << "destructor" << endl; }
	void   func()                 { cout << "AA" << endl; }
	string func2(int i)           { cout << "BB" << endl; return string("CC"); }
	int    overload_func(int i)   { cout << "DD " << i << endl; return 100; }
	int    overload_func(string s){ cout << "EE " << s << endl; return 200; }
};

int main(int argc, char** argv)
{
	auto lua = make_shared<rf::LuaBinder>();

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

	// lua実行
	lua->dostring(
		"func1()                  \n"  // 関数テスト
		"print(func2(100))        \n"
		"print(func3(200))        \n"
		"print(func3(\"hoge\"))   \n"
		"c = Test.new()           \n"  // クラスインスタンス化
		"c:func1()                \n"  // メンバ関数テスト
		"print(c:func2(100))      \n"
		"print(c:func3(200))      \n"
		"print(c:func4(\"foo\")) \n");

	// for windows console
	cout << "wait input..";
	getchar();

	return 0;
}

