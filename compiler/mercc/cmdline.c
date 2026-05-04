#include "merc/cmdline.h"

#include "common/array.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

enum FlagType {
    FLAG_String,
    FLAG_Boolean,
    FLAG_FunctionStr,
    FLAG_FunctionVoid,
};

struct FStr {
    void *context;
    bool (*fn)(void *context, const char *);
};

typedef bool (*flag_str_t)(void *context, const char *);
typedef bool (*flag_void_t)(void *context);

struct Flag {
    const char *name;
    enum FlagType type;
    void *out;
    void *context;
};

static void parse_flags(struct Flag *flags, int argc, char **argv, struct FStr anon)
{
    int counter = 1;
    while (counter < argc) {
        const char *arg = argv[counter++];
        if (arg[0] == '-') {
            bool found = false;
            const char *fname = arg + 1;

            for (int n = 0; n < arr_len(flags); ++n) {
                struct Flag *flag = &flags[n];
                const char *farg = nullptr;
                size_t name_len = strlen(flag->name);

                // Case 1: Exact match (e.g., -c filename)
                if (strcmp(flag->name, fname) == 0) {
                    if (flag->type == FLAG_String || flag->type == FLAG_FunctionStr) {
                        if (counter >= argc) {
                            fprintf(stderr, "error: `-%s` expected an argument\n", flag->name);
                            exit(1);
                        }
                        farg = argv[counter++];
                    }
                    found = true;
                } 
                // Case 2: Inline match (e.g., -c:filename or -c=filename)
                else if (strncmp(flag->name, fname, name_len) == 0) {
                    char sep = fname[name_len];
                    if (sep == ':' || sep == '=') {
                        farg = fname + name_len + 1;
                        if (*farg == '\0') {
                            fprintf(stderr, "error: `-%s` expected an argument after '%c'\n", flag->name, sep);
                            exit(1);
                        }
                        found = true;
                    }
                }

                if (found) {
                    switch (flag->type) {
                        case FLAG_String: {
                            const char **out = (const char **)flag->out;
                            *out = farg;
                        } break;
                        case FLAG_FunctionStr: {
                            flag_str_t fn = (flag_str_t)flag->out;
                            if (!fn(flag->context, farg)) {
                                exit(1);
                            }
                        } break;
                        case FLAG_Boolean: {
                            bool *out = (bool *)flag->out;
                            *out = true;
                        } break;
                        case FLAG_FunctionVoid: {
                            flag_void_t fn = (flag_void_t)flag->out;
                            if (!fn(flag->context)) {
                                exit(1);
                            }
                        } break;
                    }
                    break; 
                }
            }

            if (!found) {
                fprintf(stderr, "error: invalid argument: `%s`\n", arg);
                exit(1);
            }
        } else {
            if (!anon.fn(anon.context, arg)) {
                exit(1);
            }
        }
    }
}

bool anon_argument(void *context, const char *arg)
{
    const char **input_file = (const char **)context;
    if (*input_file != nullptr) {
        fprintf(stderr, "error: invalid argument: `%s` (input file already set to `%s`)\n", arg, *input_file);
        return false;
    }
    *input_file = arg;
    return true;
}

/// -C <name>[:=]<value>
bool parse_codegen_opt(void *context, const char *flag)
{
    Cmdline *cmdline = (Cmdline*)context;
    const char **codegen_opts = arr_init(const char *, 2);
    // Non boolean options
    arr_push(codegen_opts, "stage");

    // TODO: make this easier to maintain
    
    for(int n = 0; n < arr_len(codegen_opts); ++n) {
	if (!strncmp(flag, codegen_opts[n], strlen(codegen_opts[n]))) {
	    const char *sep = flag + strlen(codegen_opts[n]);
	    if (*sep != ':' && *sep != '=') {
		goto failure;
	    }
	    const char *arg = sep + 1;
	    if (!strcmp(codegen_opts[n], "stage")) {
		if (!strcmp(arg, "asm"))
		    cmdline->target_stage = STAGE_Asm;
		else if (!strcmp(arg, "ir"))
		    cmdline->target_stage = STAGE_Ir;
		else if (!strcmp(arg, "exe"))
		    cmdline->target_stage = STAGE_Executable;
		else if (!strcmp(arg, "obj"))
		    cmdline->target_stage = STAGE_Object;
		else {
		    fprintf(stderr, "error: option `stage` expected one of { exe | asm | obj | ir } not `%s`\n", arg);
		    goto failure;
		}
		
		goto success;
	    }
	}
    }
    // ===========================

 failure:
    arr_destroy(codegen_opts);
    return false;
    
 success:
    arr_destroy(codegen_opts);
    return true;
}

Cmdline cmdline_parse(int argc, char **argv)
{
    Cmdline cmdline = {0};
    struct Flag *flags = arr_init(struct Flag, 5);

    arr_push(flags, ((struct Flag) { "c", FLAG_String, &cmdline.input_file }));
    arr_push(flags, ((struct Flag) { "o", FLAG_String, &cmdline.output_file }));
    arr_push(flags, ((struct Flag) { "v", FLAG_Boolean, &cmdline.verbose }));
    arr_push(flags, ((struct Flag) { "C", FLAG_FunctionStr, (void*)parse_codegen_opt, &cmdline }));
    
    parse_flags(flags, argc, argv, (struct FStr) { &cmdline.input_file, anon_argument });
    arr_destroy(flags);
    return cmdline;
}
