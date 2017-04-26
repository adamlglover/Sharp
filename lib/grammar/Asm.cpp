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
        expect_instr = false;
        return true;
    }

    return false;
}

string Asm::expect_identifier() {
    if(current().getid() == IDENTIFIER) {
        string name = current().gettoken();
        npos++;
        return name;
    } else {
        tk->geterrors()->newerror(GENERIC, current(), "expected identifier");
        return "";
    }
}

bool Asm::label_exists(string label) {
    for(unsigned int i = 0; i < label_map->size(); i++) {
        if(label_map->get(i).key == label)
            return true;
    }

     return false;
}

int64_t Asm::current_address() {
    return assembler->__asm64.size();
}

int64_t Asm::get_label(string label) {
    for(unsigned int i = 0; i < label_map->size(); i++) {
        if(label_map->get(i).key == label)
            return label_map->get(i).value;
    }

    return 0;
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
        if(current() == "+") {
            npos++;

            int rx = i2.high_bytes;
            expect_int();

            int64_t offset = i2.high_bytes;
            i2.high_bytes = offset + rx;
        }

        i2.low_bytes = -1;
    } else if(current() == "$") {
        npos++;

        string name = expect_identifier();
        if(label_exists(name)) {
            i2.high_bytes = get_label(name);
        } else {
            npos--;
            tk->geterrors()->newerror(GENERIC, current(), "unidentified label after mnemonic '$'");
            npos++;
        }

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

            if(current().gettoken().find('.') != string::npos || current().gettoken().find('e') != string::npos
               || current().gettoken().find('E') != string::npos)
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
    label_map = &instance->current_scope()->label_map;
    RuntimeNote note = RuntimeNote(instance->_current->sourcefile, instance->_current->geterrors()->getline(pAst->line),
                                   pAst->line, pAst->col);
    keypair<std::string, int64_t> label;

    if(tk->geterrors()->_errs())
    {
        cout << note.getNote("Assembler messages:");
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
                expect_int_or_register();
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
            } else if(instruction_is("mod")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_MOD, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("pop")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_MOD, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("inc")) {
                expect_int_or_register();

                assembler.push_i64(SET_Di(i64, op_INC, itmp.high_bytes));
            } else if(instruction_is("dec")) {
                expect_int_or_register();

                assembler.push_i64(SET_Di(i64, op_DEC, i2.high_bytes));
            } else if(instruction_is("movr")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_MOVR, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("movx")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_MOVX, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("lt")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_LT, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("brh")) {
                assembler.push_i64(SET_Ei(i64, op_BRH));
            } else if(instruction_is("bre")) {
                assembler.push_i64(SET_Ei(i64, op_BRE));
            } else if(instruction_is("ife")) {
                assembler.push_i64(SET_Ei(i64, op_IFE));
            } else if(instruction_is("ifne")) {
                assembler.push_i64(SET_Ei(i64, op_IFNE));
            } else if(instruction_is("gt")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_GT, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("ge")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_GTE, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("le")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_LTE, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("movl")) {
                expect_int();

                assembler.push_i64(SET_Di(i64, op_MOVL, i2.high_bytes));
            } else if(instruction_is("obj_next")) {
                assembler.push_i64(SET_Ei(i64, op_OBJECT_NXT));
            } else if(instruction_is("obj_prev")) {
                assembler.push_i64(SET_Ei(i64, op_OBJECT_PREV));
            } else if(instruction_is("rmov")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int_or_register();

                assembler.push_i64(SET_Ci(i64, op_RMOV, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("mov")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int();

                assembler.push_i64(SET_Ci(i64, op_MOV, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("movd")) {
                expect_int_or_register();
                itmp = i2;
                expect(",");
                expect_int();

                assembler.push_i64(SET_Ci(i64, op_MOVD, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
            } else if(instruction_is("movbi")) {
                expect_int();

                if(i2.low_bytes != -1) {
                    assembler.push_i64(SET_Ci(i64, op_MOVBI, abs(i2.high_bytes), (i2.high_bytes<0), i2.low_bytes));
                } else {
                    itmp = i2;
                    expect(",");
                    expect_int();

                    assembler.push_i64(SET_Ci(i64, op_MOVBI, abs(itmp.high_bytes), (itmp.high_bytes<0), i2.high_bytes));
                }

            } else if(instruction_is("_sizeof")) {
                expect_int_or_register();

                assembler.push_i64(SET_Di(i64, op_SIZEOF, i2.high_bytes));
            } else if(instruction_is("put")) {
                expect_int_or_register();

                assembler.push_i64(SET_Di(i64, op_PUT, i2.high_bytes));
            } else if(instruction_is("_putc")) {
                expect_int_or_register();

                assembler.push_i64(SET_Di(i64, op_PUTC, i2.high_bytes));
            } else if(instruction_is("chklen")) {
                expect_int_or_register();

                assembler.push_i64(SET_Di(i64, op_CHECKLEN, i2.high_bytes));
            } else if(current() == ".") {
                npos++;

                string name = expect_identifier();
                if(!label_exists(name)) {
                    label.set(name, current_address());
                    label_map->add(label);
                } else {
                    npos--;
                    tk->geterrors()->newerror(GENERIC, current(), "redefinition of label `" + name + "`");
                    npos++;
                }

                expect_instr = true;
                expect(":");
            } else if(instruction_is("goto")) {
                expect("$");
                string name = expect_identifier();
                if(label_exists(name)) {
                    assembler.push_i64(SET_Di(i64, op_GOTO, get_label(name)));
                } else {
                    npos--;
                    tk->geterrors()->newerror(GENERIC, current(), "unidentified label after mnemonic '$'");
                    npos++;
                }
            } else {
                npos++;
                tk->geterrors()->newerror(GENERIC, current(), "expected instruction");
            }
        }

        if(expect_instr) {
            tk->geterrors()->newerror(GENERIC, current(), "expected instruction");
        }

        if(tk->geterrors()->_errs())
        {

            cout << note.getNote("Assembler messages:");
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
