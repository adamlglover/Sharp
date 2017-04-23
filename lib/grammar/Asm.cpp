//
// Created by bknun on 4/22/2017.
//

#include "Asm.h"
#include "parser/tokenizer/tokenizer.h"
#include "parser/parseerrors.h"
#include "parser/tokenizer/tokenizer.h"
#include "runtime.h"
#include "../runtime/interp/Opcode.h"
#include "../runtime/interp/register.h"

bool Asm::isend() {
    return npos >= tk->getentities().size() || current().gettokentype() == _EOF;
}

bool Asm::instruction_is(string name) {
    if(current().getid() == IDENTIFIER && current().gettoken() == name) {
        npos++;
        return true;
    }

    return false;
}

void Asm::expect_int_or_register() {
    if(current() == "%") {
        // register
        npos++;
        if(current() == "adx") {
            i2.high_bytes = adx;
        } else if(current() == "cx") {
            i2.high_bytes = cx;
        } else if(current() == "cmt") {
            i2.high_bytes = cmt;
        } else if(current() == "ebx") {
            i2.high_bytes = ebx;
        } else if(current() == "ecx") {
            i2.high_bytes = ecx;
        } else if(current() == "ecf") {
            i2.high_bytes = ecf;
        } else if(current() == "edf") {
            i2.high_bytes = edf;
        } else if(current() == "ehf") {
            i2.high_bytes = ehf;
        } else if(current() == "bmr") {
            i2.high_bytes = bmr;
        } else if(current() == "egx") {
            i2.high_bytes = egx;
        } else {
            // error
            tk->geterrors()->newerror(GENERIC, current(), "symbol `" + current().gettoken() + "` is not a register");
        }

        npos++;
        i2.low_bytes = -1;
    } else
        expect_int();
}

void Asm::expect_int() {
    bool hash = false;
    if(current() == "#") {
        hash = true;
        npos++;
    }

    if(current().getid() == INTEGER_LITERAL || current().getid() == HEX_LITERAL) {
        double x;
        string int_string = runtime::invalidate_underscores(current().gettoken());

        if(runtime::all_integers(int_string)) {
            x = std::strtod (int_string.c_str(), NULL);
            if(x > DA_MAX || x < DA_MIN) {
                stringstream ss;
                ss << "integral number too large: " + int_string;
                tk->geterrors()->newerror(GENERIC, current(), ss.str());
            }
            i2.high_bytes = (int64_t )x;
            i2.low_bytes = -1;
        }else {
            x = std::strtod (int_string.c_str(), NULL);
            if((int64_t )x > DA_MAX || (int64_t )x < DA_MIN) {
                stringstream ss;
                ss << "integral number too large: " + int_string;
                tk->geterrors()->newerror(GENERIC, current(), ss.str());
            }

            i2.high_bytes = (int64_t )x;

            if(current().gettoken().find('.') != string::npos)
                i2.low_bytes = abs(runtime::get_low_bytes(x));
            else
                i2.low_bytes = -1;
        }
    } else if(current().getid() == CHAR_LITERAL) {
        if(hash)
            tk->geterrors()->newerror(GENERIC, current(), "invalid format, symbol '#' requires integer literal following it");

        if(current().gettoken().size() > 1) {
            switch(current().gettoken().at(1)) {
                case 'n':
                    i2.high_bytes = '\n';
                    break;
                case 't':
                    i2.high_bytes = '\t';
                    break;
                case 'b':
                    i2.high_bytes = '\b';
                    break;
                case 'v':
                    i2.high_bytes = '\v';
                    break;
                case 'r':
                    i2.high_bytes = '\r';
                    break;
                case 'f':
                    i2.high_bytes = '\f';
                    break;
                case '\\':
                    i2.high_bytes = '\\';
                    break;
                default:
                    i2.high_bytes = current().gettoken().at(1);
                    break;
            }
            i2.low_bytes = -1;
        } else {
            i2.high_bytes = current().gettoken().at(0);
            i2.low_bytes = -1;
        }
    } else if(current().gettoken() == "true" || current().gettoken() == "false") {
        if(hash)
            tk->geterrors()->newerror(GENERIC, current(), "invalid format, symbol '#' requires integer literal following it");

        if(current().gettoken() == "true") {
            i2.high_bytes = 1;
            i2.low_bytes = -1;
        } else {
            i2.high_bytes = 0;
            i2.low_bytes = -1;
        }
    } else {
        // error
        i2.high_bytes = 0;
        i2.low_bytes = -1;
        tk->geterrors()->newerror(GENERIC, current(), "expected integer literal");
    }

    npos++;
}

void Asm::expect(string token) {
    if(current() == token) {
        npos++;
    } else {
        tk->geterrors()->newerror(GENERIC, current(), "expected `" + token + "`");
    }
}

void Asm::parse(m64Assembler &assembler, runtime *instance, string& code, ast* pAst) {
    if(code == "") return;

    this->assembler = &assembler;
    this->instance = instance;
    this->code = code;
    tk = new tokenizer(code, "stdin");
    RuntimeNote note = RuntimeNote(instance->_current->sourcefile, instance->_current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);

    if(tk->geterrors()->_errs())
    {
        cout << note.getNote("in inline assembly");
        tk->geterrors()->print_errors();

        errors+= tk->geterrors()->error_count();
        uo_errors+= tk->geterrors()->uoerror_count();
    } else {
        /* parse assembly */
        int2_t itmp;

        while(!isend())
        {
            if(instruction_is("nop")) {
                assembler.push_i64(SET_Ei(i64, op_NOP));
            } else if(instruction_is("int")) {
                expect_int();
                assembler.push_i64(SET_Di(i64, op_INT, i2.high_bytes));
            } else if(instruction_is("movi")) {
                expect_int();
                itmp = i2;
                expect(",");
                expect_int_or_register();
                assembler.push_i64(SET_Di(i64, op_MOVI, itmp.high_bytes), i2.high_bytes);
            } else if(instruction_is("ret")) {
                assembler.push_i64(SET_Ei(i64, op_RET));
            } else if(instruction_is("hlt")) {
                assembler.push_i64(SET_Ei(i64, op_HLT));
            } else if(instruction_is("new")) {
                expect_int_or_register();
                assembler.push_i64(SET_Di(i64, op_NEW, i2.high_bytes));
            } else if(instruction_is("check_cast")) {
                assembler.push_i64(SET_Ei(i64, op_CHECK_CAST));
            } else if(instruction_is("mov8")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_MOV8, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("mov16")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_MOV16, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("mov32")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_MOV32, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("mov64")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_MOV64, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("pushr")) {
                expect_int_or_register();

                assembler.push_i64(SET_Di(i64, op_PUSHR, i2.high_bytes));
            } else if(instruction_is("add")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_ADD, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("sub")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_SUB, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("mul")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_MUL, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("div")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_DIV, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else {
                    npos++;
                    tk->geterrors()->newerror(GENERIC, current(), "expected instruction");
                }
            }

        if(tk->geterrors()->_errs())
        {

            cout << note.getNote("in inline assembly");
            tk->geterrors()->print_errors();

            errors+= tk->geterrors()->error_count();
            uo_errors+= tk->geterrors()->uoerror_count();
        }
    }

    tk->free();
    this->code = "";
}

token_entity Asm::current() {
    return tk->getentities().get(npos);
}
