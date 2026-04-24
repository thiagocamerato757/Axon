/*
This file defines common types used throughout the project. 
It includes fixed-width integer types, floating-point types, and boolean 
types for consistency and readability.
*/
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h> 
#include <stdbool.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   i8; 
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;
typedef uint8_t  b8;
typedef uint32_t b32;

#endif // TYPES_H