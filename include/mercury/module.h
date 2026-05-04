#ifndef MODULE_H
#define MODULE_H

#include "common/string_view.h"
#include "common/array.h"

typedef ssize ModuleId;
typedef struct {
    const char *name;
    const char *path;
    char *source;
    StringView *source_lines;
} Module;

bool module_init(Module *module, const char *name, const char *path,
		 array(char) source);
void module_deinit(Module *module);

#endif /* MODULE_H */
