a.exe: test.cc maze.hh
	g++ -g -std=c++11 test.cc -lpthread -lgdi32 -o a.exe -IC:\lib\CImg-3.1.0_pre040122

g: a.exe
	cgdb ./a.exe

r: a.exe
	./a.out 640 480
