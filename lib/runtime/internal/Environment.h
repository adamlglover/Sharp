//
// Created by BraxtonN on 2/17/2017.
//

#ifndef SHARP_ENVIRONMENT_H
#define SHARP_ENVIRONMENT_H

#include "../oo/string.h"
#include "../oo/ClassObject.h"
#include "sh_asp.h"

class Sh_object;
class Method;
class ArrayObject;
struct _gc_object;

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
    void newClass(Sh_object*,int64_t);
    void newNative(Sh_object*, int8_t);
    void newArray(Sh_object*, int64_t);
    void newRefrence(Sh_object*);

    // TODO: create the aux classes to be used internally
    // Alloce'd by new()
    static ClassObject Throwable;
    static ClassObject StackOverflowErr;
    static ClassObject RuntimeException; // TODO: compare exceptions by name not id
    static ClassObject ThreadStackException;
    static ClassObject IndexOutOfBoundsException;
    static ClassObject NullptrException;

    Sh_object* global_heap;

    ClassObject* classes;

    String* strings;
    sh_asp* __address_spaces;

    void shutdown();

    nString getstring(int64_t ref);

    static void init(_gc_object*,int64_t);
    static void init(Sh_object*,int64_t);
    static void init(data_stack*,int64_t);

    static void free(Sh_object*, int64_t);
    static void freesticky(_gc_object*, int64_t);
    static void gcinsert_stack(Sh_object *, int64_t);
};

extern Environment* env;

#define mvers versions.BASE


#endif //SHARP_ENVIRONMENT_H
