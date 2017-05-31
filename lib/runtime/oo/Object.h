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
#include "ClassObject.h"

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
class Object {
public:
    Object();

    Object(double);

    double *HEAD;

    gc_mark mark;
    int64_t size;
    ClassObject* klass;
    Object *_Node;
    Monitor monitor;
    List<Object*> refs;
    Object* _rNode;

    void inc_ref(Object *ptr);
    void del_ref();
    void free();
    void createnative(int64_t size);
    void createnative(int64_t size, double init);
    void createstr(int64_t size);
    void createstr(nString& str);
    void createclass(int64_t klass);
    void mutate(Object *pObject);
    void null();
    void createobjectarry(int64_t size);
    void checkcast(int64_t klass);

    void createclass(ClassObject *klass);

#ifdef DEBUGGING
    string toString() {
        stringstream ss;

        ss << "{";
        ss << "size:" << size << " mark:" << mark;
        ss << (klass==NULL?"" : (" klass:" + klass->name.str()));
        ss << " refs:" << refs.size();
        ss << (_rNode==NULL? " refrence:false" : " refrence:true");
        if(size>0) {
            ss << " data:[";
            for(unsigned int i=0; i < size; i++) {
                if(klass==NULL) {
                    ss << HEAD[i] << ",";
                } else {
                    ss << _Node[i].toString() << ",";
                }
            }
            ss << "]";
        }

        ss << "}";

        return ss.str();
    }
#endif
};

#endif //SHARP_OBJECT_H
