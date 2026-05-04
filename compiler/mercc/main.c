#include "common/array.h"

#include "mercury/context.h"
#include "merc/cmdline.h"

int main(int argc, char **argv)
{
    Cmdline cmdline = cmdline_parse(argc, argv);
    (void)cmdline;
    
    Context context = context_init();
    ModuleId root_mod = context_open_module(&context, "root", cmdline.input_file);
    if (root_mod == MERC_INVALID_MODULE) {
	fprintf(stderr, "error: %s\n", context_get_error(&context));
	return 1;
    }

    context_deinit(&context);
    return 0;
}
