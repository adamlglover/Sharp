//
// Created by BraxtonN on 2/24/2017.
//

#ifndef SHARP_OPCODE_H
#define SHARP_OPCODE_H

#include "../../../stdimports.h"

/**
 * Bit layout
 *
 * Class D Instruction
    | 123456789012345678901234567890123456789012345678901234 | 12345678 |  max: 7FFFFFFFFFFFFF
    |						argument						  |  opcode  |

    max 36028797018963967 min -36028797018963968

    Class C Instruction
    | 190123456789012345678901234 | 0123456789012345678901234567 | 8 | 12345678 | max: FFFFFFF
    |			 argument		 |			argument		      | +-|  opcode  |

    max 134217727 min -134217727
 */

#define OPCODE_MASK 0xff

#define SET_Ei(i, op) (i=op)

#define SET_Di(i, op, a1) (i=((op) | ((int64_t)a1 << 8)))

#define SET_Ci(i, op, a1, n, a2) (i=((op | ((n & 1) << 8) | ((int64_t)a1 << 9)) | ((int64_t)a2 << 36)))

#define GET_OP(i) (i & OPCODE_MASK)
#define GET_Da(i) (i >> 8)
#define GET_Ca(i) (((i >> 8) & 1) ? (-1*(i >> 9 & 0x7FFFFFF)) : (i >> 9 & 0x7FFFFFF))
#define GET_Cb(i) (i >> 36)

#define DA_MAX 36028797018963967
#define DA_MIN -36028797018963968

#define CA_MAX 134217727
#define CA_MIN -134217727

#define DISPATCH() goto *opcode_table[GET_OP(*pc)]

#define _brh pc++; goto interp;
#define _brh_NOINCREMENT goto interp;

#define NOP _brh

#define _int(x) vm->interrupt(x); _brh

#define movi(x) regs[*(pc+1)]=x; pc++; _brh

#define ret { pop(); return; } _brh

#define hlt self->state=thread_killed; _brh

#define _new(x) \
{ \
    ptr->createnative(regs[x]);\
}; _brh

#define check_cast \
{ \
    \
}; _brh

#define mov8(r,x) regs[r]=(int8_t)regs[x]; _brh

#define mov16(r,x) regs[r]=(int16_t)regs[x]; _brh

#define mov32(r,x) regs[r]=(int32_t)regs[x]; _brh

#define mov64(r,x) regs[r]=(int64_t)regs[x]; _brh

#define pushr(r) thread_stack->push(regs[r]); _brh

#define _add(r,x) regs[0x0008]=regs[r]+regs[x]; _brh

#define _sub(r,x) regs[0x0008]=regs[r]-regs[x]; _brh _brh

#define _mul(r,x) regs[0x0008]=regs[r]*regs[x]; _brh _brh

#define _div(r,x) regs[0x0008]=regs[r]/regs[x]; _brh _brh

#define mod(r,x) regs[0x0008]=(int64_t)regs[r]%(int64_t)regs[x]; _brh

#define _pop _brh

#define inc(r) regs[r]++; _brh

#define dec(r) regs[r]--; _brh

#define movr(r,x) regs[r]=regs[x]; _brh

#define movx(r,x) _nativeread(r,x) _brh // TODO: make a movxd to ret value from register

#define lt(r,x) regs[0x0002]=regs[r]<regs[x]; _brh

#define brh pc=&env->bytecode[(int64_t)regs[0x0000]]; _brh_NOINCREMENT

#define bre if(regs[0x0002])pc=&env->bytecode[(int64_t)regs[0x0000]]; else _brh

#define ife if((regs[0x0002]) == false)pc=&env->bytecode[(int64_t)regs[0x0000]]; else  _brh

#define ifne if((!regs[0x0002]) == false)pc=&env->bytecode[(int64_t)regs[0x0000]]; else _brh

#define gt(r,x) regs[0x0002]=regs[r]>regs[x]; _brh

#define gte(r,x) regs[0x0002]=regs[r]>=regs[x]; _brh

#define lte(r,x) regs[0x0002]=regs[r]<regs[x]; _brh

#define movl(x) ptr=x; _brh

#define object_nxt ptr=ptr->nxt; _brh // TODO: add arg(x) to pick index of the node to jump to

#define object_prev ptr=ptr->prev; _brh // ToDO: for future check if node is null

#define movbi(x) regs[0x0008]=x; pc++; _brh

#define _sizeof(r) regs[r]=ptr->size; _brh

#define _put(r) cout << regs[r]; _brh

#define putc(r) cout << (char)regs[r]; _brh

#define _checklen(r) if(regs[r]>=ptr->size) \
    { \
        throw Exception("in"); \
    }else { _brh }

#define _init_opcode_table \
    static void* opcode_table[] = { \
        &&_NOP,	\
        &&_INT,	\
        &&MOVI,	\
        &&RET,	\
        &&HLT,	\
        &&NEW,	\
        &&CHECK_CAST,	\
		&&MOV8,	\
		&&MOV16,    \
		&&MOV32,	\
		&&MOV64,	\
		&&PUSHR,	\
		&&ADD,	\
		&&SUB,	\
		&&MUL,	\
		&&DIV,	\
		&&MOD,	\
		&&POP,	\
		&&INC,	\
		&&DEC,	\
		&&MOVR,	\
		&&MOVX,	\
		&&LT,	\
		&&BRH,	\
		&&BRE,	\
		&&IFE,	\
		&&IFNE,	\
		&&GT,	\
		&&GTE,	\
		&&LTE,	\
		&&MOVL,	\
		&&OBJECT_NXT,	\
		&&OBJECT_PREV,	\
		&&RMOV,	\
		&&MOV,	\
		&&MOVD,	\
		&&MOVBI,	\
		&&_SIZEOF,	\
		&&PUT,	\
		&&PUTC,	\
		&&CHECKLEN \
    };

/*
 * All opcodes removed must be filled with _NOP instruction to prevent
 * invalid instruction dispatching
 */
enum OPCODE {
    op_NOP=0x0,
    op_INT=0x1,
    op_MOVI=0x2,
    op_RET=0x3,
    op_HLT=0x4,
    op_NEW=0x5,
    op_CHECK_CAST=0x6,
    op_MOV8=0x7,
    op_MOV16=0x8,
    op_MOV32=0x9,
    op_MOV64=0xa,
    op_PUSHR=0xb,
    op_ADD=0xc,
    op_SUB=0xd,
    op_MUL=0xe,
    op_DIV=0xf,
    op_MOD=0x10,
    op_POP=0x11,
    op_INC=0x12,
    op_DEC=0x13,
    op_MOVR=0x14,
    op_MOVX=0x15,
    op_LT=0x16,
    op_BRH=0x17,
    op_BRE=0x18,
    op_IFE=0x19,
    op_IFNE=0x1a,
    op_GT=0x1b,
    op_GTE=0x1c,
    op_LTE=0x1d,
    op_MOVL=0x1e,
    op_OBJECT_NXT=0x1f,
    op_OBJECT_PREV=0x20,
    op_RMOV=0x21,
    op_MOV=0x22,
    op_MOVD=0x23,
    op_MOVBI=0x24,
    op_SIZEOF=0x25,
    op_PUT=0x26,
    op_PUTC=0x27,
    op_CHECKLEN=0x28,

    op_MOVU8=0x29,
    op_MOVU16=0x2a,
    op_MOVU32=0x2b,
    op_MOVU64=0x2c,

    op_MOVN=0x2d, /* move ptr = &ptr->_Node[r]; */
	op_GOTO=0x2e,
    op_MOVG=0x2f, /* move ptr = &env->objects[r]; */
    op_MOVSELF=0x30, /* move ptr = instance; */

    op_OPT=0xff, /* unused special instruction for compiler */
};

#endif //SHARP_OPCODE_H
