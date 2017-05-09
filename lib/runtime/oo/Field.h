//
// Created by BraxtonN on 2/24/2017.
//

#ifndef SHARP_FIELD_H
#define SHARP_FIELD_H

#include "../../../stdimports.h"
#include "string.h"

class ClassObject;

class Field {
public:
    Field()
    :
        name()
    {
    }

    void init(string name, int64_t id, int type, bool _static, bool arry,
              ClassObject* owner)
    {
        this->name.init();
        this->name = name;
        this->id = id;
        this->type=type;
        this->isstatic=_static;
        this->array=arry;
    }

    void operator=(Field& field) {

        this->id = field.id;
        this->name = field.name;
        this->type = field.type;
        this->owner = field.owner;
        this->isstatic = field.isstatic;
    }

    nString name;
    int64_t id;
    int type;
    bool isstatic;
    bool array;
    ClassObject* owner;

    void free() {
        owner = NULL;
        name.free();
    }
};


#endif //SHARP_FIELD_H
