#include "mercury/module.h"

#include "common/array.h"

#include <stdio.h>

bool module_init(Module *module, const char *name, const char *path,
		 // Array of char
		 array(char) source) {

    if (!module || !name || !path || !source)
	return false;

    module->name = strdup(name);
    module->path = strdup(path);
    module->source = source;
    module->source_lines = arr_init(StringView, 10);
    
    int begin = 0;
    int cur   = 0;
    while (cur < arr_len(source)) {
	if (source[cur] == '\n') {
	    usize length = cur - begin;
	    StringView view = sv_init(&source[begin], length);
	    arr_push(module->source_lines, view);
	    //printf("Line: %.*s\n", (int)length, view.data);
	    begin = cur + 1;
	}
	cur++;
    }
    
    return true;
}

void module_deinit(Module *module)
{
    free((void*)module->path);
    free((void*)module->name);

    arr_destroy(module->source);
    arr_destroy(module->source_lines);
    memset(module, 0, sizeof(*module));
}
