#ifndef ELF_TOOLCHAIN_PROCESS_VARS_H
#define ELF_TOOLCHAIN_PROCESS_VARS_H

// process variables
struct process_vars {
    int argc;
    char **argv;
    char **env;

    process_vars() : argc(0), argv(nullptr), env(nullptr) {}

    process_vars(int argc, char **argv, char **env) : argc(argc), argv(argv), env(env) {}
};

#endif //ELF_TOOLCHAIN_PROCESS_VARS_H
