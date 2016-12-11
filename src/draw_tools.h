#ifndef RENDERER_DRAW_TOOLS_H
#define RENDERER_DRAW_TOOLS_H

#include "tgaimage.h"
#include "geometric_models.cpp"

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void drawTriangle(Point p1, Point p2, Point p3, TGAImage &image, TGAColor color);

#endif //RENDERER_DRAW_TOOLS_H
