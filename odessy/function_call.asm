/**********************************
* Sharp x64 asm                   *
* Created by BraxtonN on 5/1/2017 *
* Description:                    *
***********************************/

.function_call:
    movl <message>
    iframe
    pushref
    call <foo>
    ret
