#include <string>
#include <vector>
#include <sstream>

#include "CImg.h"
#include "types.hh"

void draw(const Cells& cells, const std::string& filename, const bool write_solution, const int CELL_SIZE = 6) {
    int w = cells.size();
    int h = cells[0].size();
    const int CELL_SIZE_2 = CELL_SIZE / 2;
    const unsigned char pink[] = {255, 100, 100};
    const unsigned char white[] = {255, 255, 255};
    const unsigned char black[] = {0, 0, 0};

    cimg_library::CImg<unsigned char> mazeim(w * CELL_SIZE + 1, h * CELL_SIZE + 1, 1, 3);
    mazeim.fill(white[0]);

    const int TOTAL_WIDTH = w * CELL_SIZE + 1;
    const int TOTAL_HEIGHT = h * CELL_SIZE + 1;
    // draw horizontal lines
    for(int i = 0; i < h + 1; ++i) {
        mazeim.draw_line(0, i * CELL_SIZE, TOTAL_WIDTH, i * CELL_SIZE, black);
    }
    // draw vertical lines
    for(int i = 0; i < w + 1; ++i) {
        mazeim.draw_line(i * CELL_SIZE, 0, i * CELL_SIZE, TOTAL_HEIGHT, black);
    }
    // draw entry
    mazeim.draw_line(0 * CELL_SIZE + 1, 0 * CELL_SIZE, (0 + 1) * CELL_SIZE - 1, 0 * CELL_SIZE, white);
    // draw exit
    mazeim.draw_line((w - 1) * CELL_SIZE + 1, h * CELL_SIZE, w * CELL_SIZE - 1, h * CELL_SIZE, white);

    for(int x = 0; x < w; ++x) {
        for(int y = 0; y < h; ++y) {
            switch(cells[x][y].parent) {
                case Dir::LEFT : mazeim.draw_line(x * CELL_SIZE, y * CELL_SIZE + 1, x * CELL_SIZE, (y + 1) * CELL_SIZE - 1, white); break;
                case Dir::UP   : mazeim.draw_line(x * CELL_SIZE + 1, y * CELL_SIZE, (x + 1) * CELL_SIZE - 1, y * CELL_SIZE, white); break;
                case Dir::RIGHT: mazeim.draw_line((x + 1) * CELL_SIZE, y * CELL_SIZE + 1, (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE - 1, white); break;
                case Dir::DOWN : mazeim.draw_line(x * CELL_SIZE + 1, (y + 1) * CELL_SIZE, (x + 1) * CELL_SIZE - 1, (y + 1) * CELL_SIZE, white); break;
                case Dir::NONE : break;
            }
        }
    }

    std::ostringstream oss;
    oss << filename << "_" << w << "x" << h << ".bmp";
    cimg_library::CImg<unsigned char> canvas(w * CELL_SIZE + 1 + CELL_SIZE * 4, h * CELL_SIZE + 1 + CELL_SIZE * 4, 1, 3);
    canvas.fill(white[0]);
    canvas.draw_image(CELL_SIZE * 2, CELL_SIZE * 2, mazeim);
    canvas.save(oss.str().c_str());

    // If no need to write solution, return
    if(!write_solution)
        return;

    
    auto hsv = cimg_library::CImg<unsigned char>::HSV_LUT256();
    unsigned char color[3] = {};
    auto setColor = [&](unsigned char idx) {
        color[0] = hsv(idx, 0, 0);
        color[1] = hsv(idx, 0, 1);
        color[2] = hsv(idx, 0, 2);
    };
    unsigned colorIdx = 0;
    setColor(colorIdx);
    mazeim.draw_line(CELL_SIZE_2, CELL_SIZE_2, CELL_SIZE_2, 0, color);
    Point cur{w - 1, h - 1};
    while(cur.x != 0 || cur.y != 0) {
        int x = cur.x;
        int y = cur.y;
        ++colorIdx;
        setColor(colorIdx);
        switch(cells[cur.x][cur.y].parent) {
            case Dir::LEFT : mazeim.draw_line((x + 1) * CELL_SIZE - CELL_SIZE_2, (y + 1) * CELL_SIZE - CELL_SIZE_2, (x    ) * CELL_SIZE - CELL_SIZE_2, (y + 1) * CELL_SIZE - CELL_SIZE_2, color); break;
            case Dir::UP   : mazeim.draw_line((x + 1) * CELL_SIZE - CELL_SIZE_2, (y + 1) * CELL_SIZE - CELL_SIZE_2, (x + 1) * CELL_SIZE - CELL_SIZE_2, (y    ) * CELL_SIZE - CELL_SIZE_2, color); break;
            case Dir::RIGHT: mazeim.draw_line((x + 1) * CELL_SIZE - CELL_SIZE_2, (y + 1) * CELL_SIZE - CELL_SIZE_2, (x + 2) * CELL_SIZE - CELL_SIZE_2, (y + 1) * CELL_SIZE - CELL_SIZE_2, color); break;
            case Dir::DOWN : mazeim.draw_line((x + 1) * CELL_SIZE - CELL_SIZE_2, (y + 1) * CELL_SIZE - CELL_SIZE_2, (x + 1) * CELL_SIZE - CELL_SIZE_2, (y + 2) * CELL_SIZE - CELL_SIZE_2, color); break;
            case Dir::NONE : break;
        }
        cur.moveto(cells[cur.x][cur.y].parent);
    }
    ++colorIdx;
    setColor(colorIdx);
    mazeim.draw_line(w * CELL_SIZE - CELL_SIZE_2, h * CELL_SIZE - CELL_SIZE_2, w * CELL_SIZE - CELL_SIZE_2, (h + 1) * CELL_SIZE - CELL_SIZE_2, pink);

    oss.str("");
    oss << filename << "_" << w << "x" << h << "_solution.bmp";
    canvas.fill(white[0]);
    canvas.draw_image(CELL_SIZE * 2, CELL_SIZE * 2, mazeim);
    canvas.save(oss.str().c_str());
}