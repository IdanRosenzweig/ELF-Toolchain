#ifndef LOADER_PROCESS_VARS_H
#define LOADER_PROCESS_VARS_H

// process variables
struct process_vars {
    int argc;
    char **argv;
    char **env;

    process_vars() : argc(0), argv(nullptr), env(nullptr) {}
    process_vars(int argc, char **argv, char **env) : argc(argc), argv(argv), env(env) {}
};

#endif //LOADER_PROCESS_VARS_H
