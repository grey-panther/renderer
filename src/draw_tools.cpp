#include <math.h>
#include "tgaimage.h"
#include "draw_tools.h"

// С int вычислениями
void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool coordsSwapped = false;

    if (abs(x1 - x0) < abs(y1 - y0)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        coordsSwapped = true;
    }

    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int y = y0;
    int dx = x1 - x0;
    int dx2 = dx * 2;
    int errorY = 0;
    bool increaseY = (y1 >= y0);
    int dErrorY = increaseY ? 2 * (y1 - y0) : 2 * (y0 - y1); // dErrorY > 0

    for (int x = x0; x <= x1; x++) {
        if (coordsSwapped) {
            image.set(y, x, color);
        }
        else {
            image.set(x, y, color);
        }

        errorY += dErrorY;
        if (errorY >= dx)
        {
            y += increaseY ? 1 : -1;

            errorY -= dx2;
        }
    }
}

// С float вычислениями
//void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
//    if (x0 > x1) {
//        std::swap(x0, x1);
//    }
//    if (y0 > y1) {
//        std::swap(y0, y1);
//    }
//
//    bool coordsSwapped = false;
//    if ((x1 - x0) < (y1 - y0)) {
//        std::swap(x0, y0);
//        std::swap(x1, y1);
//        coordsSwapped = true;
//    }
//
//    int y = y0;
//    int dx = x1 - x0;
//    int errorY = 0;
//    float dy = (y1 - y0) / float(dx);
//
//    for (int x = x0; x <= x1; x++) {
//        if (coordsSwapped) {
//            image.set(y, x, color);
//        }
//        else {
//            image.set(x, y, color);
//        }
//
//        errorY += dy;
//        if (errorY >= 0.5)
//        {
//            y++;
//            errorY -= 1;
//        }
//    }
//}