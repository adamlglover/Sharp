Object Dump file:
################################

func:@0 [application#Main.main] in file: odessy/test3.sharp:7:19: note:  
		    static fn main(string[] args) {
		                  ^


main:
[0x0] 0:	iframe
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	movi #129, ebx
[0x5] 5:	pushr ebx
[0x6] 6:	call @16 // <std#string.convertInt>
[0x7] 7:	call @26 // <std#System.print>
[0x8] 8:	movi #9, ebx
[0xa] 10:	movr ecx, ebx
[0xb] 11:	movr adx, fp
[0xc] 12:	smovr ecx+1
[0xd] 13:	movl 2
[0xe] 14:	inc sp
[0xf] 15:	iframe
[0x10] 16:	inc sp
[0x11] 17:	movsl #0
[0x12] 18:	new_class @2 // std#string
[0x13] 19:	inc sp
[0x14] 20:	movsl #0
[0x15] 21:	newstr @0
[0x16] 22:	call @3 // <std#string.string>
[0x17] 23:	movl 2
[0x18] 24:	popref
[0x19] 25:	inc sp
[0x1a] 26:	iframe
[0x1b] 27:	movl 2
[0x1c] 28:	pushref
[0x1d] 29:	inc sp
[0x1e] 30:	movsl #0
[0x1f] 31:	newstr @1
[0x20] 32:	call @14 // <std#string.$operator+=>
[0x21] 33:	pop
[0x22] 34:	iframe
[0x23] 35:	inc sp
[0x24] 36:	iframe
[0x25] 37:	movl 2
[0x26] 38:	pushref
[0x27] 39:	movi #10, ebx
[0x29] 41:	pushr ebx
[0x2a] 42:	call @12 // <std#string.$operator+>
[0x2b] 43:	call @26 // <std#System.print>
[0x2c] 44:	iframe
[0x2d] 45:	inc sp
[0x2e] 46:	iframe
[0x2f] 47:	inc sp
[0x30] 48:	iframe
[0x31] 49:	inc sp
[0x32] 50:	iframe
[0x33] 51:	inc sp
[0x34] 52:	movsl #0
[0x35] 53:	new_class @2 // std#string
[0x36] 54:	inc sp
[0x37] 55:	movsl #0
[0x38] 56:	newstr @2
[0x39] 57:	call @3 // <std#string.string>
[0x3a] 58:	movi #118, ebx
[0x3c] 60:	pushr ebx
[0x3d] 61:	call @12 // <std#string.$operator+>
[0x3e] 62:	call @15 // <std#string.value>
[0x3f] 63:	movi #0, ebx
[0x41] 65:	movsl #0
[0x42] 66:	chklen ebx
[0x43] 67:	movx ebx, ebx
[0x44] 68:	pop
[0x45] 69:	pushr ebx
[0x46] 70:	call @27 // <std#System.print>
[0x47] 71:	iframe
[0x48] 72:	inc sp
[0x49] 73:	iframe
[0x4a] 74:	inc sp
[0x4b] 75:	iframe
[0x4c] 76:	inc sp
[0x4d] 77:	movsl #0
[0x4e] 78:	new_class @2 // std#string
[0x4f] 79:	inc sp
[0x50] 80:	movsl #0
[0x51] 81:	newstr @2
[0x52] 82:	call @3 // <std#string.string>
[0x53] 83:	inc sp
[0x54] 84:	movsl #0
[0x55] 85:	newstr @3
[0x56] 86:	call @7 // <std#string.$operator+>
[0x57] 87:	call @26 // <std#System.print>
[0x58] 88:	ret

func:@1 [application#Main.Main] in file: odessy/test3.sharp:5:6: note:  
		class Main {
		     ^


Main:
[0x0] 0:	ret

func:@2 [std.err#RuntimeErr.RuntimeErr] in file: src/std/error/RuntimeErr.sharp:6:6: note:  
		class RuntimeErr base Throwable {
		     ^


RuntimeErr:
[0x0] 0:	ret

func:@3 [std#string.string] in file: src/std/string.sharp:7:15: note:  
		        string( var[] immstr )
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

func:@4 [std#string.replace] in file: src/std/string.sharp:12:19: note:  
		        fn replace( var pos, var replaceChar ) : string
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
[0xd] 13:	iframe
[0xe] 14:	inc sp
[0xf] 15:	movsl #0
[0x10] 16:	new_class @1 // std.err#RuntimeErr
[0x11] 17:	inc sp
[0x12] 18:	movsl #0
[0x13] 19:	newstr @4
[0x14] 20:	call @21 // <std.err#Throwable.Throwable>
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

func:@5 [std#string.append] in file: src/std/string.sharp:22:18: note:  
		        fn append(var[] immstr) : string
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
[0xd] 13:	movl 2
[0xe] 14:	popref
[0xf] 15:	movi #0, ebx
[0x11] 17:	movr ecx, ebx
[0x12] 18:	movr adx, fp
[0x13] 19:	smovr ecx+3
[0x14] 20:	movi #0, ebx
[0x16] 22:	movr ecx, ebx
[0x17] 23:	movr adx, fp
[0x18] 24:	smovr ecx+4
[0x19] 25:	movl 0
[0x1a] 26:	movn #0
[0x1b] 27:	sizeof ebx
[0x1c] 28:	movr egx, ebx
[0x1d] 29:	movr adx, fp
[0x1e] 30:	smov ebx+4
[0x1f] 31:	lt ebx, egx
[0x20] 32:	movr ebx, cmt
[0x21] 33:	movi #66, adx
[0x23] 35:	ifne
[0x24] 36:	movl 0
[0x25] 37:	movn #0
[0x26] 38:	pushref
[0x27] 39:	movr adx, fp
[0x28] 40:	smov ebx+4
[0x29] 41:	movsl #0
[0x2a] 42:	chklen ebx
[0x2b] 43:	movx ebx, ebx
[0x2c] 44:	sdelref
[0x2d] 45:	pushr ebx
[0x2e] 46:	movl 2
[0x2f] 47:	pushref
[0x30] 48:	movr adx, fp
[0x31] 49:	smov ebx+3
[0x32] 50:	movi #1, ecx
[0x34] 52:	addl ecx, @3
[0x35] 53:	movsl #0
[0x36] 54:	movr adx, sp
[0x37] 55:	smov egx+[-1]
[0x38] 56:	chklen ebx
[0x39] 57:	rmov ebx, egx
[0x3a] 58:	sdelref
[0x3b] 59:	pop
[0x3c] 60:	movr adx, fp
[0x3d] 61:	smov ebx+4
[0x3e] 62:	movi #1, ecx
[0x40] 64:	addl ecx, @4
[0x41] 65:	goto @25
[0x42] 66:	movi #0, ebx
[0x44] 68:	movr ecx, ebx
[0x45] 69:	movr adx, fp
[0x46] 70:	smovr ecx+5
[0x47] 71:	movl 1
[0x48] 72:	sizeof ebx
[0x49] 73:	movr egx, ebx
[0x4a] 74:	movr adx, fp
[0x4b] 75:	smov ebx+5
[0x4c] 76:	lt ebx, egx
[0x4d] 77:	movr ebx, cmt
[0x4e] 78:	movi #110, adx
[0x50] 80:	ifne
[0x51] 81:	movl 1
[0x52] 82:	pushref
[0x53] 83:	movr adx, fp
[0x54] 84:	smov ebx+5
[0x55] 85:	movsl #0
[0x56] 86:	chklen ebx
[0x57] 87:	movx ebx, ebx
[0x58] 88:	sdelref
[0x59] 89:	pushr ebx
[0x5a] 90:	movl 2
[0x5b] 91:	pushref
[0x5c] 92:	movr adx, fp
[0x5d] 93:	smov ebx+3
[0x5e] 94:	movi #1, ecx
[0x60] 96:	addl ecx, @3
[0x61] 97:	movsl #0
[0x62] 98:	movr adx, sp
[0x63] 99:	smov egx+[-1]
[0x64] 100:	chklen ebx
[0x65] 101:	rmov ebx, egx
[0x66] 102:	sdelref
[0x67] 103:	pop
[0x68] 104:	movr adx, fp
[0x69] 105:	smov ebx+5
[0x6a] 106:	movi #1, ecx
[0x6c] 108:	addl ecx, @5
[0x6d] 109:	goto @71
[0x6e] 110:	inc sp
[0x6f] 111:	iframe
[0x70] 112:	pushref
[0x71] 113:	movl 2
[0x72] 114:	pushref
[0x73] 115:	movl 2
[0x74] 116:	sizeof ebx
[0x75] 117:	pushr ebx
[0x76] 118:	call @35 // <std#Array.copyOf>
[0x77] 119:	movl 0
[0x78] 120:	movn #0
[0x79] 121:	popref
[0x7a] 122:	iframe
[0x7b] 123:	movl 0
[0x7c] 124:	movn #0
[0x7d] 125:	sizeof ebx
[0x7e] 126:	pushr ebx
[0x7f] 127:	call @27 // <std#System.print>
[0x80] 128:	movl 0
[0x81] 129:	movr adx, fp
[0x82] 130:	smovobj @-5
[0x83] 131:	ret

func:@6 [std#string.at] in file: src/std/string.sharp:41:14: note:  
		        fn at( var pos ) : __int8
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
[0x9] 9:	movi #44, adx
[0xb] 11:	ifne
[0xc] 12:	inc sp
[0xd] 13:	iframe
[0xe] 14:	inc sp
[0xf] 15:	movsl #0
[0x10] 16:	new_class @1 // std.err#RuntimeErr
[0x11] 17:	inc sp
[0x12] 18:	iframe
[0x13] 19:	inc sp
[0x14] 20:	iframe
[0x15] 21:	inc sp
[0x16] 22:	iframe
[0x17] 23:	inc sp
[0x18] 24:	iframe
[0x19] 25:	inc sp
[0x1a] 26:	movsl #0
[0x1b] 27:	new_class @2 // std#string
[0x1c] 28:	inc sp
[0x1d] 29:	movsl #0
[0x1e] 30:	newstr @6
[0x1f] 31:	call @3 // <std#string.string>
[0x20] 32:	movl 0
[0x21] 33:	movn #0
[0x22] 34:	sizeof ebx
[0x23] 35:	pushr ebx
[0x24] 36:	call @12 // <std#string.$operator+>
[0x25] 37:	inc sp
[0x26] 38:	movsl #0
[0x27] 39:	newstr @5
[0x28] 40:	call @7 // <std#string.$operator+>
[0x29] 41:	call @15 // <std#string.value>
[0x2a] 42:	call @21 // <std.err#Throwable.Throwable>
[0x2b] 43:	throw 
[0x2c] 44:	movl 0
[0x2d] 45:	movn #0
[0x2e] 46:	pushref
[0x2f] 47:	movr adx, fp
[0x30] 48:	smov ebx+1
[0x31] 49:	movsl #0
[0x32] 50:	chklen ebx
[0x33] 51:	movx ebx, ebx
[0x34] 52:	sdelref
[0x35] 53:	movr adx, fp
[0x36] 54:	smovr ebx+[-5]
[0x37] 55:	ret

func:@7 [std#string.$operator+] in file: src/std/string.sharp:51:20: note:  
		        fn operator+( var[] immstr ) : string
		                   ^


$operator+:
[0x0] 0:	movl 2
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	new_class @2 // std#string
[0x6] 6:	movl 0
[0x7] 7:	movn #0
[0x8] 8:	pushref
[0x9] 9:	call @3 // <std#string.string>
[0xa] 10:	movl 2
[0xb] 11:	popref
[0xc] 12:	inc sp
[0xd] 13:	iframe
[0xe] 14:	movl 2
[0xf] 15:	pushref
[0x10] 16:	movl 1
[0x11] 17:	pushref
[0x12] 18:	call @5 // <std#string.append>
[0x13] 19:	movsl #0
[0x14] 20:	movr adx, fp
[0x15] 21:	smovobj @-5
[0x16] 22:	ret

func:@8 [std#string.$operator+] in file: src/std/string.sharp:57:20: note:  
		        fn operator+( string str ) : string
		                   ^


$operator+:
[0x0] 0:	movl 2
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	new_class @2 // std#string
[0x6] 6:	movl 0
[0x7] 7:	movn #0
[0x8] 8:	pushref
[0x9] 9:	call @3 // <std#string.string>
[0xa] 10:	movl 2
[0xb] 11:	popref
[0xc] 12:	inc sp
[0xd] 13:	iframe
[0xe] 14:	movl 2
[0xf] 15:	pushref
[0x10] 16:	inc sp
[0x11] 17:	iframe
[0x12] 18:	movl 1
[0x13] 19:	pushref
[0x14] 20:	call @15 // <std#string.value>
[0x15] 21:	call @5 // <std#string.append>
[0x16] 22:	movsl #0
[0x17] 23:	movr adx, fp
[0x18] 24:	smovobj @-5
[0x19] 25:	ret

func:@9 [std#string.$operator==] in file: src/std/string.sharp:63:20: note:  
		        fn operator==( string str ) : var
		                   ^


$operator==:
[0x0] 0:	movi #0, egx
[0x2] 2:	movr adx, fp
[0x3] 3:	smovr egx+2
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
[0x13] 19:	smovr ecx+3
[0x14] 20:	inc sp
[0x15] 21:	iframe
[0x16] 22:	movl 1
[0x17] 23:	pushref
[0x18] 24:	movr adx, fp
[0x19] 25:	smov ebx+2
[0x1a] 26:	movi #1, ecx
[0x1c] 28:	addl ecx, @2
[0x1d] 29:	pushr ebx
[0x1e] 30:	call @6 // <std#string.at>
[0x1f] 31:	movr adx, sp
[0x20] 32:	smov egx+0
[0x21] 33:	pop
[0x22] 34:	movr adx, fp
[0x23] 35:	smov ebx+3
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

func:@10 [std#string.$operator==] in file: src/std/string.sharp:75:20: note:  
		        fn operator==( var[] str ) : var
		                   ^


$operator==:
[0x0] 0:	movi #0, egx
[0x2] 2:	movr adx, fp
[0x3] 3:	smovr egx+2
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
[0x13] 19:	smovr ecx+3
[0x14] 20:	movl 1
[0x15] 21:	pushref
[0x16] 22:	movr adx, fp
[0x17] 23:	smov ebx+2
[0x18] 24:	movi #1, ecx
[0x1a] 26:	addl ecx, @2
[0x1b] 27:	movsl #0
[0x1c] 28:	chklen ebx
[0x1d] 29:	movx ebx, ebx
[0x1e] 30:	sdelref
[0x1f] 31:	movr egx, ebx
[0x20] 32:	movr adx, fp
[0x21] 33:	smov ebx+3
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

func:@11 [std#string.$operator=] in file: src/std/string.sharp:87:20: note:  
		        fn operator=( var[] immstr )
		                   ^


$operator=:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	pushref
[0x3] 3:	movl 1
[0x4] 4:	pushref
[0x5] 5:	movl 1
[0x6] 6:	sizeof ebx
[0x7] 7:	pushr ebx
[0x8] 8:	call @35 // <std#Array.copyOf>
[0x9] 9:	movl 0
[0xa] 10:	movn #0
[0xb] 11:	popref
[0xc] 12:	ret

func:@12 [std#string.$operator+] in file: src/std/string.sharp:92:20: note:  
		        fn operator+( var char ) : string
		                   ^


$operator+:
[0x0] 0:	movl 2
[0x1] 1:	movi #1, ebx
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	newi ebx
[0x6] 6:	movl 2
[0x7] 7:	popref
[0x8] 8:	movr adx, fp
[0x9] 9:	smov ebx+1
[0xa] 10:	pushr ebx
[0xb] 11:	movl 2
[0xc] 12:	pushref
[0xd] 13:	movi #0, ebx
[0xf] 15:	movsl #0
[0x10] 16:	movr adx, sp
[0x11] 17:	smov egx+[-1]
[0x12] 18:	chklen ebx
[0x13] 19:	rmov ebx, egx
[0x14] 20:	sdelref
[0x15] 21:	pop
[0x16] 22:	movl 3
[0x17] 23:	inc sp
[0x18] 24:	iframe
[0x19] 25:	inc sp
[0x1a] 26:	movsl #0
[0x1b] 27:	new_class @2 // std#string
[0x1c] 28:	movl 0
[0x1d] 29:	movn #0
[0x1e] 30:	pushref
[0x1f] 31:	call @3 // <std#string.string>
[0x20] 32:	movl 3
[0x21] 33:	popref
[0x22] 34:	inc sp
[0x23] 35:	iframe
[0x24] 36:	movl 3
[0x25] 37:	pushref
[0x26] 38:	movl 2
[0x27] 39:	pushref
[0x28] 40:	call @5 // <std#string.append>
[0x29] 41:	pop
[0x2a] 42:	movl 3
[0x2b] 43:	movr adx, fp
[0x2c] 44:	smovobj @-5
[0x2d] 45:	ret

func:@13 [std#string.$operator+=] in file: src/std/string.sharp:103:20: note:  
		        fn operator+=( var char ) : string
		                   ^


$operator+=:
[0x0] 0:	movl 2
[0x1] 1:	movi #1, ebx
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	newi ebx
[0x6] 6:	movl 2
[0x7] 7:	popref
[0x8] 8:	movr adx, fp
[0x9] 9:	smov ebx+1
[0xa] 10:	pushr ebx
[0xb] 11:	movl 2
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
[0x1a] 26:	movl 2
[0x1b] 27:	pushref
[0x1c] 28:	call @5 // <std#string.append>
[0x1d] 29:	movsl #0
[0x1e] 30:	movr adx, fp
[0x1f] 31:	smovobj @-5
[0x20] 32:	ret

func:@14 [std#string.$operator+=] in file: src/std/string.sharp:111:20: note:  
		        fn operator+=( var[] immstr ) : string
		                   ^


$operator+=:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 0
[0x3] 3:	pushref
[0x4] 4:	movl 1
[0x5] 5:	pushref
[0x6] 6:	call @5 // <std#string.append>
[0x7] 7:	movsl #0
[0x8] 8:	movr adx, fp
[0x9] 9:	smovobj @-5
[0xa] 10:	ret

func:@15 [std#string.value] in file: src/std/string.sharp:116:17: note:  
		        fn value() : var[]
		                ^


value:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	movr adx, fp
[0x3] 3:	smovobj @-5
[0x4] 4:	ret

func:@16 [std#string.convertInt] in file: src/std/string.sharp:121:29: note:  
		        static fn convertInt( var number ) : string
		                            ^


convertInt:
[0x0] 0:	movi #0, ebx
[0x2] 2:	movr egx, ebx
[0x3] 3:	movr adx, fp
[0x4] 4:	smov ebx+0
[0x5] 5:	test ebx, egx
[0x6] 6:	movr ebx, cmt
[0x7] 7:	movr cmt, ebx
[0x8] 8:	movi #24, adx
[0xa] 10:	ifne
[0xb] 11:	inc sp
[0xc] 12:	iframe
[0xd] 13:	inc sp
[0xe] 14:	movsl #0
[0xf] 15:	new_class @2 // std#string
[0x10] 16:	inc sp
[0x11] 17:	movsl #0
[0x12] 18:	newstr @7
[0x13] 19:	call @3 // <std#string.string>
[0x14] 20:	movsl #0
[0x15] 21:	movr adx, fp
[0x16] 22:	smovobj @-5
[0x17] 23:	ret
[0x18] 24:	movl 1
[0x19] 25:	inc sp
[0x1a] 26:	iframe
[0x1b] 27:	inc sp
[0x1c] 28:	movsl #0
[0x1d] 29:	new_class @2 // std#string
[0x1e] 30:	inc sp
[0x1f] 31:	movsl #0
[0x20] 32:	newstr @8
[0x21] 33:	call @3 // <std#string.string>
[0x22] 34:	movl 1
[0x23] 35:	popref
[0x24] 36:	movl 2
[0x25] 37:	inc sp
[0x26] 38:	iframe
[0x27] 39:	inc sp
[0x28] 40:	movsl #0
[0x29] 41:	new_class @2 // std#string
[0x2a] 42:	inc sp
[0x2b] 43:	movsl #0
[0x2c] 44:	newstr @8
[0x2d] 45:	call @3 // <std#string.string>
[0x2e] 46:	movl 2
[0x2f] 47:	popref
[0x30] 48:	movi #0, ebx
[0x32] 50:	pushr ebx
[0x33] 51:	movr adx, fp
[0x34] 52:	smov ebx+0
[0x35] 53:	mov64 ebx, ebx
[0x36] 54:	movr adx, fp
[0x37] 55:	smovr ebx+0
[0x38] 56:	popr ecx
[0x39] 57:	gt ebx, ecx
[0x3a] 58:	movr ebx, cmt
[0x3b] 59:	movr cmt, ebx
[0x3c] 60:	movi #97, adx
[0x3e] 62:	ifne
[0x3f] 63:	inc sp
[0x40] 64:	iframe
[0x41] 65:	movl 1
[0x42] 66:	pushref
[0x43] 67:	movi #48, ebx
[0x45] 69:	pushr ebx
[0x46] 70:	movi #10, ebx
[0x48] 72:	movr egx, ebx
[0x49] 73:	movr adx, fp
[0x4a] 74:	smov ebx+0
[0x4b] 75:	mod ebx, egx -> ebx
[0x4d] 77:	popr ecx
[0x4e] 78:	add ebx, ecx -> ebx
[0x50] 80:	pushr ebx
[0x51] 81:	call @13 // <std#string.$operator+=>
[0x52] 82:	pop
[0x53] 83:	movi #10, ebx
[0x55] 85:	movr ecx, ebx
[0x56] 86:	divl ecx, @0
[0x57] 87:	iframe
[0x58] 88:	inc sp
[0x59] 89:	iframe
[0x5a] 90:	movl 1
[0x5b] 91:	pushref
[0x5c] 92:	call @15 // <std#string.value>
[0x5d] 93:	sizeof ebx
[0x5e] 94:	pushr ebx
[0x5f] 95:	call @27 // <std#System.print>
[0x60] 96:	goto @48
[0x61] 97:	inc sp
[0x62] 98:	iframe
[0x63] 99:	movl 1
[0x64] 100:	pushref
[0x65] 101:	call @15 // <std#string.value>
[0x66] 102:	sizeof ebx
[0x67] 103:	movr ecx, ebx
[0x68] 104:	movr adx, fp
[0x69] 105:	smovr ecx+3
[0x6a] 106:	movi #0, ebx
[0x6c] 108:	movr egx, ebx
[0x6d] 109:	movr adx, fp
[0x6e] 110:	smov ebx+3
[0x6f] 111:	gt ebx, egx
[0x70] 112:	movr ebx, cmt
[0x71] 113:	movi #146, adx
[0x73] 115:	ifne
[0x74] 116:	iframe
[0x75] 117:	inc sp
[0x76] 118:	iframe
[0x77] 119:	movl 1
[0x78] 120:	pushref
[0x79] 121:	movr adx, fp
[0x7a] 122:	smov ebx+3
[0x7b] 123:	pushr ebx
[0x7c] 124:	call @6 // <std#string.at>
[0x7d] 125:	call @27 // <std#System.print>
[0x7e] 126:	inc sp
[0x7f] 127:	iframe
[0x80] 128:	movl 2
[0x81] 129:	pushref
[0x82] 130:	inc sp
[0x83] 131:	iframe
[0x84] 132:	movl 1
[0x85] 133:	pushref
[0x86] 134:	movr adx, fp
[0x87] 135:	smov ebx+3
[0x88] 136:	pushr ebx
[0x89] 137:	call @6 // <std#string.at>
[0x8a] 138:	call @13 // <std#string.$operator+=>
[0x8b] 139:	pop
[0x8c] 140:	movr adx, fp
[0x8d] 141:	smov ebx+3
[0x8e] 142:	movi #1, ecx
[0x90] 144:	subl ecx, @3
[0x91] 145:	goto @106
[0x92] 146:	movl 2
[0x93] 147:	movr adx, fp
[0x94] 148:	smovobj @-5
[0x95] 149:	ret

func:@17 [std#string.string] in file: src/std/string.sharp:3:6: note:  
		class string {
		     ^


string:
[0x0] 0:	ret

func:@18 [application#Start.transposeArray] in file: src/srt/srt_main.sharp:9:34: note:  
		    private macros transposeArray(var[] args) {
		                                 ^


transposeArray:
[0x0] 0:	movl 0
[0x1] 1:	del
[0x2] 2:	ret

func:@19 [application#Start.__init] in file: src/srt/srt_main.sharp:18:28: note:  
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
[0xb] 11:	call @0 // <application#Main.main>
[0xc] 12:	movr adx, fp
[0xd] 13:	smov ebx+2
[0xe] 14:	movr adx, fp
[0xf] 15:	smovr ebx+[-5]
[0x10] 16:	ret

func:@20 [application#Start.Start] in file: src/srt/srt_main.sharp:5:6: note:  
		class Start {
		     ^


Start:
[0x0] 0:	ret

func:@21 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:10:14: note:  
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

func:@22 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:14:14: note:  
		    Throwable(string message) {
		             ^


Throwable:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@23 [std.err#Throwable.getStackTrace] in file: src/std/error/Throwable.sharp:18:21: note:  
		    fn getStackTrace() : string {
		                    ^


getStackTrace:
[0x0] 0:	ret

func:@24 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:6:6: note:  
		class Throwable {
		     ^


Throwable:
[0x0] 0:	ret

func:@25 [std#System.print] in file: src/std/System.sharp:10:27: note:  
		    public static fn print(var[] data) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	movl 0
[0x2] 2:	pushref
[0x3] 3:	call @30 // <libcore#Libcore.os.printData>
[0x4] 4:	ret

func:@26 [std#System.print] in file: src/std/System.sharp:14:27: note:  
		    public static fn print(string str) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	movl 0
[0x4] 4:	pushref
[0x5] 5:	call @15 // <std#string.value>
[0x6] 6:	call @30 // <libcore#Libcore.os.printData>
[0x7] 7:	ret

func:@27 [std#System.print] in file: src/std/System.sharp:18:27: note:  
		    public static fn print(var data) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+0
[0x3] 3:	pushr ebx
[0x4] 4:	movi #0, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	call @32 // <libcore#Libcore.os.__print_>
[0x8] 8:	ret

func:@28 [std#System.printChar] in file: src/std/System.sharp:22:31: note:  
		    public static fn printChar(var data) {
		                              ^


printChar:
[0x0] 0:	iframe
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+0
[0x3] 3:	pushr ebx
[0x4] 4:	movi #1, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	call @32 // <libcore#Libcore.os.__print_>
[0x8] 8:	ret

func:@29 [std#System.System] in file: src/std/System.sharp:8:6: note:  
		class System {
		     ^


System:
[0x0] 0:	ret

func:@30 [libcore#Libcore.os.printData] in file: src/std/internal/Libcore.sharp:11:35: note:  
		        public static fn printData(var[] data) {
		                                  ^


printData:
[0x0] 0:	iframe
[0x1] 1:	movl 0
[0x2] 2:	pushref
[0x3] 3:	call @31 // <libcore#Libcore.os.__print_>
[0x4] 4:	ret

func:@31 [libcore#Libcore.os.__print_] in file: src/std/internal/Libcore.sharp:19:35: note:  
		        private static fn __print_(var[] message) {
		                                  ^


__print_:
[0x0] 0:	movl 0
[0x1] 1:	chknull
[0x2] 2:	not cmt, cmt
[0x3] 3:	movr ebx, cmt
[0x4] 4:	movr cmt, ebx
[0x5] 5:	loadx adx
[0x6] 6:	iadd adx, #33
[0x7] 7:	ifne
[0x8] 8:	movi #0, ebx
[0xa] 10:	pushr ebx
[0xb] 11:	movl 0
[0xc] 12:	movr adx, sp
[0xd] 13:	smov ebx+0
[0xe] 14:	sizeof egx
[0xf] 15:	lt ebx, egx
[0x10] 16:	movi #36, adx
[0x12] 18:	ifne
[0x13] 19:	movx ebx, ebx
[0x14] 20:	movr ecx, ebx
[0x15] 21:	movr adx, fp
[0x16] 22:	smovr ecx+1
[0x17] 23:	iframe
[0x18] 24:	movr adx, fp
[0x19] 25:	smov ebx+1
[0x1a] 26:	pushr ebx
[0x1b] 27:	movi #1, ebx
[0x1d] 29:	pushr ebx
[0x1e] 30:	call @32 // <libcore#Libcore.os.__print_>
[0x1f] 31:	movr adx, sp
[0x20] 32:	smov ebx+0
[0x21] 33:	inc ebx
[0x22] 34:	smovr ebx+0
[0x23] 35:	goto @11
[0x24] 36:	pop
[0x25] 37:	goto @70
[0x26] 38:	iframe
[0x27] 39:	movi #110, ebx
[0x29] 41:	pushr ebx
[0x2a] 42:	movi #1, ebx
[0x2c] 44:	pushr ebx
[0x2d] 45:	call @32 // <libcore#Libcore.os.__print_>
[0x2e] 46:	iframe
[0x2f] 47:	movi #117, ebx
[0x31] 49:	pushr ebx
[0x32] 50:	movi #1, ebx
[0x34] 52:	pushr ebx
[0x35] 53:	call @32 // <libcore#Libcore.os.__print_>
[0x36] 54:	iframe
[0x37] 55:	movi #108, ebx
[0x39] 57:	pushr ebx
[0x3a] 58:	movi #1, ebx
[0x3c] 60:	pushr ebx
[0x3d] 61:	call @32 // <libcore#Libcore.os.__print_>
[0x3e] 62:	iframe
[0x3f] 63:	movi #108, ebx
[0x41] 65:	pushr ebx
[0x42] 66:	movi #1, ebx
[0x44] 68:	pushr ebx
[0x45] 69:	call @32 // <libcore#Libcore.os.__print_>
[0x46] 70:	ret

func:@32 [libcore#Libcore.os.__print_] in file: src/std/internal/Libcore.sharp:35:34: note:  
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

func:@33 [libcore#Libcore.os.os] in file: src/std/internal/Libcore.sharp:10:13: note:  
		    class os {
		            ^


os:
[0x0] 0:	ret

func:@34 [libcore#Libcore.Libcore] in file: src/std/internal/Libcore.sharp:8:6: note:  
		class Libcore {
		     ^


Libcore:
[0x0] 0:	ret

func:@35 [std#Array.copyOf] in file: src/std/Array.sharp:5:14: note:  
		    fn copyOf(var[] data, var len) : var[] {
		             ^


copyOf:
[0x0] 0:	movl 3
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+2
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	newi ebx
[0x6] 6:	movl 3
[0x7] 7:	popref
[0x8] 8:	movi #0, ebx
[0xa] 10:	movr ecx, ebx
[0xb] 11:	movr adx, fp
[0xc] 12:	smovr ecx+4
[0xd] 13:	movr adx, fp
[0xe] 14:	smov ebx+2
[0xf] 15:	movr egx, ebx
[0x10] 16:	movr adx, fp
[0x11] 17:	smov ebx+4
[0x12] 18:	lt ebx, egx
[0x13] 19:	movr ebx, cmt
[0x14] 20:	movi #49, adx
[0x16] 22:	ifne
[0x17] 23:	movl 1
[0x18] 24:	pushref
[0x19] 25:	movr adx, fp
[0x1a] 26:	smov ebx+4
[0x1b] 27:	movsl #0
[0x1c] 28:	chklen ebx
[0x1d] 29:	movx ebx, ebx
[0x1e] 30:	sdelref
[0x1f] 31:	pushr ebx
[0x20] 32:	movl 3
[0x21] 33:	pushref
[0x22] 34:	movr adx, fp
[0x23] 35:	smov ebx+4
[0x24] 36:	movsl #0
[0x25] 37:	movr adx, sp
[0x26] 38:	smov egx+[-1]
[0x27] 39:	chklen ebx
[0x28] 40:	rmov ebx, egx
[0x29] 41:	sdelref
[0x2a] 42:	pop
[0x2b] 43:	movr adx, fp
[0x2c] 44:	smov ebx+4
[0x2d] 45:	movi #1, ecx
[0x2f] 47:	addl ecx, @4
[0x30] 48:	goto @13
[0x31] 49:	movl 3
[0x32] 50:	movr adx, fp
[0x33] 51:	smovobj @-5
[0x34] 52:	ret

func:@36 [std#Array.Array] in file: src/std/Array.sharp:3:6: note:  
		class Array {
		     ^


Array:
[0x0] 0:	ret

