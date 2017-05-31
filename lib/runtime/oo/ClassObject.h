//
// Created by BraxtonN on 2/17/2017.
//

#ifndef SHARP_CLASSOBJECT_H
#define SHARP_CLASSOBJECT_H

#include "../../../stdimports.h"
#include "string.h"

class Field;
class Object;
class Method;

class ClassObject {
public:
    ClassObject()
    :
            name(),
            flds(NULL),
            methods(NULL),
            super(NULL),
            id(-1),
            fieldCount(0),
            methodCount(0)
    {
    }

    ClassObject(string name, Field* fields, int64_t fc,
                ClassObject* super, int64_t id)
    :
            name(name),
            flds(fields),
            methods(NULL),
            super(super),
            id(id),
            fieldCount(fc),
            methodCount(0)
    {
    }

    void operator=(const ClassObject& klass) {
        name = klass.name;
        flds = klass.flds;
        methods = klass.methods;
        super = klass.super;
        id = klass.id;
        fieldCount = klass.fieldCount;
        methodCount = klass.methodCount;
    }

    nString name;
    Field* flds;
    int64_t *methods;
    ClassObject* super;
    int64_t id, fieldCount, methodCount;

    void free();

    Field* getfield(string name);
    int64_t fieldindex(string name);

    void init();

    bool hasBaseClass(ClassObject *pObject);
};


#endif //SHARP_CLASSOBJECT_H
