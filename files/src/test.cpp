#include <iostream>
#include <memory>
#include <string>

#include "luabinder.hpp"

using namespace std;

// テスト用の関数
string f1()        { cout << "f1"    << endl; return string("f1_str"); }
void   f2(int    n){ cout << "f2_1"  << n << endl; }
void   f2(string s){ cout << "f2_2 " << s << endl; }

// テスト用の規定クラス
class Base
{
public:
	Base()                   { cout << "constructor" << endl; }
	virtual ~Base()          { cout << "destructor" << endl; }
	virtual int f1()         { cout << "f1" << endl; return 100; }
	void        f2(int    n) { cout << "f2 " << n << endl; }
	void        f2(string s) { cout << "f2 overload " << s << endl; }
};

// テスト用の派生クラス
class Derived : public Base
{
public:
	Derived(int n) :Base() { cout << "derived constructor " << n << endl; }
	int f1() override      { cout << "f1 override " << endl; return 200; }
	void f4(Base& base)    { cout << "userdata argument " << endl; base.f1(); }
};

int main(int argc, char** argv)
{
	auto lua = make_shared<rf::LuaBinder>();

	// 関数バインド
	lua->def("func1", f1);
	lua->def("func2", (void(*)(int))    f2);
	lua->def("func3", (void(*)(string)) f2);

	// クラスバインド
	lua->def_class<Base>("Base")->
		def("new",   rf::LuaBinder::constructor<Base()>()).
		def("func1", &Base::f1).
		def("func2", (void(Base::*)(int))    &Base::f2).
		def("func3", (void(Base::*)(string)) &Base::f2);

	// 派生クラスバインド
	lua->def_subclass<Derived>("Derived", "Base")->
		def("new", rf::LuaBinder::constructor<Derived(int)>()).
		def("func1", &Derived::f1).
		def("func4", &Derived::f4);

	// lua実行
	lua->dostring(
		"print(\"----<function>------\")\n"
		"print(func1())                 \n"
		"func2(1000)                    \n"
		"func3(\"hoge\")                \n"
		"print(\"------<class>-------\")\n"
		"c1 = Base:new()                \n"
		"print(c1:func1())              \n"
		"c1:func2(2000)                 \n"
		"c1:func3(\"foo\")              \n"
		"print(\"------<class2>------\")\n"
		"c2 = Derived:new(3000)         \n"
		"print(c2:func1())              \n"
		"c2:func2(4000)                 \n"
		"c2:func3(\"bar\")              \n"
		"c2:func4(c1)                   \n"
		"print(\"-----<release>------\")\n"
		"c1 = nil                       \n"
		"c2 = nil                       \n"
		"collectgarbage(\"collect\")    \n");

	// for windows console
	cout << "wait input..";
	getchar();

	return 0;
}

