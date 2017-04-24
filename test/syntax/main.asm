

/*************************************
*
* Main assembly
*
*************************************/

.fn main:
    .locals 8
    .regs 5

    .L1:
    movl 8          // more data
    movx 6          // do this
    movbi #4.55
    movbi #4,#7

    .L2:            // acces stack @0xff
    ret             // write file
!
