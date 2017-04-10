//
// Created by BraxtonN on 1/31/2017.
//

#ifndef SHARP_PARAM_H
#define SHARP_PARAM_H

#include <list>
#include "Field.h"
#include "NativeField.h"

class ClassObject;

class Param
{
public:
    Param()
    :
            field()
    {
    }

    Param(Field field)
    {
        this->field = field;
    }

    bool match(Param& param);
    static bool match(list<Param>& p1, list<Param>& p2);
    void free() {
        field.free();
    }


    Field field;
};

#endif //SHARP_PARAM_H
