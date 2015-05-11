.PHONY:build rebuild clean

rebuild: build clean ;

build:
	make a -C lua
	gcc -lstdc++ -ldl -I./lua test.cpp lua/lua.a

clean:
	-rm a.out
	-make clean -C lua