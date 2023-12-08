#ifndef LOADER_FLAGS_ENTITY_H
#define LOADER_FLAGS_ENTITY_H

template <typename LOAD_FLAGS>
class flags_entity {
protected:
    LOAD_FLAGS load_flags;

public:
    void setLoadFlags(LOAD_FLAGS loadFlags) {
        load_flags = loadFlags;
    }
};

#endif //LOADER_FLAGS_ENTITY_H
