a.out: test.cc maze.hh
	g++ -O3 -std=c++11 test.cc -lX11 -lpthread -o a.out 

g: a.out
	cgdb ./a.out

r: a.out
	./a.out 300 180
