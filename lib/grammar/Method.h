//
// Created by BraxtonN on 1/31/2017.
//

#ifndef SHARP_METHOD_H
#define SHARP_METHOD_H

#include <list>
#include "../../stdimports.h"
#include "Param.h"
#include "../util/List2.h"
#include "m64Assembler.h"

class ClassObject;

enum return_type {
    lclass_object,
    lnative_object,
    lvoid,
    lundefined,
};

class Method {

public:
    Method()
    :
            name(""),
            pklass(NULL),
            klass(NULL),
            nobj(fnof),
            module(""),
            note(),
            array(false),
            code(),
            sourceFile(0),
            local_count(0)
    {
        this->exceptions.init();
        this->line_table.init();
        this->modifiers.init();
        this->params.init();
    }

    Method(string name, string module, ClassObject* klass, List<Param>& params, List<AccessModifier>& modifiers,
           ClassObject* rtype, RuntimeNote note, long long sourceFile)
    :
            name(name),
            pklass(klass),
            klass(rtype),
            nobj(fnof),
            module(module),
            note(note),
            array(false),
            code(),
            sourceFile(sourceFile),
            local_count(0)
    {
        this->exceptions.init();
        this->line_table.init();
        this->modifiers.init();
        this->params.init();
        this->params.addAll(params);
        this->modifiers.addAll(modifiers);
    }

    Method(string name, string module, ClassObject* klass, List<Param>& params, List<AccessModifier>& modifiers,
           NativeField rtype, RuntimeNote note, long long sourceFile)
            :
            name(name),
            pklass(klass),
            nobj(rtype),
            klass(NULL),
            module(module),
            note(note),
            array(false),
            code(),
            sourceFile(sourceFile),
            local_count(0)
    {
        this->exceptions.init();
        this->line_table.init();
        this->modifiers.init();
        this->params.init();
        this->params.addAll(params);
        this->modifiers.addAll(modifiers);
    }

    ClassObject* getParentClass() { return pklass; }
    string getName() { return name; }
    string getFullName();
    string getModule() { return module; }
    size_t paramCount() { return params.size(); }
    List<Param>* getParams() { return &params; }
    Param& getParam(int p) { return params.get(p); }
    bool isStatic() { return modifiers.find(mStatic); }
    void free() {
        modifiers.free();

        for(unsigned int i = 0; i < params.size(); i++) {
            params.get(i).free();
        }
        params.free();
    }

    RuntimeNote note;
    int64_t vaddr;
    return_type type;
    ClassObject* klass;
    NativeField nobj;
    m64Assembler code;
    ClassObject* pklass;
    long long sourceFile;
    bool array;
    int64_t local_count;
    List<keypair<int64_t, long>> line_table;
    List<ExceptionTable> exceptions;
private:
    List<AccessModifier> modifiers; // 3 max modifiers
    string name;
    string module;
    List<Param> params;
};


#endif //SHARP_METHOD_H
