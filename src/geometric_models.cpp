#ifndef GEOMETRIC_MODELS_DEFINED
#define GEOMETRIC_MODELS_DEFINED

struct Point {
    double x, y, z;

    Point(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    // Возвращает новую точку, каждая координата которой умножена на скаляр
    Point mult(int multiplier) {
        return Point(x * multiplier, y * multiplier, z * multiplier);
    }

    // Возвращает новую точку, к каждой координате которой прибавлен скаляр
    Point add(int addition) {
        return Point(x + addition, y + addition, z + addition);
    }
};

#endif