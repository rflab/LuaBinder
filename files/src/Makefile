.PHONY:build rebuild clean help prof

rebuild: clean build ;

clean:
	-rm -Rf ../bin
	-make clean -C lua

build:
	mkdir -p ../bin
	make a -C lua
	g++ -std=c++14 -ldl -Wall -Wextra -I./lua test.cpp lua/liblua.a -o ../bin/a.out

prof:
	mkdir -p ../bin
	make a -C lua
	g++ -pg -std=c++14 -ldl -Wall -Wextra -I./lua test.cpp lua/liblua.a -o ../bin/prof.out
	#./prof.out 
	#gprof prof.out gmon.out

help:
	echo 'build'
	echo 'rebuild'
	echo 'clean'
	echo 'prof -> enable -pg option'