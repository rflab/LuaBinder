.PHONY:build rebuild clean

rebuild: build clean ;

build:
	make a -C lua
	g++ -ldl -I./lua test.cpp lua/lua.a

clean:
	-rm a.out
	-make clean -C lua