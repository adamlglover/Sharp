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
     if(ptr->HEAD==NULL) { throw Exception(&Environment::NullptrException, ""); } else { ptr->HEAD[ix]=data; }

#define _nativewrite3(ix,data) \
     if(ptr->HEAD==NULL) { throw Exception(&Environment::NullptrException, ""); } else { ptr->HEAD[ix]+=data; }

#define _nativeread(r,rx) \
    if(ptr->HEAD==NULL) { throw Exception(&Environment::NullptrException, ""); } else { __rxs[r]=ptr->HEAD[(int64_t)__rxs[rx]]; }

#define Sh_InvRef(x) { \
x->HEAD=NULL; \
x->size=0;  \
x->_Node=NULL;  \
x->_rNode=NULL;  \
x->klass=NULL;  \
}

#define CHK_NULL(x) if(ptr==NULL) { throw Exception(&Environment::NullptrException, ""); } else { x }

/* Objects stored in memory */
class Sh_object {
public:
    Sh_object();

    Sh_object(int64_t type);

    double *HEAD;

    gc_mark mark;
    int64_t size;
    ClassObject* klass;
    Sh_object *_Node;
    Monitor monitor;
    List<Sh_object*> refs;
    Sh_object* _rNode;

    void inc_ref(Sh_object *ptr);
    void del_ref();
    void free();
    void createnative(int64_t size);
    void createstr(int64_t size);
    void createstr(nString& str);
    void createclass(int64_t klass);
    void mutate(Sh_object *pObject);
    void null();

    void createclass(ClassObject *klass);
};

#endif //SHARP_OBJECT_H
