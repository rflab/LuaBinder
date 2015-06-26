#include <iostream>
#include <memory>
#include <string>

#include "luabinder.hpp"

using namespace std;

// テスト用の規定クラス
class Base
{
public:
	Base()                   { cout << "base:constructor" << endl; }
	virtual ~Base()          { cout << "base:destructor" << endl; }
	virtual int m1()         { cout << "base:m1()" << endl; return 100; }
	void        m2(int    n) { cout << "base:m2() " << n << endl; }
	void        m2(string s) { cout << "base:m2() overload " << s << endl; }
};

// テスト用の派生クラス
class Derived : public Base
{
	int n_;
public:
	Derived(int n) :Base() { cout << "derived:constructor " << n << endl; n_ = n; }
	virtual ~Derived()     { cout << "derived:destructor" << endl; }
	int  m1() override     { cout << "derived:m1() override " << n_ << endl; return 200; }
	void m2(Base& base)    { cout << "derived:m2()userdata argument " << endl; base.m1(); }
};

// テスト用の関数
string f1()         { cout << "f1" << endl; return string("f1_str"); }
void   f2(int    n) { cout << "f2" << n << endl; }
void   f2(string s) { cout << "f2 overload" << s << endl; }
void   f3(Base&  b) { cout << "f3" << b.m1() << endl; } //未対応

int main(int , char** )
{
	auto lua = make_unique<rf::LuaBinder>();

	// 関数バインド
	lua->def("func1", f1);
	lua->def("func2", (void(*)(int))    f2);
	lua->def("func3", (void(*)(string)) f2);

	// クラスバインド
	lua->def_class<Base>("Base")->
		def("new", rf::LuaBinder::constructor<Base()>()).
		def("mem1", &Base::m1).
		def("mem2", (void(Base::*)(int))    &Base::m2).
		def("mem3", (void(Base::*)(string)) &Base::m2);

	// 派生クラスバインド
	lua->def_class<Derived>("Derived", "Base")->
		def("new", rf::LuaBinder::constructor<Derived(int)>()).
		def("mem1", &Derived::m1).
		def("mem4", &Derived::m2);

	// lua実行、lua->cpp呼び出し
	lua->dostring(
		"print(\"----<function>------\") \n"
		"print(func1())                  \n"
		"func2(1000)                     \n"
		"func3(\"hoge\")                 \n"
		"                                \n"
		"print(\"------<base>--------\") \n"
		"c1 = Base:new()                 \n"
		"print(c1:mem1())                \n"
		"c1:mem2(2000)                   \n"
		"c1:mem3(\"foo\")                \n"
		"                                \n"
		"print(\"------<derived>-----\") \n"
		"c2 = Derived:new(3000)          \n"
		"print(c2:mem1())                \n"
		"c2:mem2(4000)                   \n"
		"c2:mem3(\"bar\")                \n"
		"c2:mem4(c1)                     \n"
		"                                \n"
		"print(\"------<release>-----\") \n"
		"c1 = nil                        \n"
		"c2 = nil                        \n"
		"collectgarbage(\"collect\")     \n"
		"                                \n"
		"print(\"-----<def func>-----\") \n"
		"function func_lua(n)            \n"
		"	print(n)                     \n"
		"	return n*2                   \n"
		"end                             \n");
	
	// cpp->lua呼び出し
	cout << "call_function, ret = "
		<< lua->call_function<int>("func_lua", 111) << endl;

	// for windows console
	cout << "wait input..";
	getchar();

	return 0;
}

