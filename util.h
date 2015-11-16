#ifndef UTIL_H
#define UTIL_H

#include <math.h>

#define EPSILON 10e-16

inline bool __attribute__((always_inline))__attribute__((always_inline)) isZero(double x)  { return fabs(x) < EPSILON; }

#endif // UTIL_H
