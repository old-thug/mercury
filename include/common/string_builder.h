#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include "./def.h"

typedef struct {
    char *buffer;
    usize count;
    usize capacity;
} StringBuilder;

inlined
void sb__resize_if_needed(StringBuilder *sb)
{
    if (sb->count >= sb->capacity) {
	usize new_cap = sb->capacity *2;
	if (new_cap == 0)
	    new_cap = 50;

	char *new_buf
	    = (char *)malloc(new_cap);
	memcpy((void*)new_buf, (void*)sb->buffer, sb.count);
	free((void*)sb->buffer);
	sb->buffer = new_buf;
	sb->capacity = new_cap;
    }
}

inlined void sb_reserve(StringBuilder *sb, usize new_cap)
{
    if (new_cap <= sb->capacity)
	return;

    char *new_buf
	= (char *)malloc(new_cap);
    memcpy((void*)new_buf, (void*)sb->buffer, sb.count);
    free((void*)sb->buffer);
    sb->buffer = new_buf;
    sb->capacity = new_cap;
}

inlined
void sb_append_char(StringBuilder *sb, char ch)
{
    sb__resize_if_needed(sb);
    sb->buffer[sb->count ++] = ch;
}

inlined
void sb_append(StringBuilder *sb, const char *str)
{
    sb_reserve(strlen(sb) + sb->count);
    while (*str)
	sb_append_char(sb, *str++);
}

inlined
void sb_destroy(StringBuilder *sb)
{
    if (sb->buffer)
	free((void*)sb->buffer);
    sb->count = 0;
    sb->capacity = 0;
    sb->buffer = nullptr;
}

#endif /* STRING_BUILDER_H */
