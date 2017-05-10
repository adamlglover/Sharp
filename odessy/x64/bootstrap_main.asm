/*******************************************
* Sharp x64 asm                            *
* Created by bknun on 5/9/2017            *
* Description:                             *
********************************************/

.bootstrap_main:
    inc %sp
    iframe
    movl <pArgs>
    pushref

    call <application#Main.main> // call main method
    ret