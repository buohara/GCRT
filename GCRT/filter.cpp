#include "filter.h"

/**
 * [Filter::Evaluate description]
 * @param  x [description]
 * @param  y [description]
 * @return   [description]
 */

double Filter::Evaluate(double x, double y)
{
    double xInv = 1.0 / xw;
    double yInv = 1.0 / yw;

    return Evaluate1D(x * xInv) * Evaluate1D(y * yInv);
}

/**
 * [Filter::Evaluate1D description]
 * @param  val [description]
 * @return     [description]
 */

double Filter::Evaluate1D(double val)
{
    val = abs(val);

    if (val < 1.0)
    {
        return ((12.0 - 9.0 * b - 6.0 * c) * val * val * val +
            (-18.0 + 12.0 * b + 6.0 * c) * val * val +
            (6.0 - 2.0 * b)) * (1.0 / 6.0);
    }
    else if (val < 2.0)
    {
        return ((-b - 6.0 * c) * val * val * val +
            (6.0 * b + 30.0 * c) * val * val +
            (-12.0 * b - 48.0 * c) * val +
            (8.0 * b + 24.0 * c)) * (1.0 / 6.0);
    }
    else
    {
        return 0.0;
    }
}