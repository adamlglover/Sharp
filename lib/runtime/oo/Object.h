//
// Created by BraxtonN on 2/17/2017.
//

#ifndef SHARP_OBJECT_H
#define SHARP_OBJECT_H

#include <cstdint>
#include "../alloc/mark.h"
#include "../../../stdimports.h"
#include "string.h"
#include "../internal/Monitor.h"
#include "Exception.h"
#include "../../util/List.h"

#define ref_cap 0x16e

class ArrayObject;
class ClassObject;
class Reference;

enum Type {
    nativeint=-1,
    dynamicobject=-8,
    nilobject=-10
};

#define _nativewrite(i,data) \
    HEAD[i]=data;


#define _nativewrite2(ix,data) \
    ptr->HEAD[ix]=data;

#define _nativewrite3(ix,data) \
    ptr->HEAD[ix]+=data;

#define _nativeread(r,rx) \
    __rxs[r]=ptr->HEAD[(int64_t)__rxs[rx]];

#define Sh_InvRef(x) { \
x->HEAD=NULL; \
x->nxt==NULL;  \
x->prev=NULL;  \
x->size=0;  \
x->_Node=NULL;  \
x->_rNode=NULL;  \
}

/* Objects stored in memory */
class Sh_object {
public:
    Sh_object();

    Sh_object(int64_t type);

    double *HEAD;

    gc_mark mark;
    int64_t size;
    Sh_object *_Node, *prev, *nxt;
    Monitor monitor;
    List<Sh_object*> refs;
    Sh_object* _rNode;

    void _Sh_IncRef(Sh_object*);
    void free();
    void createnative(int64_t size);
    void createstr(int64_t size);
    void copy_object(Sh_object *pObject);
};

#endif //SHARP_OBJECT_H
