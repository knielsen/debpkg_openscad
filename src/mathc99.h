#ifndef MATHC99_H_
#define MATHC99_H_

#ifdef WIN32

#include <cmath>
//for native win32 builds we need to provide C99 math functions by ourselves
double round(double a);
float fmin(float a, float b);
float fmax(float a, float b);

#else

#include <math.h>

#endif

#endif
