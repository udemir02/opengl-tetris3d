#if !defined(HEADER_BASE_H)
#define HEADER_BASE_H

#include <stddef.h>

typedef signed char  S8;
typedef signed short S16;
typedef signed int   S32;
typedef signed long  S64;

typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned int   U32;
typedef unsigned long  U64;

typedef float  F32;
typedef double F64;

typedef int B32;

#define local    static
#define global   static
//#define internal static

#define KILOBYTES(x) x * 1024LL
#define MEGABYTES(x) KILOBYTES(x) * 1024LL
#define GIGABYTES(x) MEGABYTES(x) * 1024LL

#endif
