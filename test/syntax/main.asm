

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

    .L2:            // acces stack @0xff
    ret             // write file
!
