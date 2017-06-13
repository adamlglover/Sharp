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
[0x14] 20:	pushref
[0x15] 21:	movi #0, ebx
[0x17] 23:	movsl #0
[0x18] 24:	chklen ebx
[0x19] 25:	movnd ebx
[0x1a] 26:	sdelref
[0x1b] 27:	pop
[0x1c] 28:	pushref
[0x1d] 29:	call @4 // <application#Main.foo>
[0x1e] 30:	movl 3
[0x1f] 31:	movi #1, ecx
[0x21] 33:	newi ecx
[0x22] 34:	movi #32, ebx
[0x24] 36:	_putc ebx
[0x25] 37:	iframe
[0x26] 38:	movr adx, fp
[0x27] 39:	smov ebx+4
[0x28] 40:	movi #1, ecx
[0x2a] 42:	addl ecx,4
[0x2b] 43:	inc ebx
[0x2c] 44:	pushr ebx
[0x2d] 45:	call @3 // <application#Main.printc>
[0x2e] 46:	movi #32, ebx
[0x30] 48:	_putc ebx
[0x31] 49:	iframe
[0x32] 50:	movr adx, fp
[0x33] 51:	smov ebx+4
[0x34] 52:	movi #1, ecx
[0x36] 54:	addl ecx,4
[0x37] 55:	mov8 ebx, ebx
[0x38] 56:	pushr ebx
[0x39] 57:	call @3 // <application#Main.printc>
[0x3a] 58:	movi #32, ebx
[0x3c] 60:	_putc ebx
[0x3d] 61:	iframe
[0x3e] 62:	movr adx, fp
[0x3f] 63:	smov ebx+4
[0x40] 64:	movi #1, ecx
[0x42] 66:	addl ecx,4
[0x43] 67:	inc ebx
[0x44] 68:	pushr ebx
[0x45] 69:	call @3 // <application#Main.printc>
[0x46] 70:	movi #32, ebx
[0x48] 72:	_putc ebx
[0x49] 73:	iframe
[0x4a] 74:	inc sp
[0x4b] 75:	iframe
[0x4c] 76:	inc sp
[0x4d] 77:	iframe
[0x4e] 78:	movi #3, ebx
[0x50] 80:	pushr ebx
[0x51] 81:	call @5 // <application#Main.fact>
[0x52] 82:	movr adx, sp
[0x53] 83:	smov ebx+0
[0x54] 84:	pop
[0x55] 85:	pushr ebx
[0x56] 86:	call @5 // <application#Main.fact>
[0x57] 87:	movr adx, sp
[0x58] 88:	smov ebx+0
[0x59] 89:	pop
[0x5a] 90:	pushr ebx
[0x5b] 91:	call @3 // <application#Main.printc>
[0x5c] 92:	movr adx, sp
[0x5d] 93:	smov ebx+0
[0x5e] 94:	put ebx
[0x5f] 95:	ret

func:@8 [Main] in file: odessy/test2.sharp:10:6: note:  
		class Main base Base {
		     ^


Main:

func:@9 [string] in file: src/std/string.sharp:7:15: note:  
		        string( var[] immstr ) {
		              ^


string:
[0x0] 0:	ret

func:@10 [replace] in file: src/std/string.sharp:11:19: note:  
		        fn replace( var pos, var replaceChar ) : string {
		                  ^


replace:
[0x0] 0:	ret

func:@11 [append] in file: src/std/string.sharp:20:18: note:  
		        fn append(var[] immstr) : string {
		                 ^


append:
[0x0] 0:	movl 2
[0x1] 1:	ret

func:@12 [at] in file: src/std/string.sharp:36:14: note:  
		        fn at( var pos ) : __int8 {
		             ^


at:
[0x0] 0:	ret

func:@13 [$operator+] in file: src/std/string.sharp:44:20: note:  
		        fn operator+( var[] immstr ) : string {
		                   ^


$operator+:

func:@14 [$operator+] in file: src/std/string.sharp:48:20: note:  
		        fn operator+( string str ) : string {
		                   ^


$operator+:

func:@15 [$operator==] in file: src/std/string.sharp:52:20: note:  
		        fn operator==( string str ) : var {
		                   ^


$operator==:

func:@16 [$operator==] in file: src/std/string.sharp:63:20: note:  
		        fn operator==( var[] str ) : var {
		                   ^


$operator==:

func:@17 [$operator=] in file: src/std/string.sharp:74:20: note:  
		        fn operator=( var[] immstr ) {
		                   ^


$operator=:

func:@18 [$operator+] in file: src/std/string.sharp:78:20: note:  
		        fn operator+( var char ) : string {
		                   ^


$operator+:

func:@19 [value] in file: src/std/string.sharp:82:17: note:  
		        fn value() : var[] {
		                ^


value:
[0x0] 0:	ret

func:@20 [string] in file: src/std/string.sharp:3:6: note:  
		class string {
		     ^


string:

func:@21 [RuntimeErr] in file: src/std/error/RuntimeErr.sharp:6:6: note:  
		class RuntimeErr base Throwable {
		     ^


RuntimeErr:

func:@22 [Throwable] in file: src/std/error/Throwable.sharp:10:14: note:  
		    Throwable(var[] message) {
		             ^


Throwable:
[0x0] 0:	ret

func:@23 [Throwable] in file: src/std/error/Throwable.sharp:14:14: note:  
		    Throwable(string message) {
		             ^


Throwable:
[0x0] 0:	ret

func:@24 [getStackTrace] in file: src/std/error/Throwable.sharp:18:21: note:  
		    fn getStackTrace() : string {
		                    ^


getStackTrace:
[0x0] 0:	ret

func:@25 [Throwable] in file: src/std/error/Throwable.sharp:6:6: note:  
		class Throwable {
		     ^


Throwable:

func:@26 [transposeArray] in file: src/srt/srt_main.sharp:9:34: note:  
		    private macros transposeArray(var[] args) {
		                                 ^


transposeArray:

func:@27 [__init] in file: src/srt/srt_main.sharp:16:28: note:  
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

func:@28 [Start] in file: src/srt/srt_main.sharp:5:6: note:  
		class Start {
		     ^


Start:

