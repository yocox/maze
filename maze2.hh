#include <vector>
#include <cassert>
#include <iostream>

enum class CellState {
    NONE, PATH, TREE
};

enum class Dir {
    NONE, LEFT, UP, RIGHT, DOWN
};

struct Cell {
    CellState state;
    Dir parent;
};

struct Point {
    int64_t x;
    int64_t y;
    bool operator==(const Point& rhs) {
        return x == rhs.x && y == rhs.y;
    }
};

class Maze {
public:
    Maze(int64_t w, int64_t h)
        : w_{w}
        , h_{h}
        , a(w, std::vector<Cell>(h))
    {}
    
    void generate() {
        a[0][0].state = CellState::TREE;
        for (int64_t x = 0; x < w_; ++x) {
            for (int64_t y = 0; y < w_; ++y) {
                loopCancleRandomWork({1, 0});
                print("Random walk");
            }
        }
    }
    
    Dir randomNextDir(const Point& curPos, Dir prevDir) {
        while (true) {
            Dir d = static_cast<Dir>(rand() % 4 + 1);
            switch (d) {
            case Dir::LEFT:
                if (prevDir == Dir::RIGHT || curPos.x == 0) continue;
                break;
            case Dir::UP:
                if (prevDir == Dir::DOWN || curPos.y == 0) continue;
                break;
            case Dir::RIGHT:
                if (prevDir == Dir::LEFT || curPos.x == w_ - 1) continue;
                break;
            case Dir::DOWN:
                if (prevDir == Dir::UP || curPos.y == h_ - 1) continue;
                break;
            case Dir::NONE:
                assert(0 && "unreachable");
            }
            return d;
        };
    }
    
    void loopCancleRandomWork(const Point& start) {
        if (a[start.x][start.y].state != CellState::NONE) {
            return;
        }
        Point curPos = start;
        Dir prevDir = Dir::NONE;
        while(true) {
            Dir nextDir = randomNextDir(curPos, prevDir);
            Point nextPos(curPos);
            switch(nextDir) {
            case Dir::LEFT : nextPos.x -= 1; break;
            case Dir::UP   : nextPos.y -= 1; break;
            case Dir::RIGHT: nextPos.x += 1; break;
            case Dir::DOWN : nextPos.y += 1; break;
            case Dir::NONE : exit(1); break;
            }
            
            // If next cell is tree, end the work, reverse the dir on the path to connect to the tree
            if (a[nextPos.x][nextPos.y].state == CellState::TREE) {
                Dir treeParentDir = nextDir;
                while (true) {
                    Point parentPos = curPos;
                    switch(a[curPos.x][curPos.y].parent) {
                    case Dir::LEFT :
                        a[curPos.x][curPos.y].parent = treeParentDir;
                        a[curPos.x][curPos.y].state = CellState::TREE;
                        if (curPos == start) return;
                        --curPos.x;
                        treeParentDir = Dir::RIGHT;
                        break;
                    case Dir::UP   :
                        a[curPos.x][curPos.y].parent = treeParentDir;
                        a[curPos.x][curPos.y].state = CellState::TREE;
                        if (curPos == start) return;
                        --curPos.y;
                        treeParentDir = Dir::DOWN ;
                        break;
                    case Dir::RIGHT:
                        a[curPos.x][curPos.y].parent = treeParentDir;
                        a[curPos.x][curPos.y].state = CellState::TREE;
                        if (curPos == start) return;
                        ++curPos.x;
                        treeParentDir = Dir::LEFT ;
                        break;
                    case Dir::DOWN:
                        a[curPos.x][curPos.y].parent = treeParentDir;
                        a[curPos.x][curPos.y].state = CellState::TREE;
                        if (curPos == start) return;
                        ++curPos.y;
                        treeParentDir = Dir::UP   ;
                        break;
                    case Dir::NONE :
                        a[curPos.x][curPos.y].parent = treeParentDir;
                        a[curPos.x][curPos.y].state = CellState::TREE;
                        if (curPos == start) return;
                        break;
                    }
                }
                return;
            }
            
            // If next cell is path, cancle the loop, and continue the random walk
            if (a[nextPos.x][nextPos.y].state == CellState::PATH) {
                cancleLoop(curPos, nextPos);
                continue;
            }
            
            // If next cell is empty cell, work on
            switch(nextDir) {
            case Dir::LEFT : a[nextPos.x][nextPos.y].parent = Dir::RIGHT; a[nextPos.x][nextPos.y].state = CellState::PATH; break;
            case Dir::UP   : a[nextPos.x][nextPos.y].parent = Dir::DOWN ; a[nextPos.x][nextPos.y].state = CellState::PATH; break;
            case Dir::RIGHT: a[nextPos.x][nextPos.y].parent = Dir::LEFT ; a[nextPos.x][nextPos.y].state = CellState::PATH; break;
            case Dir::DOWN : a[nextPos.x][nextPos.y].parent = Dir::UP   ; a[nextPos.x][nextPos.y].state = CellState::PATH; break;
            case Dir::NONE : exit(1); break;
            }
            curPos = nextPos;
            prevDir = nextDir;
        }
        
    }
    
    void cancleLoop(const Point& from, const Point& to) {
        Point cur = from;
        while(true) {
            switch(a[cur.x][cur.y].parent) {
            case Dir::LEFT : a[cur.x][cur.y].parent = Dir::NONE; a[cur.x][cur.y].state = CellState::NONE; --cur.x;
            case Dir::UP   : a[cur.x][cur.y].parent = Dir::NONE; a[cur.x][cur.y].state = CellState::NONE; --cur.y;
            case Dir::RIGHT: a[cur.x][cur.y].parent = Dir::NONE; a[cur.x][cur.y].state = CellState::NONE; ++cur.x;
            case Dir::DOWN : a[cur.x][cur.y].parent = Dir::NONE; a[cur.x][cur.y].state = CellState::NONE; ++cur.y;
            case Dir::NONE : std::cout << "Unreachable cancle" << std::endl; exit(1); break;
            };
            if (cur == to) {
                break;
            }
        }
    }

    void print(const std::string& title) const {
        std::cout << "======" << title << "======\n";
        constexpr char c[5] = {'_', '<', '^', '>', 'v'};
        for (int64_t y = 0; y < h_; ++y) {
            for (int64_t x = 0; x < w_; ++x) {
                std::cout << c[static_cast<int>(a[x][y].parent)];
            }
            std::cout << std::endl;
        }
        std::cout << "=====================\n";
    }

private:
    int64_t w_;
    int64_t h_;
    std::vector<std::vector<Cell>> a;
};