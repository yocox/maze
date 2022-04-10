#include <vector>
#include <cassert>
#include <iostream>
#include <random>

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

std::ostream& operator<<(std::ostream& os, Dir d) {
    switch(d) {
    case Dir::NONE : return os << "NONE ";
    case Dir::LEFT : return os << "LEFT ";
    case Dir::UP   : return os << "UP   ";
    case Dir::RIGHT: return os << "RIGHT";
    case Dir::DOWN : return os << "DOWN ";
    }
}

struct Point {
    int64_t x;
    int64_t y;
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    friend std::ostream& operator<<(std::ostream& os, const Point& p);
};

std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "Point(" << p.x << ", " << p.y << ")";
}

class Maze {
public:
    Maze(int64_t w, int64_t h)
        : w_{w}
        , h_{h}
        , a(w, std::vector<Cell>(h))
    {
    }
    
    void generate() {
        a[0][0].state = CellState::TREE;
        for (int64_t y = 0; y < h_; ++y) {
            for (int64_t x = 0; x < w_; ++x) {
                loopCancleRandomWork({x, y});
                //print("After adding new path");
            }
        }
    }
    
    Dir randomNextDir(const Point& curPos, Dir prevDir) {
        while (true) {
            Dir d = static_cast<Dir>(randDir());
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
    
    void addToTree(Dir parentDir, Point curPos, const Point& start) {
        Dir treeParentDir = parentDir;
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
    
    void randomWalkOneStep(Point& curPos, const Point& nextPos, Dir& prevDir, Dir nextDir) {
        switch(nextDir) {
        case Dir::LEFT:
            a[nextPos.x][nextPos.y].parent = Dir::RIGHT;
            a[nextPos.x][nextPos.y].state = CellState::PATH;
            break;
        case Dir::UP:
            a[nextPos.x][nextPos.y].parent = Dir::DOWN;
            a[nextPos.x][nextPos.y].state = CellState::PATH;
            break;
        case Dir::RIGHT:
            a[nextPos.x][nextPos.y].parent = Dir::LEFT;
            a[nextPos.x][nextPos.y].state = CellState::PATH;
            break;
        case Dir::DOWN:
            a[nextPos.x][nextPos.y].parent = Dir::UP;
            a[nextPos.x][nextPos.y].state = CellState::PATH;
            break;
        case Dir::NONE:
            assert(0 && "unreachable [2]");
            exit(1);
            break;
        }
        //print("After step");
        curPos = nextPos;
        prevDir = nextDir;
    }

    void loopCancleRandomWork(const Point& start) {
        if (a[start.x][start.y].state != CellState::NONE) {
            return;
        }
        Point curPos = start;
        a[curPos.x][curPos.y].state = CellState::PATH;
        Dir prevDir = Dir::NONE;
        while(true) {
            Dir nextDir = randomNextDir(curPos, prevDir);
            Point nextPos(curPos);
            switch(nextDir) {
            case Dir::LEFT : nextPos.x -= 1; break;
            case Dir::UP   : nextPos.y -= 1; break;
            case Dir::RIGHT: nextPos.x += 1; break;
            case Dir::DOWN : nextPos.y += 1; break;
            case Dir::NONE : assert(0 && "unreachable [1]"); exit(1); break;
            }
            
            // std::cout << "Current Pos: " << curPos << " nextDir:" << nextDir;
            // If next cell is tree, end the work, reverse the dir on the path to connect to the tree
            if (a[nextPos.x][nextPos.y].state == CellState::TREE) {
                // std::cout << "  Add to tree" << std::endl;
                addToTree(nextDir, curPos, start);
                //print("Added to tree");
                return;
            }
            
            // If next cell is path, cancle the loop, and continue the random walk
            else if (a[nextPos.x][nextPos.y].state == CellState::PATH) {
                // std::cout << "  cancle loop" << std::endl;
                cancleLoop(curPos, nextPos);
                curPos = nextPos;
                continue;
            }
            
            // If next cell is empty cell, work on
            else {
                // std::cout << "  step" << std::endl;
                randomWalkOneStep(curPos, nextPos, prevDir, nextDir);
            }
        }
    }
    
    void cancleLoop(const Point& from, const Point& to) {
        int count = 0;
        Point cur = from;
        while(true) {
            switch(a[cur.x][cur.y].parent) {
            case Dir::LEFT:
                a[cur.x][cur.y].parent = Dir::NONE;
                a[cur.x][cur.y].state = CellState::NONE;
                --cur.x;
                break;
            case Dir::UP:
                a[cur.x][cur.y].parent = Dir::NONE;
                a[cur.x][cur.y].state = CellState::NONE;
                --cur.y;
                break;
            case Dir::RIGHT:
                a[cur.x][cur.y].parent = Dir::NONE;
                a[cur.x][cur.y].state = CellState::NONE;
                ++cur.x;
                break;
            case Dir::DOWN:
                a[cur.x][cur.y].parent = Dir::NONE;
                a[cur.x][cur.y].state = CellState::NONE;
                ++cur.y;
                break;
            case Dir::NONE:
                a[cur.x][cur.y].parent = Dir::NONE;
                break;
            };
            //print("After cancle one cell");
            ++count;
            if (cur == to) {
                break;
            }
        }
        //std::cout << "cancle " << count << " cells" << std::endl;
    }

    void print(const std::string& title) const {
        std::cout << "====== " << title << " ======\n";
        constexpr char DIRCH[5] = {'_', '<', '^', '>', 'v'};
        constexpr char STACH[5] = {'_', 'P', 'T'};
        for (int64_t y = 0; y < h_; ++y) {
            for (int64_t x = 0; x < w_; ++x) {
                std::cout << DIRCH[static_cast<int>(a[x][y].parent)];
            }
            std::cout << std::endl;
        }
        std::cout << "---------------------\n";
        for (int64_t y = 0; y < h_; ++y) {
            for (int64_t x = 0; x < w_; ++x) {
                std::cout << STACH[static_cast<int>(a[x][y].state)];
            }
            std::cout << std::endl;
        }
        std::cout << "=====================\n";
    }

    std::uniform_int_distribution<>::result_type randDir() {
        return dirDist_(gen_);
    }

private:
    int64_t w_;
    int64_t h_;
    std::vector<std::vector<Cell>> a;
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen_{rd()}; //Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dirDist_{1, 4};
};