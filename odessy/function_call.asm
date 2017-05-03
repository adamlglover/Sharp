/**********************************
* Sharp x64 asm                   *
* Created by BraxtonN on 5/1/2017 *
* Description:                    *
***********************************/

.function_call:
    movi #0,%egx
    movi #100000,%edf
    ._run:
        movi $_run,%adx
        movl <message>
        iframe
        pushref
        call <foo>
        inc %egx
        put %egx
        lt %egx,%edf
        bre

    movl <start>            // start = new Start();
    new_class <Start>
    iframe
    pushref
    call <Start.Start>      // call constructor
    //ret
