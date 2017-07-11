#ifndef RENDERER_DRAW_TOOLS_H
#define RENDERER_DRAW_TOOLS_H

#include "tgaimage.h"
#include "Point.cpp"

void drawLine(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

void drawLineWithDepthMask(int x0, int y0, int z0, int x1, int y1, int z1, int zBuffer[], TGAImage &image, TGAColor color);

void drawTriangle(Point p1, Point p2, Point p3, int zBuffer[], TGAImage &image, TGAColor color);

#endif //RENDERER_DRAW_TOOLS_H
