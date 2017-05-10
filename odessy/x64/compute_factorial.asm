/*******************************************
* Sharp x64 asm                            *
* Created by bknun on 5/9/2017            *
* Description:                             *
********************************************/

.compute_factorial:
    inc %sp
    iframe
    movi #7,%ebx
    pushr %ebx
    call <fact>

    iframe
    movl <str>
    pushref
    call <print>

    movr %adx,%fp
    smov %ebx-0x5
    put %ebx
    ret