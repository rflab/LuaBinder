.PHONY:build rebuild clean

rebuild: clean build ;

build:
	make a -C lua
	g++ -std=c++11 -ldl -I./lua test.cpp lua/liblua.a

clean:
	-rm a.out
	-make clean -C lua