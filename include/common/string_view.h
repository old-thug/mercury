#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include "./def.h"

#include <string.h>

typedef struct {
    usize length;
    const char* data;
} StringView;

inlined
StringView sv_init(const char *data, usize length) {
    StringView view = {0};
    view.data = data;
    view.length = length;
    return view;
}

inlined
bool sv_equals(StringView s1, StringView s2) {
    if (s1.length != s2.length)
	return false;
    return !strncmp(s1.data, s2.data, s1.length);
}

inlined
bool sv_equals_str(StringView s1, const char *s2) {
    if (s1.length != strlen(s2))
	return false;
    return !strncmp(s1.data, s2, s1.length);
}

#endif /* STRING_VIEW_H */
