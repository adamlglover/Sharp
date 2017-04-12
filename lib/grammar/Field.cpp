//
// Created by BraxtonN on 2/2/2017.
//
#include "Field.h"
#include "ClassObject.h"

bool Field::operator==(const Field& f)
{
    if(f.type == this->type && f.array == this->array) {
        if(type == field_class)
            return klass != NULL && klass->match(f.klass);
        else if(type == field_native)
            return (nf == f.nf);
        else
            return true;
    }
    return false;
}