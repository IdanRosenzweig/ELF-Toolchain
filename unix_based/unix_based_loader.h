#ifndef LOADER_UNIX_BASED_LOADER_H
#define LOADER_UNIX_BASED_LOADER_H

#include "../basic/basic_loader.h"
#include "opened_unix_file.h"
#include "../utils/proc_var.h"

class proc_var_entity {
protected:
    struct proc_var proc_var;

public:
    void setProcVar(const struct proc_var &procVar) {
        proc_var = procVar;
    }
};

typedef const char *FILE_ID; // str path in file system
template <typename LOAD_FLAGS>
class unix_based_loader : public basic_loader<FILE_ID, LOAD_FLAGS>, virtual public proc_var_entity {
};

opened_unix_file open_file(FILE_ID id);

#endif //LOADER_UNIX_BASED_LOADER_H
