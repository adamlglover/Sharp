//
// Created by bknun on 4/22/2017.
//

#ifndef SHARP_ASM_H
#define SHARP_ASM_H


#include "m64Assembler.h"

class runtime;

class Asm {

public:
    Asm()
    :
            npos(0),
            instance(NULL),
            code("")
    {
    }

    void parse(m64Assembler& assembler, runtime* instance, string& code);

private:
    int64_t npos;
    m64Assembler* assembler;
    runtime* instance;
    string code;

    bool isend();

    CXX11_INLINE
    char current() { return code.at(npos); }

    void invalidate_whitespace();

    void invalidate_comments();
};


#endif //SHARP_ASM_H
