#ifndef CONTEXT_H
#define CONTEXT_H

#include "mercury/module.h"

#define MERC_INVALID_MODULE -1

typedef struct {
    Module *modules;
    const char *err;
} Context;

Context context_init();
void context_deinit(Context *context);
ModuleId context_open_module(Context *context, const char *name, const char *path);
const char *context_get_error(Context *context);
void context_set_error(Context *context, const char *);
#endif /* CONTEXT_H */
