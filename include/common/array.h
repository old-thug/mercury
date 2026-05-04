#ifndef ARRAY_H
#define ARRAY_H

#include "common/def.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    usize elem_size;
    usize count;
    usize capacity;
} array_header_t;

/// To signal that this is an array
#define array(T) T*

#define arr_init(t, cap) (t*)arr_init_with_elem_size(sizeof(t), cap)
#define arr_push(arr, item)                                             \
    do {                                                                \
        array_header_t *hdr = (array_header_t*)(arr) - 1;               \
        if (hdr->count >= hdr->capacity) {                              \
            usize new_cap = hdr->capacity * 2;                          \
            if (new_cap == 0)                                           \
                new_cap = 40;                                           \
            void *new_buf = malloc(sizeof(array_header_t) + hdr->elem_size * new_cap); \
            memcpy(new_buf, hdr, sizeof(array_header_t) + hdr->elem_size * hdr->count); \
            free((void*)hdr);                                           \
            hdr = (array_header_t*)new_buf;                             \
            hdr->capacity = new_cap;                                    \
            arr = (typeof(&arr[0]))(hdr + 1);                           \
        }                                                               \
        (arr)[hdr->count ++] = item;                                    \
    } while (0)

inlined
void *arr_init_with_elem_size(usize elem_size, usize capacity)
{
    void *mem = malloc(sizeof(array_header_t) + elem_size * capacity);
    array_header_t *hdr = (array_header_t*)mem;
    hdr->capacity = capacity;
    hdr->count = 0;
    hdr->elem_size = elem_size;

    void *buffer = (void*)(hdr + 1);
    
    return buffer;
}

inlined
void arr_destroy(void *arr)
{
    array_header_t *hdr = (array_header_t*)arr - 1;
    free(hdr);
}

#define arr_len(arr) ({                                     \
            array_header_t *hdr = (array_header_t*)arr - 1;	\
            hdr->count;                                     \
        })

inlined
void *arr_reserve(void *arr, usize new_cap)
{
    array_header_t *hdr = (array_header_t*)arr - 1;
    if (new_cap <= hdr->capacity)
        return arr;

    array_header_t *new_buf
        = (array_header_t *)malloc(sizeof(array_header_t) + hdr->elem_size * new_cap);
    memcpy((void*)new_buf, (void*)hdr, sizeof(array_header_t) + hdr->elem_size * hdr->count);
    free((void*)hdr);
    hdr = new_buf;
    hdr->capacity = new_cap;
    arr = hdr + 1;
    return arr;
}

#endif /* ARRAY_H */
