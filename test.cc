#include <stdlib.h>
#include <time.h>
#include <string>
#include "maze2.hh"

int main(int, char* argv[]) noexcept {
    std::cout << "start" << std::endl;
    srand(time(NULL));
    Maze m(std::stoi(argv[1]), std::stoi(argv[2]));
    std::cout << "start" << std::endl;
    m.generate();
    std::cout << "done" << std::endl;
    //m.save_img("maze", false, 4);
}
