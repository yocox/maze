#include <string>
#include "maze.hh"

int main(int, char* argv[]) noexcept {
    srand(time(NULL));
    maze::Maze m(std::stoi(argv[1]), std::stoi(argv[2]));
    std::cout << "shuffling..." << std::endl;
    m.random_shuffle();
    std::cout << "done" << std::endl;
    m.save_img("maze");
}
