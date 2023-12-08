#ifndef LOADER_FILE_ID_ENTITY_H
#define LOADER_FILE_ID_ENTITY_H

template <typename FILE_ID>
class file_id_entity {
protected:
    FILE_ID file_id;

public:
    void setFileId(FILE_ID fileId) {
        file_id = fileId;
    }
};

#endif //LOADER_FILE_ID_ENTITY_H
