
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

004dd0b0 <.text+0xdc0b0>:
  4dd0b0:	push   ecx
  4dd0b1:	mov    eax,ds:0x139277c
  4dd0b6:	push   esi
  4dd0b7:	mov    esi,0x3
  4dd0bc:	cmp    eax,esi
  4dd0be:	ja     0x4dd21a
  4dd0c4:	jmp    DWORD PTR [eax*4+0x4dd220]
  4dd0cb:	call   0x4277a0
  4dd0d0:	fld    DWORD PTR ds:0x77035c
  4dd0d6:	push   0x1
  4dd0d8:	push   0x0
  4dd0da:	push   ecx
  4dd0db:	mov    ecx,DWORD PTR [eax+0x4]
  4dd0de:	fstp   DWORD PTR [esp]
  4dd0e1:	push   esi
  4dd0e2:	call   0x449a30
  4dd0e7:	inc    DWORD PTR ds:0x139277c
  4dd0ed:	pop    esi
  4dd0ee:	pop    ecx
  4dd0ef:	ret
  4dd0f0:	call   0x4277a0
  4dd0f5:	mov    ecx,DWORD PTR [eax+0x4]
  4dd0f8:	call   0x4492f0
  4dd0fd:	test   al,al
  4dd0ff:	jne    0x4dd21a
  4dd105:	call   0x405330
  4dd10a:	push   0x0
  4dd10c:	mov    ecx,eax
  4dd10e:	call   0x440130
  4dd113:	xor    ecx,ecx
  4dd115:	cmp    eax,0xffffffff
  4dd118:	setne  cl
  4dd11b:	mov    ds:0x1392778,eax
  4dd120:	pop    esi
  4dd121:	add    ecx,0x2
  4dd124:	mov    DWORD PTR ds:0x139277c,ecx
  4dd12a:	pop    ecx
  4dd12b:	ret
  4dd12c:	call   0x408920
  4dd131:	mov    edx,DWORD PTR ds:0x1392778
  4dd137:	push   edx
  4dd138:	mov    ecx,eax
  4dd13a:	call   0x70e940
  4dd13f:	test   eax,eax
  4dd141:	jne    0x4dd21a
  4dd147:	mov    DWORD PTR ds:0x139277c,esi
  4dd14d:	pop    esi
  4dd14e:	pop    ecx
  4dd14f:	ret
  4dd150:	mov    eax,ds:0xbe1ea4
  4dd155:	test   eax,eax
  4dd157:	jne    0x4dd179
  4dd159:	push   0x1ac
  4dd15e:	call   0x404370
  4dd163:	add    esp,0x4
  4dd166:	test   eax,eax
  4dd168:	je     0x4dd172
  4dd16a:	mov    DWORD PTR [eax],0x7721ac
  4dd170:	jmp    0x4dd174
  4dd172:	xor    eax,eax
  4dd174:	mov    ds:0xbe1ea4,eax
  4dd179:	or     DWORD PTR [eax+0x11c],esi
  4dd17f:	call   0x405330
  4dd184:	mov    ecx,eax
  4dd186:	call   0x433b90
  4dd18b:	mov    ecx,DWORD PTR ds:0xbd7670
  4dd191:	mov    esi,eax
  4dd193:	mov    eax,ds:0x8a9ba4
  4dd198:	fld    DWORD PTR [esi+0x7c]
  4dd19b:	push   eax
  4dd19c:	fstp   DWORD PTR [esp+0x8]
  4dd1a0:	call   0x6c5ad0
  4dd1a5:	fld    DWORD PTR [esp+0x4]
  4dd1a9:	fstp   DWORD PTR [eax+0x6c]
  4dd1ac:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dd1b2:	push   ecx
  4dd1b3:	mov    ecx,DWORD PTR ds:0xbd7670
  4dd1b9:	call   0x6c5ad0
  4dd1be:	fld    DWORD PTR [esp+0x4]
  4dd1c2:	fstp   DWORD PTR [eax+0x7c]
  4dd1c5:	mov    edx,DWORD PTR [esi+0xd8]
  4dd1cb:	and    DWORD PTR [esi+0xdc],0xfffffffd
  4dd1d2:	mov    DWORD PTR [esi+0xd8],edx
  4dd1d8:	mov    eax,ds:0x8a9ba4
  4dd1dd:	mov    ecx,DWORD PTR ds:0xbd7670
  4dd1e3:	push   0x38
  4dd1e5:	push   eax
  4dd1e6:	call   0x6c5ad0
  4dd1eb:	mov    ecx,eax
  4dd1ed:	call   0x529010
  4dd1f2:	call   0x4277a0
  4dd1f7:	fld    DWORD PTR ds:0x77035c
  4dd1fd:	push   0x1
  4dd1ff:	push   0x0
  4dd201:	push   ecx
  4dd202:	mov    ecx,DWORD PTR [eax+0x4]
  4dd205:	fstp   DWORD PTR [esp]
  4dd208:	push   0x0
  4dd20a:	call   0x449a30
  4dd20f:	call   0x4494b0
  4dd214:	add    eax,0x4
  4dd217:	or     DWORD PTR [eax],0x4
  4dd21a:	pop    esi
  4dd21b:	pop    ecx
  4dd21c:	ret
