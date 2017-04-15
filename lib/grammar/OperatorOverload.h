//
// Created by BraxtonN on 2/3/2017.
//

#ifndef SHARP_OPERATOROVERLOAD_H
#define SHARP_OPERATOROVERLOAD_H

#include "Method.h"

enum _operator {
    oper_PLUS,
    oper_MINUS,
    oper_MULT,
    oper_DIV,
    oper_MOD,
    oper_INC,
    oper_DEC,
    oper_EQUALS,
    oper_EQUALS_EQ,
    oper_PLUS_EQ,
    oper_MIN_EQ,
    oper_MULT_EQ,
    oper_DIV_EQ,
    oper_AND_EQ,
    oper_OR_EQ,
    oper_NOT_EQ,
    oper_MOD_EQ,

    oper_NO
};

class OperatorOverload : public Method {
public:
    OperatorOverload(RuntimeNote note, ClassObject *klass, List<Param> &params,
                     List<AccessModifier> &modifiers, NativeField rtype, _operator op) : Method("$operator", "",klass, params, modifiers,
                                                                                        rtype, note) {
        this->op = op;
    }

    OperatorOverload(RuntimeNote note, ClassObject* klass, List<Param> &params, List<AccessModifier> &modifiers,
                     ClassObject* rtype, _operator op) : Method("$operator", "", klass, params, modifiers, rtype, note) {
        this->op = op;
    }

    _operator getOperator() { return op; }

private:
    _operator op;
};

#endif //SHARP_OPERATOROVERLOAD_H
