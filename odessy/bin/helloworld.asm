Object Dump file:
################################

func:@0 [application#Main.getArray] in file: odessy/test3.sharp:7:23: note:  
		    static fn getArray() : var[] {
		                      ^


getArray:
[0x0] 0:	ret

func:@1 [application#Main.main] in file: odessy/test3.sharp:11:19: note:  
		    static fn main(string[] args) {
		                  ^


main:
[0x0] 0:	movi #9, ebx
[0x2] 2:	movr ecx, ebx
[0x3] 3:	movr adx, fp
[0x4] 4:	smovr ecx+1
[0x5] 5:	movl 2
[0x6] 6:	inc sp
[0x7] 7:	iframe
[0x8] 8:	call @0 // <application#Main.getArray>
[0x9] 9:	movl 2
[0xa] 10:	popref
[0xb] 11:	iframe
[0xc] 12:	inc sp
[0xd] 13:	iframe
[0xe] 14:	inc sp
[0xf] 15:	iframe
[0x10] 16:	inc sp
[0x11] 17:	movsl #0
[0x12] 18:	new_class @1 // std#string
[0x13] 19:	inc sp
[0x14] 20:	movsl #0
[0x15] 21:	newstr @0
[0x16] 22:	call @3 // <std#string.string>
[0x17] 23:	movi #118, ebx
[0x19] 25:	pushr ebx
[0x1a] 26:	call @12 // <std#string.$operator+>
[0x1b] 27:	call @16 // <std#System.print>
[0x1c] 28:	iframe
[0x1d] 29:	inc sp
[0x1e] 30:	iframe
[0x1f] 31:	inc sp
[0x20] 32:	iframe
[0x21] 33:	inc sp
[0x22] 34:	movsl #0
[0x23] 35:	new_class @1 // std#string
[0x24] 36:	inc sp
[0x25] 37:	movsl #0
[0x26] 38:	newstr @0
[0x27] 39:	call @3 // <std#string.string>
[0x28] 40:	inc sp
[0x29] 41:	movsl #0
[0x2a] 42:	newstr @1
[0x2b] 43:	call @7 // <std#string.$operator+>
[0x2c] 44:	movsl #0
[0x2d] 45:	inc sp
[0x2e] 46:	iframe
[0x2f] 47:	inc sp
[0x30] 48:	smovobj @0
[0x31] 49:	call @13 // <std#string.value>
[0x32] 50:	call @15 // <std#System.print>
[0x33] 51:	iframe
[0x34] 52:	inc sp
[0x35] 53:	iframe
[0x36] 54:	inc sp
[0x37] 55:	iframe
[0x38] 56:	inc sp
[0x39] 57:	movsl #0
[0x3a] 58:	new_class @1 // std#string
[0x3b] 59:	inc sp
[0x3c] 60:	movsl #0
[0x3d] 61:	newstr @0
[0x3e] 62:	call @3 // <std#string.string>
[0x3f] 63:	inc sp
[0x40] 64:	movsl #0
[0x41] 65:	newstr @1
[0x42] 66:	call @7 // <std#string.$operator+>
[0x43] 67:	call @16 // <std#System.print>
[0x44] 68:	ret

func:@2 [application#Main.Main] in file: odessy/test3.sharp:5:6: note:  
		class Main {
		     ^


Main:
[0x0] 0:	ret

func:@3 [std#string.string] in file: src/std/string.sharp:7:15: note:  
		        string( var[] immstr ) {
		              ^


string:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 0
[0x3] 3:	pushref
[0x4] 4:	movl 1
[0x5] 5:	pushref
[0x6] 6:	call @5 // <std#string.append>
[0x7] 7:	pop
[0x8] 8:	movl 0
[0x9] 9:	movr adx, fp
[0xa] 10:	smovobj @-5
[0xb] 11:	ret

func:@4 [std#string.replace] in file: src/std/string.sharp:11:19: note:  
		        fn replace( var pos, var replaceChar ) : string {
		                  ^


replace:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	sizeof ebx
[0x3] 3:	movr egx, ebx
[0x4] 4:	movr adx, fp
[0x5] 5:	smov ebx+1
[0x6] 6:	ge ebx, egx
[0x7] 7:	movr ebx, cmt
[0x8] 8:	movr cmt, ebx
[0x9] 9:	movi #22, adx
[0xb] 11:	ifne
[0xc] 12:	inc sp
[0xd] 13:	movsl #0
[0xe] 14:	new_class @7 // std.err#RuntimeErr
[0xf] 15:	iframe
[0x10] 16:	pushref
[0x11] 17:	inc sp
[0x12] 18:	movsl #0
[0x13] 19:	newstr @2
[0x14] 20:	call @31 // <std.err#Throwable.Throwable>
[0x15] 21:	throw 
[0x16] 22:	movr adx, fp
[0x17] 23:	smov ebx+2
[0x18] 24:	pushr ebx
[0x19] 25:	movl 0
[0x1a] 26:	movn #0
[0x1b] 27:	pushref
[0x1c] 28:	movr adx, fp
[0x1d] 29:	smov ebx+1
[0x1e] 30:	movsl #0
[0x1f] 31:	movr adx, sp
[0x20] 32:	smov egx+[-1]
[0x21] 33:	chklen ebx
[0x22] 34:	rmov ebx, egx
[0x23] 35:	sdelref
[0x24] 36:	pop
[0x25] 37:	movl 0
[0x26] 38:	movr adx, fp
[0x27] 39:	smovobj @-5
[0x28] 40:	ret

func:@5 [std#string.append] in file: src/std/string.sharp:20:18: note:  
		        fn append(var[] immstr) : string {
		                 ^


append:
[0x0] 0:	movl 2
[0x1] 1:	movl 1
[0x2] 2:	sizeof ebx
[0x3] 3:	pushr ebx
[0x4] 4:	movl 0
[0x5] 5:	movn #0
[0x6] 6:	sizeof ebx
[0x7] 7:	popr ecx
[0x8] 8:	add ebx, ecx -> ebx
[0xa] 10:	inc sp
[0xb] 11:	movsl #0
[0xc] 12:	newi ebx
[0xd] 13:	movl 3
[0xe] 14:	popref
[0xf] 15:	movi #0, egx
[0x11] 17:	movr adx, fp
[0x12] 18:	smovr egx+4
[0x13] 19:	movi #0, ebx
[0x15] 21:	movr ecx, ebx
[0x16] 22:	movr adx, fp
[0x17] 23:	smovr ecx+5
[0x18] 24:	movl 0
[0x19] 25:	movn #0
[0x1a] 26:	sizeof ebx
[0x1b] 27:	movr egx, ebx
[0x1c] 28:	movr adx, fp
[0x1d] 29:	smov ebx+5
[0x1e] 30:	lt ebx, egx
[0x1f] 31:	movr ebx, cmt
[0x20] 32:	movi #65, adx
[0x22] 34:	ifne
[0x23] 35:	movl 0
[0x24] 36:	movn #0
[0x25] 37:	pushref
[0x26] 38:	movr adx, fp
[0x27] 39:	smov ebx+5
[0x28] 40:	movsl #0
[0x29] 41:	chklen ebx
[0x2a] 42:	movx ebx, ebx
[0x2b] 43:	sdelref
[0x2c] 44:	pushr ebx
[0x2d] 45:	movl 3
[0x2e] 46:	pushref
[0x2f] 47:	movr adx, fp
[0x30] 48:	smov ebx+4
[0x31] 49:	movi #1, ecx
[0x33] 51:	addl ecx, @4
[0x34] 52:	movsl #0
[0x35] 53:	movr adx, sp
[0x36] 54:	smov egx+[-1]
[0x37] 55:	chklen ebx
[0x38] 56:	rmov ebx, egx
[0x39] 57:	sdelref
[0x3a] 58:	pop
[0x3b] 59:	movr adx, fp
[0x3c] 60:	smov ebx+5
[0x3d] 61:	movi #1, ecx
[0x3f] 63:	addl ecx, @5
[0x40] 64:	goto @24
[0x41] 65:	movi #0, ebx
[0x43] 67:	movr ecx, ebx
[0x44] 68:	movr adx, fp
[0x45] 69:	smovr ecx+6
[0x46] 70:	movl 1
[0x47] 71:	sizeof ebx
[0x48] 72:	movr egx, ebx
[0x49] 73:	movr adx, fp
[0x4a] 74:	smov ebx+6
[0x4b] 75:	lt ebx, egx
[0x4c] 76:	movr ebx, cmt
[0x4d] 77:	movi #109, adx
[0x4f] 79:	ifne
[0x50] 80:	movl 1
[0x51] 81:	pushref
[0x52] 82:	movr adx, fp
[0x53] 83:	smov ebx+6
[0x54] 84:	movsl #0
[0x55] 85:	chklen ebx
[0x56] 86:	movx ebx, ebx
[0x57] 87:	sdelref
[0x58] 88:	pushr ebx
[0x59] 89:	movl 3
[0x5a] 90:	pushref
[0x5b] 91:	movr adx, fp
[0x5c] 92:	smov ebx+4
[0x5d] 93:	movi #1, ecx
[0x5f] 95:	addl ecx, @4
[0x60] 96:	movsl #0
[0x61] 97:	movr adx, sp
[0x62] 98:	smov egx+[-1]
[0x63] 99:	chklen ebx
[0x64] 100:	rmov ebx, egx
[0x65] 101:	sdelref
[0x66] 102:	pop
[0x67] 103:	movr adx, fp
[0x68] 104:	smov ebx+6
[0x69] 105:	movi #1, ecx
[0x6b] 107:	addl ecx, @6
[0x6c] 108:	goto @70
[0x6d] 109:	inc sp
[0x6e] 110:	iframe
[0x6f] 111:	pushref
[0x70] 112:	movl 3
[0x71] 113:	pushref
[0x72] 114:	movl 3
[0x73] 115:	sizeof ebx
[0x74] 116:	pushr ebx
[0x75] 117:	call @28 // <std#Array.copyOf>
[0x76] 118:	movl 0
[0x77] 119:	movn #0
[0x78] 120:	popref
[0x79] 121:	movl 0
[0x7a] 122:	movr adx, fp
[0x7b] 123:	smovobj @-5
[0x7c] 124:	ret

func:@6 [std#string.at] in file: src/std/string.sharp:36:14: note:  
		        fn at( var pos ) : __int8 {
		             ^


at:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	sizeof ebx
[0x3] 3:	movr egx, ebx
[0x4] 4:	movr adx, fp
[0x5] 5:	smov ebx+1
[0x6] 6:	ge ebx, egx
[0x7] 7:	movr ebx, cmt
[0x8] 8:	movr cmt, ebx
[0x9] 9:	movi #22, adx
[0xb] 11:	ifne
[0xc] 12:	inc sp
[0xd] 13:	movsl #0
[0xe] 14:	new_class @7 // std.err#RuntimeErr
[0xf] 15:	iframe
[0x10] 16:	pushref
[0x11] 17:	inc sp
[0x12] 18:	movsl #0
[0x13] 19:	newstr @3
[0x14] 20:	call @31 // <std.err#Throwable.Throwable>
[0x15] 21:	throw 
[0x16] 22:	movl 0
[0x17] 23:	movn #0
[0x18] 24:	pushref
[0x19] 25:	movr adx, fp
[0x1a] 26:	smov ebx+1
[0x1b] 27:	movsl #0
[0x1c] 28:	chklen ebx
[0x1d] 29:	movx ebx, ebx
[0x1e] 30:	sdelref
[0x1f] 31:	movr adx, fp
[0x20] 32:	smovr ebx+[-5]
[0x21] 33:	ret

func:@7 [std#string.$operator+] in file: src/std/string.sharp:44:20: note:  
		        fn operator+( var[] immstr ) : string {
		                   ^


$operator+:
[0x0] 0:	iframe
[0x1] 1:	movl 1
[0x2] 2:	pushref
[0x3] 3:	call @15 // <std#System.print>
[0x4] 4:	inc sp
[0x5] 5:	iframe
[0x6] 6:	movl 0
[0x7] 7:	pushref
[0x8] 8:	movl 1
[0x9] 9:	pushref
[0xa] 10:	call @5 // <std#string.append>
[0xb] 11:	pop
[0xc] 12:	ret

func:@8 [std#string.$operator+] in file: src/std/string.sharp:49:20: note:  
		        fn operator+( string str ) : string {
		                   ^


$operator+:
[0x0] 0:	iframe
[0x1] 1:	inc sp
[0x2] 2:	movsl #0
[0x3] 3:	newstr @4
[0x4] 4:	call @15 // <std#System.print>
[0x5] 5:	inc sp
[0x6] 6:	iframe
[0x7] 7:	movl 0
[0x8] 8:	pushref
[0x9] 9:	movl 1
[0xa] 10:	movn #0
[0xb] 11:	pushref
[0xc] 12:	call @5 // <std#string.append>
[0xd] 13:	pop
[0xe] 14:	ret

func:@9 [std#string.$operator==] in file: src/std/string.sharp:54:20: note:  
		        fn operator==( string str ) : var {
		                   ^


$operator==:
[0x0] 0:	movi #0, egx
[0x2] 2:	movr adx, fp
[0x3] 3:	smovr egx+3
[0x4] 4:	movi #0, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	movl 0
[0x8] 8:	movn #0
[0x9] 9:	movr adx, sp
[0xa] 10:	smov ebx+0
[0xb] 11:	sizeof egx
[0xc] 12:	lt ebx, egx
[0xd] 13:	movi #47, adx
[0xf] 15:	ifne
[0x10] 16:	movx ebx, ebx
[0x11] 17:	movr ecx, ebx
[0x12] 18:	movr adx, fp
[0x13] 19:	smovr ecx+4
[0x14] 20:	inc sp
[0x15] 21:	iframe
[0x16] 22:	movl 1
[0x17] 23:	pushref
[0x18] 24:	movr adx, fp
[0x19] 25:	smov ebx+3
[0x1a] 26:	movi #1, ecx
[0x1c] 28:	addl ecx, @3
[0x1d] 29:	pushr ebx
[0x1e] 30:	call @6 // <std#string.at>
[0x1f] 31:	movr adx, sp
[0x20] 32:	smov egx+0
[0x21] 33:	pop
[0x22] 34:	movr adx, fp
[0x23] 35:	smov ebx+4
[0x24] 36:	tne ebx, egx
[0x25] 37:	movr ebx, cmt
[0x26] 38:	movr cmt, ebx
[0x27] 39:	movi #47, adx
[0x29] 41:	ifne
[0x2a] 42:	movi #0, ebx
[0x2c] 44:	movr adx, fp
[0x2d] 45:	smovr ebx+[-5]
[0x2e] 46:	ret
[0x2f] 47:	movr adx, sp
[0x30] 48:	smov ebx+0
[0x31] 49:	inc ebx
[0x32] 50:	smovr ebx+0
[0x33] 51:	goto @7
[0x34] 52:	pop
[0x35] 53:	movi #1, ebx
[0x37] 55:	movr adx, fp
[0x38] 56:	smovr ebx+[-5]
[0x39] 57:	ret

func:@10 [std#string.$operator==] in file: src/std/string.sharp:65:20: note:  
		        fn operator==( var[] str ) : var {
		                   ^


$operator==:
[0x0] 0:	movi #0, egx
[0x2] 2:	movr adx, fp
[0x3] 3:	smovr egx+3
[0x4] 4:	movi #0, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	movl 0
[0x8] 8:	movn #0
[0x9] 9:	movr adx, sp
[0xa] 10:	smov ebx+0
[0xb] 11:	sizeof egx
[0xc] 12:	lt ebx, egx
[0xd] 13:	movi #45, adx
[0xf] 15:	ifne
[0x10] 16:	movx ebx, ebx
[0x11] 17:	movr ecx, ebx
[0x12] 18:	movr adx, fp
[0x13] 19:	smovr ecx+4
[0x14] 20:	movl 1
[0x15] 21:	pushref
[0x16] 22:	movr adx, fp
[0x17] 23:	smov ebx+3
[0x18] 24:	movi #1, ecx
[0x1a] 26:	addl ecx, @3
[0x1b] 27:	movsl #0
[0x1c] 28:	chklen ebx
[0x1d] 29:	movx ebx, ebx
[0x1e] 30:	sdelref
[0x1f] 31:	movr egx, ebx
[0x20] 32:	movr adx, fp
[0x21] 33:	smov ebx+4
[0x22] 34:	tne ebx, egx
[0x23] 35:	movr ebx, cmt
[0x24] 36:	movr cmt, ebx
[0x25] 37:	movi #45, adx
[0x27] 39:	ifne
[0x28] 40:	movi #0, ebx
[0x2a] 42:	movr adx, fp
[0x2b] 43:	smovr ebx+[-5]
[0x2c] 44:	ret
[0x2d] 45:	movr adx, sp
[0x2e] 46:	smov ebx+0
[0x2f] 47:	inc ebx
[0x30] 48:	smovr ebx+0
[0x31] 49:	goto @7
[0x32] 50:	pop
[0x33] 51:	movi #1, ebx
[0x35] 53:	movr adx, fp
[0x36] 54:	smovr ebx+[-5]
[0x37] 55:	ret

func:@11 [std#string.$operator=] in file: src/std/string.sharp:76:20: note:  
		        fn operator=( var[] immstr ) {
		                   ^


$operator=:
[0x0] 0:	movl 1
[0x1] 1:	pushref
[0x2] 2:	movl 0
[0x3] 3:	movn #0
[0x4] 4:	popref
[0x5] 5:	ret

func:@12 [std#string.$operator+] in file: src/std/string.sharp:80:20: note:  
		        fn operator+( var char ) : string {
		                   ^


$operator+:
[0x0] 0:	movl 2
[0x1] 1:	movi #1, ebx
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	newi ebx
[0x6] 6:	movl 3
[0x7] 7:	popref
[0x8] 8:	movr adx, fp
[0x9] 9:	smov ebx+1
[0xa] 10:	pushr ebx
[0xb] 11:	movl 3
[0xc] 12:	pushref
[0xd] 13:	movi #0, ebx
[0xf] 15:	movsl #0
[0x10] 16:	movr adx, sp
[0x11] 17:	smov egx+[-1]
[0x12] 18:	chklen ebx
[0x13] 19:	rmov ebx, egx
[0x14] 20:	sdelref
[0x15] 21:	pop
[0x16] 22:	inc sp
[0x17] 23:	iframe
[0x18] 24:	movl 0
[0x19] 25:	pushref
[0x1a] 26:	movl 3
[0x1b] 27:	pushref
[0x1c] 28:	call @5 // <std#string.append>
[0x1d] 29:	pop
[0x1e] 30:	movl 0
[0x1f] 31:	movr adx, fp
[0x20] 32:	smovobj @-5
[0x21] 33:	ret

func:@13 [std#string.value] in file: src/std/string.sharp:87:17: note:  
		        fn value() : var[] {
		                ^


value:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	movr adx, fp
[0x3] 3:	smovobj @-5
[0x4] 4:	ret

func:@14 [std#string.string] in file: src/std/string.sharp:3:6: note:  
		class string {
		     ^


string:
[0x0] 0:	ret

func:@15 [std#System.print] in file: src/std/System.sharp:10:27: note:  
		    public static fn print(var[] data) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	movl 0
[0x2] 2:	pushref
[0x3] 3:	call @23 // <libcore#Libcore.os.printData>
[0x4] 4:	ret

func:@16 [std#System.print] in file: src/std/System.sharp:14:27: note:  
		    public static fn print(string str) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	movl 0
[0x4] 4:	pushref
[0x5] 5:	call @13 // <std#string.value>
[0x6] 6:	call @23 // <libcore#Libcore.os.printData>
[0x7] 7:	ret

func:@17 [std#System.print] in file: src/std/System.sharp:18:27: note:  
		    public static fn print(var data) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+0
[0x3] 3:	pushr ebx
[0x4] 4:	movi #0, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	call @25 // <libcore#Libcore.os.__print_>
[0x8] 8:	ret

func:@18 [std#System.printChar] in file: src/std/System.sharp:22:31: note:  
		    public static fn printChar(var data) {
		                              ^


printChar:
[0x0] 0:	iframe
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+0
[0x3] 3:	pushr ebx
[0x4] 4:	movi #1, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	call @25 // <libcore#Libcore.os.__print_>
[0x8] 8:	ret

func:@19 [std#System.System] in file: src/std/System.sharp:8:6: note:  
		class System {
		     ^


System:
[0x0] 0:	ret

func:@20 [application#Start.transposeArray] in file: src/srt/srt_main.sharp:9:34: note:  
		    private macros transposeArray(var[] args) {
		                                 ^


transposeArray:
[0x0] 0:	movl 1
[0x1] 1:	del
[0x2] 2:	ret

func:@21 [application#Start.__init] in file: src/srt/srt_main.sharp:18:28: note:  
		    public static fn __init(var[] args) : var {
		                           ^


__init:
[0x0] 0:	movl 1
[0x1] 1:	movl 1
[0x2] 2:	del
[0x3] 3:	movi #0, ebx
[0x5] 5:	movr ecx, ebx
[0x6] 6:	movr adx, fp
[0x7] 7:	smovr ecx+2
[0x8] 8:	iframe
[0x9] 9:	movl 1
[0xa] 10:	pushref
[0xb] 11:	call @1 // <application#Main.main>
[0xc] 12:	movr adx, fp
[0xd] 13:	smov ebx+2
[0xe] 14:	movr adx, fp
[0xf] 15:	smovr ebx+[-5]
[0x10] 16:	ret

func:@22 [application#Start.Start] in file: src/srt/srt_main.sharp:5:6: note:  
		class Start {
		     ^


Start:
[0x0] 0:	ret

func:@23 [libcore#Libcore.os.printData] in file: src/std/internal/Libcore.sharp:11:35: note:  
		        public static fn printData(var[] data) {
		                                  ^


printData:
[0x0] 0:	iframe
[0x1] 1:	movl 0
[0x2] 2:	pushref
[0x3] 3:	call @24 // <libcore#Libcore.os.__print_>
[0x4] 4:	ret

func:@24 [libcore#Libcore.os.__print_] in file: src/std/internal/Libcore.sharp:19:35: note:  
		        private static fn __print_(var[] message) {
		                                  ^


__print_:
[0x0] 0:	movl 0
[0x1] 1:	sizeof ecf
[0x2] 2:	movi #0, cx
[0x4] 4:	test cx, ecf
[0x5] 5:	movi #9, adx
[0x7] 7:	ifne
[0x8] 8:	ret
[0x9] 9:	movi #11, adx
[0xb] 11:	chklen cx
[0xc] 12:	movx ebx, cx
[0xd] 13:	_putc ebx
[0xe] 14:	inc cx
[0xf] 15:	lt cx, ecf
[0x10] 16:	bre
[0x11] 17:	ret

func:@25 [libcore#Libcore.os.__print_] in file: src/std/internal/Libcore.sharp:44:34: note:  
		        public static fn __print_(var char, var isCharacter) {
		                                 ^


__print_:
[0x0] 0:	movr adx, fp
[0x1] 1:	smov ebx+1
[0x2] 2:	movr cmt, ebx
[0x3] 3:	loadx adx
[0x4] 4:	iadd adx, #7
[0x5] 5:	ifne
[0x6] 6:	movr adx, fp
[0x7] 7:	smov ebx+0
[0x8] 8:	_putc ebx
[0x9] 9:	goto @13
[0xa] 10:	movr adx, fp
[0xb] 11:	smov ebx+0
[0xc] 12:	put ebx
[0xd] 13:	ret

func:@26 [libcore#Libcore.os.os] in file: src/std/internal/Libcore.sharp:10:13: note:  
		    class os {
		            ^


os:
[0x0] 0:	ret

func:@27 [libcore#Libcore.Libcore] in file: src/std/internal/Libcore.sharp:8:6: note:  
		class Libcore {
		     ^


Libcore:
[0x0] 0:	ret

func:@28 [std#Array.copyOf] in file: src/std/Array.sharp:5:14: note:  
		    fn copyOf(var[] data, var len) : var[] {
		             ^


copyOf:
[0x0] 0:	movl 3
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+2
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	newi ebx
[0x6] 6:	movl 4
[0x7] 7:	popref
[0x8] 8:	movi #0, ebx
[0xa] 10:	movr ecx, ebx
[0xb] 11:	movr adx, fp
[0xc] 12:	smovr ecx+5
[0xd] 13:	movr adx, fp
[0xe] 14:	smov ebx+2
[0xf] 15:	movr egx, ebx
[0x10] 16:	movr adx, fp
[0x11] 17:	smov ebx+5
[0x12] 18:	lt ebx, egx
[0x13] 19:	movr ebx, cmt
[0x14] 20:	movi #49, adx
[0x16] 22:	ifne
[0x17] 23:	movl 1
[0x18] 24:	pushref
[0x19] 25:	movr adx, fp
[0x1a] 26:	smov ebx+5
[0x1b] 27:	movsl #0
[0x1c] 28:	chklen ebx
[0x1d] 29:	movx ebx, ebx
[0x1e] 30:	sdelref
[0x1f] 31:	pushr ebx
[0x20] 32:	movl 4
[0x21] 33:	pushref
[0x22] 34:	movr adx, fp
[0x23] 35:	smov ebx+5
[0x24] 36:	movsl #0
[0x25] 37:	movr adx, sp
[0x26] 38:	smov egx+[-1]
[0x27] 39:	chklen ebx
[0x28] 40:	rmov ebx, egx
[0x29] 41:	sdelref
[0x2a] 42:	pop
[0x2b] 43:	movr adx, fp
[0x2c] 44:	smov ebx+5
[0x2d] 45:	movi #1, ecx
[0x2f] 47:	addl ecx, @5
[0x30] 48:	goto @13
[0x31] 49:	movl 4
[0x32] 50:	movr adx, fp
[0x33] 51:	smovobj @-5
[0x34] 52:	ret

func:@29 [std#Array.Array] in file: src/std/Array.sharp:3:6: note:  
		class Array {
		     ^


Array:
[0x0] 0:	ret

func:@30 [std.err#RuntimeErr.RuntimeErr] in file: src/std/error/RuntimeErr.sharp:6:6: note:  
		class RuntimeErr base Throwable {
		     ^


RuntimeErr:
[0x0] 0:	ret

func:@31 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:10:14: note:  
		    Throwable(var[] message) {
		             ^


Throwable:
[0x0] 0:	movl 1
[0x1] 1:	pushref
[0x2] 2:	movl 0
[0x3] 3:	movn #0
[0x4] 4:	popref
[0x5] 5:	movl 0
[0x6] 6:	movr adx, fp
[0x7] 7:	smovobj @-5
[0x8] 8:	ret

func:@32 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:14:14: note:  
		    Throwable(string message) {
		             ^


Throwable:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@33 [std.err#Throwable.getStackTrace] in file: src/std/error/Throwable.sharp:18:21: note:  
		    fn getStackTrace() : string {
		                    ^


getStackTrace:
[0x0] 0:	ret

func:@34 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:6:6: note:  
		class Throwable {
		     ^


Throwable:
[0x0] 0:	ret

