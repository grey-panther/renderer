#ifndef GEOMETRIC_MODELS_DEFINED
#define GEOMETRIC_MODELS_DEFINED

struct Point {
    double x, y, z;

    Point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
};

#endif