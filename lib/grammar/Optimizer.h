//
// Created by bknun on 5/26/2017.
//

#ifndef SHARP_OPTIMIZER_H
#define SHARP_OPTIMIZER_H

#include "m64Assembler.h"

struct register_state
{
public:
    register_state()
            :
            id(0),
            assign_type(0),
            value(0)
    {
    }

    register_state(int id)
            :
            id(id),
            assign_type(0),
            value(0)
    {
    }
    int id;
    int assign_type;
    long long value;
};

class Optimizer {

public:
    Optimizer()
    {
        register_map.init();
    }

    void optimize(m64Assembler &code);

    ~Optimizer()
    {
        register_map.free();
        assembler=NULL;
    }

private:
    m64Assembler* assembler;
    List <register_state> register_map;

    void optimizeRegisterOverride();

    register_state &get_register(int id);

    void optimizeUnusedReferences();

    bool referenceUsed(unsigned int);
};

enum register_assignes
{
    register_unused=0,
    assign_self=1, // ++ or --
    assign_register=2,    // movr ebx,egx
    assign_stack=3, // smov ebx+3
    assign_bool=4, // lt ebx,ecx
    assign_value=5, // movi 1,ebx
    register_undefined=6,

};


#endif //SHARP_OPTIMIZER_H
