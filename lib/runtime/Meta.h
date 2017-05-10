//
// Created by BraxtonN on 2/17/2017.
//

#ifndef SHARP_META_H
#define SHARP_META_H

#include "../../stdimports.h"
#include "../util/List.h"
#include "../util/keypair.h"

class ClassObject;
class Field;

struct source_file{
    long id;
    List<nString> source_line;

    void free() {
        for(unsigned int i = 0; i < source_line.size(); i++)
            source_line.get(i).free();
        source_line.free();
    }
};

/**
 * File and debugging info, line
 * by line debugging meta data
 */
class Meta {

public:
    Meta()
    :
            sourceFiles()
    {
    }

    List<source_file> sourceFiles;

    std::string getLine(long line, long sourceFile);
    bool hasLine(long line, long sourceFile);
    void free();
};

class MetaClass {
public:
    MetaClass(ClassObject* c, int64_t sup)
    :
            super(sup),
            c(c)
    {
    }

    ClassObject* c;
    int64_t super;
};

class MetaField {
public:
    MetaField(Field* f, int64_t owner)
    :
            field(f),
            owner(owner)
    {
    }

    Field* field;
    int64_t owner;
};

#endif //SHARP_META_H
