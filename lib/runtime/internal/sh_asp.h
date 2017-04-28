//
// Created by BraxtonN on 4/26/2017.
//

#ifndef SHARP_SH_ADDRESS_SPACE_H
#define SHARP_SH_ADDRESS_SPACE_H

#include "../../../stdimports.h"
#include "../oo/Object.h"

/*
 * Contained address space for code to be executed
 *
 * Format of allocation stack
 * |    ...     |
 * | local1     |
 * | local0     | <--- frame stack begins (stack pointer start here)
 * | param1     |
 * | param0     | <--- frame init args start
 * | fp         | <--- refrence to old frame pointer in stack
 * | sp         | <--- refrence to old stack pointer in stack
 */
struct sh_asp {
    int64_t id; // refrence id to the the address space

    int64_t* bytecode;
    int frame_init; // inital stack space required for frame
    ClassObject* owner;
    nString name;
    int64_t* params;
    bool* arrayFlag; // array flag for each parameter
};

struct stack {
    union {
        double var;
        Sh_object object;
    };
};

#define ret_frame \
    { sp = __stack[fp-2].var; fp = __stack[fp-1].var; }

typedef int64_t* sharp_cache;

#endif //SHARP_SH_ADDRESS_SPACE_H
