#include "mercury/context.h"
#include "mercury/lexer/token.h"

#include "common/array.h"
#include "common/fs.h"

const char *context_get_error(Context *context) { return context->err; }
void context_set_error(Context *context, const char *err)
{
    if (context->err)
	free((void*)context->err);
    context->err = err;
}

Context context_init()
{
    Context context = {0};
    context.modules = arr_init(Module, 20);
    return context;
}

void context_deinit(Context *context)
{
    for (int n = 0; n < arr_len(context->modules); ++n) {
	Module *mod = &context->modules[n];
	module_deinit(mod);
    }
    
    arr_destroy(context->modules);
    free((void*)context->err);
}

ModuleId context_open_module(Context *context, const char *name, const char *path)
{
    if (!fs_exists(path)) {
	context_set_error(context, format("No such file or directory: `%s`", path));
	return MERC_INVALID_MODULE;
    }
    
    const char *canon = fs_canonicalpath(path);
    char *source = arr_init(char, 1024);
    if (fs_read_entire_file(canon, &source) == -1) {
	context_set_error(context, format("Failed to read file at `%s`", path));
	return MERC_INVALID_MODULE;
    }
    
    Module mod = {0};
    arr_push(context->modules, mod);
    ModuleId id = arr_len(context->modules);
    module_init(&context->modules[arr_len(context->modules) - 1], name, canon, source);

    free((void*)canon);
    return id;
}
