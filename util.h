#ifndef UTIL_H
#define UTIL_H

#include <Halide.h>

using namespace Halide;

// normalize to value between 0 and 1, so probably not good for integer types...
// first clamps val, then rescales.
inline Expr normalize(Expr val, Expr min, Expr max) {
    return (clamp(val, min, max) - min) / (max - min);
}

inline Func linspace_func(Expr start, Expr stop, RDom r, const std::string &name = "linspace") {
    Var x{"x"};
    Expr step("step");
    step = (stop - start) / (r.x.extent() - Expr(1));
    Func linspace(name);
    linspace(x) = start;
    linspace(r) = start + r * step;
    // force "stop" value to avoid floating point deviations
    linspace(r.x.extent() - 1) = stop;
    return linspace;
}

inline Func arange_func(Expr start, Expr stop, Expr step, const std::string &name = "arange") {
    Var x{"x"};
    Expr extent("extent");
    extent = ConciseCasts::i32(ceil((stop - start) / step));
    RDom r(0, extent, "r");
    Func arange(name);
    arange(x) = start;
    arange(r) = start + r * step;
    return arange;
}

// a and b are assumed to be vectors of length 3
inline Expr cross3(Func a, Func b, Var x) {
    // // c_x = a_y * b_z − a_z * b_y
    // cross(0) = a(1) * b(2) - a(2) * b(1);
    // // c_y = a_z * b_x − a_x * b_z
    // cross(1) = a(2) * b(0) - a(0) * b(2);
    // // c_z = a_x * b_y − a_y * b_x
    // cross(2) = a(0) * b(1) - a(1) * b(0);
    return a((x + 1) % 3) * b((x + 2) % 3) - a((x + 2) % 3) * b((x + 1) % 3);
}

inline Expr hstack1(Func a, Func b, Expr extent, Var x) {
    return select(x < extent,
                  a(clamp(x, 0, extent - 1)),
                  b(clamp(x - extent, 0, extent - 1)));
}

inline Expr hstack2(Func a, Func b, Expr x_extent, Var x, Var y) {
    return select(x < x_extent,
                  a(clamp(x, 0, x_extent - 1), y),
                  b(clamp(x - x_extent, 0, x_extent - 1), y));
}

inline Expr vstack1(Func a, Func b, Expr extent, Var x, Var y) {
    return select(y == 0,
                  a(clamp(x, 0, extent - 1)),
                  b(clamp(x, 0, extent - 1)));
}

inline Expr vstack2(Func a, Func b, Expr y_extent, Var x, Var y) {
    return select(y < y_extent,
                  a(x, clamp(y, 0, y_extent - 1)),
                  b(x, clamp(y - y_extent, 0, y_extent - 1)));
}

inline Expr log2(Expr x) {
    return log(x) / log(Expr(2));
}

inline Expr log10(Expr x) {
    return log(x) / log(Expr(10));
}

inline Expr norm(Expr in) {
    return sqrt(sum(in * in));
}

inline Expr repeat1(Func a, Expr extent_a, Var x) {
    return a(x % extent_a);
}

#endif
