#ifndef RENDER_COMMON_H
#define RENDER_COMMON_H

#ifdef _MSC_VER
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
typedef unsigned __int64 uint64_t;
#else
#include <stdint.h>
#endif

#include <stdbool.h>

#ifndef __FUNCTION_NAME__
#ifdef WIN32
#define __FUNCTION_NAME__ __FUNCTION__
#else
#define __FUNCTION_NAME__ __func__
#endif
#endif

#ifndef NULL
#define NULL 0
#endif

#define RADIAN(degree) degree * 3.14159265358979323846264338327950288 / 180
#define CONFINE(value, min, max) fmaxl(fminl(value, max), min)

typedef long double Real;

#define UNUSED(x) (void)(x)

#endif // RENDER_COMMON_H
