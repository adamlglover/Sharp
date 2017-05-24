//
// Created by BraxtonN on 1/31/2017.
//

#include "Method.h"
#include "ClassObject.h"

string Method::getFullName(){
    return pklass==NULL?name:pklass->getFullName()+"."+name;
}
