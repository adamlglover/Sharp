//
// Created by BraxtonN on 2/28/2017.
//

#include "CallStack.h"
#include "../oo/Method.h"
#include "Opcode.h"
#include "../internal/Thread.h"
#include "vm.h"
#include "../oo/Field.h"
#include "../oo/Object.h"
#include <iomanip>

void CallStack::push(Method *method) {
    sp++;

    if(sp >= default_cstack) throw Exception(&Environment::StackOverflowErr, ""); // stack overflow error
    current = method;
    stack[sp].callee = method;
    if(current->locals == 0)
        stack[sp].locals = NULL;
    else {
        stack[sp].locals = (Sh_object*)memalloc(sizeof(Sh_object)*current->locals);
        env->init(stack[sp].locals, current->locals);
    }
    regs = &stack[sp].rgs[0];
    locals = stack[sp].locals;
}

void CallStack::pop() {

    if((sp-1) == -1) {
        Environment::gcinsert_stack(locals, current->locals);
        stack[sp].locals=NULL;
        current = NULL;
        regs=NULL;
        sp--;
        return;
    }

    Environment::gcinsert_stack(locals, current->locals);
    sp--;
    current = stack[sp].callee;
    locals = stack[sp].locals;
    regs = stack[sp].rgs;
}


double exponent(int64_t n){
    if (n < 100000){
        // 5 or less
        if (n < 100){
            // 1 or 2
            if (n < 10)
                return n*0.1;
            else
                return n*0.01;
        }else{
            // 3 or 4 or 5
            if (n < 1000)
                return n*0.001;
            else{
                // 4 or 5
                if (n < 10000)
                    return n*0.0001;
                else
                    return n*0.00001;
            }
        }
    } else {
        // 6 or more
        if (n < 10000000) {
            // 6 or 7
            if (n < 1000000)
                return n*0.000001;
            else
                return n*0.0000001;
        } else if(n < 1000000000) {
            // 8 to 10
            if (n < 100000000)
                return n*0.00000001;
            else {
                // 9 or 10
                if (n < 1000000000)
                    return n*0.000000001;
                else
                    return n*0.0000000001;
            }
        } else if(n < 1000000000000000) {
            // 11 to 15
            if (n < 100000000000)
                return n*0.00000000001;
            else {
                // 12 to 15
                if (n < 1000000000000)
                    return n*0.000000000001;
                else if (n < 10000000000000)
                    return n*0.0000000000001;
                else if (n < 100000000000000)
                    return n*0.00000000000001;
                else
                    return n*0.000000000000001;
            }
        }
        else {
            return n*0.0000000000000001;
        }
    }
}



void CallStack::Execute() {
    Thread* self = thread_self;
    Sh_object *ptr=NULL; // ToDO: when ptr is derefrenced assign pointer to null pointer data struct in environment

    pc = &env->bytecode[current->entry];

    static void* dispatch_table[] = {
            &&_NOP, &&_INT, &&MOVI, &&RET,
            &&HLT, &&NEW, &&CHECK_CAST, &&MOV8
            , &&MOV16, &&MOV32, &&MOV64, &&PUSHR
            , &&ADD, &&SUB, &&MUL, &&DIV, &&MOD, &&POP
            , &&INC, &&DEC, &&MOVR, &&MOVX, &&LT, &&BRH, &&BRE
            , &&IFE, &&IFNE, &&GT, &&GTE, &&LTE, &&MOVL, &&OBJECT_NXT
            , &&OBJECT_PREV, &&RMOV, &&MOV, &&MOVD, &&MOVBI, &&_SIZEOF
            , &&PUT, &&PUTC, &&CHECKLEN};

    DISPATCH();
    try {
        for (;;) {
            _interp:
            if(self->suspendPending)
                Thread::suspendSelf();
            if(self->state == thread_killed)
                return;

            _NOP:
	            NOP
            _INT:
	            _int(GET_Da(*pc))
            MOVI:
	            movi(GET_Da(*pc))
            RET:
	            ret
            HLT:
	            hlt
            NEW: /* Requires register value */
                _new(GET_Da(*pc))
            CHECK_CAST:
	            check_cast
            MOV8:
	            mov8(GET_Ca(*pc),GET_Cb(*pc))
            MOV16:
	            mov16(GET_Ca(*pc),GET_Cb(*pc))
            MOV32:
	            mov32(GET_Ca(*pc),GET_Cb(*pc))
            MOV64:
	            mov64(GET_Ca(*pc),GET_Cb(*pc))
            PUSHR:
	            pushr(GET_Da(*pc))
            ADD:
	            _add(GET_Ca(*pc),GET_Cb(*pc))
            SUB:
	            _sub(GET_Ca(*pc),GET_Cb(*pc))
            MUL:
	            _mul(GET_Ca(*pc),GET_Cb(*pc))
            DIV:
	            _div(GET_Ca(*pc),GET_Cb(*pc))
            MOD:
	            mod(GET_Ca(*pc),GET_Cb(*pc))
            POP:
	            _pop
            INC:
	            inc(GET_Da(*pc))
            DEC:
	            dec(GET_Da(*pc))
            MOVR:
	            movr(GET_Ca(*pc),GET_Cb(*pc))
            MOVX: /* Requires register value */
                movx(GET_Ca(*pc),GET_Cb(*pc))
            LT:
	            lt(GET_Ca(*pc),GET_Cb(*pc))
            BRH:
	            brh
            BRE:
	            bre
            IFE:
	            ife
            IFNE:
	            ifne
            GT:
	            gt(GET_Ca(*pc),GET_Cb(*pc))
            GTE:
	            gte(GET_Ca(*pc),GET_Cb(*pc))
            LTE:
	            lte(GET_Ca(*pc),GET_Cb(*pc))
            MOVL:
	            movl(&locals[GET_Da(*pc)])
            OBJECT_NXT:
	            object_nxt
            OBJECT_PREV:
	            object_prev
            RMOV:
                _nativewrite2((int64_t)regs[GET_Ca(*pc)],regs[GET_Cb(*pc)]) _brh
            MOV:
                _nativewrite3((int64_t)regs[GET_Ca(*pc)],GET_Cb(*pc)) _brh
            MOVD:
                _nativewrite2((int64_t)regs[GET_Ca(*pc)],GET_Cb(*pc)) _brh
            MOVBI:
	            movbi(GET_Da(*pc) + exponent(*(pc+1)))
            _SIZEOF:
	            _sizeof(GET_Da(*pc))
            PUT:
	            _put(GET_Da(*pc))
            PUTC:
	            putc(GET_Da(*pc))
            CHECKLEN:
                _checklen(GET_Da(*pc))

        }
    } catch (std::bad_alloc &e) {
        // TODO: throw out of memory error
    } catch (Exception &e) {
        self->throwable = e.getThrowable();
        self->exceptionThrown = true;

        // TODO: handle exception
    }
}
