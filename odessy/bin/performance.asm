Object Dump file:
################################

func:@0 [print2] in file: odessy/test2.sharp:5:21: note:  
		    static fn print2(var[] message) {
		                    ^


print2:
[0x0] 0:	movl 0
[0x1] 1:	sizeof ecf
[0x2] 2:	movi #0, cx
[0x4] 4:	test cx, ecf
[0x5] 5:	movi #9, adx
[0x7] 7:	ifne
[0x8] 8:	ret
[0x9] 9:	movi #11, adx
[0xb] 11:	movx ebx, cx
[0xc] 12:	_putc ebx
[0xd] 13:	inc cx
[0xe] 14:	lt cx, ecf
[0xf] 15:	bre
[0x10] 16:	ret

func:@1 [Base] in file: odessy/test2.sharp:3:6: note:  
		class Base {
		     ^


Base:

func:@2 [print] in file: odessy/test2.sharp:14:20: note:  
		    static fn print(var[] message) {
		                   ^


print:
[0x0] 0:	movl 0
[0x1] 1:	sizeof ecf
[0x2] 2:	movi #0, cx
[0x4] 4:	test cx, ecf
[0x5] 5:	movi #9, adx
[0x7] 7:	ifne
[0x8] 8:	ret
[0x9] 9:	movi #11, adx
[0xb] 11:	movx ebx, cx
[0xc] 12:	_putc ebx
[0xd] 13:	inc cx
[0xe] 14:	lt cx, ecf
[0xf] 15:	bre
[0x10] 16:	ret

func:@3 [printc] in file: odessy/test2.sharp:18:21: note:  
		    static fn printc(var char) {
		                    ^


printc:
[0x0] 0:	movr adx, fp
[0x1] 1:	smov ebx+0
[0x2] 2:	put ebx
[0x3] 3:	ret

func:@4 [foo] in file: odessy/test2.sharp:22:18: note:  
		    static fn foo(Main main) {
		                 ^


foo:
[0x0] 0:	ret

func:@5 [fact] in file: odessy/test2.sharp:26:19: note:  
		    static fn fact(var x) : var {
		                  ^


fact:
[0x0] 0:	movr adx, fp
[0x1] 1:	smov ebx+0
[0x2] 2:	movi #1, ecx
[0x4] 4:	movi #11, adx
[0x6] 6:	le ebx, ecx
[0x7] 7:	ifne
[0x8] 8:	movr adx, fp
[0x9] 9:	smovr ebx+[-5]
[0xa] 10:	ret
[0xb] 11:	movr ecf, ebx
[0xc] 12:	dec ecf
[0xd] 13:	inc sp
[0xe] 14:	iframe
[0xf] 15:	pushr ecf
[0x10] 16:	call @5 // <application#Main.fact>
[0x11] 17:	movr adx, sp
[0x12] 18:	smov ebx+0
[0x13] 19:	movr adx, fp
[0x14] 20:	smov ecx+0
[0x15] 21:	mul ecx, ebx
[0x16] 22:	smovr bmr+[-5]
[0x17] 23:	ret
[0x18] 24:	movl 1
[0x19] 25:	ret

func:@6 [$operator++] in file: odessy/test2.sharp:59:23: note:  
		    static fn operator++(var x) {
		                      ^


$operator++:

func:@7 [main] in file: odessy/test2.sharp:63:19: note:  
		    static fn main(string[] args) : var {
		                  ^


main:
[0x0] 0:	movl 1
[0x1] 1:	newstr @0
[0x2] 2:	movl 2
[0x3] 3:	iframe
[0x4] 4:	movr adx, fp
[0x5] 5:	smov ebx+1
[0x6] 6:	pushref
[0x7] 7:	call @0 // <application#Base.print2>
[0x8] 8:	inc sp
[0x9] 9:	iframe
[0xa] 10:	movi #8, ebx
[0xc] 12:	pushr ebx
[0xd] 13:	call @5 // <application#Main.fact>
[0xe] 14:	iframe
[0xf] 15:	inc sp
[0x10] 16:	movsl #0
[0x11] 17:	movi #9, ebx
[0x13] 19:	new_objarry ebx
[0x14] 20:	movi #0, ebx
[0x16] 22:	chklen ebx
[0x17] 23:	movnd ebx
[0x18] 24:	pop
[0x19] 25:	pushref
[0x1a] 26:	call @4 // <application#Main.foo>
[0x1b] 27:	movl 3
[0x1c] 28:	movi #1, ecx
[0x1e] 30:	newi ecx
[0x1f] 31:	movi #32, ebx
[0x21] 33:	_putc ebx
[0x22] 34:	iframe
[0x23] 35:	movr adx, fp
[0x24] 36:	smov ebx+4
[0x25] 37:	movi #1, ecx
[0x27] 39:	addl ecx,4
[0x28] 40:	inc ebx
[0x29] 41:	pushr ebx
[0x2a] 42:	call @3 // <application#Main.printc>
[0x2b] 43:	movi #32, ebx
[0x2d] 45:	_putc ebx
[0x2e] 46:	iframe
[0x2f] 47:	movr adx, fp
[0x30] 48:	smov ebx+4
[0x31] 49:	movi #1, ecx
[0x33] 51:	addl ecx,4
[0x34] 52:	mov8 ebx, ebx
[0x35] 53:	pushr ebx
[0x36] 54:	call @3 // <application#Main.printc>
[0x37] 55:	movi #32, ebx
[0x39] 57:	_putc ebx
[0x3a] 58:	iframe
[0x3b] 59:	movr adx, fp
[0x3c] 60:	smov ebx+4
[0x3d] 61:	movi #1, ecx
[0x3f] 63:	addl ecx,4
[0x40] 64:	inc ebx
[0x41] 65:	pushr ebx
[0x42] 66:	call @3 // <application#Main.printc>
[0x43] 67:	movi #32, ebx
[0x45] 69:	_putc ebx
[0x46] 70:	iframe
[0x47] 71:	movr adx, fp
[0x48] 72:	smov ebx+4
[0x49] 73:	movr cmt, ebx
[0x4a] 74:	loadf adx,5 // store pc at <@79>
[0x4b] 75:	ifne
[0x4c] 76:	movi #1, ebx
[0x4e] 78:	skp @2 // pc = 80
[0x4f] 79:	movi #2, ebx
[0x51] 81:	pushr ebx
[0x52] 82:	call @3 // <application#Main.printc>
[0x53] 83:	movr adx, sp
[0x54] 84:	smov ebx+0
[0x55] 85:	put ebx
[0x56] 86:	ret

func:@8 [Main] in file: odessy/test2.sharp:10:6: note:  
		class Main base Base {
		     ^


Main:

func:@9 [RuntimeErr] in file: src/std/error/RuntimeErr.sharp:6:6: note:  
		class RuntimeErr base Throwable {
		     ^


RuntimeErr:

func:@10 [string] in file: src/std/string.sharp:7:15: note:  
		        string( var[] immstr ) {
		              ^


string:
[0x0] 0:	ret

func:@11 [replace] in file: src/std/string.sharp:11:19: note:  
		        fn replace( var pos, var replaceChar ) : string {
		                  ^


replace:
[0x0] 0:	ret

func:@12 [append] in file: src/std/string.sharp:20:18: note:  
		        fn append(var[] immstr) : string {
		                 ^


append:
[0x0] 0:	movl 2
[0x1] 1:	ret

func:@13 [at] in file: src/std/string.sharp:36:14: note:  
		        fn at( var pos ) : __int8 {
		             ^


at:
[0x0] 0:	ret

func:@14 [$operator+] in file: src/std/string.sharp:44:20: note:  
		        fn operator+( var[] immstr ) : string {
		                   ^


$operator+:

func:@15 [$operator+] in file: src/std/string.sharp:48:20: note:  
		        fn operator+( string str ) : string {
		                   ^


$operator+:

func:@16 [$operator==] in file: src/std/string.sharp:52:20: note:  
		        fn operator==( string str ) : var {
		                   ^


$operator==:

func:@17 [$operator==] in file: src/std/string.sharp:63:20: note:  
		        fn operator==( var[] str ) : var {
		                   ^


$operator==:

func:@18 [$operator=] in file: src/std/string.sharp:74:20: note:  
		        fn operator=( var[] immstr ) {
		                   ^


$operator=:

func:@19 [$operator+] in file: src/std/string.sharp:78:20: note:  
		        fn operator+( var char ) : string {
		                   ^


$operator+:

func:@20 [value] in file: src/std/string.sharp:82:17: note:  
		        fn value() : var[] {
		                ^


value:
[0x0] 0:	ret

func:@21 [string] in file: src/std/string.sharp:3:6: note:  
		class string {
		     ^


string:

func:@22 [transposeArray] in file: src/srt/srt_main.sharp:9:34: note:  
		    private macros transposeArray(var[] args) {
		                                 ^


transposeArray:

func:@23 [__init] in file: src/srt/srt_main.sharp:16:28: note:  
		    public static fn __init(var[] args) : var {
		                           ^


__init:
[0x0] 0:	movl 1
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	movl 1
[0x4] 4:	pushref
[0x5] 5:	call @7 // <application#Main.main>
[0x6] 6:	movr adx, sp
[0x7] 7:	smov ebx+0
[0x8] 8:	pop
[0x9] 9:	ret

func:@24 [Start] in file: src/srt/srt_main.sharp:5:6: note:  
		class Start {
		     ^


Start:

func:@25 [Throwable] in file: src/std/error/Throwable.sharp:10:14: note:  
		    Throwable(var[] message) {
		             ^


Throwable:
[0x0] 0:	ret

func:@26 [Throwable] in file: src/std/error/Throwable.sharp:14:14: note:  
		    Throwable(string message) {
		             ^


Throwable:
[0x0] 0:	ret

func:@27 [getStackTrace] in file: src/std/error/Throwable.sharp:18:21: note:  
		    fn getStackTrace() : string {
		                    ^


getStackTrace:
[0x0] 0:	ret

func:@28 [Throwable] in file: src/std/error/Throwable.sharp:6:6: note:  
		class Throwable {
		     ^


Throwable:

