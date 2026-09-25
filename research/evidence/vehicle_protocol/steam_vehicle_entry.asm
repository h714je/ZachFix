
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

004dddb0 <.text+0xdcdb0>:
  4dddb0:	sub    esp,0x90
  4dddb6:	push   ebx
  4dddb7:	push   esi
  4dddb8:	push   edi
  4dddb9:	mov    ebx,ecx
  4dddbb:	call   0x40a320
  4dddc0:	mov    esi,DWORD PTR [eax+0x8c57c]
  4dddc6:	mov    eax,ds:0x8a9ba4
  4dddcb:	mov    ecx,DWORD PTR ds:0xbd7670
  4dddd1:	push   eax
  4dddd2:	call   0x6c5fd0
  4dddd7:	fldz
  4dddd9:	fstp   DWORD PTR [eax+0x644]
  4ddddf:	call   0x4051f0
  4ddde4:	push   0x1
  4ddde6:	push   0x2590
  4dddeb:	mov    ecx,eax
  4ddded:	call   0x6b2be0
  4dddf2:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dddf8:	push   ecx
  4dddf9:	mov    ecx,DWORD PTR ds:0xbd7670
  4dddff:	mov    edi,eax
  4dde01:	call   0x6c5fd0
  4dde06:	push   0x4000
  4dde0b:	mov    DWORD PTR [eax+0x628],edi
  4dde11:	mov    DWORD PTR [eax+0x630],0x1
  4dde1b:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dde21:	mov    ecx,DWORD PTR ds:0xbd7670
  4dde27:	push   0x0
  4dde29:	push   edx
  4dde2a:	call   0x6c5fd0
  4dde2f:	mov    ecx,eax
  4dde31:	call   0x4fd840
  4dde36:	mov    eax,ds:0x8a9ba4
  4dde3b:	mov    ecx,DWORD PTR ds:0xbd7670
  4dde41:	push   0x0
  4dde43:	push   0x400
  4dde48:	push   eax
  4dde49:	call   0x6c5fd0
  4dde4e:	mov    ecx,eax
  4dde50:	call   0x4fd820
  4dde55:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dde5b:	push   0x87
  4dde60:	push   ecx
  4dde61:	mov    ecx,DWORD PTR ds:0xbd7670
  4dde67:	call   0x6c5fd0
  4dde6c:	mov    ecx,eax
  4dde6e:	call   0x528f40
  4dde73:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dde79:	mov    ecx,DWORD PTR ds:0xbd7670
  4dde7f:	push   edx
  4dde80:	call   0x6c5fd0
  4dde85:	or     DWORD PTR [eax+0x63c],0x40
  4dde8c:	mov    eax,ds:0x8a9ba4
  4dde91:	mov    ecx,DWORD PTR ds:0xbd7670
  4dde97:	push   eax
  4dde98:	call   0x6c5fd0
  4dde9d:	mov    ecx,DWORD PTR [eax+0xdc]
  4ddea3:	or     DWORD PTR [eax+0xd8],0x80000000
  4ddead:	mov    DWORD PTR [eax+0xdc],ecx
  4ddeb3:	mov    edx,DWORD PTR ds:0x8a9ba4
  4ddeb9:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddebf:	push   edx
  4ddec0:	call   0x6c5fd0
  4ddec5:	and    DWORD PTR [eax+0x638],0xffff7fff
  4ddecf:	mov    eax,ds:0x8a9ba4
  4dded4:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddeda:	push   eax
  4ddedb:	call   0x6c5fd0
  4ddee0:	mov    edi,0xfffffffe
  4ddee5:	and    DWORD PTR [eax+0x638],edi
  4ddeeb:	mov    eax,ds:0xbe1ea4
  4ddef0:	test   eax,eax
  4ddef2:	jne    0x4ddf14
  4ddef4:	push   0x1ac
  4ddef9:	call   0x404390
  4ddefe:	add    esp,0x4
  4ddf01:	test   eax,eax
  4ddf03:	je     0x4ddf0d
  4ddf05:	mov    DWORD PTR [eax],0x7721bc
  4ddf0b:	jmp    0x4ddf0f
  4ddf0d:	xor    eax,eax
  4ddf0f:	mov    ds:0xbe1ea4,eax
  4ddf14:	and    DWORD PTR [eax+0x11c],0xfffffffd
  4ddf1b:	mov    eax,ds:0xbe1ea4
  4ddf20:	and    DWORD PTR [eax+0x11c],edi
  4ddf26:	mov    eax,ds:0xbe1ea4
  4ddf2b:	or     DWORD PTR [eax+0x11c],0x8
  4ddf32:	mov    DWORD PTR [ebx+0x9f4],0xffffffff
  4ddf3c:	mov    ecx,DWORD PTR [esi+0x58]
  4ddf3f:	mov    edx,DWORD PTR [esi+0x5c]
  4ddf42:	mov    eax,DWORD PTR [esi+0x60]
  4ddf45:	mov    DWORD PTR [esi+0x3b0],ecx
  4ddf4b:	mov    ecx,DWORD PTR [esi+0x64]
  4ddf4e:	mov    DWORD PTR [esi+0x3b4],edx
  4ddf54:	mov    DWORD PTR [esi+0x3b8],eax
  4ddf5a:	mov    DWORD PTR [esi+0x3bc],ecx
  4ddf60:	mov    edx,DWORD PTR ds:0x8a9ba4
  4ddf66:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddf6c:	push   edx
  4ddf6d:	call   0x6c5fd0
  4ddf72:	fld    DWORD PTR [eax+0x58]
  4ddf75:	fsub   DWORD PTR [esi+0x58]
  4ddf78:	add    eax,0x58
  4ddf7b:	push   ecx
  4ddf7c:	fstp   DWORD PTR [esp+0x10]
  4ddf80:	fld    DWORD PTR [eax+0x4]
  4ddf83:	fsub   DWORD PTR [esi+0x5c]
  4ddf86:	fstp   DWORD PTR [esp+0x14]
  4ddf8a:	fld    DWORD PTR [eax+0x8]
  4ddf8d:	fsub   DWORD PTR [esi+0x60]
  4ddf90:	fstp   DWORD PTR [esp+0x18]
  4ddf94:	fld    DWORD PTR [eax+0xc]
  4ddf97:	lea    eax,[esp+0x20]
  4ddf9b:	fsub   DWORD PTR [esi+0x64]
  4ddf9e:	fstp   DWORD PTR [esp+0x1c]
  4ddfa2:	fldz
  4ddfa4:	fst    DWORD PTR [esp+0x58]
  4ddfa8:	fst    DWORD PTR [esp+0x54]
  4ddfac:	fst    DWORD PTR [esp+0x50]
  4ddfb0:	fst    DWORD PTR [esp+0x4c]
  4ddfb4:	fst    DWORD PTR [esp+0x44]
  4ddfb8:	fst    DWORD PTR [esp+0x40]
  4ddfbc:	fst    DWORD PTR [esp+0x3c]
  4ddfc0:	fst    DWORD PTR [esp+0x38]
  4ddfc4:	fst    DWORD PTR [esp+0x30]
  4ddfc8:	fst    DWORD PTR [esp+0x2c]
  4ddfcc:	fst    DWORD PTR [esp+0x28]
  4ddfd0:	fstp   DWORD PTR [esp+0x24]
  4ddfd4:	fld1
  4ddfd6:	fst    DWORD PTR [esp+0x5c]
  4ddfda:	fst    DWORD PTR [esp+0x48]
  4ddfde:	fst    DWORD PTR [esp+0x34]
  4ddfe2:	fstp   DWORD PTR [esp+0x20]
  4ddfe6:	fld    DWORD PTR [esi+0x7c]
  4ddfe9:	fchs
  4ddfeb:	fstp   DWORD PTR [esp]
  4ddfee:	push   eax
  4ddfef:	call   0x73a9f4
  4ddff4:	mov    edx,DWORD PTR [esp+0x10]
  4ddff8:	mov    ecx,DWORD PTR [esp+0xc]
  4ddffc:	mov    eax,DWORD PTR [esp+0x14]
  4de000:	mov    DWORD PTR [esp+0x8c],ecx
  4de007:	mov    ecx,DWORD PTR [esp+0x18]
  4de00b:	mov    DWORD PTR [esp+0x90],edx
  4de012:	lea    edx,[esp+0x1c]
  4de016:	mov    DWORD PTR [esp+0x94],eax
  4de01d:	push   edx
  4de01e:	lea    eax,[esp+0x60]
  4de022:	mov    DWORD PTR [esp+0x9c],ecx
  4de029:	push   eax
  4de02a:	mov    ecx,eax
  4de02c:	push   ecx
  4de02d:	call   0x73a9d0
  4de032:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de038:	mov    ecx,DWORD PTR ds:0xbd7670
  4de03e:	push   0x0
  4de040:	push   0x0
  4de042:	push   0x0
  4de044:	push   0x0
  4de046:	push   0x775a40
  4de04b:	push   esi
  4de04c:	push   edx
  4de04d:	call   0x6c5fd0
  4de052:	mov    ecx,eax
  4de054:	call   0x6c2880
  4de059:	mov    eax,DWORD PTR [esi+0x134]
  4de05f:	and    DWORD PTR [esi+0x130],0xfffffff8
  4de066:	or     DWORD PTR [esi+0x130],0x1000
  4de070:	mov    ecx,eax
  4de072:	mov    DWORD PTR [esi+0x134],ecx
  4de078:	call   0x40a320
  4de07d:	mov    DWORD PTR [eax+0x8c5b8],0x11
  4de087:	call   0x40a320
  4de08c:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de092:	mov    ecx,DWORD PTR ds:0xbd7670
  4de098:	mov    edi,eax
  4de09a:	push   edx
  4de09b:	add    edi,0x8c57c
  4de0a1:	call   0x6c5fd0
  4de0a6:	mov    eax,ds:0x148132c
  4de0ab:	test   eax,eax
  4de0ad:	je     0x4de0b8
  4de0af:	push   edi
  4de0b0:	push   0x35
  4de0b2:	push   esi
  4de0b3:	call   eax
  4de0b5:	add    esp,0xc
  4de0b8:	mov    eax,DWORD PTR [esi+0x44]
  4de0bb:	test   eax,eax
  4de0bd:	je     0x4de0c8
  4de0bf:	push   edi
  4de0c0:	push   0x35
  4de0c2:	push   esi
  4de0c3:	call   eax
  4de0c5:	add    esp,0xc
  4de0c8:	call   0x40a320
  4de0cd:	mov    DWORD PTR [eax+0x8c5b8],0x13
  4de0d7:	call   0x40a320
  4de0dc:	mov    ecx,DWORD PTR ds:0xbd7670
  4de0e2:	mov    edi,eax
  4de0e4:	mov    eax,ds:0x8a9ba4
  4de0e9:	push   eax
  4de0ea:	add    edi,0x8c57c
  4de0f0:	call   0x6c5fd0
  4de0f5:	mov    eax,ds:0x148132c
  4de0fa:	test   eax,eax
  4de0fc:	je     0x4de107
  4de0fe:	push   edi
  4de0ff:	push   0x35
  4de101:	push   esi
  4de102:	call   eax
  4de104:	add    esp,0xc
  4de107:	mov    eax,DWORD PTR [esi+0x44]
  4de10a:	test   eax,eax
  4de10c:	je     0x4de117
  4de10e:	push   edi
  4de10f:	push   0x35
  4de111:	push   esi
  4de112:	call   eax
  4de114:	add    esp,0xc
  4de117:	mov    eax,ds:0xbe1ea4
  4de11c:	test   eax,eax
  4de11e:	jne    0x4de140
  4de120:	push   0x1ac
  4de125:	call   0x404390
  4de12a:	add    esp,0x4
  4de12d:	test   eax,eax
  4de12f:	je     0x4de139
  4de131:	mov    DWORD PTR [eax],0x7721bc
  4de137:	jmp    0x4de13b
  4de139:	xor    eax,eax
  4de13b:	mov    ds:0xbe1ea4,eax
  4de140:	cmp    DWORD PTR [eax+0x150],0x0
  4de147:	jne    0x4de152
  4de149:	push   0x1
  4de14b:	mov    ecx,esi
  4de14d:	call   0x549f80
  4de152:	mov    ecx,ebx
  4de154:	call   0x4dced0
  4de159:	push   esi
  4de15a:	push   0x1
  4de15c:	call   0x4183d0
  4de161:	mov    ecx,eax
  4de163:	call   0x46f050
  4de168:	call   0x405360
  4de16d:	mov    ecx,eax
  4de16f:	call   0x42d300
  4de174:	call   0x40a320
  4de179:	mov    cx,WORD PTR [eax+0x9906c]
  4de180:	cmp    cx,WORD PTR [esi+0x2e]
  4de184:	je     0x4de190
  4de186:	or     DWORD PTR [esi+0x434],0x2000
  4de190:	call   0x40a320
  4de195:	mov    dx,WORD PTR [esi+0x2e]
  4de199:	mov    ecx,esi
  4de19b:	mov    WORD PTR [eax+0x9906c],dx
  4de1a2:	call   0x54e2e0
  4de1a7:	fld    DWORD PTR ds:0x7740d4
  4de1ad:	pop    edi
  4de1ae:	fstp   DWORD PTR [esi+0x134c]
  4de1b4:	pop    esi
  4de1b5:	mov    DWORD PTR [ebx+0x660],0x0
  4de1bf:	pop    ebx
  4de1c0:	add    esp,0x90
  4de1c6:	ret
  4de1c7:	int3
  4de1c8:	int3
  4de1c9:	int3
  4de1ca:	int3
  4de1cb:	int3
  4de1cc:	int3
  4de1cd:	int3
  4de1ce:	int3
  4de1cf:	int3
