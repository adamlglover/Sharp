//
// Created by BraxtonN on 2/17/2017.
//

#include "ClassObject.h"
#include "../oo/Field.h"
#include "../oo/Object.h"
#include "../internal/Environment.h"

void ClassObject::free() {

    if(flds != NULL) {
        for(int64_t i = 0; i < fieldCount; i++)
            flds[i].free();
        std::free(flds); flds = NULL;
    }

    if(methods != NULL)
        std::free(this->methods); methods = NULL;
    name.free();
}

Field *ClassObject::getfield(string name) {
    for(unsigned int i = 0; i < fieldCount; i++) {
        if(flds[i].name == name)
            return &flds[i];
    }
    return NULL;
}

int64_t ClassObject::fieldindex(string name) {
    for(unsigned int i = 0; i < fieldCount; i++) {
        if(flds[i].name == name)
            return i;
    }
    return -1;
}

void ClassObject::init() {
    this->name.init();
    flds = NULL;
    methods = NULL;
    super = NULL;
    id = 0;
    fieldCount = 0;
    methodCount = 0;
}
