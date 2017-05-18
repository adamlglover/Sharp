//
// Created by BraxtonN on 4/26/2017.
//

#ifndef SHARP_SH_ADDRESS_SPACE_H
#define SHARP_SH_ADDRESS_SPACE_H

#include "../../../stdimports.h"
#include "../oo/Object.h"

struct line_table;

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
    int self;      // allocate 1 stack frame for self?
    bool* arrayFlag; // array flag for each parameter
    int param_size;
    long sourceFile;
    int64_t cache_size;
    List<ExceptionTable> exceptions;
    List<line_table> lineNumbers;

    void free() {
        name.free();
        sourceFile=0;
        exceptions.free();
        lineNumbers.free();

        if(param_size != 0) {
            if(arrayFlag != NULL)
                std::free(arrayFlag);
            if(params != NULL)
                std::free(params);
        }

        owner = NULL;
        if(bytecode != NULL) {
            std::free(bytecode);
        }
    }

    void init() {
        name.init();
        sourceFile=0;
        exceptions.init();
        lineNumbers.init();
        params = NULL;
        arrayFlag = NULL;
        param_size = 0;
        owner = NULL;
        frame_init = 0;
        bytecode = NULL;
        self = 0;
        id = 0;
    }
};

struct line_table {
    int64_t pc;
    int64_t line_number;
};

struct data_stack {
    double var;
    Sh_object object;

    void modul(int64_t v) {
        var = (int64_t)var&v;
    }
};

#define ret_frame(x) \
    if(curr_adsp == main->id) return; else { x }

#define pc_offset 2
#define fp_offset 3
#define sp_offset 4

typedef int64_t* sharp_cache;

#endif //SHARP_SH_ADDRESS_SPACE_H
