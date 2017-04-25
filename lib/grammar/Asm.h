//
// Created by bknun on 4/22/2017.
//

#ifndef SHARP_ASM_H
#define SHARP_ASM_H


#include "m64Assembler.h"
#include "parser/tokenizer/tokenentity.h"

class runtime;
class Errors;
class tokenizer;
class ast;

struct int2_t {
    int64_t high_bytes;
    int64_t low_bytes;
};

class Asm {

public:
    Asm()
    :
            npos(0),
            instance(NULL),
            code(""),
            expect_instr(false)
    {
    }

    void parse(m64Assembler& assembler, runtime* instance, string& code, ast* pAst);

    int64_t errors, uo_errors;
private:
    int64_t npos, i64;
    int2_t i2;
    m64Assembler* assembler;
    runtime* instance;
    string code;
    tokenizer* tk;
    bool expect_instr;
    List<keypair<std::string, int64_t>>* label_map;

    bool isend();

    token_entity current();

    bool instruction_is(string name);

    void expect_int();

    void expect_int_or_register();

    void expect(string token);

    string expect_identifier();

    bool label_exists(string label);

    int64_t current_address();

    int64_t get_label(string name);
};


#endif //SHARP_ASM_H
