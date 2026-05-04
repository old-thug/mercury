#ifndef TEST_H
#define TEST_H

#include "common/def.h"

array(char) create_source(const char *source)
{
    int len = strlen(source);
    array(char) arr = arr_init(char, len);

    for(int n = 0; n < len; ++n)
        arr_push(arr, source[n]);
    return arr;
}

#endif /* TEST_H */
