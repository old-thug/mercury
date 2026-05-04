#ifndef CMDLINE_H
#define CMDLINE_H

enum CompileTargetStage {
    STAGE_Executable,
    STAGE_Object,
    STAGE_Ir,
    STAGE_Asm,
};

typedef struct
{
    const char *input_file;	/* -c <path> */
    const char *output_file;	/* -o <path> */
    bool        verbose;	/* -v */
    enum CompileTargetStage
                target_stage; /* -stage:<stage> */
} Cmdline;

Cmdline cmdline_parse(int argc, char **argv);

#endif /* CMDLINE_H */
