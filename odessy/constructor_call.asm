/*******************************************
* Sharp x64 asm                            *
* Created by bknun on 5/1/2017            *
* Description:                             *
********************************************/

.constructor_call:
    movl 0          // get reference to self
    movn 0          // get address to x in class Start
    movi #1,%ehf
    new_i %ehf

    movl 0
    del_ref
    ret