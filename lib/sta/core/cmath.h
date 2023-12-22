#ifndef _STA_MATH_
#define _STA_MATH_

#include "sta.h"
#include <math.h>

BEGIN_NP_BLOCK

#undef abs
#undef acos
#undef asin
#undef atan
#undef atan2
#undef ceil
#undef cos
#undef cosh
#undef exp
#undef fabs
#undef floor
#undef fmod
#undef frexp
#undef ldexp
#undef log
#undef log10
#undef modf
#undef pow
#undef sin
#undef sinh
#undef sqrt
#undef tan
#undef tanh

using ::acos;
using ::asin;
using ::atan;
using ::atan2;
using ::ceil;
using ::cos;
using ::cosh;
using ::exp;
using ::fabs;
using ::floor;
using ::fmod;
using ::frexp;
using ::ldexp;
using ::log;
using ::log10;
using ::modf;
using ::pow;
using ::sin;
using ::sinh;
using ::sqrt;
using ::tan;
using ::tanh;

#ifndef __CORRECT_ISO_CPP_MATH_H_PROTO
inline sta::f32 abs(sta::f32 x) {
	return fabsf(x);
}
inline sta::f32 acos(sta::f32 x) {
	return acosf(x);
}
inline sta::f32 asin(sta::f32 x) {
	return asinf(x);
}
inline sta::f32 atan(sta::f32 x) {
	return atanf(x);
}
inline sta::f32 atan2(sta::f32 y, sta::f32 x) {
	return atan2f(y, x);
}
inline sta::f32 ceil(sta::f32 x) {
	return ceilf(x);
}
inline sta::f32 cos(sta::f32 x) {
	return cosf(x);
}
inline sta::f32 cosh(sta::f32 x) {
	return coshf(x);
}
inline sta::f32 exp(sta::f32 x) {
	return expf(x);
}
inline sta::f32 fabs(sta::f32 x) {
	return fabsf(x);
}
inline sta::f32 floor(sta::f32 x) {
	return floorf(x);
}
inline sta::f32 fmod(sta::f32 x, sta::f32 y) {
	return fmodf(x, y);
}
inline sta::f32 frexp(sta::f32 x, sta::int32* exp) {
	return frexpf(x, exp);
}
inline sta::f32 ldexp(sta::f32 x, sta::int32 exp) {
	return ldexpf(x, exp);
}
inline sta::f32 log(sta::f32 x) {
	return logf(x);
}
inline sta::f32 log10(sta::f32 x) {
	return log10f(x);
}
inline sta::f32 modf(sta::f32 x, sta::f32* inptr) {
	return modff(x, inptr);
}
inline sta::f32 pow(sta::f32 x, sta::f32 y) {
	return powf(x, y);
}
#if 1 // DR 550 removed this
inline sta::f32 pow(sta::f32 x, sta::int32 y) {
	return pow((sta::f64)x, (sta::f64)y);
}
#endif
inline sta::f32 sin(sta::f32 x) {
	return sinf(x);
}
inline sta::f32 sinh(sta::f32 x) {
	return sinhf(x);
}
inline sta::f32 sqrt(sta::f32 x) {
	return sqrtf(x);
}
inline sta::f32 tan(sta::f32 x) {
	return tanf(x);
}
inline sta::f32 tanh(sta::f32 x) {
	return tanhf(x);
}
inline sta::f64 abs(sta::f64 x) {
	return fabs(x);
}
inline sta::f64 pow(sta::f64 x, sta::int32 y) {
	return pow((sta::f64)x, (sta::f64)y);
}

# ifdef __STALIBCXX_HAS_LONG_DOUBLE__
inline sta::f128 abs(sta::f128 x) {
	return fabsl(x);
}
inline sta::f128 acos(sta::f128 x) {
	return acosl(x);
}
inline sta::f128 asin(sta::f128 x) {
	return asinl(x);
}
inline sta::f128 atan(sta::f128 x) {
	return atanl(x);
}
inline sta::f128 atan2(sta::f128 y, sta::f128 x) {
	return atan2l(y, x);
}
inline sta::f128 ceil(sta::f128 x) {
	return ceill(x);
}
inline sta::f128 cos(sta::f128 x) {
	return cosl(x);
}
inline sta::f128 cosh(sta::f128 x) {
	return coshl(x);
}
inline sta::f128 exp(sta::f128 x) {
	return expl(x);
}
inline sta::f128 fabs(sta::f128 x) {
	return fabsl(x);
}
inline sta::f128 floor(sta::f128 x) {
	return floorl(x);
}
inline sta::f128 frexp(sta::f128 x, sta::int32* exp) {
	return frexpl(x, exp);
}
inline sta::f128 fmod(sta::f128 x, sta::f128 y) {
	return fmodl(x, y);
}
inline sta::f128 ldexp(sta::f128 x, sta::int32 y) {
	return ldexpl(x, y);
}
inline sta::f128 log(sta::f128 x) {
	return logl(x);
}
inline sta::f128 log10(sta::f128 x) {
	return log10l(x);
}
inline sta::f128 modf(sta::f128 x, sta::f128* iptr) {
	return modfl(x, iptr);
}
inline sta::f128 pow(sta::f128 x, sta::f128 y) {
	return powl(x, y);
}
inline sta::f128 pow(sta::f128 x, sta::int32 y) {
	return powl(x, (sta::f128)y);
}
inline sta::f128 sin(sta::f128 x) {
	return sinl(x);
}
inline sta::f128 sinh(sta::f128 x) {
	return sinhl(x);
}
inline sta::f128 sqrt(sta::f128 x) {
	return sqrtl(x);
}
inline sta::f128 tan(sta::f128 x) {
	return tanl(x);
}
inline sta::f128 tanh(sta::f128 x) {
	return tanhl(x);
}
# endif	
#endif

END_NP_BLOCK

#endif