Object Dump file:
################################

func:@0 [application#Base.print2] in file: odessy/test2.sharp:5:21: note:  
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

func:@1 [application#Base.Base] in file: odessy/test2.sharp:3:6: note:  
		class Base {
		     ^


Base:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@2 [application#Main.print] in file: odessy/test2.sharp:15:20: note:  
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

func:@3 [application#Main.printc] in file: odessy/test2.sharp:19:21: note:  
		    static fn printc(var char) {
		                    ^


printc:
[0x0] 0:	movr adx, fp
[0x1] 1:	smov ebx+0
[0x2] 2:	put ebx
[0x3] 3:	ret

func:@4 [application#Main.foo] in file: odessy/test2.sharp:23:18: note:  
		    static fn foo(Main main) {
		                 ^


foo:
[0x0] 0:	ret

func:@5 [application#Main.fact] in file: odessy/test2.sharp:27:19: note:  
		    static fn fact(var x) : var {
		                  ^


fact:
[0x0] 0:	movi #1, ebx
[0x2] 2:	movr egx, ebx
[0x3] 3:	movr adx, fp
[0x4] 4:	smov ebx+0
[0x5] 5:	test ebx, egx
[0x6] 6:	movr ebx, cmt
[0x7] 7:	movr cmt, ebx
[0x8] 8:	loadf adx,5 // store pc at <@13>
[0x9] 9:	ifne
[0xa] 10:	movr adx, fp
[0xb] 11:	smov ebx+0
[0xc] 12:	skp @16 // pc = 28
[0xd] 13:	inc sp
[0xe] 14:	iframe
[0xf] 15:	movi #1, ebx
[0x11] 17:	movr egx, ebx
[0x12] 18:	movr adx, fp
[0x13] 19:	smov ebx+0
[0x14] 20:	sub ebx, egx -> ebx
[0x16] 22:	pushr ebx
[0x17] 23:	call @5 // <application#Main.fact(var)>
[0x18] 24:	movr egx, ebx
[0x19] 25:	movr adx, fp
[0x1a] 26:	smov ebx+0
[0x1b] 27:	mul ebx, egx -> ebx
[0x1d] 29:	movr adx, fp
[0x1e] 30:	smovr ebx+[-5]
[0x1f] 31:	ret

func:@6 [application#Main.$operator++] in file: odessy/test2.sharp:31:23: note:  
		    static fn operator++(var x) {
		                      ^


$operator++:
[0x0] 0:	movi #1, ebx
[0x2] 2:	movr egx, ebx
[0x3] 3:	movr adx, fp
[0x4] 4:	smov ebx+0
[0x5] 5:	add ebx, egx -> ebx

func:@7 [application#Main.$operator+] in file: odessy/test2.sharp:35:23: note:  
		    static fn operator+(var x) : var {
		                      ^


$operator+:
[0x0] 0:	movi #8, ebx
[0x2] 2:	movr ecx, ebx
[0x3] 3:	movr adx, fp
[0x4] 4:	smovr ecx+0
[0x5] 5:	movi #1, ebx
[0x7] 7:	movr adx, fp
[0x8] 8:	smovr ebx+[-5]
[0x9] 9:	ret

func:@8 [application#Main.$operator<<] in file: odessy/test2.sharp:40:23: note:  
		    static fn operator<<(var x) : var {
		                      ^


$operator<<:
[0x0] 0:	movi #0, ebx
[0x2] 2:	movr ecx, ebx
[0x3] 3:	movr adx, fp
[0x4] 4:	smovr ecx+0
[0x5] 5:	movi #1, ebx
[0x7] 7:	movr adx, fp
[0x8] 8:	smovr ebx+[-5]
[0x9] 9:	ret

func:@9 [application#Main.$operator<<] in file: odessy/test2.sharp:45:23: note:  
		    static fn operator<<(Main x) : var {
		                      ^


$operator<<:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	inc sp
[0x3] 3:	movsl #0
[0x4] 4:	new_class @1 // application#Main
[0x5] 5:	call @1 // <application#Base.Base()>
[0x6] 6:	movl 0
[0x7] 7:	popref
[0x8] 8:	movi #1, ebx
[0xa] 10:	movr adx, fp
[0xb] 11:	smovr ebx+[-5]
[0xc] 12:	ret

func:@10 [application#Main.$operator+=] in file: odessy/test2.sharp:50:23: note:  
		    static fn operator+=(var x) : var {
		                      ^


$operator+=:
[0x0] 0:	movbi #12, #9
[0x2] 2:	movr ecx, ebx
[0x3] 3:	movr adx, fp
[0x4] 4:	smovr ecx+0
[0x5] 5:	movi #1, ebx
[0x7] 7:	movr adx, fp
[0x8] 8:	smovr ebx+[-5]
[0x9] 9:	ret

func:@11 [application#Main.main] in file: odessy/test2.sharp:56:19: note:  
		    static fn main(string[] args) : var {
		                  ^


main:
[0x0] 0:	movl 1
[0x1] 1:	inc sp
[0x2] 2:	movsl #0
[0x3] 3:	newstr @0
[0x4] 4:	movl 1
[0x5] 5:	popref
[0x6] 6:	movl 2
[0x7] 7:	movi #10, ebx
[0x9] 9:	inc sp
[0xa] 10:	movsl #0
[0xb] 11:	newi ebx
[0xc] 12:	movl 2
[0xd] 13:	popref
[0xe] 14:	movi #0, ebx
[0x10] 16:	pushr ebx
[0x11] 17:	movl 2
[0x12] 18:	movr adx, sp
[0x13] 19:	smov ebx+0
[0x14] 20:	sizeof egx
[0x15] 21:	lt ebx, egx
[0x16] 22:	movi #53, adx
[0x18] 24:	ifne
[0x19] 25:	movx ebx, ebx
[0x1a] 26:	movr ecx, ebx
[0x1b] 27:	movr adx, fp
[0x1c] 28:	smovr ecx+3
[0x1d] 29:	iframe
[0x1e] 30:	inc sp
[0x1f] 31:	iframe
[0x20] 32:	movi #1, ebx
[0x22] 34:	pushr ebx
[0x23] 35:	call @5 // <application#Main.fact(var)>
[0x24] 36:	call @3 // <application#Main.printc(var)>
[0x25] 37:	movi #32, ebx
[0x27] 39:	_putc ebx
[0x28] 40:	iframe
[0x29] 41:	movr adx, fp
[0x2a] 42:	smov ebx+3
[0x2b] 43:	pushr ebx
[0x2c] 44:	call @3 // <application#Main.printc(var)>
[0x2d] 45:	movi #32, ebx
[0x2f] 47:	_putc ebx
[0x30] 48:	movr adx, sp
[0x31] 49:	smov ebx+0
[0x32] 50:	inc ebx
[0x33] 51:	smovr ebx+0
[0x34] 52:	goto @17
[0x35] 53:	pop
[0x36] 54:	movi #0, ebx
[0x38] 56:	movr ecx, ebx
[0x39] 57:	movr adx, fp
[0x3a] 58:	smovr ecx+4
[0x3b] 59:	movi #10, ebx
[0x3d] 61:	movr egx, ebx
[0x3e] 62:	movr adx, fp
[0x3f] 63:	smov ebx+4
[0x40] 64:	lt ebx, egx
[0x41] 65:	movr ebx, cmt
[0x42] 66:	movr cmt, ebx
[0x43] 67:	movi #76, adx
[0x45] 69:	ifne
[0x46] 70:	movr adx, fp
[0x47] 71:	smov ebx+4
[0x48] 72:	movi #1, ecx
[0x4a] 74:	addl ecx, @4
[0x4b] 75:	goto @59
[0x4c] 76:	movi #5, ebx
[0x4e] 78:	movr ecx, ebx
[0x4f] 79:	movr adx, fp
[0x50] 80:	smovr ecx+5
[0x51] 81:	movi #0, ebx
[0x53] 83:	pushr ebx
[0x54] 84:	movr adx, fp
[0x55] 85:	smov ebx+5
[0x56] 86:	movi #1, ecx
[0x58] 88:	subl ecx, @5
[0x59] 89:	dec ebx
[0x5a] 90:	popr ecx
[0x5b] 91:	gt ebx, ecx
[0x5c] 92:	movr ebx, cmt
[0x5d] 93:	movr cmt, ebx
[0x5e] 94:	movi #107, adx
[0x60] 96:	ifne
[0x61] 97:	iframe
[0x62] 98:	movr adx, fp
[0x63] 99:	smov ebx+5
[0x64] 100:	pushr ebx
[0x65] 101:	call @3 // <application#Main.printc(var)>
[0x66] 102:	nop
[0x67] 103:	movi #32, ebx
[0x69] 105:	_putc ebx
[0x6a] 106:	goto @81
[0x6b] 107:	iframe
[0x6c] 108:	inc sp
[0x6d] 109:	movsl #0
[0x6e] 110:	newstr @1
[0x6f] 111:	call @2 // <application#Main.print(var[])>
[0x70] 112:	movr adx, fp
[0x71] 113:	smov ebx+5
[0x72] 114:	movi #1, ecx
[0x74] 116:	addl ecx, @5
[0x75] 117:	movi #10, ebx
[0x77] 119:	movr egx, ebx
[0x78] 120:	movr adx, fp
[0x79] 121:	smov ebx+5
[0x7a] 122:	lt ebx, egx
[0x7b] 123:	movr ebx, cmt
[0x7c] 124:	movr cmt, ebx
[0x7d] 125:	movi #112, adx
[0x7f] 127:	ife
[0x80] 128:	inc sp
[0x81] 129:	iframe
[0x82] 130:	inc sp
[0x83] 131:	movsl #0
[0x84] 132:	new_class @2 // std.err#RuntimeErr
[0x85] 133:	inc sp
[0x86] 134:	movsl #0
[0x87] 135:	newstr @2
[0x88] 136:	call @32 // <std.err#Throwable.Throwable(var[])>
[0x89] 137:	throw 
[0x8a] 138:	inc sp
[0x8b] 139:	iframe
[0x8c] 140:	inc sp
[0x8d] 141:	movsl #0
[0x8e] 142:	new_class @2 // std.err#RuntimeErr
[0x8f] 143:	inc sp
[0x90] 144:	movsl #0
[0x91] 145:	newstr @3
[0x92] 146:	call @32 // <std.err#Throwable.Throwable(var[])>
[0x93] 147:	throw 
[0x94] 148:	iframe
[0x95] 149:	inc sp
[0x96] 150:	movsl #0
[0x97] 151:	newstr @4
[0x98] 152:	call @2 // <application#Main.print(var[])>
[0x99] 153:	iframe
[0x9a] 154:	movl 7
[0x9b] 155:	movn #0
[0x9c] 156:	pushref
[0x9d] 157:	call @2 // <application#Main.print(var[])>
[0x9e] 158:	inc sp
[0x9f] 159:	iframe
[0xa0] 160:	inc sp
[0xa1] 161:	movsl #0
[0xa2] 162:	new_class @2 // std.err#RuntimeErr
[0xa3] 163:	inc sp
[0xa4] 164:	movsl #0
[0xa5] 165:	newstr @5
[0xa6] 166:	call @32 // <std.err#Throwable.Throwable(var[])>
[0xa7] 167:	throw 
[0xa8] 168:	goto @171
[0xa9] 169:	nop
[0xaa] 170:	nop
[0xab] 171:	goto @174
[0xac] 172:	nop
[0xad] 173:	nop
[0xae] 174:	iframe
[0xaf] 175:	inc sp
[0xb0] 176:	movsl #0
[0xb1] 177:	newstr @6
[0xb2] 178:	call @2 // <application#Main.print(var[])>
[0xb3] 179:	inc sp
[0xb4] 180:	iframe
[0xb5] 181:	inc sp
[0xb6] 182:	movsl #0
[0xb7] 183:	new_class @2 // std.err#RuntimeErr
[0xb8] 184:	inc sp
[0xb9] 185:	movsl #0
[0xba] 186:	newstr @7
[0xbb] 187:	call @32 // <std.err#Throwable.Throwable(var[])>
[0xbc] 188:	throw 
[0xbd] 189:	nop
[0xbe] 190:	nop
[0xbf] 191:	iframe
[0xc0] 192:	inc sp
[0xc1] 193:	movsl #0
[0xc2] 194:	newstr @8
[0xc3] 195:	call @2 // <application#Main.print(var[])>
[0xc4] 196:	nop
[0xc5] 197:	nop
[0xc6] 198:	iframe
[0xc7] 199:	inc sp
[0xc8] 200:	movsl #0
[0xc9] 201:	newstr @9
[0xca] 202:	call @2 // <application#Main.print(var[])>
[0xcb] 203:	iframe
[0xcc] 204:	inc sp
[0xcd] 205:	movsl #0
[0xce] 206:	newstr @10
[0xcf] 207:	call @0 // <application#Base.print2(var[])>
[0xd0] 208:	movi #11, ebx
[0xd2] 210:	movr ecx, ebx
[0xd3] 211:	movr adx, fp
[0xd4] 212:	smovr ecx+5
[0xd5] 213:	nop
[0xd6] 214:	nop
[0xd7] 215:	movi #6, ebx
[0xd9] 217:	movr egx, ebx
[0xda] 218:	movr adx, fp
[0xdb] 219:	smov ebx+5
[0xdc] 220:	test ebx, egx
[0xdd] 221:	movr ebx, cmt
[0xde] 222:	movr cmt, ebx
[0xdf] 223:	loadx adx
[0xe0] 224:	iadd adx, #9
[0xe1] 225:	ifne
[0xe2] 226:	iframe
[0xe3] 227:	inc sp
[0xe4] 228:	movsl #0
[0xe5] 229:	newstr @11
[0xe6] 230:	call @2 // <application#Main.print(var[])>
[0xe7] 231:	goto @281
[0xe8] 232:	movi #10, ebx
[0xea] 234:	movr egx, ebx
[0xeb] 235:	movr adx, fp
[0xec] 236:	smov ebx+5
[0xed] 237:	test ebx, egx
[0xee] 238:	movr ebx, cmt
[0xef] 239:	movr cmt, ebx
[0xf0] 240:	loadx adx
[0xf1] 241:	iadd adx, #9
[0xf2] 242:	ifne
[0xf3] 243:	iframe
[0xf4] 244:	inc sp
[0xf5] 245:	movsl #0
[0xf6] 246:	newstr @12
[0xf7] 247:	call @2 // <application#Main.print(var[])>
[0xf8] 248:	goto @281
[0xf9] 249:	movl 2
[0xfa] 250:	chknull
[0xfb] 251:	movr ebx, cmt
[0xfc] 252:	movr cmt, ebx
[0xfd] 253:	loadx adx
[0xfe] 254:	iadd adx, #9
[0xff] 255:	ifne
[0x100] 256:	iframe
[0x101] 257:	inc sp
[0x102] 258:	movsl #0
[0x103] 259:	newstr @13
[0x104] 260:	call @0 // <application#Base.print2(var[])>
[0x105] 261:	goto @281
[0x106] 262:	movl 2
[0x107] 263:	chknull
[0x108] 264:	not cmt, cmt
[0x109] 265:	movr ebx, cmt
[0x10a] 266:	movr cmt, ebx
[0x10b] 267:	loadx adx
[0x10c] 268:	iadd adx, #9
[0x10d] 269:	ifne
[0x10e] 270:	iframe
[0x10f] 271:	inc sp
[0x110] 272:	movsl #0
[0x111] 273:	newstr @14
[0x112] 274:	call @0 // <application#Base.print2(var[])>
[0x113] 275:	goto @281
[0x114] 276:	iframe
[0x115] 277:	inc sp
[0x116] 278:	movsl #0
[0x117] 279:	newstr @15
[0x118] 280:	call @2 // <application#Main.print(var[])>
[0x119] 281:	iframe
[0x11a] 282:	inc sp
[0x11b] 283:	movsl #0
[0x11c] 284:	newstr @16
[0x11d] 285:	call @2 // <application#Main.print(var[])>
[0x11e] 286:	movi #32, ebx
[0x120] 288:	_putc ebx
[0x121] 289:	movr adx, sp
[0x122] 290:	smov ebx+0
[0x123] 291:	put ebx
[0x124] 292:	ret

func:@12 [std.err#RuntimeErr.RuntimeErr] in file: src/std/error/RuntimeErr.sharp:6:6: note:  
		class RuntimeErr base Throwable {
		     ^


RuntimeErr:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@13 [std#string.string] in file: src/std/string.sharp:7:15: note:  
		        string( var[] immstr )
		              ^


string:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 0
[0x3] 3:	pushref
[0x4] 4:	movl 1
[0x5] 5:	pushref
[0x6] 6:	call @15 // <std#string.append(var[])>
[0x7] 7:	sdel
[0x8] 8:	movl 0
[0x9] 9:	movr adx, fp
[0xa] 10:	smovobj @-5
[0xb] 11:	ret

func:@14 [std#string.replace] in file: src/std/string.sharp:12:19: note:  
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
[0x10] 16:	new_class @2 // std.err#RuntimeErr
[0x11] 17:	inc sp
[0x12] 18:	movsl #0
[0x13] 19:	newstr @17
[0x14] 20:	call @32 // <std.err#Throwable.Throwable(var[])>
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

func:@15 [std#string.append] in file: src/std/string.sharp:22:18: note:  
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
[0x78] 120:	call @47 // <std#Array.copyOf(var[],var)>
[0x79] 121:	movl 0
[0x7a] 122:	movn #0
[0x7b] 123:	popref
[0x7c] 124:	movl 2
[0x7d] 125:	del
[0x7e] 126:	movl 0
[0x7f] 127:	movr adx, fp
[0x80] 128:	smovobj @-5
[0x81] 129:	ret

func:@16 [std#string.at] in file: src/std/string.sharp:41:14: note:  
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
[0x10] 16:	new_class @2 // std.err#RuntimeErr
[0x11] 17:	inc sp
[0x12] 18:	iframe
[0x13] 19:	inc sp
[0x14] 20:	iframe
[0x15] 21:	inc sp
[0x16] 22:	movsl #0
[0x17] 23:	new_class @3 // std#string
[0x18] 24:	inc sp
[0x19] 25:	movsl #0
[0x1a] 26:	newstr @18
[0x1b] 27:	call @13 // <std#string.string(var[])>
[0x1c] 28:	inc sp
[0x1d] 29:	iframe
[0x1e] 30:	movl 0
[0x1f] 31:	movn #0
[0x20] 32:	sizeof ebx
[0x21] 33:	pushr ebx
[0x22] 34:	call @27 // <std#string.convertInt(var)>
[0x23] 35:	call @18 // <std#string.$operator+(std#string)>
[0x24] 36:	movsl #0
[0x25] 37:	dec sp
[0x26] 38:	inc sp
[0x27] 39:	iframe
[0x28] 40:	inc sp
[0x29] 41:	movr adx, sp
[0x2a] 42:	smovobj @0
[0x2b] 43:	inc sp
[0x2c] 44:	movsl #0
[0x2d] 45:	newstr @19
[0x2e] 46:	call @17 // <std#string.$operator+(var[])>
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
[0x3b] 59:	call @27 // <std#string.convertInt(var)>
[0x3c] 60:	call @18 // <std#string.$operator+(std#string)>
[0x3d] 61:	call @33 // <std.err#Throwable.Throwable(std#string)>
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

func:@17 [std#string.$operator+] in file: src/std/string.sharp:51:20: note:  
		        fn operator+( var[] immstr ) : string
		                   ^


$operator+:
[0x0] 0:	movl 2
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	new_class @3 // std#string
[0x6] 6:	movl 0
[0x7] 7:	movn #0
[0x8] 8:	pushref
[0x9] 9:	call @13 // <std#string.string(var[])>
[0xa] 10:	movl 2
[0xb] 11:	popref
[0xc] 12:	inc sp
[0xd] 13:	iframe
[0xe] 14:	movl 2
[0xf] 15:	inc sp
[0x10] 16:	movr adx, sp
[0x11] 17:	smovobj @0
[0x12] 18:	movl 1
[0x13] 19:	pushref
[0x14] 20:	call @15 // <std#string.append(var[])>
[0x15] 21:	movsl #0
[0x16] 22:	movr adx, fp
[0x17] 23:	smovobj @-5
[0x18] 24:	ret

func:@18 [std#string.$operator+] in file: src/std/string.sharp:57:20: note:  
		        fn operator+( string str ) : string
		                   ^


$operator+:
[0x0] 0:	movl 2
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	new_class @3 // std#string
[0x6] 6:	movl 0
[0x7] 7:	movn #0
[0x8] 8:	pushref
[0x9] 9:	call @13 // <std#string.string(var[])>
[0xa] 10:	movl 2
[0xb] 11:	popref
[0xc] 12:	inc sp
[0xd] 13:	iframe
[0xe] 14:	movl 2
[0xf] 15:	inc sp
[0x10] 16:	movr adx, sp
[0x11] 17:	smovobj @0
[0x12] 18:	inc sp
[0x13] 19:	iframe
[0x14] 20:	movl 1
[0x15] 21:	inc sp
[0x16] 22:	movr adx, sp
[0x17] 23:	smovobj @0
[0x18] 24:	call @26 // <std#string.value()>
[0x19] 25:	call @15 // <std#string.append(var[])>
[0x1a] 26:	movsl #0
[0x1b] 27:	movr adx, fp
[0x1c] 28:	smovobj @-5
[0x1d] 29:	ret

func:@19 [std#string.$operator==] in file: src/std/string.sharp:63:20: note:  
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
[0xd] 13:	movi #52, adx
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
[0x1e] 30:	call @16 // <std#string.at(var)>
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

func:@20 [std#string.$operator==] in file: src/std/string.sharp:75:20: note:  
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
[0xd] 13:	movi #50, adx
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

func:@21 [std#string.$operator=] in file: src/std/string.sharp:87:20: note:  
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
[0x8] 8:	call @47 // <std#Array.copyOf(var[],var)>
[0x9] 9:	movl 0
[0xa] 10:	movn #0
[0xb] 11:	popref
[0xc] 12:	ret

func:@22 [std#string.$operator+] in file: src/std/string.sharp:92:20: note:  
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
[0x1b] 27:	new_class @3 // std#string
[0x1c] 28:	movl 0
[0x1d] 29:	movn #0
[0x1e] 30:	pushref
[0x1f] 31:	call @13 // <std#string.string(var[])>
[0x20] 32:	movl 3
[0x21] 33:	popref
[0x22] 34:	inc sp
[0x23] 35:	iframe
[0x24] 36:	movl 3
[0x25] 37:	pushref
[0x26] 38:	movl 2
[0x27] 39:	pushref
[0x28] 40:	call @15 // <std#string.append(var[])>
[0x29] 41:	sdel
[0x2a] 42:	movl 3
[0x2b] 43:	movr adx, fp
[0x2c] 44:	smovobj @-5
[0x2d] 45:	ret

func:@23 [std#string.$operator+=] in file: src/std/string.sharp:103:20: note:  
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
[0x1c] 28:	call @15 // <std#string.append(var[])>
[0x1d] 29:	movsl #0
[0x1e] 30:	movr adx, fp
[0x1f] 31:	smovobj @-5
[0x20] 32:	ret

func:@24 [std#string.$operator+=] in file: src/std/string.sharp:111:20: note:  
		        fn operator+=( var[] immstr ) : string
		                   ^


$operator+=:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 0
[0x3] 3:	pushref
[0x4] 4:	movl 1
[0x5] 5:	pushref
[0x6] 6:	call @15 // <std#string.append(var[])>
[0x7] 7:	movsl #0
[0x8] 8:	movr adx, fp
[0x9] 9:	smovobj @-5
[0xa] 10:	ret

func:@25 [std#string.$operator+=] in file: src/std/string.sharp:116:20: note:  
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
[0xa] 10:	call @26 // <std#string.value()>
[0xb] 11:	call @15 // <std#string.append(var[])>
[0xc] 12:	movsl #0
[0xd] 13:	movr adx, fp
[0xe] 14:	smovobj @-5
[0xf] 15:	ret

func:@26 [std#string.value] in file: src/std/string.sharp:121:17: note:  
		        fn value() : var[]
		                ^


value:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	returnref
[0x3] 3:	ret

func:@27 [std#string.convertInt] in file: src/std/string.sharp:126:29: note:  
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
[0xf] 15:	new_class @3 // std#string
[0x10] 16:	inc sp
[0x11] 17:	movsl #0
[0x12] 18:	newstr @20
[0x13] 19:	call @13 // <std#string.string(var[])>
[0x14] 20:	movsl #0
[0x15] 21:	movr adx, fp
[0x16] 22:	smovobj @-5
[0x17] 23:	ret
[0x18] 24:	movl 1
[0x19] 25:	inc sp
[0x1a] 26:	iframe
[0x1b] 27:	inc sp
[0x1c] 28:	movsl #0
[0x1d] 29:	new_class @3 // std#string
[0x1e] 30:	inc sp
[0x1f] 31:	movsl #0
[0x20] 32:	newstr @21
[0x21] 33:	call @13 // <std#string.string(var[])>
[0x22] 34:	movl 1
[0x23] 35:	popref
[0x24] 36:	movl 2
[0x25] 37:	inc sp
[0x26] 38:	iframe
[0x27] 39:	inc sp
[0x28] 40:	movsl #0
[0x29] 41:	new_class @3 // std#string
[0x2a] 42:	inc sp
[0x2b] 43:	movsl #0
[0x2c] 44:	newstr @21
[0x2d] 45:	call @13 // <std#string.string(var[])>
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
[0x51] 81:	call @23 // <std#string.$operator+=(var)>
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
[0x5f] 95:	call @26 // <std#string.value()>
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
[0x7f] 127:	call @16 // <std#string.at(var)>
[0x80] 128:	call @23 // <std#string.$operator+=(var)>
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

func:@28 [std#string.string] in file: src/std/string.sharp:3:6: note:  
		class string {
		     ^


string:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@29 [application#Start.transposeArray] in file: src/srt/srt_main.sharp:9:34: note:  
		    private macros transposeArray(var[] args) {
		                                 ^


transposeArray:
[0x0] 0:	movl 0
[0x1] 1:	del
[0x2] 2:	ret

func:@30 [application#Start.__init] in file: src/srt/srt_main.sharp:18:28: note:  
		    public static fn __init(var[] args) : var {
		                           ^


__init:
[0x0] 0:	movl 1
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	new_class @3 // std#string
[0x6] 6:	call @28 // <std#string.string()>
[0x7] 7:	movl 1
[0x8] 8:	popref
[0x9] 9:	movl 1
[0xa] 10:	del
[0xb] 11:	movi #0, ebx
[0xd] 13:	movr ecx, ebx
[0xe] 14:	movr adx, fp
[0xf] 15:	smovr ecx+2
[0x10] 16:	inc sp
[0x11] 17:	iframe
[0x12] 18:	movl 1
[0x13] 19:	pushref
[0x14] 20:	call @11 // <application#Main.main(std#string[])>
[0x15] 21:	pop
[0x16] 22:	movr adx, fp
[0x17] 23:	smov ebx+2
[0x18] 24:	movr adx, fp
[0x19] 25:	smovr ebx+[-5]
[0x1a] 26:	ret

func:@31 [application#Start.Start] in file: src/srt/srt_main.sharp:5:6: note:  
		class Start {
		     ^


Start:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@32 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:10:14: note:  
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

func:@33 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:14:14: note:  
		    Throwable(string message) {
		             ^


Throwable:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 1
[0x3] 3:	pushref
[0x4] 4:	call @26 // <std#string.value()>
[0x5] 5:	movl 0
[0x6] 6:	movn #0
[0x7] 7:	popref
[0x8] 8:	movl 0
[0x9] 9:	movr adx, fp
[0xa] 10:	smovobj @-5
[0xb] 11:	ret

func:@34 [std.err#Throwable.getStackTrace] in file: src/std/error/Throwable.sharp:18:21: note:  
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
[0xa] 10:	newstr @21
[0xb] 11:	movl 0
[0xc] 12:	movn #1
[0xd] 13:	popref
[0xe] 14:	inc sp
[0xf] 15:	iframe
[0x10] 16:	inc sp
[0x11] 17:	movsl #0
[0x12] 18:	new_class @3 // std#string
[0x13] 19:	movl 0
[0x14] 20:	movn #1
[0x15] 21:	pushref
[0x16] 22:	call @13 // <std#string.string(var[])>
[0x17] 23:	movsl #0
[0x18] 24:	movr adx, fp
[0x19] 25:	smovobj @-5
[0x1a] 26:	ret

func:@35 [std.err#Throwable.getMessage] in file: src/std/error/Throwable.sharp:26:18: note:  
		    fn getMessage() : var[] {
		                 ^


getMessage:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	returnref
[0x3] 3:	ret

func:@36 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:6:6: note:  
		class Throwable {
		     ^


Throwable:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@37 [std#System.print] in file: src/std/System.sharp:10:27: note:  
		    public static fn print(var[] data) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	movl 0
[0x2] 2:	pushref
[0x3] 3:	call @42 // <libcore#Libcore.os.printData(var[])>
[0x4] 4:	ret

func:@38 [std#System.print] in file: src/std/System.sharp:14:27: note:  
		    public static fn print(string str) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	movl 0
[0x4] 4:	pushref
[0x5] 5:	call @26 // <std#string.value()>
[0x6] 6:	call @42 // <libcore#Libcore.os.printData(var[])>
[0x7] 7:	ret

func:@39 [std#System.print] in file: src/std/System.sharp:18:27: note:  
		    public static fn print(var data) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+0
[0x3] 3:	pushr ebx
[0x4] 4:	movi #0, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	call @44 // <libcore#Libcore.os.__print_(var,var)>
[0x8] 8:	ret

func:@40 [std#System.printChar] in file: src/std/System.sharp:22:31: note:  
		    public static fn printChar(var data) {
		                              ^


printChar:
[0x0] 0:	iframe
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+0
[0x3] 3:	pushr ebx
[0x4] 4:	movi #1, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	call @44 // <libcore#Libcore.os.__print_(var,var)>
[0x8] 8:	ret

func:@41 [std#System.System] in file: src/std/System.sharp:8:6: note:  
		class System {
		     ^


System:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@42 [libcore#Libcore.os.printData] in file: src/std/internal/Libcore.sharp:11:35: note:  
		        public static fn printData(var[] data) {
		                                  ^


printData:
[0x0] 0:	iframe
[0x1] 1:	movl 0
[0x2] 2:	pushref
[0x3] 3:	call @43 // <libcore#Libcore.os.__print_(var[])>
[0x4] 4:	ret

func:@43 [libcore#Libcore.os.__print_] in file: src/std/internal/Libcore.sharp:19:35: note:  
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
[0x1e] 30:	call @44 // <libcore#Libcore.os.__print_(var,var)>
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
[0x2d] 45:	call @44 // <libcore#Libcore.os.__print_(var,var)>
[0x2e] 46:	iframe
[0x2f] 47:	movi #117, ebx
[0x31] 49:	pushr ebx
[0x32] 50:	movi #1, ebx
[0x34] 52:	pushr ebx
[0x35] 53:	call @44 // <libcore#Libcore.os.__print_(var,var)>
[0x36] 54:	iframe
[0x37] 55:	movi #108, ebx
[0x39] 57:	pushr ebx
[0x3a] 58:	movi #1, ebx
[0x3c] 60:	pushr ebx
[0x3d] 61:	call @44 // <libcore#Libcore.os.__print_(var,var)>
[0x3e] 62:	iframe
[0x3f] 63:	movi #108, ebx
[0x41] 65:	pushr ebx
[0x42] 66:	movi #1, ebx
[0x44] 68:	pushr ebx
[0x45] 69:	call @44 // <libcore#Libcore.os.__print_(var,var)>
[0x46] 70:	ret

func:@44 [libcore#Libcore.os.__print_] in file: src/std/internal/Libcore.sharp:35:34: note:  
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

func:@45 [libcore#Libcore.os.os] in file: src/std/internal/Libcore.sharp:10:13: note:  
		    class os {
		            ^


os:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@46 [libcore#Libcore.Libcore] in file: src/std/internal/Libcore.sharp:8:6: note:  
		class Libcore {
		     ^


Libcore:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@47 [std#Array.copyOf] in file: src/std/Array.sharp:5:14: note:  
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

func:@48 [std#Array.Array] in file: src/std/Array.sharp:3:6: note:  
		class Array {
		     ^


Array:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

