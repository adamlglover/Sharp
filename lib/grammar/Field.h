//
// Created by BraxtonN on 1/31/2017.
//

#ifndef SHARP_FIELD_H
#define SHARP_FIELD_H

#include "../../stdimports.h"
#include "NativeField.h"
#include "AccessModifier.h"
#include "RuntimeNote.h"
#include <list>

class ClassObject;

enum field_type {
    field_class,
    field_native,
    field_unresolved,
};

class Field {
public:
    Field(NativeField nf, uint64_t uid, string name, ClassObject* parent, list<AccessModifier>* modifiers,
          RuntimeNote note)
    :
            nf(nf),
            uid(uid),
            name(name),
            fullName(""),
            parent(parent),
            klass(NULL),
            note(note),
            refrence(false),
            pointer(false)
    {
        this->modifiers = new list<AccessModifier>();
        if(modifiers == NULL)
            this->modifiers = NULL;
        else
            *this->modifiers = *modifiers;
    }

    Field(ClassObject* klass, uint64_t uid, string name, ClassObject* parent, list<AccessModifier>* modifiers,
          RuntimeNote note)
            :
            nf(fnof),
            uid(uid),
            name(name),
            fullName(""),
            parent(parent),
            klass(klass),
            note(note),
            refrence(false),
            pointer(false)
    {
        this->modifiers = new list<AccessModifier>();
        if(modifiers == NULL)
            this->modifiers = NULL;
        else
            *this->modifiers = *modifiers;
    }

    Field()
            :
            nf(fnof),
            uid(0),
            name(""),
            fullName(""),
            modifiers(NULL),
            note("","",0,0),
            refrence(false),
            pointer(false)
    {
    }

    bool operator==(const Field& f);

    void operator=(const Field& f)
    {
        free();

        nf = f.nf;
        klass = f.klass;
        uid = f.uid;
        name = f.name;
        fullName = f.fullName;
        parent = f.parent;
        modifiers = f.modifiers;
        refrence = f.refrence;
    }
    void free(){
        klass = NULL;
        parent = NULL;

        if(modifiers != NULL) {
            modifiers->clear();
            delete (modifiers);
        }
        modifiers = NULL;
    }

    bool isField() {
        return nf != fnof || nf >= fdynamic;
    }

    bool isStatic() {
        if(modifiers == NULL) return false;

        for(AccessModifier modifier : *modifiers) {
            if(modifier == mStatic)
                return true;
        }
        return false;
    }

    bool refrence, pointer, array;
    field_type type;
    RuntimeNote note;
    NativeField nf;
    ClassObject* klass;
    int64_t uid, vaddr;
    string name, fullName;
    ClassObject* parent;
    list<AccessModifier>* modifiers; // 3 max modifiers
};


#endif //SHARP_FIELD_H
