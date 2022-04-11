#pragma once
#include <vector>
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
    bool operator==(const Point& rhs) const {
        return x == rhs.x && y == rhs.y;
    }
    Point moveto(const Dir d) noexcept {
        switch(d) {
            case Dir::NONE :      return *this; break;
            case Dir::LEFT : --x; return *this; break;
            case Dir::UP   : --y; return *this; break;
            case Dir::RIGHT: ++x; return *this; break;
            case Dir::DOWN : ++y; return *this; break;
        }
        return Point{0, 0};
    }
    int64_t x;
    int64_t y;
};

using Cells = std::vector<std::vector<Cell>>;

inline std::ostream& operator<<(std::ostream& os, Dir d) {
    switch(d) {
    case Dir::NONE : return os << "NONE ";
    case Dir::LEFT : return os << "LEFT ";
    case Dir::UP   : return os << "UP   ";
    case Dir::RIGHT: return os << "RIGHT";
    case Dir::DOWN : return os << "DOWN ";
    }
}

inline std::ostream& operator<<(std::ostream& os, const Point& p) {
    return os << "Point(" << p.x << ", " << p.y << ")";
}
