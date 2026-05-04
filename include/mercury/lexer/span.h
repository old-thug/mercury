#ifndef SPAN_H
#define SPAN_H

#include "common/def.h"

typedef struct {
    usize line;
    usize begin;
    usize end;

    // Id of Module origin
    ssize id;
} Span;

inlined
Span span_init(usize line, usize begin, usize end, ssize id)
{
    return (Span) { line, begin, end, id };
}

inlined
usize span_length(Span span)
{
    return span.end - span.begin;
}

#endif /* SPAN_H */
