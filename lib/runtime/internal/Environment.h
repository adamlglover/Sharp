//
// Created by BraxtonN on 2/17/2017.
//

#ifndef SHARP_ENVIRONMENT_H
#define SHARP_ENVIRONMENT_H

#include "../oo/string.h"
#include "../oo/ClassObject.h"
#include "sh_asp.h"

class Object;
class Method;
class ArrayObject;
struct GcObject;

class Environment {
public:
    Environment()
    :
            global_heap(NULL),
            classes(NULL),
            strings(NULL),
            __address_spaces(NULL)
    {
    }

    int CallMainMethod(Method*, void*, int);
    void DropLocals();
    ClassObject* findClass(string name);
    ClassObject* tryFindClass(string name);
    ClassObject* findClass(int64_t id);

    void newClass(int64_t,int64_t);
    void newClass(Object*,int64_t);
    void newNative(Object*, int8_t);
    void newArray(Object*, int64_t);
    void newRefrence(Object*);

    // TODO: create the aux classes to be used internally
    // Alloce'd by new()
    static ClassObject Throwable;
    static ClassObject StackOverflowErr;
    static ClassObject RuntimeErr; // TODO: compare exceptions by name not id
    static ClassObject ThreadStackException;
    static ClassObject IndexOutOfBoundsException;
    static ClassObject NullptrException;
    static ClassObject ClassCastException;

    Object* global_heap;

    ClassObject* classes;

    nString* sourceFiles;
    String* strings;
    sh_asp* __address_spaces;

    void shutdown();

    nString& getstring(int64_t ref);

    static void init(GcObject*,int64_t);
    static void init(Object*,int64_t);
    static void init(data_stack*,int64_t);

    static void free(Object*, int64_t);
    static void freesticky(GcObject*, int64_t);
    static void gcinsert_stack(Object *, int64_t);

    nString getstringfield(string name, Object *pObject);

    Object *findfield(std::string name, Object *object);
};

extern Environment* env;

#define mvers versions.BASE

#define AUX_CLASSES 7

#endif //SHARP_ENVIRONMENT_H
