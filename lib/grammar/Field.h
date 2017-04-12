//
// Created by BraxtonN on 1/31/2017.
//

#ifndef SHARP_FIELD_H
#define SHARP_FIELD_H

#include "../../stdimports.h"
#include "NativeField.h"
#include "AccessModifier.h"
#include "RuntimeNote.h"
#include "../util/List2.h"
#include <list>

class ClassObject;

enum field_type {
    field_class=0,
    field_native=1,
    field_unresolved=2,
};

class Field {
public:
    Field(NativeField nf, uint64_t uid, string name, ClassObject* parent, List<AccessModifier>& modifiers,
          RuntimeNote note)
    :
            nf(nf),
            uid(uid),
            name(name),
            fullName(""),
            parent(parent),
            klass(NULL),
            note(note),
            array(false)
    {
        this->modifiers.init();
        this->modifiers.addAll(modifiers);
    }

    Field(ClassObject* klass, uint64_t uid, string name, ClassObject* parent, List<AccessModifier>& modifiers,
          RuntimeNote note)
            :
            nf(fnof),
            uid(uid),
            name(name),
            fullName(""),
            parent(parent),
            klass(klass),
            note(note),
            array(false)
    {
        this->modifiers.init();
        this->modifiers.addAll(modifiers);
    }

    Field()
            :
            nf(fnof),
            uid(0),
            name(""),
            fullName(""),
            modifiers(),
            note("","",0,0),
            array(false)
    {
    }

    bool operator==(const Field& f);

    void operator=(Field f)
    {
        free();

        type = f.type;
        nf = f.nf;
        klass = f.klass;
        uid = f.uid;
        name = f.name;
        fullName = f.fullName;
        parent = f.parent;
        modifiers.addAll(f.modifiers);
        array = f.array;
    }

    void free(){
        klass = NULL;
        parent = NULL;

        modifiers.free();
    }

    bool isField() {
        return nf != fnof || nf >= fdynamic;
    }

    bool isStatic() {
        for(unsigned int i = 0; i < modifiers.size(); i++) {
            if(modifiers.at(i) == mStatic)
                return true;
        }
        return false;
    }

    bool array;
    field_type type;
    RuntimeNote note;
    NativeField nf;
    ClassObject* klass;
    int64_t uid, vaddr;
    string name, fullName;
    ClassObject* parent;
    List<AccessModifier> modifiers; // 3 max modifiers
};


#endif //SHARP_FIELD_H
