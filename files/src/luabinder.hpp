#ifndef _RF_LUA_BINER_
#define _RF_LUA_BINER_

#include <stdio.h>

#include <iostream>
#include <string>
#include <tuple>
#include <array>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#include "lua.hpp"

// コンパイラ依存？
#define nullptr NULL
#define final
#define throw(x)

// これをC++関数内でthrowするとLua関数の戻り値をfalseになる
// 古いコンパイラだとto_stringが使えない
#ifdef _MSC_VER
	#define LUA_RUNTIME_ERROR(x) std::runtime_error(std::string("c++ runtime exception. L") + ::std::to_string(__LINE__) + " " + __FUNCTION__ + ":" + x)
	#define LUA_DOMEIN_ERROR(x) std::domain_error(std::string("c++ domein error exception. L") + ::std::to_string(__LINE__) + " " + __FUNCTION__ + ":" + x)
	#define LUA_ARGUMENT_ERROR(x) std::invalid_argument(std::string("c++ invalid argument exception. L") + ::std::to_string(__LINE__) + " " + __FUNCTION__ + ":" + x)
#else
	#define LUA_RUNTIME_ERROR(x) std::runtime_error("c++ runtime exception.")
	#define LUA_DOMEIN_ERROR(x) std::domain_error("c++ omein error exception.")
	#define LUA_ARGUMENT_ERROR(x) std::invalid_argument("c++ invalid argument exception.")
	#define make_unique make_shared
	#define unique_ptr shared_ptr
#endif

namespace rf
{
	using std::size_t;
	using std::cout;
	using std::endl;
	using std::stringstream;
	using std::string;
	using std::tuple;
	using std::enable_if;
	using std::unique_ptr;
	using std::make_unique;

	class LuaBinder final
	{
	private:
		lua_State* L_;

	private:
		bool open()
		{
			L_ = luaL_newstate(); // コンテキスト作成 
			luaL_openlibs(L_); // Lua標準ライブラリ読み込み
			return true;
		}

		bool close()
		{
			lua_close(L_);
			return true;
		}
		
		static bool  dump_stack(lua_State *L)
		{
			int stack_size = lua_gettop(L);
			printf("------------------------------------------\n");
			for (int i = 0; i < stack_size; i++)
			{
				int type = lua_type(L, stack_size - i);
				printf("Stack[%2d(%3d) %10s] : ", stack_size - i, -i - 1, lua_typename(L, type));

				switch (type)
				{
				case LUA_TNUMBER:
					printf("%f\n", lua_tonumber(L, stack_size - i));
					break;
				case LUA_TBOOLEAN:
					if (lua_toboolean(L, stack_size - i))
						printf("true\n");
					else
						printf("false\n");
					break;
				case LUA_TSTRING:
					printf("%s\n", lua_tostring(L, stack_size - i));
					break;
				case LUA_TNIL:
					printf("\n");
					break;
				default:
					printf("%s\n", lua_typename(L, type));
					break;
				}
			}
			printf("------------------------------------------\n");
			return true;
		}

		bool dump_stack()
		{
			return dump_stack(L_);
		}

		bool luaresult(int lua_ret)
		{			
			if (lua_ret != LUA_OK)
			{
				cout << lua_tostring(L_, -1) << endl;
				return false;
			}

			return true;
		}

		static int traceback(lua_State* L)
		{
			dump_stack(L);
			const char* msg = lua_tostring(L, -1);
			if (msg)
			{
				luaL_traceback(L, L, msg, 1);
				cout << lua_tostring(L, -1) << endl;
				lua_pop(L, 1);
			}
			return 1;
		}

		// 引数インデックス用シーケンス

		template<typename T, T ... N>
		struct intetgral_sequence
		{};

		template<typename T, typename S>
		struct push_back;

		template<template<typename T, T ... V> class Seq,
			template<typename T, T V> class Val,
			typename T, T ... V1, T V2>
		struct push_back < Seq<T, V1...>, Val<T, V2> >
		{
			typedef Seq<T, V1..., V2> type;
		};

		template<typename T, T V1, T V2>
		struct make_integral_sequence;

		template<typename T, T V1, T V2>
		struct make_integral_sequence
		{
			typedef typename
				push_back < typename make_integral_sequence<T, V1, V2 - 1>::type,
				std::integral_constant<T, V2 - 1 > > ::type type;
		};

		template<typename T, T V>
		struct make_integral_sequence < T, V, V >
		{
			typedef intetgral_sequence<T> type;
		};

		// Lua型判定

		template<typename T>
		struct is_boolean
		{
			static const bool value =
				std::is_same < T, bool >::value;
		};

		template<typename T>
		struct is_number
		{
			static const bool value =
				((!is_boolean<T>::value)
				&& ((std::is_integral<T>::value)
				|| (std::is_floating_point<T>::value)));
		};

		template<typename T>
		struct is_string
		{
			static const bool value =
				((std::is_same<T, string>::value)
				|| (std::is_same<T, char const*>::value));
		};

		template<typename T>
		struct is_basic_type
		{
			static const bool value =
				((is_boolean<T>::value)
				|| (is_number<T>::value)
				|| (is_string<T>::value));
		};

		// スタック操作 オーバーロードでC++->Lua

		static void push_stack(lua_State* L, bool a)
		{
			lua_pushboolean(L, a);
		}

		static void push_stack(lua_State* L, char const* a)
		{
			lua_pushstring(L, a);
		}

		static void push_stack(lua_State* L, const string& a)
		{
			lua_pushstring(L, a.c_str());
		}

		template<typename T>
		static void push_stack(lua_State* L, T a)
		{
			lua_pushnumber(L, static_cast<lua_Number>(a));
		}

		// どうすりゃいいかわからん
		//template<typename T>
		//static void push_stack(lua_State* L, T a, enable_if<is_number<T>::value>::type* = 0)
		//{
		//	lua_pushnumber(L, static_cast<lua_Number>(a));
		//}
		//
		//template<typename T>
		//static void push_stack(lua_State* L, T a, enable_if<!is_basic_type<T>::value>::type* = 0)
		//{
		//	void* p = lua_newuserdata(L, sizeof(T));
		//	new(p) T(a);
		//
		//	lua_pushvalue(L, lua_upvalueindex(1)); // クラスを取り出す
		//	lua_setmetatable(L, userdata); // メタテーブルに追加する
		//
		//	return 1; // インスタンス1つを返す
		//
		//	lua_pushnumber(L, static_cast<lua_Number>(a));
		//}

		// スタック操作 型推論でLua->C++
		
		template<typename T>
		static T get_stack(lua_State* L, int index, typename enable_if<is_number<T>::value>::type* = 0)
		{
			if (lua_type(L, index) != LUA_TNUMBER)
				throw LUA_ARGUMENT_ERROR(string("not a number arg:") + std::to_string(index));
			return static_cast<T>(lua_tonumber(L, index));
		}

		template<typename T>
		static bool get_stack(lua_State* L, int index, typename enable_if<is_boolean<T>::value>::type* = 0)
		{
			if (lua_type(L, index) != LUA_TBOOLEAN)
				throw LUA_ARGUMENT_ERROR(string("not a boolean arg:") + std::to_string(index));
			return lua_toboolean(L, index) == 0 ? false : true;
		}


		template<typename T>
		static const char *get_stack(lua_State* L, int index, typename enable_if<is_string<T>::value>::type* = 0)
		{
			if (lua_type(L, index) != LUA_TSTRING)
				throw LUA_ARGUMENT_ERROR(string("not a string arg:") + std::to_string(index));
			const char* str = lua_tostring(L, index);
			if (str == nullptr)
				throw LUA_RUNTIME_ERROR("nil ptr");
			return str;
		}

		//template<typename T>
		//static const char *get_stack(lua_State* L, int index, typename enable_if<std::is_same<T, char const*>::value>::type* = 0)
		//{
		//	return lua_tostring(L, index);
		//}
		//
		//template<typename T>
		//static const string get_stack(lua_State* L, int index, typename enable_if<std::is_same<T, string>::value>::type* = 0)
		//{
		//	return lua_tostring(L, index);
		//}

		template<typename T>
		static T& get_stack(lua_State* L, int index, typename enable_if<!is_basic_type<T>::value>::type* = 0)
		{
			if (lua_type(L, index) != LUA_TUSERDATA)
				throw LUA_ARGUMENT_ERROR(string("not a userdata arg:") + std::to_string(index));
			typedef typename std::remove_reference<T>::type type;
			auto obj = static_cast<type*>(lua_touserdata(L, index));
			return *obj;
		}

		template<typename...Args, size_t...Ixs>
		static tuple<Args...> get_tuple(lua_State* L, intetgral_sequence<size_t, Ixs...>)
		{
			return tuple<Args...>(get_stack<Args>(L, Ixs)...);
		}

		
		// 引数型情報とLuaから呼び出す関数をもつスタブのようなオブジェクト
		// 関数、void関数、メンバ関数、voidメンバ関数を特殊化する
		template<typename Func, typename Seq, typename IsMember = void>
		struct invoker;

		// 戻り値がある関数
		template<typename Ret, typename... Args, size_t ... Ixs>
		struct invoker < Ret(*)(Args...), intetgral_sequence<size_t, Ixs...> >
		{
			static int apply(lua_State* L)
			{
				if (lua_gettop(L) != sizeof...(Ixs))
				{
					return luaL_error(L, "argument number error expected=%d, top=%d",
						sizeof...(Ixs), lua_gettop(L)); // longjmp
				}

				auto f = reinterpret_cast<Ret(*)(Args...)>(lua_tocfunction(L, lua_upvalueindex(1)));
				
				try
				{
					Ret r = f(get_stack<Args>(L, Ixs)...);
					LuaBinder::push_stack(L, r);
				}
				catch (const string s)
				{
					LuaBinder::push_stack(L, s);
					return 1;
				}
				catch (const std::exception &e)
				{
					push_stack(L, e.what());
					traceback(L);
					lua_pop(L, 1);
					return luaL_error(L, e.what()); // longjmp
				}
				catch (...)
				{
					push_stack(L, "unknown error.");
					traceback(L);
					lua_pop(L, 1);
					return luaL_error(L, "unknown exception");
				}

				return 1;
			}
		};

		// 戻り値がvoidの関数
		template<typename... Args, size_t ... Ixs>
		struct invoker < void(*)(Args...), intetgral_sequence<size_t, Ixs...> >
		{
			static int apply(lua_State* L)
			{
				if (lua_gettop(L) != sizeof...(Ixs))
				{
					return luaL_error(L, "argument number error expected=%d, top=%d",
						sizeof...(Ixs), lua_gettop(L)); // longjmp
				}

				auto f = reinterpret_cast<void(*)(Args...)>(lua_tocfunction(L, lua_upvalueindex(1)));
				
				try
				{
					f(get_stack<Args>(L, Ixs)...);
				}
				catch (const string s)
				{
					LuaBinder::push_stack(L, s);
					return 1;
				}
				catch (const std::exception &e)
				{
					push_stack(L, e.what());
					traceback(L);
					lua_pop(L, 1);
					return luaL_error(L, e.what()); // longjmp
				}
				catch (...)
				{
					push_stack(L, "unknown error.");
					traceback(L);
					lua_pop(L, 1);
					return luaL_error(L, "unknown exception");
				}

				return 0;
			}
		};

		//// 面倒なのでやめ
		//template<template<class, class> class Tuple, typename Ret, typename... Args, size_t ... Ixs>
		//struct invoker < Tuple<Ret, bool>(*)(Args...), intetgral_sequence<size_t, Ixs...> >
		//{
		//	static int apply(lua_State* L)
		//	{
		//		if (lua_gettop(L) != sizeof...(Ixs))
		//		{
		//			return luaL_error(L, "argument number error expected=%d, top=%d",
		//				sizeof...(Ixs), lua_gettop(L)); // longjmp
		//		}
		//
		//		auto f = reinterpret_cast<void(*)(Args...)>(lua_tocfunction(L, lua_upvalueindex(1)));
		//		Ret ret_value;
		//		bool result;
		//		std::tie(ret_value, result) = f(get_stack<Args>(L, Ixs)...);
		//		LuaBinder::push_stack<>(L, ret_value);
		//		LuaBinder::push_stack<>(L, result);
		//		return 2;
		//	}
		//};

		// 戻り値があるメンバ関数
		template<typename Ret, class T, typename ... Args, size_t ... Ixs>
		struct invoker< Ret(T::*)(Args...), intetgral_sequence<size_t, Ixs...>,
			typename enable_if<std::is_member_function_pointer<Ret(T::*)(Args...)>::value && !std::is_void<Ret>::value>::type>
		{
			static int apply(lua_State* L)
			{
				if (lua_gettop(L) != sizeof...(Ixs)+1)
				{
					return luaL_error(L, "argument number error expected=%d, top=%d",
						sizeof...(Ixs)+1, lua_gettop(L)); // longjmp
				}

				auto self = static_cast<T*>(lua_touserdata(L, 1));
				if (self == nullptr){
					cout << "no self specified" << endl;
				}

				typedef typename std::remove_reference<Ret(T::*)(Args...)>::type mf_type;
				void* buf = lua_touserdata(L, lua_upvalueindex(1));
				auto a = static_cast<std::array<mf_type, 1>*> (buf);
				mf_type fp = (*a)[0];

				try
				{
					Ret r = (self->*fp)(get_stack<Args>(L, Ixs)...);
					push_stack(L, r);
				}
				catch (const string s)
				{
					LuaBinder::push_stack(L, s);
					return 1;
				}
				catch (const std::exception &e)
				{
					push_stack(L, e.what());
					traceback(L);
					lua_pop(L, 1);
					return luaL_error(L, e.what()); // longjmp
				}
				catch (...)
				{
					push_stack(L, "unknown error.");
					traceback(L);
					lua_pop(L, 1);
					return luaL_error(L, "unknown exception");
				}
			
				return 1;
			}
		};
		
		// 戻り値がvoidのメンバ関数
		template<typename Ret, class T, typename ... Args, size_t ... Ixs>
		struct invoker< Ret(T::*)(Args...), intetgral_sequence<size_t, Ixs...>,
			typename enable_if<std::is_member_function_pointer<void(T::*)(Args...)>::value && std::is_void<Ret>::value>::type>
		{
			static int apply(lua_State* L)
			{
				if (lua_gettop(L) != sizeof...(Ixs)+1)
				{
					return luaL_error(L, "argument number error expected=%d, top=%d",
						sizeof...(Ixs)+1, lua_gettop(L)); // longjmp
				}

				auto self = static_cast<T*>(lua_touserdata(L, 1));
				if (self == nullptr){
					cout << "no self specified" << endl;
				}
		
				typedef typename std::remove_reference<void(T::*)(Args...)>::type mf_type;
				void* buf = lua_touserdata(L, lua_upvalueindex(1));
				auto a = static_cast<std::array<mf_type, 1>*> (buf);
				mf_type fp = (*a)[0];
		
				try
				{
					(self->*fp)(get_stack<Args>(L, Ixs)...);
				}
				catch (const string s)
				{
					LuaBinder::push_stack(L, s);
					return 1;
				}
				catch (const std::exception &e)
				{
					push_stack(L, e.what());
					traceback(L);
					lua_pop(L, 1);
					return luaL_error(L, e.what()); // longjmp
				}
				catch (...)
				{
					push_stack(L, "unknown error.");
					traceback(L);
					lua_pop(L, 1);
					return luaL_error(L, "unknown exception");
				}

				return 0;
			}
		};

		//// 面倒なのでやめ
		//template<class T, typename Ret, typename ... Args, size_t ... Ixs>
		//struct invoker< tuple<Ret, bool>(T::*)(Args...), intetgral_sequence<size_t, Ixs...>,
		//	typename enable_if<std::is_member_function_pointer<Ret(T::*)(Args...)>::value>::type>
		//{
		//	static int apply(lua_State* L)
		//	{
		//		if (lua_gettop(L) != sizeof...(Ixs)+1)
		//		{
		//			return luaL_error(L, "argument number error expected=%d, top=%d",
		//				sizeof...(Ixs)+1, lua_gettop(L)); // longjmp
		//		}
		//
		//		auto self = static_cast<T*>(lua_touserdata(L, 1));
		//		if (self == nullptr){
		//			cout << "no self specified" << endl;
		//		}
		//
		//		typedef typename std::remove_reference<void(T::*)(Args...)>::type mf_type;
		//		void* buf = lua_touserdata(L, lua_upvalueindex(1));
		//		auto a = static_cast<std::array<mf_type, 1>*> (buf);
		//		mf_type fp = (*a)[0];
		//
		//		Ret ret_value;
		//		bool result;
		//		std::tie(ret_value, result) = (self->*fp)(get_stack<Args>(L, Ixs)...);
		//		LuaBinder::push_stack<>(L, ret_value);
		//		LuaBinder::push_stack<>(L, result);
		//		return 2;
		//	}
		//};

		// 引数なしコンストラクタ
		template<class T>
		static int default_constructor(lua_State* L)
		{
			void *p = lua_newuserdata(L, sizeof(T));
			int userdata = lua_gettop(L);
			
			// void* instance = new(p)T;
			new(p) T;
			
			lua_pushvalue(L, lua_upvalueindex(1)); // クラスを取り出す
			lua_setmetatable(L, userdata); // メタテーブルに追加する

			return 1; // インスタンス1つを返す
		}


		template<class T>
		static int destructor(lua_State* L)
		{
			// ポインタ指定の配置newなのでdeleteしない
			T* instance = static_cast<T*>(lua_touserdata(L, -1));
			instance->~T();
			return 0;
		}

		template<typename T, typename ... Args, size_t ... Ixs>
		static void* call_constructor(void* p, intetgral_sequence<size_t, Ixs...>, tuple<Args...>& args)
		{
			return new(p)T(std::get<Ixs>(args)...);
		}

	public:

		LuaBinder(){
			open();
		}

		~LuaBinder(){
			close();
		}

		// Luaファイルを実行
		bool dofile(const string& filename)
		{
			int top = lua_gettop(L_);
			lua_pushcfunction(L_, traceback);
			lua_insert(L_, top);
			int func = lua_gettop(L_);

			if ((!luaresult(luaL_loadfile(L_, filename.c_str())))
			|| (!luaresult(lua_pcall(L_, 0, LUA_MULTRET, func))))
				return false;

			lua_remove(L_, func);
			lua_settop(L_, top);

			return true;
		}

		// Luaスクリプト文字列を実行
		bool dostring(const string& str)
		{
			int top = lua_gettop(L_);
			lua_pushcfunction(L_, traceback);
			lua_insert(L_, top);
			int func = lua_gettop(L_);

			if ((!luaresult(luaL_loadstring(L_, str.c_str())))
			|| (!luaresult(lua_pcall(L_, 0, LUA_MULTRET, func))))
				return false;

			lua_remove(L_, func);
			lua_settop(L_, top);

			return true;
		}

		// 関数バインド
		//
		// ＜使用例＞
		//	LuaBinder lua;
		//	lua.def("func1", func);
		//	lua.def("func2", func2);
		//	lua.def("func3", (int(*)(int))    overload_func);
		//	lua.def("func4", (int(*)(string)) overload_func);
		template<typename R, typename ... Args>
		void def(const string& func_name, R(*f)(Args...))
		{
			typedef typename make_integral_sequence<size_t, 1, sizeof...(Args)+1>::type seq;
			lua_CFunction upvalue = invoker<decltype(f), seq>::apply;

			// 登録する関数はinvokerから呼び出すので関数型にキャストしてクロージャに入れる
			lua_pushcfunction(L_, reinterpret_cast<lua_CFunction>(f));
			lua_pushcclosure(L_, upvalue, 1);
			lua_setglobal(L_, func_name.c_str());
		}

		// クラスバインド
		//
		// ＜使用例＞
		// LuaBinder lua;
		// lua.def_class<Test>("Test")->
		//	def("func1", &Test::func).
		//	def("func2", &Test::func2).
		//	def("func3", (int(Test::*)(int))    &Test::overload_func).
		//	def("func4", (int(Test::*)(string)) &Test::overload_func);
		template<class T> class class_chain;

		template<class T>
		unique_ptr<class_chain<T> > def_class(const string& name)
		{
			return make_unique<class_chain<T> >(L_, name);
		}
		
		// コンストラクタ登録用オブジェクト
		// 通常はdefに渡す
		template<class T>
		struct constructor;

		template<typename T, typename ... Args>
		struct constructor<T(Args...)>
		{
			static int apply(lua_State* L)
			{
				if (lua_gettop(L) != sizeof...(Args)+1)
				{
					return luaL_error(L, "constructor argument number error expected=%d, top=%d",
						sizeof...(Args)+1, lua_gettop(L)); // longjmp
				}

				void* p = lua_newuserdata(L, sizeof(T));
				int userdata = lua_gettop(L);

				try
				{
					// 1がself、2～top-1が引数、topはnewuserdataで今積んだばっかり。
					typedef typename make_integral_sequence<size_t, 2, sizeof...(Args)+2>::type lua_index_seq;
					auto args = get_tuple<Args...>(L, lua_index_seq());

					typedef typename make_integral_sequence<size_t, 0, sizeof...(Args)>::type cpp_index_seq;
					call_constructor<T>(p, cpp_index_seq(), args);
				}
				catch (const string s)
				{
					LuaBinder::push_stack(L, s);
					return 1;
				}
				catch (const std::exception &e)
				{
					push_stack(L, e.what());
					traceback(L);
					lua_pop(L, 1);
					return luaL_error(L, e.what()); // longjmp
				}
				catch (...)
				{
					push_stack(L, "unknown error.");
					traceback(L);
					lua_pop(L, 1);
					return luaL_error(L, "unknown exception");
				}

				lua_pushvalue(L, lua_upvalueindex(1)); // クラス名を取り出す
				luaL_getmetatable(L, lua_tostring(L, -1)); // メタテーブルをプッシュ
				lua_setmetatable(L, userdata); // インスタンスにメタテーブルを追加する
				lua_pop(L, 1); // クラス名をポップ

				return 1; //インスタンスを返す
			}
		};

		// メンバ関数登録用オブジェクト
		// 通常はdef_classを使えばいいはず
		template<class T>
		class class_chain
		{
		private:
			string name_;
			lua_State* L_;

		public:
			class_chain(lua_State* L, const string& name)
				:name_(name), L_(L)
			{
				// local table = {}
				lua_newtable(L_);
				int table = lua_gettop(L_);

				// レジストリ上にメタテーブルを登録し(重複の場合は無視)
				// スタックにそのテーブルを乗せる
				luaL_newmetatable(L_, name.c_str());
				int metatable = lua_gettop(L_);

				//getmetatableの動作を変更したい場合
				// // metatable[__metatable] = metatable
				// lua_pushliteral(L_, "__metatable");
				// lua_pushvalue(L_, metatable);
				// lua_settable(L_, metatable);

				// metatable[__index] = table
				lua_pushliteral(L_, "__index");
				lua_pushvalue(L_, table);
				lua_settable(L_, metatable);

				// metatable[__gc] = delete
				lua_pushliteral(L_, "__gc");
				lua_pushcfunction(L_, &destructor<T>);
				lua_settable(L_, metatable);

				// デフォルトコンストラクタ、引数なしのnew関数でクラスを生成するようにする
				// ためのメンバ関数メタテーブル、コンストラクタをluaに登録する
				// table.new = new(metatable)
				lua_pushliteral(L_, "new");
				lua_pushvalue(L_, metatable);
				lua_pushcclosure(L_, default_constructor<T>, 1);
				lua_settable(L_, table);

				// _G[name] = table
				lua_pushvalue(L_, table);
				lua_setglobal(L_, name.c_str());

				// スタッククリア
				lua_pop(L_, 2);
			}

			// コンストラクタ、引数ありコンストラクタでクラスを生成するようにする
			// メンバ関数メタテーブル、コンストラクタをluaに登録する
			template<class S, typename ... Args>
			const class_chain<T>& def(const string &name, constructor<S(Args...)>) const
			{
				// C++側引数->スタックの参照テーブル
				lua_CFunction f = constructor<S(Args...)>::apply;
	
				// クラスをpush
				lua_getglobal(L_, name_.c_str());
				int c = lua_gettop(L_);

				// c[name] = constructor
				lua_pushstring(L_, name.c_str());
				lua_pushcfunction(L_, f);
				lua_settable(L_, c);
	
				// クラスのメタテーブルをプッシュ
				luaL_getmetatable(L_, name_.c_str());
				int metatable = lua_gettop(L_);
	
				// metatable[__index]をプッシュ
				lua_getfield(L_, metatable, "__index");
				int methodtable = lua_gettop(L_);
	
				// metatable[__index] = constructor と name 
				lua_pushstring(L_, name.c_str());
				lua_pushstring(L_, name_.c_str());
				lua_pushcclosure(L_, f, 1);
				lua_settable(L_, methodtable);
	
				// スタッククリア
				lua_pop(L_, 2);
	
				return *this;
			}

			// メンバ関数登録
			// 関数と実体(クラスの元となるオブジェクト)をluaに登録する
			template<class S, typename Ret, typename... Args>
			const class_chain<T>& def(const string & method_name, Ret(S::*f)(Args...),
				typename enable_if<std::is_member_function_pointer<Ret(S::*)(Args...)>::value>::type* = 0) const
			{
				// C++側引数->スタックの参照テーブル
				typedef typename make_integral_sequence<size_t, 2, sizeof...(Args)+2>::type seq;
				lua_CFunction closure = invoker<Ret(S::*)(Args...), seq>::apply;

				luaL_getmetatable(L_, name_.c_str());
				int metatable = lua_gettop(L_);

				lua_getfield(L_, metatable, "__index");
				int methodtable = lua_gettop(L_);

				lua_pushstring(L_, method_name.c_str());

				// メンバ関数ポインタのサイズはsizeof(int)でなく特別
				// メンバ関数は実体としてしかコピー出来ない
				// つまり↓は通らない
				// (void*)f; 
				typedef typename std::remove_reference<Ret(S::*)(Args...)>::type mf_type;
				void* buf = lua_newuserdata(L_, sizeof(std::array<mf_type, 1>));
				auto a = static_cast<std::array<mf_type, 1>*>(buf);
				(*a)[0] = f;

				lua_pushcclosure(L_, closure, 1);
				lua_settable(L_, methodtable);

				lua_pop(L_, 2);

				return *this;
			}
		};
	};
}

#endif
