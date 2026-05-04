#define CBUILD_IMPLEMENTATION
#define CBUILD_NO_PREFIX
#include "./cbuild.h"

#define CC_FLAGS "-Werror", "-Wall", "-I./include", "-std=c23", "-D_GNU_SOURCE"
const char *CC = "clang";
const char *CC_EXTRA_FLAG = "";

#define SOURCE_DIR "./compiler"
#define BINARY_DIR "./bin"
#define BUILD_DIR  "./build"

#define LIB_NAME "libmercury.so"
#define EXE_NAME "mercc"

void run_compiler_tests();
void run_language_tests();

int build_compiler()
{
    const char *dir = PATH(SOURCE_DIR, "mercc");
    const char *out = PATH(BUILD_DIR,  "bin");
    
    DONT_LOG(MKDIRS(out));
    DONT_LOG(MKDIRS(dir));

    Cmd bcmd = {0};
    cmd_append(&bcmd, CC, "-lmercury", "-L"BINARY_DIR, "-Wl,-rpath="BINARY_DIR);
    
    Files sources = all_files_with_extension(dir, "c", true);
    for (int n = 0; n < sources.count; ++n) {
	Cmd cmd = {0};
	cmd_append(&cmd, CC, CC_FLAGS, CC_EXTRA_FLAG);

	File file = sources.buffer[n];
	const char *output = PATH(out, replace_extension(file.name, "obj"));
	cmd_append(&cmd, "-c", file.path);
	cmd_append(&cmd, "-o", output);
	
	cmd_append(&bcmd, output);
	if (cmd_run_sync(cmd)) {
	    return 0;
	}
    }

    cmd_append(&bcmd, "-o", PATH(BINARY_DIR, EXE_NAME));
    if (!cmd_run_sync(bcmd))
	return 0;

    return 1;
}

int build_compiler_library()
{
    const char *dir = PATH(SOURCE_DIR, "lib");
    const char *out = PATH(BUILD_DIR,  "lib");
    
    DONT_LOG(MKDIRS(out));
    DONT_LOG(MKDIRS(dir));

    Cmd bcmd = {0};
    cmd_append(&bcmd, CC, "-shared");
    
    Files sources = all_files_with_extension(dir, "c", true);
    for (int n = 0; n < sources.count; ++n) {
	Cmd cmd = {0};
	cmd_append(&cmd, CC, CC_FLAGS, CC_EXTRA_FLAG);

	File file = sources.buffer[n];
	const char *output = PATH(out, replace_extension(file.name, "obj"));
	cmd_append(&cmd, "-c", file.path);
	cmd_append(&cmd, "-o", output);
	
	cmd_append(&bcmd, output);
	if (cmd_run_sync(cmd)) {
	    return 0;
	}
    }

    cmd_append(&bcmd, "-o", PATH(BINARY_DIR, LIB_NAME));
    if (!cmd_run_sync(bcmd))
	return 0;

    return 1;
}

int main(int argc, char **argv)
{
    AUTO_REBUILD(argc, argv);
    arg_set_context(argc, argv);

    if (arg_has_flag("-debug")) {
	CC_EXTRA_FLAG = CONCAT(CC_EXTRA_FLAG, "-ggdb ");
    }
    
    DONT_LOG(MKDIRS(BINARY_DIR));
    DONT_LOG(MKDIRS(BUILD_DIR));
    
    int build_lib = false;
    int build_exe = false;
    
    if (arg_has_flag("-lib"))
	build_lib = true;
    
    if (arg_has_flag("-exe")) {
	build_lib = true;
	build_exe = true;
    }
    
    if (arg_has_flag("-exe-only")) {
	build_exe = true;
    }

    if (build_lib) build_compiler_library();
    if (build_exe) build_compiler();

    if (arg_has_flag("-test-lang")) {
	run_language_tests();
    }

    if (arg_has_flag("-test-compiler")) {
	run_compiler_tests();
    }
}

void run_compiler_tests() {
    const char *dir = PATH("tests", "c");
    const char *out = PATH(dir, "out");

    DONT_LOG(MKDIRS(dir));
    DONT_LOG(MKDIRS(out));

    Files files = all_files_with_extension(dir, "c", false);
    for(int n = 0; n < files.count; ++n) {
	File file = files.buffer[n];

	const char *output = PATH(out, replace_extension(file.name, "test"));
	
	Cmd cmd = {0};
	cmd_append(&cmd, CC, "-I./include", "-std=c23", "-L"BINARY_DIR, "-Wl,-rpath="BINARY_DIR, "-lmercury");
	cmd_append(&cmd, file.path, "-o", output);
	if (cmd_run_sync(cmd)) {
	    CBUILD_LOG(ERROR, "Test (%s) failed", file.name);
	} else {
	    cmd.count = 0;
	    cmd_append(&cmd, output);
	    if (cmd_run_sync(cmd)) {
		CBUILD_LOG(ERROR, "Test (%s) failed", file.name);
	    } else {
		CBUILD_LOG(INFO, "Test (%s) passed", file.name);
	    }
	}
    }
}

void run_language_tests() {
}
