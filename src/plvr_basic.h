#ifndef PLRV_BASIC_H
#define PLRV_BASIC_H

#include <stdint.h>

// unsigned ints
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
// signed ints
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
// NOTE: For bool types I just use int with a meaningful name
//       (or one of the signed int types if size matters)
// NOTE: For float32 and float64, I just use float and double

// https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Function-Attributes.html
#define macro inline __attribute__((always_inline))
#define unused __attribute__ ((unused))
// #define unused

// NOTE: Debug output
// Below are ANSI escape codes to change text color
#define ANSI_RED "\033[0;31m"
#define ANSI_BLUE "\33[0;34m"
#define ANSI_GREEN "\33[0;32m"
#define ANSI_RESET "\033[39;39m"

#endif  // PLRV_BASIC_H
