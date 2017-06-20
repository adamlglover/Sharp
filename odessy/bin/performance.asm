Object Dump file:
################################

func:@0 [application#Main.main] in file: odessy/test3.sharp:7:19: note:  
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
[0x8] 8:	inc sp
[0x9] 9:	movsl #0
[0xa] 10:	new_class @2 // std#string
[0xb] 11:	inc sp
[0xc] 12:	movsl #0
[0xd] 13:	newstr @0
[0xe] 14:	call @3 // <std#string.string(var[])>
[0xf] 15:	movl 2
[0x10] 16:	popref
[0x11] 17:	inc sp
[0x12] 18:	iframe
[0x13] 19:	movl 2
[0x14] 20:	pushref
[0x15] 21:	inc sp
[0x16] 22:	movsl #0
[0x17] 23:	newstr @1
[0x18] 24:	call @14 // <std#string.$operator+=(var[])>
[0x19] 25:	sdel
[0x1a] 26:	iframe
[0x1b] 27:	inc sp
[0x1c] 28:	iframe
[0x1d] 29:	movi #129, ebx
[0x1f] 31:	pushr ebx
[0x20] 32:	call @17 // <std#string.convertInt(var)>
[0x21] 33:	call @27 // <std#System.print(std#string)>
[0x22] 34:	iframe
[0x23] 35:	inc sp
[0x24] 36:	iframe
[0x25] 37:	movl 2
[0x26] 38:	pushref
[0x27] 39:	movi #10, ebx
[0x29] 41:	pushr ebx
[0x2a] 42:	call @12 // <std#string.$operator+(var)>
[0x2b] 43:	call @27 // <std#System.print(std#string)>
[0x2c] 44:	iframe
[0x2d] 45:	movi #21, ebx
[0x2f] 47:	pushr ebx
[0x30] 48:	call @28 // <std#System.print(var)>
[0x31] 49:	inc sp
[0x32] 50:	iframe
[0x33] 51:	movl 2
[0x34] 52:	pushref
[0x35] 53:	movi #99, ebx
[0x37] 55:	pushr ebx
[0x38] 56:	call @6 // <std#string.at(var)>
[0x39] 57:	pop
[0x3a] 58:	ret

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
[0x6] 6:	call @5 // <std#string.append(var[])>
[0x7] 7:	sdel
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
[0x13] 19:	newstr @2
[0x14] 20:	call @22 // <std.err#Throwable.Throwable(var[])>
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
[0x21] 33:	movr cmt, ebx
[0x22] 34:	movi #67, adx
[0x24] 36:	ifne
[0x25] 37:	movl 0
[0x26] 38:	movn #0
[0x27] 39:	pushref
[0x28] 40:	movr adx, fp
[0x29] 41:	smov ebx+4
[0x2a] 42:	movsl #0
[0x2b] 43:	chklen ebx
[0x2c] 44:	movx ebx, ebx
[0x2d] 45:	sdelref
[0x2e] 46:	pushr ebx
[0x2f] 47:	movl 2
[0x30] 48:	pushref
[0x31] 49:	movr adx, fp
[0x32] 50:	smov ebx+3
[0x33] 51:	movi #1, ecx
[0x35] 53:	addl ecx, @3
[0x36] 54:	movsl #0
[0x37] 55:	movr adx, sp
[0x38] 56:	smov egx+[-1]
[0x39] 57:	chklen ebx
[0x3a] 58:	rmov ebx, egx
[0x3b] 59:	sdelref
[0x3c] 60:	pop
[0x3d] 61:	movr adx, fp
[0x3e] 62:	smov ebx+4
[0x3f] 63:	movi #1, ecx
[0x41] 65:	addl ecx, @4
[0x42] 66:	goto @25
[0x43] 67:	movi #0, ebx
[0x45] 69:	movr ecx, ebx
[0x46] 70:	movr adx, fp
[0x47] 71:	smovr ecx+5
[0x48] 72:	movl 1
[0x49] 73:	sizeof ebx
[0x4a] 74:	movr egx, ebx
[0x4b] 75:	movr adx, fp
[0x4c] 76:	smov ebx+5
[0x4d] 77:	lt ebx, egx
[0x4e] 78:	movr ebx, cmt
[0x4f] 79:	movr cmt, ebx
[0x50] 80:	movi #112, adx
[0x52] 82:	ifne
[0x53] 83:	movl 1
[0x54] 84:	pushref
[0x55] 85:	movr adx, fp
[0x56] 86:	smov ebx+5
[0x57] 87:	movsl #0
[0x58] 88:	chklen ebx
[0x59] 89:	movx ebx, ebx
[0x5a] 90:	sdelref
[0x5b] 91:	pushr ebx
[0x5c] 92:	movl 2
[0x5d] 93:	pushref
[0x5e] 94:	movr adx, fp
[0x5f] 95:	smov ebx+3
[0x60] 96:	movi #1, ecx
[0x62] 98:	addl ecx, @3
[0x63] 99:	movsl #0
[0x64] 100:	movr adx, sp
[0x65] 101:	smov egx+[-1]
[0x66] 102:	chklen ebx
[0x67] 103:	rmov ebx, egx
[0x68] 104:	sdelref
[0x69] 105:	pop
[0x6a] 106:	movr adx, fp
[0x6b] 107:	smov ebx+5
[0x6c] 108:	movi #1, ecx
[0x6e] 110:	addl ecx, @5
[0x6f] 111:	goto @72
[0x70] 112:	inc sp
[0x71] 113:	iframe
[0x72] 114:	pushref
[0x73] 115:	movl 2
[0x74] 116:	pushref
[0x75] 117:	movl 2
[0x76] 118:	sizeof ebx
[0x77] 119:	pushr ebx
[0x78] 120:	call @36 // <std#Array.copyOf(var[],var)>
[0x79] 121:	movl 0
[0x7a] 122:	movn #0
[0x7b] 123:	popref
[0x7c] 124:	movl 2
[0x7d] 125:	del
[0x7e] 126:	movl 0
[0x7f] 127:	movr adx, fp
[0x80] 128:	smovobj @-5
[0x81] 129:	ret

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
[0x9] 9:	movi #63, adx
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
[0x16] 22:	movsl #0
[0x17] 23:	new_class @2 // std#string
[0x18] 24:	inc sp
[0x19] 25:	movsl #0
[0x1a] 26:	newstr @3
[0x1b] 27:	call @3 // <std#string.string(var[])>
[0x1c] 28:	inc sp
[0x1d] 29:	iframe
[0x1e] 30:	movl 0
[0x1f] 31:	movn #0
[0x20] 32:	sizeof ebx
[0x21] 33:	pushr ebx
[0x22] 34:	call @17 // <std#string.convertInt(var)>
[0x23] 35:	call @8 // <std#string.$operator+(std#string)>
[0x24] 36:	movsl #0
[0x25] 37:	dec sp
[0x26] 38:	inc sp
[0x27] 39:	iframe
[0x28] 40:	inc sp
[0x29] 41:	movr adx, sp
[0x2a] 42:	smovobj @0
[0x2b] 43:	inc sp
[0x2c] 44:	movsl #0
[0x2d] 45:	newstr @4
[0x2e] 46:	call @7 // <std#string.$operator+(var[])>
[0x2f] 47:	movsl #0
[0x30] 48:	dec sp
[0x31] 49:	inc sp
[0x32] 50:	iframe
[0x33] 51:	inc sp
[0x34] 52:	movr adx, sp
[0x35] 53:	smovobj @0
[0x36] 54:	inc sp
[0x37] 55:	iframe
[0x38] 56:	movr adx, fp
[0x39] 57:	smov ebx+1
[0x3a] 58:	pushr ebx
[0x3b] 59:	call @17 // <std#string.convertInt(var)>
[0x3c] 60:	call @8 // <std#string.$operator+(std#string)>
[0x3d] 61:	call @23 // <std.err#Throwable.Throwable(std#string)>
[0x3e] 62:	throw 
[0x3f] 63:	movl 0
[0x40] 64:	movn #0
[0x41] 65:	pushref
[0x42] 66:	movr adx, fp
[0x43] 67:	smov ebx+1
[0x44] 68:	movsl #0
[0x45] 69:	chklen ebx
[0x46] 70:	movx ebx, ebx
[0x47] 71:	sdelref
[0x48] 72:	movr adx, fp
[0x49] 73:	smovr ebx+[-5]
[0x4a] 74:	ret

func:@7 [std#string.$operator+] in file: src/std/string.sharp:51:20: note:  
		        fn operator+( var[] immstr ) : string
		                   ^


$operator+:
[0x0] 0:	iframe
[0x1] 1:	inc sp
[0x2] 2:	movsl #0
[0x3] 3:	newstr @5
[0x4] 4:	call @26 // <std#System.print(var[])>
[0x5] 5:	iframe
[0x6] 6:	movl 0
[0x7] 7:	movn #0
[0x8] 8:	pushref
[0x9] 9:	call @26 // <std#System.print(var[])>
[0xa] 10:	iframe
[0xb] 11:	inc sp
[0xc] 12:	movsl #0
[0xd] 13:	newstr @6
[0xe] 14:	call @26 // <std#System.print(var[])>
[0xf] 15:	iframe
[0x10] 16:	movl 1
[0x11] 17:	pushref
[0x12] 18:	call @26 // <std#System.print(var[])>
[0x13] 19:	iframe
[0x14] 20:	inc sp
[0x15] 21:	movsl #0
[0x16] 22:	newstr @7
[0x17] 23:	call @26 // <std#System.print(var[])>
[0x18] 24:	movl 2
[0x19] 25:	inc sp
[0x1a] 26:	iframe
[0x1b] 27:	inc sp
[0x1c] 28:	movsl #0
[0x1d] 29:	new_class @2 // std#string
[0x1e] 30:	movl 0
[0x1f] 31:	movn #0
[0x20] 32:	pushref
[0x21] 33:	call @3 // <std#string.string(var[])>
[0x22] 34:	movl 2
[0x23] 35:	popref
[0x24] 36:	inc sp
[0x25] 37:	iframe
[0x26] 38:	movl 2
[0x27] 39:	inc sp
[0x28] 40:	movr adx, sp
[0x29] 41:	smovobj @0
[0x2a] 42:	movl 1
[0x2b] 43:	pushref
[0x2c] 44:	call @5 // <std#string.append(var[])>
[0x2d] 45:	movsl #0
[0x2e] 46:	movr adx, fp
[0x2f] 47:	smovobj @-5
[0x30] 48:	ret

func:@8 [std#string.$operator+] in file: src/std/string.sharp:62:20: note:  
		        fn operator+( string str ) : string
		                   ^


$operator+:
[0x0] 0:	iframe
[0x1] 1:	inc sp
[0x2] 2:	movsl #0
[0x3] 3:	newstr @8
[0x4] 4:	call @26 // <std#System.print(var[])>
[0x5] 5:	iframe
[0x6] 6:	movl 0
[0x7] 7:	movn #0
[0x8] 8:	pushref
[0x9] 9:	call @26 // <std#System.print(var[])>
[0xa] 10:	iframe
[0xb] 11:	inc sp
[0xc] 12:	movsl #0
[0xd] 13:	newstr @6
[0xe] 14:	call @26 // <std#System.print(var[])>
[0xf] 15:	iframe
[0x10] 16:	inc sp
[0x11] 17:	iframe
[0x12] 18:	movl 1
[0x13] 19:	pushref
[0x14] 20:	call @16 // <std#string.value()>
[0x15] 21:	call @26 // <std#System.print(var[])>
[0x16] 22:	iframe
[0x17] 23:	inc sp
[0x18] 24:	movsl #0
[0x19] 25:	newstr @7
[0x1a] 26:	call @26 // <std#System.print(var[])>
[0x1b] 27:	movl 2
[0x1c] 28:	inc sp
[0x1d] 29:	iframe
[0x1e] 30:	inc sp
[0x1f] 31:	movsl #0
[0x20] 32:	new_class @2 // std#string
[0x21] 33:	movl 0
[0x22] 34:	movn #0
[0x23] 35:	pushref
[0x24] 36:	call @3 // <std#string.string(var[])>
[0x25] 37:	movl 2
[0x26] 38:	popref
[0x27] 39:	inc sp
[0x28] 40:	iframe
[0x29] 41:	movl 2
[0x2a] 42:	inc sp
[0x2b] 43:	movr adx, sp
[0x2c] 44:	smovobj @0
[0x2d] 45:	inc sp
[0x2e] 46:	iframe
[0x2f] 47:	movl 1
[0x30] 48:	inc sp
[0x31] 49:	movr adx, sp
[0x32] 50:	smovobj @0
[0x33] 51:	call @16 // <std#string.value()>
[0x34] 52:	call @5 // <std#string.append(var[])>
[0x35] 53:	movsl #0
[0x36] 54:	movr adx, fp
[0x37] 55:	smovobj @-5
[0x38] 56:	ret

func:@9 [std#string.$operator==] in file: src/std/string.sharp:73:20: note:  
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
[0x1e] 30:	call @6 // <std#string.at(var)>
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

func:@10 [std#string.$operator==] in file: src/std/string.sharp:85:20: note:  
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

func:@11 [std#string.$operator=] in file: src/std/string.sharp:97:20: note:  
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
[0x8] 8:	call @36 // <std#Array.copyOf(var[],var)>
[0x9] 9:	movl 0
[0xa] 10:	movn #0
[0xb] 11:	popref
[0xc] 12:	ret

func:@12 [std#string.$operator+] in file: src/std/string.sharp:102:20: note:  
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
[0x1f] 31:	call @3 // <std#string.string(var[])>
[0x20] 32:	movl 3
[0x21] 33:	popref
[0x22] 34:	inc sp
[0x23] 35:	iframe
[0x24] 36:	movl 3
[0x25] 37:	pushref
[0x26] 38:	movl 2
[0x27] 39:	pushref
[0x28] 40:	call @5 // <std#string.append(var[])>
[0x29] 41:	sdel
[0x2a] 42:	movl 3
[0x2b] 43:	movr adx, fp
[0x2c] 44:	smovobj @-5
[0x2d] 45:	ret

func:@13 [std#string.$operator+=] in file: src/std/string.sharp:113:20: note:  
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
[0x1c] 28:	call @5 // <std#string.append(var[])>
[0x1d] 29:	movsl #0
[0x1e] 30:	movr adx, fp
[0x1f] 31:	smovobj @-5
[0x20] 32:	ret

func:@14 [std#string.$operator+=] in file: src/std/string.sharp:121:20: note:  
		        fn operator+=( var[] immstr ) : string
		                   ^


$operator+=:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 0
[0x3] 3:	pushref
[0x4] 4:	movl 1
[0x5] 5:	pushref
[0x6] 6:	call @5 // <std#string.append(var[])>
[0x7] 7:	movsl #0
[0x8] 8:	movr adx, fp
[0x9] 9:	smovobj @-5
[0xa] 10:	ret

func:@15 [std#string.$operator+=] in file: src/std/string.sharp:126:20: note:  
		        fn operator+=( string str ) : string
		                   ^


$operator+=:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 0
[0x3] 3:	pushref
[0x4] 4:	inc sp
[0x5] 5:	iframe
[0x6] 6:	movl 1
[0x7] 7:	inc sp
[0x8] 8:	movr adx, sp
[0x9] 9:	smovobj @0
[0xa] 10:	call @16 // <std#string.value()>
[0xb] 11:	call @5 // <std#string.append(var[])>
[0xc] 12:	movsl #0
[0xd] 13:	movr adx, fp
[0xe] 14:	smovobj @-5
[0xf] 15:	ret

func:@16 [std#string.value] in file: src/std/string.sharp:131:17: note:  
		        fn value() : var[]
		                ^


value:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	returnref
[0x3] 3:	ret

func:@17 [std#string.convertInt] in file: src/std/string.sharp:136:29: note:  
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
[0x12] 18:	newstr @9
[0x13] 19:	call @3 // <std#string.string(var[])>
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
[0x20] 32:	newstr @10
[0x21] 33:	call @3 // <std#string.string(var[])>
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
[0x2c] 44:	newstr @10
[0x2d] 45:	call @3 // <std#string.string(var[])>
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
[0x3c] 60:	movi #88, adx
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
[0x51] 81:	call @13 // <std#string.$operator+=(var)>
[0x52] 82:	sdel
[0x53] 83:	movi #10, ebx
[0x55] 85:	movr ecx, ebx
[0x56] 86:	divl ecx, @0
[0x57] 87:	goto @48
[0x58] 88:	movi #1, ebx
[0x5a] 90:	pushr ebx
[0x5b] 91:	inc sp
[0x5c] 92:	iframe
[0x5d] 93:	movl 1
[0x5e] 94:	pushref
[0x5f] 95:	call @16 // <std#string.value()>
[0x60] 96:	movsl #0
[0x61] 97:	sizeof ebx
[0x62] 98:	sdel
[0x63] 99:	popr ecx
[0x64] 100:	sub ebx, ecx -> ebx
[0x66] 102:	movr ecx, ebx
[0x67] 103:	movr adx, fp
[0x68] 104:	smovr ecx+3
[0x69] 105:	movi #0, ebx
[0x6b] 107:	movr egx, ebx
[0x6c] 108:	movr adx, fp
[0x6d] 109:	smov ebx+3
[0x6e] 110:	ge ebx, egx
[0x6f] 111:	movr ebx, cmt
[0x70] 112:	movr cmt, ebx
[0x71] 113:	movi #136, adx
[0x73] 115:	ifne
[0x74] 116:	inc sp
[0x75] 117:	iframe
[0x76] 118:	movl 2
[0x77] 119:	pushref
[0x78] 120:	inc sp
[0x79] 121:	iframe
[0x7a] 122:	movl 1
[0x7b] 123:	pushref
[0x7c] 124:	movr adx, fp
[0x7d] 125:	smov ebx+3
[0x7e] 126:	pushr ebx
[0x7f] 127:	call @6 // <std#string.at(var)>
[0x80] 128:	call @13 // <std#string.$operator+=(var)>
[0x81] 129:	sdel
[0x82] 130:	movr adx, fp
[0x83] 131:	smov ebx+3
[0x84] 132:	movi #1, ecx
[0x86] 134:	subl ecx, @3
[0x87] 135:	goto @105
[0x88] 136:	movl 2
[0x89] 137:	movr adx, fp
[0x8a] 138:	smovobj @-5
[0x8b] 139:	ret

func:@18 [std#string.string] in file: src/std/string.sharp:3:6: note:  
		class string {
		     ^


string:
[0x0] 0:	ret

func:@19 [application#Start.transposeArray] in file: src/srt/srt_main.sharp:9:34: note:  
		    private macros transposeArray(var[] args) {
		                                 ^


transposeArray:
[0x0] 0:	movl 0
[0x1] 1:	del
[0x2] 2:	ret

func:@20 [application#Start.__init] in file: src/srt/srt_main.sharp:18:28: note:  
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
[0xb] 11:	call @0 // <application#Main.main(std#string[])>
[0xc] 12:	movr adx, fp
[0xd] 13:	smov ebx+2
[0xe] 14:	movr adx, fp
[0xf] 15:	smovr ebx+[-5]
[0x10] 16:	ret

func:@21 [application#Start.Start] in file: src/srt/srt_main.sharp:5:6: note:  
		class Start {
		     ^


Start:
[0x0] 0:	ret

func:@22 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:10:14: note:  
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

func:@23 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:14:14: note:  
		    Throwable(string message) {
		             ^


Throwable:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 1
[0x3] 3:	pushref
[0x4] 4:	call @16 // <std#string.value()>
[0x5] 5:	movl 0
[0x6] 6:	movn #0
[0x7] 7:	popref
[0x8] 8:	movl 0
[0x9] 9:	movr adx, fp
[0xa] 10:	smovobj @-5
[0xb] 11:	ret

func:@24 [std.err#Throwable.getStackTrace] in file: src/std/error/Throwable.sharp:18:21: note:  
		    fn getStackTrace() : string {
		                    ^


getStackTrace:
[0x0] 0:	movl 0
[0x1] 1:	movn #1
[0x2] 2:	chknull
[0x3] 3:	movr ebx, cmt
[0x4] 4:	movr cmt, ebx
[0x5] 5:	movi #14, adx
[0x7] 7:	ifne
[0x8] 8:	inc sp
[0x9] 9:	movsl #0
[0xa] 10:	newstr @10
[0xb] 11:	movl 0
[0xc] 12:	movn #1
[0xd] 13:	popref
[0xe] 14:	inc sp
[0xf] 15:	iframe
[0x10] 16:	inc sp
[0x11] 17:	movsl #0
[0x12] 18:	new_class @2 // std#string
[0x13] 19:	movl 0
[0x14] 20:	movn #1
[0x15] 21:	pushref
[0x16] 22:	call @3 // <std#string.string(var[])>
[0x17] 23:	movsl #0
[0x18] 24:	movr adx, fp
[0x19] 25:	smovobj @-5
[0x1a] 26:	ret

func:@25 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:6:6: note:  
		class Throwable {
		     ^


Throwable:
[0x0] 0:	ret

func:@26 [std#System.print] in file: src/std/System.sharp:10:27: note:  
		    public static fn print(var[] data) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	movl 0
[0x2] 2:	pushref
[0x3] 3:	call @31 // <libcore#Libcore.os.printData(var[])>
[0x4] 4:	ret

func:@27 [std#System.print] in file: src/std/System.sharp:14:27: note:  
		    public static fn print(string str) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	movl 0
[0x4] 4:	pushref
[0x5] 5:	call @16 // <std#string.value()>
[0x6] 6:	call @31 // <libcore#Libcore.os.printData(var[])>
[0x7] 7:	ret

func:@28 [std#System.print] in file: src/std/System.sharp:18:27: note:  
		    public static fn print(var data) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+0
[0x3] 3:	pushr ebx
[0x4] 4:	movi #0, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	call @33 // <libcore#Libcore.os.__print_(var,var)>
[0x8] 8:	ret

func:@29 [std#System.printChar] in file: src/std/System.sharp:22:31: note:  
		    public static fn printChar(var data) {
		                              ^


printChar:
[0x0] 0:	iframe
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+0
[0x3] 3:	pushr ebx
[0x4] 4:	movi #1, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	call @33 // <libcore#Libcore.os.__print_(var,var)>
[0x8] 8:	ret

func:@30 [std#System.System] in file: src/std/System.sharp:8:6: note:  
		class System {
		     ^


System:
[0x0] 0:	ret

func:@31 [libcore#Libcore.os.printData] in file: src/std/internal/Libcore.sharp:11:35: note:  
		        public static fn printData(var[] data) {
		                                  ^


printData:
[0x0] 0:	iframe
[0x1] 1:	movl 0
[0x2] 2:	pushref
[0x3] 3:	call @32 // <libcore#Libcore.os.__print_(var[])>
[0x4] 4:	ret

func:@32 [libcore#Libcore.os.__print_] in file: src/std/internal/Libcore.sharp:19:35: note:  
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
[0x1e] 30:	call @33 // <libcore#Libcore.os.__print_(var,var)>
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
[0x2d] 45:	call @33 // <libcore#Libcore.os.__print_(var,var)>
[0x2e] 46:	iframe
[0x2f] 47:	movi #117, ebx
[0x31] 49:	pushr ebx
[0x32] 50:	movi #1, ebx
[0x34] 52:	pushr ebx
[0x35] 53:	call @33 // <libcore#Libcore.os.__print_(var,var)>
[0x36] 54:	iframe
[0x37] 55:	movi #108, ebx
[0x39] 57:	pushr ebx
[0x3a] 58:	movi #1, ebx
[0x3c] 60:	pushr ebx
[0x3d] 61:	call @33 // <libcore#Libcore.os.__print_(var,var)>
[0x3e] 62:	iframe
[0x3f] 63:	movi #108, ebx
[0x41] 65:	pushr ebx
[0x42] 66:	movi #1, ebx
[0x44] 68:	pushr ebx
[0x45] 69:	call @33 // <libcore#Libcore.os.__print_(var,var)>
[0x46] 70:	ret

func:@33 [libcore#Libcore.os.__print_] in file: src/std/internal/Libcore.sharp:35:34: note:  
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

func:@34 [libcore#Libcore.os.os] in file: src/std/internal/Libcore.sharp:10:13: note:  
		    class os {
		            ^


os:
[0x0] 0:	ret

func:@35 [libcore#Libcore.Libcore] in file: src/std/internal/Libcore.sharp:8:6: note:  
		class Libcore {
		     ^


Libcore:
[0x0] 0:	ret

func:@36 [std#Array.copyOf] in file: src/std/Array.sharp:5:14: note:  
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
[0x14] 20:	movr cmt, ebx
[0x15] 21:	movi #50, adx
[0x17] 23:	ifne
[0x18] 24:	movl 1
[0x19] 25:	pushref
[0x1a] 26:	movr adx, fp
[0x1b] 27:	smov ebx+4
[0x1c] 28:	movsl #0
[0x1d] 29:	chklen ebx
[0x1e] 30:	movx ebx, ebx
[0x1f] 31:	sdelref
[0x20] 32:	pushr ebx
[0x21] 33:	movl 3
[0x22] 34:	pushref
[0x23] 35:	movr adx, fp
[0x24] 36:	smov ebx+4
[0x25] 37:	movsl #0
[0x26] 38:	movr adx, sp
[0x27] 39:	smov egx+[-1]
[0x28] 40:	chklen ebx
[0x29] 41:	rmov ebx, egx
[0x2a] 42:	sdelref
[0x2b] 43:	pop
[0x2c] 44:	movr adx, fp
[0x2d] 45:	smov ebx+4
[0x2e] 46:	movi #1, ecx
[0x30] 48:	addl ecx, @4
[0x31] 49:	goto @13
[0x32] 50:	movl 3
[0x33] 51:	movr adx, fp
[0x34] 52:	smovobj @-5
[0x35] 53:	ret

func:@37 [std#Array.Array] in file: src/std/Array.sharp:3:6: note:  
		class Array {
		     ^


Array:
[0x0] 0:	ret

