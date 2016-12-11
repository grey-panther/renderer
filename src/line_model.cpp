
#include <limits>
#include <cmath>

static const double NaN = std::numeric_limits<double>::quiet_NaN();

class Line2DModel {
private:
    double k;
    double b;
    bool degenerate = false;

public:
    Line2DModel(double x1, double y1, double x2, double y2) {
        if (double_equals(x1, x2)) {
            degenerate = true;
            return;
        }

        k = (y2 - y1) / (x2 - x1);
        b = y1 - k * x1;
    }

    double calculateY(double x) {
        if (degenerate) {
            return NaN;
        }

        return k * x + b;
    }

private:
    bool double_equals(double a, double b, double epsilon = 0.001)
    {
        return std::abs(a - b) < epsilon;
    }
};