    _sizeof %ecf               // get size of message
    movi #0, %cx

._print:
    movi $_print+2,%adx    // store address of label <_print> omit store instr
    movx %ebx,%cx          // get char at [index]
    _putc %ebx
    inc %cx
    lt %cx,%ecf            // check if we have reached the end of the string
    bre
ret