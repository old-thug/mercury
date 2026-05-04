#ifndef DEF_H
#define DEF_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define min(a, b) ((a < b)? a: b)
#define max(a, b) ((a > b)? a: b)

#define _first_arg(arg, ...) arg
#define _rest_arg(arg, ...) __VA_ARGS__

#define todo(...) do {                                                  \
        fprintf(stderr, "%s:%d: todo: " _first_arg(__VA_ARGS__) "\n", __FILE__, __LINE__ _rest_arg(__VA_ARGS__)); \
        abort();                                                        \
    } while (0)

typedef int8_t   s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef size_t usize;
typedef ptrdiff_t ssize;

#define inlined static inline

inlined
const char *format(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    auto size = vsnprintf(nullptr, 0, fmt, args);
    char *buffer = (char*)malloc(size + 1);
    
    va_start(args, fmt);
    size = vsnprintf(buffer, size + 1, fmt, args);
    buffer[size] = 0;
    return buffer;
}

#endif /* DEF_H */
