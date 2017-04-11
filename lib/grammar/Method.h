//
// Created by BraxtonN on 1/31/2017.
//

#ifndef SHARP_METHOD_H
#define SHARP_METHOD_H

#include <list>
#include "../../stdimports.h"
#include "Param.h"
#include "../util/List2.h"

class ClassObject;

class Method {

public:
    Method(string name, string module, ClassObject* klass, List<Param>& params, List<AccessModifier>& modifiers,
           ClassObject* rtype, RuntimeNote note)
    :
            name(name),
            pklass(klass),
            rType(rtype),
            n_rType(fnof),
            module(module),
            note(note)
    {
        this->modifiers.init();
        this->params.init();
        this->params.addAll(params);
        this->modifiers.addAll(modifiers);
    }

    Method(string name, string module, ClassObject* klass, List<Param>& params, List<AccessModifier>& modifiers,
           NativeField rtype, RuntimeNote note)
            :
            name(name),
            pklass(klass),
            n_rType(rtype),
            rType(NULL),
            module(module),
            note(note)
    {
        this->modifiers.init();
        this->params.init();
        this->params.addAll(params);
        this->modifiers.addAll(modifiers);
    }

    ClassObject* getParentClass() { return pklass; }
    ClassObject* getReturnType() { return rType; }
    string getName() { return name; }
    string getModule() { return module; }
    size_t paramCount() { return params.size(); }
    List<Param>* getParams() { return &params; }
    Param& getParam(int p) { return params.get(p); }
    void clear() {
        modifiers.free();

        for(unsigned int i = 0; i < params.size(); i++) {
            params.get(i).free();
        }
        params.free();
    }

    RuntimeNote note;
    uint64_t vaddr;
private:
    List<AccessModifier> modifiers; // 3 max modifiers
    ClassObject* pklass;
    string name;
    string module;
    List<Param> params;
    ClassObject* rType;
    NativeField n_rType;
};


#endif //SHARP_METHOD_H
