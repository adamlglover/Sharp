//
// Created by BraxtonN on 1/31/2017.
//

#ifndef SHARP_CLASSOBJECT_H
#define SHARP_CLASSOBJECT_H

#include <list>
#include "../../stdimports.h"
#include "Method.h"
#include "Field.h"
#include "NativeField.h"
#include "AccessModifier.h"
#include "OperatorOverload.h"
#include "../util/keypair.h"

class ClassObject {

public:
    ClassObject()
    :
            name(""),
            module_name(""),
            uid(0),
            modifier(),
            base(NULL),
            super(NULL),
            head(NULL),
            note(),
            fullName("")
    {
        functions.init();
        constructors.init();
        macros.init();
        overloads.init();
        fields.init();
        childClasses.init();
    }
    ClassObject(string name, string pmodule, long uid, AccessModifier modifier, RuntimeNote note)
    :
            name(name),
            module_name(pmodule),
            uid(uid),
            modifier(modifier),
            base(NULL),
            super(NULL),
            head(NULL),
            note(note),
            fullName("")
    {
        functions.init();
        constructors.init();
        macros.init();
        overloads.init();
        fields.init();
        childClasses.init();
    }

    ClassObject(string name, string pmodule, long uid, AccessModifier modifier, RuntimeNote note,
                    ClassObject* parent)
            :
            name(name),
            module_name(pmodule),
            uid(uid),
            modifier(modifier),
            base(NULL),
            head(NULL),
            super(parent),
            note(note),
            fullName("")
    {
        functions.init();
        constructors.init();
        macros.init();
        overloads.init();
        fields.init();
        childClasses.init();
    }

    AccessModifier getAccessModifier() { return modifier; }
    long getUID() { return uid; }
    string getName() { return name; }
    string getModuleName() { return module_name; }
    ClassObject* getSuperClass() { return super; }
    ClassObject* getBaseClass() { return base; }
    ClassObject* getHeadClass() { return head; }
    bool match(ClassObject* klass) {
        return klass != NULL && klass->uid == uid;
    }
    void setBaseClass(ClassObject* base) {
        this->base = base;
    }
    void setSuperClass(ClassObject* sup) {
        this->super = sup;
    }
    void setHead(ClassObject* sup) {
        this->head = sup;
    }
    void setFullName(string fullName) {
        this->fullName = fullName;
    }
    string getFullName() {
        return fullName;
    }

    void operator=(ClassObject& klass) {
        this->macros = klass.macros;
        this->base = klass.base;
        this->childClasses = klass.childClasses;
        this->constructors = klass.constructors;
        this->fields = klass.fields;
        this->fullName = klass.fullName;
        this->functions = klass.functions;
        this->head = klass.head;
        this->modifier = klass.modifier;
        this->module_name = klass.module_name;
        this->name = klass.name;
        this->note = klass.note;
        this->overloads = klass.overloads;
        this->super = klass.super;
        this->uid = klass.uid;
        this->vaddr = klass.vaddr;
    }

    size_t constructorCount();
    Method* getConstructor(int p);
    Method* getConstructor(List<Param>& params, bool ubase=true);
    bool addConstructor(Method constr);

    size_t functionCount();
    Method* getFunction(int p);
    Method* getFunction(string name, List<Param>& params, bool ubase =false);
    Method* getFunction(string name, int64_t _offset);
    bool addFunction(Method function);

    size_t overloadCount();
    OperatorOverload* getOverload(size_t p);
    OperatorOverload* getPostIncOverload();
    OperatorOverload* getPostDecOverload();
    OperatorOverload* getPreIncOverload();
    OperatorOverload* getPreDecOverload();
    OperatorOverload* getOverload(_operator op, List<Param>& params, bool ubase =false);
    OperatorOverload* getOverload(_operator op, int64_t _offset);
    bool hasOverload(_operator op);
    bool addOperatorOverload(OperatorOverload overload);

    size_t fieldCount();
    Field* getField(int p);
    Field* getField(string name, bool ubase =false);
    bool addField(Field field);

    size_t macrosCount();
    Method* getMacros(int p);
    Method* getMacros(string name, List<Param>& params, bool ubase =false);
    Method* getMacros(string name, int64_t _offset);
    bool addMacros(Method macros);

    size_t childClassCount();
    ClassObject* getChildClass(int p);
    ClassObject* getChildClass(string name);
    bool addChildClass(ClassObject constr);
    void free();

    RuntimeNote note;

    bool curcular(ClassObject *pObject);

    bool matchBase(ClassObject *pObject);

    int64_t vaddr;

    bool hasBaseClass(ClassObject *pObject);

    long getFieldIndex(string name);

    int baseClassDepth(ClassObject *pObject);

    long getTotalFieldCount();

    long getTotalFunctionCount();

private:
    AccessModifier modifier;
    long uid;
    string name;
    string fullName;
    string module_name;
    List<Method> constructors;
    List<Method> functions;
    List<Method> macros;
    List<OperatorOverload> overloads;
    List<Field> fields;
    List<ClassObject> childClasses;
    ClassObject *super, *head;
    ClassObject* base;

};

#define TOTAL_FUNCS(x) x->functionCount()+x->constructorCount()+x->overloadCount()+x->macrosCount()


#endif //SHARP_CLASSOBJECT_H
