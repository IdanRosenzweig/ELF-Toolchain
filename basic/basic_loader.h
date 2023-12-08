#ifndef LOADER_BASIC_LOADER_H
#define LOADER_BASIC_LOADER_H

#include "file_id_entity.h"
#include "flags_entity.h"

class load_use_case {
public:
    virtual void load_and_run() const = 0;
};

template<typename FILE_ID, typename LOAD_FLAGS>
class basic_loader : virtual public file_id_entity<FILE_ID>, public flags_entity<LOAD_FLAGS>, virtual public load_use_case {
};

#endif //LOADER_BASIC_LOADER_H
