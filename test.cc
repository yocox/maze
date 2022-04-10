#include <stdlib.h>
#include <time.h>
#include <string>
#include "maze2.hh"

int main(int argc, char* argv[]) noexcept {
    // srand(time(NULL));
    if (argc < 3 || argc > 4) {
        std::cout << "Usage maze <w> <h> [random_seed]" << std::endl;
        return 0;
    }
    if (argc == 4) {
        srand(std::stoi(argv[3]));
    } else {
        srand(time(NULL));
    }
    Maze m(std::stoi(argv[1]), std::stoi(argv[2]));
    std::cout << "start" << std::endl;
    m.generate();
    std::cout << "done" << std::endl;
    //m.print("Result");
    //m.save_img("maze", false, 4);
}
