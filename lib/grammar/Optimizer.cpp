//
// Created by bknun on 5/26/2017.
//

#include "Optimizer.h"
#include "../runtime/interp/register.h"
#include "../runtime/interp/Opcode.h"

register_state& Optimizer::get_register(int id) {
    return register_map.get(id);
}

bool Optimizer::referenceUsed(unsigned int start) {
    int64_t x64;
    for(unsigned int i = start; i < assembler->size(); i++) {
        x64 = assembler->__asm64.get(i);

        switch(GET_OP(x64)) {
            case op_MOVL:
            case op_MOVSL:
            case op_MOVG:
            {
                return false;
            }
            case op_DEL:
            case op_MOVND:
            case op_NEW_OBJ_ARRY:
            case op_LOCK:
            case op_ULOCK:
            case op_MOVN:
            case op_NEW_CLASS:
            case op_DELREF:
            case op_PUSHREF:
            case op_CHECKLEN:
            case op_SIZEOF:
            case op_NEWi:
            case op_NEWSTR:
            case op_CHECK_CAST:
            case op_POPREF:
            case op_MUTL:
            {
                return true;
            }
            default:
                break;
        }
    }

    return false;
}

void Optimizer::optimizeUnusedReferences() {
    int64_t x64;
    for(unsigned int i = 0; i < assembler->size(); i++) {
        x64 = assembler->__asm64.get(i);

        switch(GET_OP(x64)) {
            case op_MOVL:
            case op_MOVSL:
            case op_MOVN:
            case op_MOVG:
            case op_MOVND:
            {
                if(!referenceUsed(i+1)) {
                    assembler->__asm64.remove(i);
                    readjustAddresses(i);
                    optimizedOpcodes++;
                }
                break;
            }
            default:
                break;
        }
    }
}

void Optimizer::readjustAddresses(unsigned int stopAddr) {
    int64_t x64, op, addr;
    for(unsigned int i = 0; i < stopAddr; i++) {
        x64 = assembler->__asm64.get(i);

        switch (GET_OP(x64)) {
            case op_SKP:
            case op_SKPE:
            case op_SKPNE:
            case op_GOTO:
                op=GET_OP(x64);
                addr=GET_Da(x64);

                /*
                 * We only want to update data which is referencing data below us
                 */
                if(addr > stopAddr)
                {
                    // update address
                    assembler->__asm64.replace(i, SET_Di(x64, op, --addr));
                }
                break;
            case op_MOVI:
                if(unique_addr_lst.find(i)) {
                    addr=GET_Da(x64);

                    /*
                     * We only want to update data which is referencing data below us
                     */
                    if(addr > stopAddr)
                    {
                        // update address
                        assembler->__asm64.replace(i, SET_Di(x64, op_MOVI, --addr));
                    }
                }
                break;
        }
    }
}

void Optimizer::optimizeRegisterOverride() {
    int64_t x64;
    for(unsigned int i = 0; i < assembler->size(); i++) {
        x64 = assembler->__asm64.get(i);

        switch(GET_OP(x64)) {
            case op_MOVR:
            {
                register_state &left=get_register(GET_Ca(x64)), &right=get_register(GET_Cb(x64));
                if(left.assign_type == assign_register && right.id != sp) {
                    if(left.value==right.id) {
                        assembler->__asm64.remove(i);
                        readjustAddresses(i);
                        optimizedOpcodes++;
                        i--;
                    }
                }

                left.assign_type=assign_register;
                left.value=right.id;
                break;
            }
            case op_MOV8:
            case op_MOV16:
            case op_MOV32:
            case op_MOV64:
            {
                register_state &left=get_register(GET_Ca(x64));
                left.assign_type=assign_self;
                left.value=0;
                break;
            }
            case op_SMOV:
            {
                register_state &left=get_register(GET_Ca(x64));
                left.assign_type=assign_stack;
                left.value=0;
                break;
            }
            case op_LT:
            case op_GT:
            case op_GTE:
            case op_LTE:
            case op_TEST:
            {
                register_state &left=get_register(0x2);
                left.assign_type=assign_bool;
                left.value=0;
            }
            case op_EXP:
            {
                register_state &left=get_register(0x8);
                left.assign_type=assign_value;
                left.value=0;
                break;
            }
            case op_SIZEOF:
            case op_LOADX:
            {
                register_state &left=get_register(GET_Da(x64));
                left.assign_type=assign_value;
                left.value=0;
                break;
            }
            case op_SDELREF:
            case op_PUSHREF:
            case op_PUSHR:
            case op_POP:
            {
                register_state &left=get_register(sp);
                left.assign_type=assign_self;
                left.value=0;
                break;
            }
            case op_MOVI:
            {
                register_state &left=get_register(assembler->__asm64.get(++i));
                left.assign_type=assign_value;
                left.value=0;
                break;
            }
            case op_CALL:
            {
                for(unsigned int i = 0; i < fp+1; i++) {
                    get_register(i).assign_type=register_undefined;
                    get_register(i).value=0;
                }
                break;
            }
            case op_MOVBI:
            {
                register_state &left=get_register(0x8);
                i++;
                left.assign_type=assign_value;
                left.value=0;
                break;
            }
            default:
                break;
        }
    }
}

void Optimizer::optimize(m64Assembler &code, List<long>& unique_addrs) {
    this->assembler = &code;
    this->unique_addr_lst.addAll(unique_addrs);
    optimizedOpcodes=0;

    if(code.size()==0)
        return;

    /* initally set all registers to be unusable at start */
    if(register_map.size() == 0) {
        for(int i = 0; i < fp+1; i++) {
            register_map.add(register_state(i));
        }
    } else {
        for(int i = 0; i < fp+1; i++) {
            register_map.get(i).value=0;
            register_map.get(i).assign_type=0;
        }
    }

    optimizeRegisterOverride();
    optimizeUnusedReferences();
}
