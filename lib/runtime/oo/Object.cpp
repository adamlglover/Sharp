//
// Created by BraxtonN on 2/17/2017.
//

#include "Object.h"
#include "../internal/Environment.h"
#include "ClassObject.h"
#include "../alloc/GC.h"
#include <stdio.h>
#include <cstring>

Object::Object() {
    this->mark = gc_orange;
    this->monitor = Monitor();
    this->HEAD = NULL;
    this->klass=NULL;
    _Node = NULL;
    refs.init();
}

Object::Object(double val) {
    this->mark = gc_orange;
    this->klass=NULL;
    this->monitor = Monitor();
    this->HEAD = NULL;
    _Node = NULL;
    refs.init();

    createnative(1,val);
}

void Object::free() {
    mark = gc_orange;
    if(HEAD != NULL)
        std::free(HEAD); HEAD = NULL;

    if(_Node != NULL) {
        for(int64_t i = 0; i < size; i++) {
            _Node[i].free();
        }
        std::free(_Node); _Node = NULL;
    }

    size=0;
    refs.free();
    klass=NULL;
}

void Object::createnative(int64_t size) {
    if(mark == gc_green) {
        GC::_insert(this);
    }

    if(size == 0)
        HEAD = NULL;
    else
        HEAD= (double*)memalloc(sizeof(double)*size);
    this->size=size;
    klass=NULL;
    _Node=NULL, _rNode=NULL;
    mark = gc_green;

    for(int64_t i=0; i<size; i++){
        _nativewrite(i,0)
    }
}

void Object::inc_ref(Object *ptr) {
    if(ptr!=this)
    {
        this->refs.addif(ptr);
        ptr->HEAD=HEAD;
        ptr->klass=klass;
        ptr->size=size;
        ptr->_Node=_Node;
        ptr->_rNode=this;
        ptr->mark = gc_green;
    }
}

void Object::createstr(int64_t ref) {
    nString str;
    str = env->getstring(ref);

    createstr(str);
}

void Object::checkcast(int64_t klass) {
    if(this->klass == NULL)
        throw Exception(&Environment::ClassCastException, "invalid cast on non-class object");

    ClassObject* k = env->findClass(klass);
    if(!k->hasBaseClass(this->klass)) {
        stringstream ss;
        ss << "illegal cast of class '" << this->klass->name.str() << "' and '";
        ss << k->name.str() << "'";
        throw Exception(&Environment::ClassCastException, ss.str());
    }
}

void Object::createclass(int64_t k) {
    ClassObject* klass = env->findClass(k);
    createclass(klass);
}

void Object::createclass(ClassObject *klass) {
    if(mark == gc_green) {
        GC::_insert(this);
    }

    this->klass =klass;
    HEAD = NULL;
    _rNode =NULL;

    mark = gc_green;
    size =klass->fieldCount;
    if(size > 0)
        _Node =(Object*)memalloc(sizeof(Object) * size);
    else
        _Node = NULL;
    Environment::init(_Node, size);
}

void Object::createobjectarry(int64_t sz) {
    if(mark == gc_green) {
        GC::_insert(this);
    }

    HEAD = NULL;
    _rNode =NULL;

    mark = gc_green;
    this->size =sz;
    if(sz > 0)
        _Node =(Object*)memalloc(sizeof(Object) * sz);
    else
        _Node = NULL;
    Environment::init(_Node, size);
}

void Object::del_ref() {
    if(_rNode != NULL) {
        _rNode->refs.remove(this);

        HEAD=NULL;
        klass=NULL;
        size=0;
        _Node=NULL;
        _rNode=NULL;
    }
}

/**
 * Objects cannot be coppied as they can be infinatley large,
 * they can only be mutated
 */
void Object::mutate(Object *object) {
    if(mark == gc_green) {
        GC::_insert(this);
    }

    std::memcpy(this, object, sizeof(Object));

    if(object->_rNode != NULL) {
        object->_rNode->refs.replace(object, this);
    }
    object->refs.free();

    for(unsigned int i = 0; i < refs.size(); i++) {
        refs.get(i)->_rNode = this;
    }
    Sh_InvRef(object)
}

void Object::null() {
    if(mark == gc_green) {
        GC::_insert(this);
    }

    Sh_InvRef(this)
}

void Object::createstr(nString &str) {
    if(mark == gc_green) {
        GC::_insert(this);
    }

    if(str.len == 0)
        HEAD = NULL;
    else
        HEAD= (double*)memalloc(sizeof(double)*str.len);
    this->size=str.len;
    klass=NULL;
    _Node=NULL, _rNode=NULL;
    mark = gc_green;

    for(int64_t i=0; i<size; i++){
        _nativewrite(i,str.chars[i])
    }
}

void Object::createnative(int64_t size, double init) {
    if(mark == gc_green) {
        GC::_insert(this);
    }

    if(size == 0)
        HEAD = NULL;
    else
        HEAD= (double*)memalloc(sizeof(double)*size);
    this->size=size;
    klass=NULL;
    _Node=NULL, _rNode=NULL;
    mark = gc_green;

    for(int64_t i=0; i<size; i++){
        _nativewrite(i,init)
    }
}
