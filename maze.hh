#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <unordered_set>
#include <CImg.h>

namespace maze {

enum class D {
    NONE, LEFT, UP, RIGHT, DOWN
};

struct Point {
    int x;
    int y;
    Point(const int x, const int y) noexcept : x(x), y(y) {}
    bool operator==(const Point& rhs) const noexcept {
        return this->x == rhs.x && this->y == rhs.y;
    }
    bool operator!=(const Point& rhs) const noexcept {
        return ! (*this == rhs);
    }
    static Point rand(const int W, const int H) noexcept {
        return Point{std::rand() % W + 1, std::rand() % H + 1};
    }
    Point go(const D d) const noexcept {
        switch(d) {
            case D::NONE : return Point(x    , y    ); break;
            case D::LEFT : return Point(x + 1, y    ); break;
            case D::UP   : return Point(x    , y - 1); break;
            case D::RIGHT: return Point(x - 1, y    ); break;
            case D::DOWN : return Point(x    , y + 1); break;
        }
    }
    Point to(const D d) noexcept {
        switch(d) {
            case D::NONE :      return *this; break;
            case D::LEFT : --x; return *this; break;
            case D::UP   : --y; return *this; break;
            case D::RIGHT: ++x; return *this; break;
            case D::DOWN : ++y; return *this; break;
        }
    }
} ;

} // namespace

namespace std {
    template <>
    struct hash<maze::Point> {
        size_t operator()(const maze::Point& p) const noexcept {
            return p.x * 107 + p.y;
        } ;
    } ;
}

inline std::wostream& operator<<(std::wostream& os, const maze::Point& p) noexcept {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

namespace maze {

class Maze {
  public:
    Maze(const int w, const int h)
      : w_(w)
      , h_(h)
      , m_(w_ + 2, std::vector<D>(h_ + 2, D::NONE))
    {
        init();
    }

    void init() noexcept {
        for (int y = 1; y < h_ + 1; ++y) {
            m_[1][y] = D::UP;
            for (int x = 2; x < w_ + 1; ++x) {
                m_[x][y] = D::LEFT;
            }
        }
        m_[1][1] = D::NONE;
    }

    void print(std::wostream& os = std::wcout) noexcept {
        for (int y = 1; y < h_ + 1; ++y) {
            for (int x = 1; x < w_ + 1; ++x) {
                switch(m_[x][y]) {
                    case D::NONE : os << L" "; break;
                    case D::LEFT : os << L"<"; break;
                    case D::UP   : os << L"^"; break;
                    case D::RIGHT: os << L">"; break;
                    case D::DOWN : os << L"v"; break;
                }
            }
            std::wcout << std::endl;
        }
    }

    bool is_parent_child(const Point& parent, const Point& child) noexcept {
        Point cur {child};
        while(m_[cur.x][cur.y] != D::NONE) {
            switch(m_[cur.x][cur.y]) {
                case D::NONE : return false; break;
                case D::LEFT : --cur.x; break;
                case D::UP   : --cur.y; break;
                case D::RIGHT: ++cur.x; break;
                case D::DOWN : ++cur.y; break;
            }
            if(parent == cur) {
                return true;
            }
        }
        return false;
    }

    std::pair<Point, Point> select_random_switch_point() noexcept {
        Point p = Point::rand(w_, h_);
        return select_random_switch_point(p);
    }

    std::pair<Point, Point> select_random_switch_point(const Point& from) noexcept {
        Point p = from;
        int try_count = 0;
        while (p != Point{0, 0}) {
            ++try_count;
            if(try_count == 4) {
                p = Point::rand(w_, h_);
                try_count = 0;
            }
            D d = static_cast<D>(rand() % 4 + 1);
            Point p2 = p.go(d);
            if (p2.x == 0 || p2.x == w_ + 1 || p2.y == 0 || p2.y == h_ + 1) {
                continue;
            }
            else if (p2.go(m_[p2.x][p2.y]) == p || p.go(m_[p.x][p.y]) == p2) {
                continue;
            } else {
                return {p, p2};
            }
            p = Point::rand(w_, h_);
        }
        return {Point{0, 0}, Point{0, 0}};
    }

    void inverse_path(std::vector<Point>::const_iterator b, std::vector<Point>::const_iterator e) noexcept {
        for(auto i = e; i >= b; --i) {
            const auto& p = *i;
            if(p == Point{1, 2} && m_[1][2] == D::UP)
                continue;
            if(p == Point{2, 1} && m_[2][1] == D::LEFT)
                continue;
            switch(m_[p.x][p.y]) {
                case D::UP   : m_[p.x    ][p.y - 1] = D::DOWN ; break;
                case D::DOWN : m_[p.x    ][p.y + 1] = D::UP   ; break;
                case D::LEFT : m_[p.x - 1][p.y    ] = D::RIGHT; break;
                case D::RIGHT: m_[p.x + 1][p.y    ] = D::LEFT ; break;
                case D::NONE : assert(false);
            }
        }
    }

    void connect_pp(const std::pair<Point, Point>& pp) noexcept {
        if(pp.second == Point{1, 1})
            return;

        if(pp.second.x == pp.first.x - 1) {
            m_[pp.second.x][pp.second.y] = D::RIGHT;
        }
        else if(pp.second.x == pp.first.x + 1) {
            m_[pp.second.x][pp.second.y] = D::LEFT;
        }
        else if(pp.second.y == pp.first.y - 1) {
            m_[pp.second.x][pp.second.y] = D::DOWN;
        }
        else if(pp.second.y == pp.first.y + 1) {
            m_[pp.second.x][pp.second.y] = D::UP;
        }
    }

    bool switch_path() noexcept {
        return switch_path_with_point_pair(select_random_switch_point());
    }

    bool switch_path(const Point& from) noexcept {
        return switch_path_with_point_pair(select_random_switch_point(from));
    }

    bool switch_path_with_point_pair(const std::pair<Point, Point>& ppair) {
        std::pair<Point, Point> pp = ppair;
        while (is_parent_child(pp.second, pp.first) || pp.first.go(m_[pp.first.x][pp.first.y]) == pp.second) {
            pp = select_random_switch_point();
        }

        std::unordered_set<Point> path1;
        Point cur = pp.first;
        while (true) {
            path1.insert(cur);
            if(m_[cur.x][cur.y] == D::NONE) {
                break;
            }
            cur.to(m_[cur.x][cur.y]);
        }

        std::vector<Point> path2;
        cur = pp.second;
        while (true) {
            if(m_[cur.x][cur.y] == D::NONE) {
                break;
            }
            if(path1.count(cur)) {
                break;
            }
            path2.push_back(cur);
            cur.to(m_[cur.x][cur.y]);
        }

        int cut_point_pos = path2.size() - 2;
        if(!path2.empty()) {
            inverse_path(path2.begin() + 0, path2.begin() + cut_point_pos);
        }

        connect_pp(pp);
        return true;
    }

    void random_shuffle(const bool print_progress = false) {
        if(print_progress)
            std::wcout << L"initializing random shuffle..." << std::endl;

        std::vector<Point> ps;
        ps.reserve(w_ * h_);
        for(int x = 1; x < w_ + 1; ++x) {
            for(int y = 1; y < h_ + 1; ++y) {
                ps.emplace_back(x, y);
            }
        }

        std::random_shuffle(ps.begin(), ps.end());
        for(int i = 0; i < w_ * h_; ++i) {
            if(print_progress) {
                std::wcout << L"random shuffling... " << (i + 1) * 100 / (w_ * h_) << L"%\r";
                std::wcout.flush();
            }
            switch_path(ps[i]);
        }

        if(print_progress)
            std::wcout << std::endl;
    }

    void save_img(const std::string& filename, const int CELL_SIZE = 6) {
        const int CELL_SIZE_2 = CELL_SIZE / 2;
        const unsigned char pink[] = {255, 100, 100};
        const unsigned char white[] = {255, 255, 255};
        const unsigned char black[] = {0, 0, 0};

        cimg_library::CImg<unsigned char> im(w_ * CELL_SIZE + 1, h_ * CELL_SIZE + 1, 1, 3);
        im.fill(white[0]);

        const int TOTAL_WIDTH = w_ * CELL_SIZE + 1;
        const int TOTAL_HEIGHT = h_ * CELL_SIZE + 1;
        for(int i = 0; i < h_ + 1; ++i) {
            im.draw_line(0, i * CELL_SIZE, TOTAL_WIDTH, i * CELL_SIZE, black);
        }
        for(int i = 0; i < w_ + 1; ++i) {
            im.draw_line(i * CELL_SIZE, 0, i * CELL_SIZE, TOTAL_HEIGHT, black);
        }

        for(int x = 0; x < w_; ++x) {
            for(int y = 0; y < h_; ++y) {
                switch(m_[x + 1][y + 1]) {
                    case D::LEFT : im.draw_line(x * CELL_SIZE, y * CELL_SIZE + 1, x * CELL_SIZE, (y + 1) * CELL_SIZE - 1, white); break;
                    case D::UP   : im.draw_line(x * CELL_SIZE + 1, y * CELL_SIZE, (x + 1) * CELL_SIZE - 1, y * CELL_SIZE, white); break;
                    case D::RIGHT: im.draw_line((x + 1) * CELL_SIZE, y * CELL_SIZE + 1, (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE - 1, white); break;
                    case D::DOWN : im.draw_line(x * CELL_SIZE + 1, (y + 1) * CELL_SIZE, (x + 1) * CELL_SIZE - 1, (y + 1) * CELL_SIZE, white); break;
                    case D::NONE : break;
                }
            }
        }

        std::ostringstream oss;
        oss << filename << "_" << w_ << "x" << h_ << ".png";
        im.save(oss.str().c_str());

        Point cur{w_, h_};
        while(cur.x != 1 || cur.y != 1) {
            int x = cur.x;
            int y = cur.y;
            switch(m_[cur.x][cur.y]) {
                case D::LEFT : im.draw_line(x * CELL_SIZE - CELL_SIZE_2, y * CELL_SIZE - CELL_SIZE_2, (x - 1) * CELL_SIZE - CELL_SIZE_2, y       * CELL_SIZE - CELL_SIZE_2, pink); break;
                case D::UP   : im.draw_line(x * CELL_SIZE - CELL_SIZE_2, y * CELL_SIZE - CELL_SIZE_2, x       * CELL_SIZE - CELL_SIZE_2, (y - 1) * CELL_SIZE - CELL_SIZE_2, pink); break;
                case D::RIGHT: im.draw_line(x * CELL_SIZE - CELL_SIZE_2, y * CELL_SIZE - CELL_SIZE_2, (x + 1) * CELL_SIZE - CELL_SIZE_2, y       * CELL_SIZE - CELL_SIZE_2, pink); break;
                case D::DOWN : im.draw_line(x * CELL_SIZE - CELL_SIZE_2, y * CELL_SIZE - CELL_SIZE_2, x       * CELL_SIZE - CELL_SIZE_2, (y + 1) * CELL_SIZE - CELL_SIZE_2, pink); break;
                case D::NONE : break;
            }
            cur.to(m_[cur.x][cur.y]);
        }

        oss.str("");
        oss << filename << "_" << w_ << "x" << h_ << "_solution.png";
        im.save(oss.str().c_str());
    }

  private:
    int w_;
    int h_;
    std::vector<std::vector<D>> m_;
} ;

} // namespace maze
