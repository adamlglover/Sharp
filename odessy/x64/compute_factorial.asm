/*******************************************
* Sharp x64 asm                            *
* Created by bknun on 5/9/2017            *
* Description:                             *
********************************************/

.compute_factorial:
    iframe
    movl <str>
    pushref
    call <print>

    inc %sp
    iframe
    movi #10240,%ebx
    pushr %ebx
    call <fact>

    movr %adx,%sp
    smov %ebx
    put %ebx
    ret