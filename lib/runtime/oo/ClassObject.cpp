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

    std::free(this->methods);
    name.free();
}

ClassObject *ClassObject::newdup() {
    ClassObject* klass = NULL;//new ClassObject(name.str(), flds, fieldCount,
                              //           methods, methodCount, NULL,
                              //           id);

    if(super != NULL) {
        klass->super = super->newdup();
    }
    return klass;
}
