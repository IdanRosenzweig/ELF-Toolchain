#ifndef LOADER_PROC_VAR_H
#define LOADER_PROC_VAR_H

struct proc_var { // process variables
    int argc;
    char **argv;
    char **env;

    proc_var() : argc(0), argv(nullptr), env(nullptr) {}

    proc_var(int argc, char **argv, char **env) : argc(argc), argv(argv), env(env) {}
};

#endif //LOADER_PROC_VAR_H
