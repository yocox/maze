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

    }
    
    Dir randomNextDir(const Point& curPos, Dir prevDir) {
        while (true) {
            Dir d = static_cast<Dir>(rand() % 4 + 1);
            switch (d) {
            case Dir::LEFT:
                if (prevDir == Dir::RIGHT || curPos.x == 0) continue;
                break;
            case Dir::UP:
                if (prevDir == Dir::RIGHT || curPos.x == 0) continue;
                break;
            case Dir::RIGHT:
                if (prevDir == Dir::RIGHT || curPos.x == 0) continue;
                break;
            case Dir::DOWN:
                if (prevDir == Dir::RIGHT || curPos.x == 0) continue;
                break;
            case Dir::NONE:
                assert(0 && "unreachable");
        
            
            }
        };
    }

    void loop_cancle_random_work(const Point& start) {
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
                    case Dir::LEFT :a[curPos.x][curPos.y].parent = treeParentDir; a[curPos.x][curPos.y].state = CellState::TREE; --curPos.x; treeParentDir = Dir::RIGHT; break;
                    case Dir::UP   :a[curPos.x][curPos.y].parent = treeParentDir; a[curPos.x][curPos.y].state = CellState::TREE; --curPos.y; treeParentDir = Dir::DOWN ; break;
                    case Dir::RIGHT:a[curPos.x][curPos.y].parent = treeParentDir; a[curPos.x][curPos.y].state = CellState::TREE; ++curPos.x; treeParentDir = Dir::LEFT ; break;
                    case Dir::DOWN :a[curPos.x][curPos.y].parent = treeParentDir; a[curPos.x][curPos.y].state = CellState::TREE; ++curPos.y; treeParentDir = Dir::UP   ; break;
                    case Dir::NONE : exit(1); break;
                    }
                    if (curPos == start) {
                        break;
                    }
                }
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

private:
    int64_t w_;
    int64_t h_;
    std::vector<std::vector<Cell>> a;
};