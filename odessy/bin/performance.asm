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
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 0
[0x3] 3:	movn #0
[0x4] 4:	movi #0, adx
[0x6] 6:	movx ebx, adx
[0x7] 7:	pushr ebx
[0x8] 8:	call @5 // <application#Main.fact(var)>
[0x9] 9:	pop
[0xa] 10:	ret

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
[0x5] 5:	call @12 // <application#Main.Main()>
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
[0x2] 2:	movr ebx, bmr
[0x3] 3:	movr ecx, ebx
[0x4] 4:	movr adx, fp
[0x5] 5:	smovr ecx+0
[0x6] 6:	movi #1, ebx
[0x8] 8:	movr adx, fp
[0x9] 9:	smovr ebx+[-5]
[0xa] 10:	ret

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
[0x88] 136:	call @34 // <std.err#Throwable.Throwable(var[])>
[0x89] 137:	throw 
[0x8a] 138:	inc sp
[0x8b] 139:	iframe
[0x8c] 140:	inc sp
[0x8d] 141:	movsl #0
[0x8e] 142:	new_class @2 // std.err#RuntimeErr
[0x8f] 143:	inc sp
[0x90] 144:	movsl #0
[0x91] 145:	newstr @3
[0x92] 146:	call @34 // <std.err#Throwable.Throwable(var[])>
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
[0x9e] 158:	goto @161
[0x9f] 159:	nop
[0xa0] 160:	nop
[0xa1] 161:	goto @164
[0xa2] 162:	nop
[0xa3] 163:	nop
[0xa4] 164:	iframe
[0xa5] 165:	inc sp
[0xa6] 166:	movsl #0
[0xa7] 167:	newstr @5
[0xa8] 168:	call @2 // <application#Main.print(var[])>
[0xa9] 169:	nop
[0xaa] 170:	nop
[0xab] 171:	iframe
[0xac] 172:	inc sp
[0xad] 173:	movsl #0
[0xae] 174:	newstr @6
[0xaf] 175:	call @2 // <application#Main.print(var[])>
[0xb0] 176:	iframe
[0xb1] 177:	inc sp
[0xb2] 178:	movsl #0
[0xb3] 179:	newstr @7
[0xb4] 180:	call @0 // <application#Base.print2(var[])>
[0xb5] 181:	movi #11, ebx
[0xb7] 183:	movr ecx, ebx
[0xb8] 184:	movr adx, fp
[0xb9] 185:	smovr ecx+5
[0xba] 186:	nop
[0xbb] 187:	nop
[0xbc] 188:	movi #6, ebx
[0xbe] 190:	movr egx, ebx
[0xbf] 191:	movr adx, fp
[0xc0] 192:	smov ebx+5
[0xc1] 193:	test ebx, egx
[0xc2] 194:	movr ebx, cmt
[0xc3] 195:	movr cmt, ebx
[0xc4] 196:	loadx adx
[0xc5] 197:	iadd adx, #9
[0xc6] 198:	ifne
[0xc7] 199:	iframe
[0xc8] 200:	inc sp
[0xc9] 201:	movsl #0
[0xca] 202:	newstr @8
[0xcb] 203:	call @2 // <application#Main.print(var[])>
[0xcc] 204:	goto @254
[0xcd] 205:	movi #10, ebx
[0xcf] 207:	movr egx, ebx
[0xd0] 208:	movr adx, fp
[0xd1] 209:	smov ebx+5
[0xd2] 210:	test ebx, egx
[0xd3] 211:	movr ebx, cmt
[0xd4] 212:	movr cmt, ebx
[0xd5] 213:	loadx adx
[0xd6] 214:	iadd adx, #9
[0xd7] 215:	ifne
[0xd8] 216:	iframe
[0xd9] 217:	inc sp
[0xda] 218:	movsl #0
[0xdb] 219:	newstr @9
[0xdc] 220:	call @2 // <application#Main.print(var[])>
[0xdd] 221:	goto @254
[0xde] 222:	movl 2
[0xdf] 223:	chknull
[0xe0] 224:	movr ebx, cmt
[0xe1] 225:	movr cmt, ebx
[0xe2] 226:	loadx adx
[0xe3] 227:	iadd adx, #9
[0xe4] 228:	ifne
[0xe5] 229:	iframe
[0xe6] 230:	inc sp
[0xe7] 231:	movsl #0
[0xe8] 232:	newstr @10
[0xe9] 233:	call @0 // <application#Base.print2(var[])>
[0xea] 234:	goto @254
[0xeb] 235:	movl 2
[0xec] 236:	chknull
[0xed] 237:	not cmt, cmt
[0xee] 238:	movr ebx, cmt
[0xef] 239:	movr cmt, ebx
[0xf0] 240:	loadx adx
[0xf1] 241:	iadd adx, #9
[0xf2] 242:	ifne
[0xf3] 243:	iframe
[0xf4] 244:	inc sp
[0xf5] 245:	movsl #0
[0xf6] 246:	newstr @11
[0xf7] 247:	call @0 // <application#Base.print2(var[])>
[0xf8] 248:	goto @254
[0xf9] 249:	iframe
[0xfa] 250:	inc sp
[0xfb] 251:	movsl #0
[0xfc] 252:	newstr @12
[0xfd] 253:	call @2 // <application#Main.print(var[])>
[0xfe] 254:	movl 8
[0xff] 255:	inc sp
[0x100] 256:	iframe
[0x101] 257:	inc sp
[0x102] 258:	movsl #0
[0x103] 259:	new_class @10 // std#List
[0x104] 260:	call @63 // <std#List.List()>
[0x105] 261:	movl 8
[0x106] 262:	popref
[0x107] 263:	iframe
[0x108] 264:	inc sp
[0x109] 265:	movsl #0
[0x10a] 266:	newstr @13
[0x10b] 267:	call @2 // <application#Main.print(var[])>
[0x10c] 268:	iframe
[0x10d] 269:	movl 8
[0x10e] 270:	pushref
[0x10f] 271:	inc sp
[0x110] 272:	iframe
[0x111] 273:	inc sp
[0x112] 274:	movsl #0
[0x113] 275:	new_class @11 // std#Integer
[0x114] 276:	movi #9, ebx
[0x116] 278:	pushr ebx
[0x117] 279:	call @64 // <std#Integer.Integer(var)>
[0x118] 280:	call @52 // <std#List.add(dynamic object)>
[0x119] 281:	iframe
[0x11a] 282:	inc sp
[0x11b] 283:	iframe
[0x11c] 284:	inc sp
[0x11d] 285:	iframe
[0x11e] 286:	inc sp
[0x11f] 287:	iframe
[0x120] 288:	movl 8
[0x121] 289:	pushref
[0x122] 290:	movi #0, ebx
[0x124] 292:	pushr ebx
[0x125] 293:	call @57 // <std#List.get(var)>
[0x126] 294:	movsl #0
[0x127] 295:	check_cast 11 // std#Integer
[0x128] 296:	call @66 // <std#Integer.getValue()>
[0x129] 297:	call @65 // <std#Integer.toString(var)>
[0x12a] 298:	call @40 // <std#System.print(std#string)>
[0x12b] 299:	iframe
[0x12c] 300:	inc sp
[0x12d] 301:	movsl #0
[0x12e] 302:	newstr @14
[0x12f] 303:	call @2 // <application#Main.print(var[])>
[0x130] 304:	iframe
[0x131] 305:	movl 8
[0x132] 306:	pushref
[0x133] 307:	inc sp
[0x134] 308:	iframe
[0x135] 309:	inc sp
[0x136] 310:	movsl #0
[0x137] 311:	new_class @11 // std#Integer
[0x138] 312:	movi #10, ebx
[0x13a] 314:	pushr ebx
[0x13b] 315:	call @64 // <std#Integer.Integer(var)>
[0x13c] 316:	call @52 // <std#List.add(dynamic object)>
[0x13d] 317:	iframe
[0x13e] 318:	inc sp
[0x13f] 319:	iframe
[0x140] 320:	inc sp
[0x141] 321:	iframe
[0x142] 322:	inc sp
[0x143] 323:	iframe
[0x144] 324:	movl 8
[0x145] 325:	pushref
[0x146] 326:	movi #1, ebx
[0x148] 328:	pushr ebx
[0x149] 329:	call @57 // <std#List.get(var)>
[0x14a] 330:	movsl #0
[0x14b] 331:	check_cast 11 // std#Integer
[0x14c] 332:	call @66 // <std#Integer.getValue()>
[0x14d] 333:	call @65 // <std#Integer.toString(var)>
[0x14e] 334:	call @40 // <std#System.print(std#string)>
[0x14f] 335:	iframe
[0x150] 336:	inc sp
[0x151] 337:	movsl #0
[0x152] 338:	newstr @15
[0x153] 339:	call @2 // <application#Main.print(var[])>
[0x154] 340:	iframe
[0x155] 341:	movl 8
[0x156] 342:	pushref
[0x157] 343:	inc sp
[0x158] 344:	iframe
[0x159] 345:	inc sp
[0x15a] 346:	movsl #0
[0x15b] 347:	new_class @11 // std#Integer
[0x15c] 348:	movi #22, ebx
[0x15e] 350:	pushr ebx
[0x15f] 351:	call @64 // <std#Integer.Integer(var)>
[0x160] 352:	call @52 // <std#List.add(dynamic object)>
[0x161] 353:	iframe
[0x162] 354:	inc sp
[0x163] 355:	movsl #0
[0x164] 356:	newstr @16
[0x165] 357:	call @2 // <application#Main.print(var[])>
[0x166] 358:	iframe
[0x167] 359:	inc sp
[0x168] 360:	iframe
[0x169] 361:	inc sp
[0x16a] 362:	iframe
[0x16b] 363:	inc sp
[0x16c] 364:	iframe
[0x16d] 365:	movl 8
[0x16e] 366:	pushref
[0x16f] 367:	movi #2, ebx
[0x171] 369:	pushr ebx
[0x172] 370:	call @57 // <std#List.get(var)>
[0x173] 371:	movsl #0
[0x174] 372:	check_cast 11 // std#Integer
[0x175] 373:	call @66 // <std#Integer.getValue()>
[0x176] 374:	call @65 // <std#Integer.toString(var)>
[0x177] 375:	call @40 // <std#System.print(std#string)>
[0x178] 376:	iframe
[0x179] 377:	inc sp
[0x17a] 378:	movsl #0
[0x17b] 379:	newstr @17
[0x17c] 380:	call @2 // <application#Main.print(var[])>
[0x17d] 381:	movi #32, ebx
[0x17f] 383:	_putc ebx
[0x180] 384:	movr adx, sp
[0x181] 385:	smov ebx+0
[0x182] 386:	put ebx
[0x183] 387:	ret

func:@12 [application#Main.Main] in file: odessy/test2.sharp:10:6: note:  
		class Main base Base {
		     ^


Main:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	movi #1, ebx
[0x4] 4:	newi ebx
[0x5] 5:	movi #9, ebx
[0x7] 7:	pushr ebx
[0x8] 8:	movl 0
[0x9] 9:	movn #0
[0xa] 10:	movi #0, adx
[0xc] 12:	popr ecx
[0xd] 13:	rmov adx, ecx
[0xe] 14:	movl 0
[0xf] 15:	movr adx, fp
[0x10] 16:	smovobj @-5
[0x11] 17:	ret

func:@13 [std.err#RuntimeErr.RuntimeErr] in file: src/std/error/RuntimeErr.sharp:6:6: note:  
		class RuntimeErr base Throwable {
		     ^


RuntimeErr:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@14 [std#string.string] in file: src/std/string.sharp:7:15: note:  
		        string( var[] immstr )
		              ^


string:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 0
[0x3] 3:	pushref
[0x4] 4:	movl 1
[0x5] 5:	pushref
[0x6] 6:	call @16 // <std#string.append(var[])>
[0x7] 7:	sdel
[0x8] 8:	movl 0
[0x9] 9:	movr adx, fp
[0xa] 10:	smovobj @-5
[0xb] 11:	ret

func:@15 [std#string.replace] in file: src/std/string.sharp:12:19: note:  
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
[0x13] 19:	newstr @18
[0x14] 20:	call @34 // <std.err#Throwable.Throwable(var[])>
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

func:@16 [std#string.append] in file: src/std/string.sharp:22:18: note:  
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
[0x78] 120:	call @49 // <std#Array.copyOf(var[],var)>
[0x79] 121:	movl 0
[0x7a] 122:	movn #0
[0x7b] 123:	popref
[0x7c] 124:	movl 2
[0x7d] 125:	del
[0x7e] 126:	movl 0
[0x7f] 127:	movr adx, fp
[0x80] 128:	smovobj @-5
[0x81] 129:	ret

func:@17 [std#string.at] in file: src/std/string.sharp:41:14: note:  
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
[0x1a] 26:	newstr @19
[0x1b] 27:	call @14 // <std#string.string(var[])>
[0x1c] 28:	inc sp
[0x1d] 29:	iframe
[0x1e] 30:	movl 0
[0x1f] 31:	movn #0
[0x20] 32:	sizeof ebx
[0x21] 33:	pushr ebx
[0x22] 34:	call @28 // <std#string.convertInt(var)>
[0x23] 35:	call @19 // <std#string.$operator+(std#string)>
[0x24] 36:	movsl #0
[0x25] 37:	dec sp
[0x26] 38:	inc sp
[0x27] 39:	iframe
[0x28] 40:	inc sp
[0x29] 41:	movr adx, sp
[0x2a] 42:	smovobj @0
[0x2b] 43:	inc sp
[0x2c] 44:	movsl #0
[0x2d] 45:	newstr @20
[0x2e] 46:	call @18 // <std#string.$operator+(var[])>
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
[0x3b] 59:	call @28 // <std#string.convertInt(var)>
[0x3c] 60:	call @19 // <std#string.$operator+(std#string)>
[0x3d] 61:	call @35 // <std.err#Throwable.Throwable(std#string)>
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

func:@18 [std#string.$operator+] in file: src/std/string.sharp:51:20: note:  
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
[0x9] 9:	call @14 // <std#string.string(var[])>
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
[0x14] 20:	call @16 // <std#string.append(var[])>
[0x15] 21:	movsl #0
[0x16] 22:	movr adx, fp
[0x17] 23:	smovobj @-5
[0x18] 24:	ret

func:@19 [std#string.$operator+] in file: src/std/string.sharp:57:20: note:  
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
[0x9] 9:	call @14 // <std#string.string(var[])>
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
[0x18] 24:	call @27 // <std#string.value()>
[0x19] 25:	call @16 // <std#string.append(var[])>
[0x1a] 26:	movsl #0
[0x1b] 27:	movr adx, fp
[0x1c] 28:	smovobj @-5
[0x1d] 29:	ret

func:@20 [std#string.$operator==] in file: src/std/string.sharp:63:20: note:  
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
[0x1e] 30:	call @17 // <std#string.at(var)>
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

func:@21 [std#string.$operator==] in file: src/std/string.sharp:75:20: note:  
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

func:@22 [std#string.$operator=] in file: src/std/string.sharp:87:20: note:  
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
[0x8] 8:	call @49 // <std#Array.copyOf(var[],var)>
[0x9] 9:	movl 0
[0xa] 10:	movn #0
[0xb] 11:	popref
[0xc] 12:	ret

func:@23 [std#string.$operator+] in file: src/std/string.sharp:92:20: note:  
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
[0x1f] 31:	call @14 // <std#string.string(var[])>
[0x20] 32:	movl 3
[0x21] 33:	popref
[0x22] 34:	inc sp
[0x23] 35:	iframe
[0x24] 36:	movl 3
[0x25] 37:	pushref
[0x26] 38:	movl 2
[0x27] 39:	pushref
[0x28] 40:	call @16 // <std#string.append(var[])>
[0x29] 41:	sdel
[0x2a] 42:	movl 3
[0x2b] 43:	movr adx, fp
[0x2c] 44:	smovobj @-5
[0x2d] 45:	ret

func:@24 [std#string.$operator+=] in file: src/std/string.sharp:103:20: note:  
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
[0x1c] 28:	call @16 // <std#string.append(var[])>
[0x1d] 29:	movsl #0
[0x1e] 30:	movr adx, fp
[0x1f] 31:	smovobj @-5
[0x20] 32:	ret

func:@25 [std#string.$operator+=] in file: src/std/string.sharp:111:20: note:  
		        fn operator+=( var[] immstr ) : string
		                   ^


$operator+=:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 0
[0x3] 3:	pushref
[0x4] 4:	movl 1
[0x5] 5:	pushref
[0x6] 6:	call @16 // <std#string.append(var[])>
[0x7] 7:	movsl #0
[0x8] 8:	movr adx, fp
[0x9] 9:	smovobj @-5
[0xa] 10:	ret

func:@26 [std#string.$operator+=] in file: src/std/string.sharp:116:20: note:  
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
[0xa] 10:	call @27 // <std#string.value()>
[0xb] 11:	call @16 // <std#string.append(var[])>
[0xc] 12:	movsl #0
[0xd] 13:	movr adx, fp
[0xe] 14:	smovobj @-5
[0xf] 15:	ret

func:@27 [std#string.value] in file: src/std/string.sharp:121:17: note:  
		        fn value() : var[]
		                ^


value:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	returnref
[0x3] 3:	ret

func:@28 [std#string.convertInt] in file: src/std/string.sharp:126:29: note:  
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
[0x12] 18:	newstr @21
[0x13] 19:	call @14 // <std#string.string(var[])>
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
[0x20] 32:	newstr @22
[0x21] 33:	call @14 // <std#string.string(var[])>
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
[0x2c] 44:	newstr @22
[0x2d] 45:	call @14 // <std#string.string(var[])>
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
[0x51] 81:	call @24 // <std#string.$operator+=(var)>
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
[0x5f] 95:	call @27 // <std#string.value()>
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
[0x7f] 127:	call @17 // <std#string.at(var)>
[0x80] 128:	call @24 // <std#string.$operator+=(var)>
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

func:@29 [std#string.size] in file: src/std/string.sharp:145:16: note:  
		        fn size() : var 
		               ^


size:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	sizeof ebx
[0x3] 3:	movr adx, fp
[0x4] 4:	smovr ebx+[-5]
[0x5] 5:	ret

func:@30 [std#string.string] in file: src/std/string.sharp:3:6: note:  
		class string {
		     ^


string:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@31 [application#Start.transposeArray] in file: src/srt/srt_main.sharp:9:34: note:  
		    private macros transposeArray(var[] args) {
		                                 ^


transposeArray:
[0x0] 0:	movl 0
[0x1] 1:	del
[0x2] 2:	ret

func:@32 [application#Start.__init] in file: src/srt/srt_main.sharp:18:28: note:  
		    public static fn __init(var[] args) : var {
		                           ^


__init:
[0x0] 0:	movl 1
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	new_class @3 // std#string
[0x6] 6:	call @30 // <std#string.string()>
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

func:@33 [application#Start.Start] in file: src/srt/srt_main.sharp:5:6: note:  
		class Start {
		     ^


Start:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@34 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:10:14: note:  
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

func:@35 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:14:14: note:  
		    Throwable(string message) {
		             ^


Throwable:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 1
[0x3] 3:	pushref
[0x4] 4:	call @27 // <std#string.value()>
[0x5] 5:	movl 0
[0x6] 6:	movn #0
[0x7] 7:	popref
[0x8] 8:	movl 0
[0x9] 9:	movr adx, fp
[0xa] 10:	smovobj @-5
[0xb] 11:	ret

func:@36 [std.err#Throwable.getStackTrace] in file: src/std/error/Throwable.sharp:18:21: note:  
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
[0xa] 10:	newstr @22
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
[0x16] 22:	call @14 // <std#string.string(var[])>
[0x17] 23:	movsl #0
[0x18] 24:	movr adx, fp
[0x19] 25:	smovobj @-5
[0x1a] 26:	ret

func:@37 [std.err#Throwable.getMessage] in file: src/std/error/Throwable.sharp:26:18: note:  
		    fn getMessage() : var[] {
		                 ^


getMessage:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	returnref
[0x3] 3:	ret

func:@38 [std.err#Throwable.Throwable] in file: src/std/error/Throwable.sharp:6:6: note:  
		class Throwable {
		     ^


Throwable:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@39 [std#System.print] in file: src/std/System.sharp:10:27: note:  
		    public static fn print(var[] data) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	movl 0
[0x2] 2:	pushref
[0x3] 3:	call @44 // <libcore#Libcore.os.printData(var[])>
[0x4] 4:	ret

func:@40 [std#System.print] in file: src/std/System.sharp:14:27: note:  
		    public static fn print(string str) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	inc sp
[0x2] 2:	iframe
[0x3] 3:	movl 0
[0x4] 4:	pushref
[0x5] 5:	call @27 // <std#string.value()>
[0x6] 6:	call @44 // <libcore#Libcore.os.printData(var[])>
[0x7] 7:	ret

func:@41 [std#System.print] in file: src/std/System.sharp:18:27: note:  
		    public static fn print(var data) {
		                          ^


print:
[0x0] 0:	iframe
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+0
[0x3] 3:	pushr ebx
[0x4] 4:	movi #0, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	call @46 // <libcore#Libcore.os.__print_(var,var)>
[0x8] 8:	ret

func:@42 [std#System.printChar] in file: src/std/System.sharp:22:31: note:  
		    public static fn printChar(var data) {
		                              ^


printChar:
[0x0] 0:	iframe
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+0
[0x3] 3:	pushr ebx
[0x4] 4:	movi #1, ebx
[0x6] 6:	pushr ebx
[0x7] 7:	call @46 // <libcore#Libcore.os.__print_(var,var)>
[0x8] 8:	ret

func:@43 [std#System.System] in file: src/std/System.sharp:8:6: note:  
		class System {
		     ^


System:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@44 [libcore#Libcore.os.printData] in file: src/std/internal/Libcore.sharp:11:35: note:  
		        public static fn printData(var[] data) {
		                                  ^


printData:
[0x0] 0:	iframe
[0x1] 1:	movl 0
[0x2] 2:	pushref
[0x3] 3:	call @45 // <libcore#Libcore.os.__print_(var[])>
[0x4] 4:	ret

func:@45 [libcore#Libcore.os.__print_] in file: src/std/internal/Libcore.sharp:19:35: note:  
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
[0x1e] 30:	call @46 // <libcore#Libcore.os.__print_(var,var)>
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
[0x2d] 45:	call @46 // <libcore#Libcore.os.__print_(var,var)>
[0x2e] 46:	iframe
[0x2f] 47:	movi #117, ebx
[0x31] 49:	pushr ebx
[0x32] 50:	movi #1, ebx
[0x34] 52:	pushr ebx
[0x35] 53:	call @46 // <libcore#Libcore.os.__print_(var,var)>
[0x36] 54:	iframe
[0x37] 55:	movi #108, ebx
[0x39] 57:	pushr ebx
[0x3a] 58:	movi #1, ebx
[0x3c] 60:	pushr ebx
[0x3d] 61:	call @46 // <libcore#Libcore.os.__print_(var,var)>
[0x3e] 62:	iframe
[0x3f] 63:	movi #108, ebx
[0x41] 65:	pushr ebx
[0x42] 66:	movi #1, ebx
[0x44] 68:	pushr ebx
[0x45] 69:	call @46 // <libcore#Libcore.os.__print_(var,var)>
[0x46] 70:	ret

func:@46 [libcore#Libcore.os.__print_] in file: src/std/internal/Libcore.sharp:35:34: note:  
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

func:@47 [libcore#Libcore.os.os] in file: src/std/internal/Libcore.sharp:10:13: note:  
		    class os {
		            ^


os:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@48 [libcore#Libcore.Libcore] in file: src/std/internal/Libcore.sharp:8:6: note:  
		class Libcore {
		     ^


Libcore:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@49 [std#Array.copyOf] in file: src/std/Array.sharp:5:14: note:  
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

func:@50 [std#Array.copyOf] in file: src/std/Array.sharp:15:14: note:  
		    fn copyOf(dynamic_object[] data, var len) : dynamic_object[] {
		             ^


copyOf:
[0x0] 0:	movl 3
[0x1] 1:	movr adx, fp
[0x2] 2:	smov ebx+2
[0x3] 3:	inc sp
[0x4] 4:	movsl #0
[0x5] 5:	new_objarry ebx
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
[0x15] 21:	movi #48, adx
[0x17] 23:	ifne
[0x18] 24:	movl 1
[0x19] 25:	pushref
[0x1a] 26:	movr adx, fp
[0x1b] 27:	smov ebx+4
[0x1c] 28:	movsl #0
[0x1d] 29:	chklen ebx
[0x1e] 30:	movnd ebx
[0x1f] 31:	sdelref
[0x20] 32:	pushref
[0x21] 33:	movl 3
[0x22] 34:	pushref
[0x23] 35:	movr adx, fp
[0x24] 36:	smov ebx+4
[0x25] 37:	movsl #0
[0x26] 38:	chklen ebx
[0x27] 39:	movnd ebx
[0x28] 40:	sdelref
[0x29] 41:	popref
[0x2a] 42:	movr adx, fp
[0x2b] 43:	smov ebx+4
[0x2c] 44:	movi #1, ecx
[0x2e] 46:	addl ecx, @4
[0x2f] 47:	goto @13
[0x30] 48:	movl 3
[0x31] 49:	movr adx, fp
[0x32] 50:	smovobj @-5
[0x33] 51:	ret

func:@51 [std#Array.Array] in file: src/std/Array.sharp:3:6: note:  
		class Array {
		     ^


Array:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@52 [std#List.add] in file: src/std/List.sharp:9:11: note:  
		    fn add( dynamic_object object ) {
		          ^


add:
[0x0] 0:	movl 2
[0x1] 1:	movi #1, ebx
[0x3] 3:	pushr ebx
[0x4] 4:	inc sp
[0x5] 5:	iframe
[0x6] 6:	movl 0
[0x7] 7:	pushref
[0x8] 8:	call @60 // <std#List.size()>
[0x9] 9:	movr adx, sp
[0xa] 10:	smov ebx+0
[0xb] 11:	pop
[0xc] 12:	popr ecx
[0xd] 13:	add ebx, ecx -> ebx
[0xf] 15:	inc sp
[0x10] 16:	movsl #0
[0x11] 17:	new_objarry ebx
[0x12] 18:	movl 2
[0x13] 19:	popref
[0x14] 20:	movi #0, ebx
[0x16] 22:	movr ecx, ebx
[0x17] 23:	movr adx, fp
[0x18] 24:	smovr ecx+3
[0x19] 25:	movi #0, ebx
[0x1b] 27:	pushr ebx
[0x1c] 28:	movl 0
[0x1d] 29:	movn #0
[0x1e] 30:	movr adx, sp
[0x1f] 31:	smov ebx+0
[0x20] 32:	sizeof egx
[0x21] 33:	lt ebx, egx
[0x22] 34:	movi #60, adx
[0x24] 36:	ifne
[0x25] 37:	movnd ebx
[0x26] 38:	pushref
[0x27] 39:	movl 4
[0x28] 40:	popref
[0x29] 41:	movl 4
[0x2a] 42:	pushref
[0x2b] 43:	movl 2
[0x2c] 44:	pushref
[0x2d] 45:	movr adx, fp
[0x2e] 46:	smov ebx+3
[0x2f] 47:	movi #1, ecx
[0x31] 49:	addl ecx, @3
[0x32] 50:	movsl #0
[0x33] 51:	chklen ebx
[0x34] 52:	movnd ebx
[0x35] 53:	sdelref
[0x36] 54:	popref
[0x37] 55:	movr adx, sp
[0x38] 56:	smov ebx+0
[0x39] 57:	inc ebx
[0x3a] 58:	smovr ebx+0
[0x3b] 59:	goto @28
[0x3c] 60:	pop
[0x3d] 61:	movl 1
[0x3e] 62:	pushref
[0x3f] 63:	movl 2
[0x40] 64:	pushref
[0x41] 65:	inc sp
[0x42] 66:	iframe
[0x43] 67:	movl 0
[0x44] 68:	pushref
[0x45] 69:	call @60 // <std#List.size()>
[0x46] 70:	movr adx, sp
[0x47] 71:	smov ebx+0
[0x48] 72:	pop
[0x49] 73:	movsl #0
[0x4a] 74:	chklen ebx
[0x4b] 75:	movnd ebx
[0x4c] 76:	sdelref
[0x4d] 77:	popref
[0x4e] 78:	inc sp
[0x4f] 79:	iframe
[0x50] 80:	pushref
[0x51] 81:	movl 2
[0x52] 82:	pushref
[0x53] 83:	movl 2
[0x54] 84:	sizeof ebx
[0x55] 85:	pushr ebx
[0x56] 86:	call @50 // <std#Array.copyOf(dynamic object[],var)>
[0x57] 87:	movl 0
[0x58] 88:	movn #0
[0x59] 89:	popref
[0x5a] 90:	ret

func:@53 [std#List.add] in file: src/std/List.sharp:21:11: note:  
		    fn add( var pos, dynamic_object object ) {
		          ^


add:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	sizeof ebx
[0x3] 3:	skpne -1 // pc = 2
[0x4] 4:	movr egx, ebx
[0x5] 5:	movr adx, fp
[0x6] 6:	smov ebx+1
[0x7] 7:	gt ebx, egx
[0x8] 8:	movr ebx, cmt
[0x9] 9:	movr cmt, ebx
[0xa] 10:	movi #64, adx
[0xc] 12:	ifne
[0xd] 13:	inc sp
[0xe] 14:	iframe
[0xf] 15:	inc sp
[0x10] 16:	movsl #0
[0x11] 17:	new_class @2 // std.err#RuntimeErr
[0x12] 18:	inc sp
[0x13] 19:	iframe
[0x14] 20:	inc sp
[0x15] 21:	iframe
[0x16] 22:	inc sp
[0x17] 23:	movsl #0
[0x18] 24:	new_class @3 // std#string
[0x19] 25:	inc sp
[0x1a] 26:	movsl #0
[0x1b] 27:	newstr @23
[0x1c] 28:	call @14 // <std#string.string(var[])>
[0x1d] 29:	inc sp
[0x1e] 30:	iframe
[0x1f] 31:	movl 0
[0x20] 32:	movn #0
[0x21] 33:	sizeof ebx
[0x22] 34:	pushr ebx
[0x23] 35:	call @65 // <std#Integer.toString(var)>
[0x24] 36:	call @19 // <std#string.$operator+(std#string)>
[0x25] 37:	movsl #0
[0x26] 38:	dec sp
[0x27] 39:	inc sp
[0x28] 40:	iframe
[0x29] 41:	inc sp
[0x2a] 42:	movr adx, sp
[0x2b] 43:	smovobj @0
[0x2c] 44:	inc sp
[0x2d] 45:	movsl #0
[0x2e] 46:	newstr @24
[0x2f] 47:	call @18 // <std#string.$operator+(var[])>
[0x30] 48:	movsl #0
[0x31] 49:	dec sp
[0x32] 50:	inc sp
[0x33] 51:	iframe
[0x34] 52:	inc sp
[0x35] 53:	movr adx, sp
[0x36] 54:	smovobj @0
[0x37] 55:	inc sp
[0x38] 56:	iframe
[0x39] 57:	movr adx, fp
[0x3a] 58:	smov ebx+1
[0x3b] 59:	pushr ebx
[0x3c] 60:	call @65 // <std#Integer.toString(var)>
[0x3d] 61:	call @19 // <std#string.$operator+(std#string)>
[0x3e] 62:	call @35 // <std.err#Throwable.Throwable(std#string)>
[0x3f] 63:	throw 
[0x40] 64:	movi #1, ebx
[0x42] 66:	pushr ebx
[0x43] 67:	inc sp
[0x44] 68:	iframe
[0x45] 69:	movl 0
[0x46] 70:	pushref
[0x47] 71:	call @60 // <std#List.size()>
[0x48] 72:	movr adx, sp
[0x49] 73:	smov ebx+0
[0x4a] 74:	pop
[0x4b] 75:	popr ecx
[0x4c] 76:	add ebx, ecx -> ebx
[0x4e] 78:	movr ecx, ebx
[0x4f] 79:	movr adx, fp
[0x50] 80:	smovr ecx+3
[0x51] 81:	movl 4
[0x52] 82:	movr adx, fp
[0x53] 83:	smov ebx+3
[0x54] 84:	inc sp
[0x55] 85:	movsl #0
[0x56] 86:	new_objarry ebx
[0x57] 87:	movl 4
[0x58] 88:	popref
[0x59] 89:	movi #0, ebx
[0x5b] 91:	movr ecx, ebx
[0x5c] 92:	movr adx, fp
[0x5d] 93:	smovr ecx+5
[0x5e] 94:	movi #0, ebx
[0x60] 96:	movr ecx, ebx
[0x61] 97:	movr adx, fp
[0x62] 98:	smovr ecx+6
[0x63] 99:	movr adx, fp
[0x64] 100:	smov ebx+1
[0x65] 101:	movr egx, ebx
[0x66] 102:	movr adx, fp
[0x67] 103:	smov ebx+6
[0x68] 104:	lt ebx, egx
[0x69] 105:	movr ebx, cmt
[0x6a] 106:	movr cmt, ebx
[0x6b] 107:	movi #138, adx
[0x6d] 109:	ifne
[0x6e] 110:	movl 0
[0x6f] 111:	movn #0
[0x70] 112:	pushref
[0x71] 113:	movr adx, fp
[0x72] 114:	smov ebx+6
[0x73] 115:	movsl #0
[0x74] 116:	chklen ebx
[0x75] 117:	movnd ebx
[0x76] 118:	sdelref
[0x77] 119:	pushref
[0x78] 120:	movl 4
[0x79] 121:	pushref
[0x7a] 122:	movr adx, fp
[0x7b] 123:	smov ebx+5
[0x7c] 124:	movi #1, ecx
[0x7e] 126:	addl ecx, @5
[0x7f] 127:	movsl #0
[0x80] 128:	chklen ebx
[0x81] 129:	movnd ebx
[0x82] 130:	sdelref
[0x83] 131:	popref
[0x84] 132:	movr adx, fp
[0x85] 133:	smov ebx+6
[0x86] 134:	movi #1, ecx
[0x88] 136:	addl ecx, @6
[0x89] 137:	goto @99
[0x8a] 138:	movl 2
[0x8b] 139:	pushref
[0x8c] 140:	movl 4
[0x8d] 141:	pushref
[0x8e] 142:	movr adx, fp
[0x8f] 143:	smov ebx+5
[0x90] 144:	movi #1, ecx
[0x92] 146:	addl ecx, @5
[0x93] 147:	movsl #0
[0x94] 148:	chklen ebx
[0x95] 149:	movnd ebx
[0x96] 150:	sdelref
[0x97] 151:	popref
[0x98] 152:	movi #1, ebx
[0x9a] 154:	movr egx, ebx
[0x9b] 155:	movr adx, fp
[0x9c] 156:	smov ebx+1
[0x9d] 157:	add ebx, egx -> ebx
[0x9f] 159:	movr ecx, ebx
[0xa0] 160:	movr adx, fp
[0xa1] 161:	smovr ecx+7
[0xa2] 162:	movr adx, fp
[0xa3] 163:	smov ebx+3
[0xa4] 164:	movr egx, ebx
[0xa5] 165:	movr adx, fp
[0xa6] 166:	smov ebx+7
[0xa7] 167:	lt ebx, egx
[0xa8] 168:	movr ebx, cmt
[0xa9] 169:	movr cmt, ebx
[0xaa] 170:	movi #203, adx
[0xac] 172:	ifne
[0xad] 173:	movl 0
[0xae] 174:	movn #0
[0xaf] 175:	pushref
[0xb0] 176:	movi #1, ebx
[0xb2] 178:	movr egx, ebx
[0xb3] 179:	movr adx, fp
[0xb4] 180:	smov ebx+7
[0xb5] 181:	sub ebx, egx -> ebx
[0xb7] 183:	movsl #0
[0xb8] 184:	chklen ebx
[0xb9] 185:	movnd ebx
[0xba] 186:	sdelref
[0xbb] 187:	pushref
[0xbc] 188:	movl 4
[0xbd] 189:	pushref
[0xbe] 190:	movr adx, fp
[0xbf] 191:	smov ebx+7
[0xc0] 192:	movsl #0
[0xc1] 193:	chklen ebx
[0xc2] 194:	movnd ebx
[0xc3] 195:	sdelref
[0xc4] 196:	popref
[0xc5] 197:	movr adx, fp
[0xc6] 198:	smov ebx+7
[0xc7] 199:	movi #1, ecx
[0xc9] 201:	addl ecx, @7
[0xca] 202:	goto @162
[0xcb] 203:	inc sp
[0xcc] 204:	iframe
[0xcd] 205:	pushref
[0xce] 206:	movl 4
[0xcf] 207:	pushref
[0xd0] 208:	movl 4
[0xd1] 209:	sizeof ebx
[0xd2] 210:	pushr ebx
[0xd3] 211:	call @50 // <std#Array.copyOf(dynamic object[],var)>
[0xd4] 212:	movl 0
[0xd5] 213:	movn #0
[0xd6] 214:	popref
[0xd7] 215:	movl 4
[0xd8] 216:	del
[0xd9] 217:	ret

func:@54 [std#List.addAll] in file: src/std/List.sharp:44:14: note:  
		    fn addAll( var pos, List collection ) {
		             ^


addAll:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	sizeof ebx
[0x3] 3:	skpne -1 // pc = 2
[0x4] 4:	movr egx, ebx
[0x5] 5:	movr adx, fp
[0x6] 6:	smov ebx+1
[0x7] 7:	gt ebx, egx
[0x8] 8:	movr ebx, cmt
[0x9] 9:	movr cmt, ebx
[0xa] 10:	movi #64, adx
[0xc] 12:	ifne
[0xd] 13:	inc sp
[0xe] 14:	iframe
[0xf] 15:	inc sp
[0x10] 16:	movsl #0
[0x11] 17:	new_class @2 // std.err#RuntimeErr
[0x12] 18:	inc sp
[0x13] 19:	iframe
[0x14] 20:	inc sp
[0x15] 21:	iframe
[0x16] 22:	inc sp
[0x17] 23:	movsl #0
[0x18] 24:	new_class @3 // std#string
[0x19] 25:	inc sp
[0x1a] 26:	movsl #0
[0x1b] 27:	newstr @23
[0x1c] 28:	call @14 // <std#string.string(var[])>
[0x1d] 29:	inc sp
[0x1e] 30:	iframe
[0x1f] 31:	movl 0
[0x20] 32:	movn #0
[0x21] 33:	sizeof ebx
[0x22] 34:	pushr ebx
[0x23] 35:	call @65 // <std#Integer.toString(var)>
[0x24] 36:	call @19 // <std#string.$operator+(std#string)>
[0x25] 37:	movsl #0
[0x26] 38:	dec sp
[0x27] 39:	inc sp
[0x28] 40:	iframe
[0x29] 41:	inc sp
[0x2a] 42:	movr adx, sp
[0x2b] 43:	smovobj @0
[0x2c] 44:	inc sp
[0x2d] 45:	movsl #0
[0x2e] 46:	newstr @24
[0x2f] 47:	call @18 // <std#string.$operator+(var[])>
[0x30] 48:	movsl #0
[0x31] 49:	dec sp
[0x32] 50:	inc sp
[0x33] 51:	iframe
[0x34] 52:	inc sp
[0x35] 53:	movr adx, sp
[0x36] 54:	smovobj @0
[0x37] 55:	inc sp
[0x38] 56:	iframe
[0x39] 57:	movr adx, fp
[0x3a] 58:	smov ebx+1
[0x3b] 59:	pushr ebx
[0x3c] 60:	call @65 // <std#Integer.toString(var)>
[0x3d] 61:	call @19 // <std#string.$operator+(std#string)>
[0x3e] 62:	call @35 // <std.err#Throwable.Throwable(std#string)>
[0x3f] 63:	throw 
[0x40] 64:	movl 3
[0x41] 65:	inc sp
[0x42] 66:	iframe
[0x43] 67:	movl 2
[0x44] 68:	pushref
[0x45] 69:	call @60 // <std#List.size()>
[0x46] 70:	inc sp
[0x47] 71:	iframe
[0x48] 72:	movl 0
[0x49] 73:	pushref
[0x4a] 74:	call @60 // <std#List.size()>
[0x4b] 75:	movr adx, sp
[0x4c] 76:	smov ebx+0
[0x4d] 77:	pop
[0x4e] 78:	popr ecx
[0x4f] 79:	add ebx, ecx -> ebx
[0x51] 81:	inc sp
[0x52] 82:	movsl #0
[0x53] 83:	new_objarry ebx
[0x54] 84:	movl 3
[0x55] 85:	popref
[0x56] 86:	movi #0, ebx
[0x58] 88:	movr ecx, ebx
[0x59] 89:	movr adx, fp
[0x5a] 90:	smovr ecx+4
[0x5b] 91:	movi #0, ebx
[0x5d] 93:	movr ecx, ebx
[0x5e] 94:	movr adx, fp
[0x5f] 95:	smovr ecx+5
[0x60] 96:	movr adx, fp
[0x61] 97:	smov ebx+1
[0x62] 98:	movr egx, ebx
[0x63] 99:	movr adx, fp
[0x64] 100:	smov ebx+5
[0x65] 101:	lt ebx, egx
[0x66] 102:	movr ebx, cmt
[0x67] 103:	movr cmt, ebx
[0x68] 104:	movi #135, adx
[0x6a] 106:	ifne
[0x6b] 107:	movl 0
[0x6c] 108:	movn #0
[0x6d] 109:	pushref
[0x6e] 110:	movr adx, fp
[0x6f] 111:	smov ebx+5
[0x70] 112:	movsl #0
[0x71] 113:	chklen ebx
[0x72] 114:	movnd ebx
[0x73] 115:	sdelref
[0x74] 116:	pushref
[0x75] 117:	movl 3
[0x76] 118:	pushref
[0x77] 119:	movr adx, fp
[0x78] 120:	smov ebx+4
[0x79] 121:	movi #1, ecx
[0x7b] 123:	addl ecx, @4
[0x7c] 124:	movsl #0
[0x7d] 125:	chklen ebx
[0x7e] 126:	movnd ebx
[0x7f] 127:	sdelref
[0x80] 128:	popref
[0x81] 129:	movr adx, fp
[0x82] 130:	smov ebx+5
[0x83] 131:	movi #1, ecx
[0x85] 133:	addl ecx, @5
[0x86] 134:	goto @96
[0x87] 135:	movi #0, ebx
[0x89] 137:	pushr ebx
[0x8a] 138:	inc sp
[0x8b] 139:	iframe
[0x8c] 140:	movl 2
[0x8d] 141:	pushref
[0x8e] 142:	call @62 // <std#List.getArray()>
[0x8f] 143:	movr adx, sp
[0x90] 144:	smov ebx+0
[0x91] 145:	sizeof egx
[0x92] 146:	lt ebx, egx
[0x93] 147:	movi #177, adx
[0x95] 149:	ifne
[0x96] 150:	inc sp
[0x97] 151:	iframe
[0x98] 152:	movl 2
[0x99] 153:	pushref
[0x9a] 154:	call @62 // <std#List.getArray()>
[0x9b] 155:	movnd ebx
[0x9c] 156:	movl 6
[0x9d] 157:	popref
[0x9e] 158:	movl 6
[0x9f] 159:	pushref
[0xa0] 160:	movl 3
[0xa1] 161:	pushref
[0xa2] 162:	movr adx, fp
[0xa3] 163:	smov ebx+4
[0xa4] 164:	movi #1, ecx
[0xa6] 166:	addl ecx, @4
[0xa7] 167:	movsl #0
[0xa8] 168:	chklen ebx
[0xa9] 169:	movnd ebx
[0xaa] 170:	sdelref
[0xab] 171:	popref
[0xac] 172:	movr adx, sp
[0xad] 173:	smov ebx+0
[0xae] 174:	inc ebx
[0xaf] 175:	smovr ebx+0
[0xb0] 176:	goto @138
[0xb1] 177:	pop
[0xb2] 178:	movi #1, ebx
[0xb4] 180:	movr egx, ebx
[0xb5] 181:	movr adx, fp
[0xb6] 182:	smov ebx+1
[0xb7] 183:	add ebx, egx -> ebx
[0xb9] 185:	movr ecx, ebx
[0xba] 186:	movr adx, fp
[0xbb] 187:	smovr ecx+7
[0xbc] 188:	inc sp
[0xbd] 189:	iframe
[0xbe] 190:	movl 0
[0xbf] 191:	pushref
[0xc0] 192:	call @60 // <std#List.size()>
[0xc1] 193:	movr egx, ebx
[0xc2] 194:	movr adx, fp
[0xc3] 195:	smov ebx+7
[0xc4] 196:	lt ebx, egx
[0xc5] 197:	movr ebx, cmt
[0xc6] 198:	movr cmt, ebx
[0xc7] 199:	movi #232, adx
[0xc9] 201:	ifne
[0xca] 202:	movl 0
[0xcb] 203:	movn #0
[0xcc] 204:	pushref
[0xcd] 205:	movi #1, ebx
[0xcf] 207:	movr egx, ebx
[0xd0] 208:	movr adx, fp
[0xd1] 209:	smov ebx+7
[0xd2] 210:	sub ebx, egx -> ebx
[0xd4] 212:	movsl #0
[0xd5] 213:	chklen ebx
[0xd6] 214:	movnd ebx
[0xd7] 215:	sdelref
[0xd8] 216:	pushref
[0xd9] 217:	movl 3
[0xda] 218:	pushref
[0xdb] 219:	movr adx, fp
[0xdc] 220:	smov ebx+7
[0xdd] 221:	movsl #0
[0xde] 222:	chklen ebx
[0xdf] 223:	movnd ebx
[0xe0] 224:	sdelref
[0xe1] 225:	popref
[0xe2] 226:	movr adx, fp
[0xe3] 227:	smov ebx+7
[0xe4] 228:	movi #1, ecx
[0xe6] 230:	addl ecx, @7
[0xe7] 231:	goto @188
[0xe8] 232:	inc sp
[0xe9] 233:	iframe
[0xea] 234:	pushref
[0xeb] 235:	movl 3
[0xec] 236:	pushref
[0xed] 237:	movl 3
[0xee] 238:	sizeof ebx
[0xef] 239:	pushr ebx
[0xf0] 240:	call @50 // <std#Array.copyOf(dynamic object[],var)>
[0xf1] 241:	movl 0
[0xf2] 242:	movn #0
[0xf3] 243:	popref
[0xf4] 244:	movl 3
[0xf5] 245:	del
[0xf6] 246:	ret

func:@55 [std#List.addAll] in file: src/std/List.sharp:67:14: note:  
		    fn addAll(List collection) {
		             ^


addAll:
[0x0] 0:	iframe
[0x1] 1:	movl 0
[0x2] 2:	pushref
[0x3] 3:	inc sp
[0x4] 4:	iframe
[0x5] 5:	movl 0
[0x6] 6:	pushref
[0x7] 7:	call @60 // <std#List.size()>
[0x8] 8:	movl 1
[0x9] 9:	pushref
[0xa] 10:	call @54 // <std#List.addAll(var,std#List)>
[0xb] 11:	ret

func:@56 [std#List.clear] in file: src/std/List.sharp:71:13: note:  
		    fn clear() {
		            ^


clear:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	del
[0x3] 3:	ret

func:@57 [std#List.get] in file: src/std/List.sharp:75:11: note:  
		    fn get( var pos ) : dynamic_object {
		          ^


get:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	pushref
[0x3] 3:	movr adx, fp
[0x4] 4:	smov ebx+1
[0x5] 5:	movsl #0
[0x6] 6:	chklen ebx
[0x7] 7:	movnd ebx
[0x8] 8:	sdelref
[0x9] 9:	returnref
[0xa] 10:	ret

func:@58 [std#List.remove] in file: src/std/List.sharp:79:14: note:  
		    fn remove( var pos ) {
		             ^


remove:
[0x0] 0:	ret

func:@59 [std#List.set] in file: src/std/List.sharp:83:11: note:  
		    fn set( var pos, dynamic_object object )
		          ^


set:
[0x0] 0:	ret

func:@60 [std#List.size] in file: src/std/List.sharp:88:12: note:  
		    fn size() : var 
		           ^


size:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	sizeof ebx
[0x3] 3:	movr adx, fp
[0x4] 4:	smovr ebx+[-5]
[0x5] 5:	ret

func:@61 [std#List.subList] in file: src/std/List.sharp:93:15: note:  
		    fn subList( var start, var end ) : List
		              ^


subList:
[0x0] 0:	ret

func:@62 [std#List.getArray] in file: src/std/List.sharp:98:16: note:  
		    fn getArray() : dynamic_object[]
		               ^


getArray:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	returnref
[0x3] 3:	ret

func:@63 [std#List.List] in file: src/std/List.sharp:5:6: note:  
		class List {
		     ^


List:
[0x0] 0:	movl 0
[0x1] 1:	movr adx, fp
[0x2] 2:	smovobj @-5
[0x3] 3:	ret

func:@64 [std#Integer.Integer] in file: src/std/Integer.sharp:6:19: note:  
		    public Integer( var value )
		                  ^


Integer:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	movi #1, ebx
[0x4] 4:	newi ebx
[0x5] 5:	movr adx, fp
[0x6] 6:	smov ebx+1
[0x7] 7:	pushr ebx
[0x8] 8:	movl 0
[0x9] 9:	movn #0
[0xa] 10:	movi #0, adx
[0xc] 12:	popr ecx
[0xd] 13:	rmov adx, ecx
[0xe] 14:	movl 0
[0xf] 15:	movr adx, fp
[0x10] 16:	smovobj @-5
[0x11] 17:	ret

func:@65 [std#Integer.toString] in file: src/std/Integer.sharp:11:23: note:  
		    static fn toString( var number ) : string 
		                      ^


toString:
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
[0x12] 18:	newstr @21
[0x13] 19:	call @14 // <std#string.string(var[])>
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
[0x20] 32:	newstr @22
[0x21] 33:	call @14 // <std#string.string(var[])>
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
[0x2c] 44:	newstr @22
[0x2d] 45:	call @14 // <std#string.string(var[])>
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
[0x51] 81:	call @24 // <std#string.$operator+=(var)>
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
[0x5f] 95:	call @27 // <std#string.value()>
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
[0x7f] 127:	call @17 // <std#string.at(var)>
[0x80] 128:	call @24 // <std#string.$operator+=(var)>
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

func:@66 [std#Integer.getValue] in file: src/std/Integer.sharp:30:16: note:  
		    fn getValue() : var {
		               ^


getValue:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	movi #0, adx
[0x4] 4:	movx ebx, adx
[0x5] 5:	movr adx, fp
[0x6] 6:	smovr ebx+[-5]
[0x7] 7:	ret

func:@67 [std#Integer.toString] in file: src/std/Integer.sharp:34:23: note:  
		    public fn toString() : string 
		                      ^


toString:
[0x0] 0:	inc sp
[0x1] 1:	iframe
[0x2] 2:	movl 0
[0x3] 3:	movn #0
[0x4] 4:	movi #0, adx
[0x6] 6:	movx ebx, adx
[0x7] 7:	pushr ebx
[0x8] 8:	call @65 // <std#Integer.toString(var)>
[0x9] 9:	movsl #0
[0xa] 10:	movr adx, fp
[0xb] 11:	smovobj @-5
[0xc] 12:	ret

func:@68 [std#Integer.Integer] in file: src/std/Integer.sharp:3:6: note:  
		class Integer {
		     ^


Integer:
[0x0] 0:	movl 0
[0x1] 1:	movn #0
[0x2] 2:	movi #1, ebx
[0x4] 4:	newi ebx
[0x5] 5:	movl 0
[0x6] 6:	movr adx, fp
[0x7] 7:	smovobj @-5
[0x8] 8:	ret

