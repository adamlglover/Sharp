//
// Created by bknun on 4/22/2017.
//

#include "Asm.h"
#include "parser/tokenizer/tokenizer.h"

void Asm::invalidate_whitespace() {
    while(!isend() && tokenizer::iswhitespace(current()))
    {
        npos++;
    }
}

void Asm::invalidate_comments() {
    if(current() != '#') return;

    while(!isend() && current() != '\n')
    {
        npos++;
    }

    if (!isend())
    {
        invalidate_whitespace();
        invalidate_comments();
    }
}

bool Asm::isend() {
    return npos >= code.size();
}

void Asm::parse(m64Assembler &assembler, runtime *instance, string& code) {
    if(code == "") return;

    this->assembler = &assembler;
    this->instance = instance;
    this->code = code;

    while(npos >= code.size()) {
        invalidate_whitespace();
        invalidate_comments();

        // TODO: start parsing text
    }

    this->code = "";
}
