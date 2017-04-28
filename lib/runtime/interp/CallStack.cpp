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






void CallStack::Execute() {
    Thread* self = thread_self;

}
