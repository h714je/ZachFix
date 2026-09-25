
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

004dcfe0 <.text+0xdbfe0>:
  4dcfe0:	push   ecx
  4dcfe1:	mov    eax,ds:0x139277c
  4dcfe6:	push   esi
  4dcfe7:	mov    esi,0x3
  4dcfec:	cmp    eax,esi
  4dcfee:	ja     0x4dd14a
  4dcff4:	jmp    DWORD PTR [eax*4+0x4dd150]
  4dcffb:	call   0x427780
  4dd000:	fld    DWORD PTR ds:0x77036c
  4dd006:	push   0x1
  4dd008:	push   0x0
  4dd00a:	push   ecx
  4dd00b:	mov    ecx,DWORD PTR [eax+0x4]
  4dd00e:	fstp   DWORD PTR [esp]
  4dd011:	push   esi
  4dd012:	call   0x449a00
  4dd017:	inc    DWORD PTR ds:0x139277c
  4dd01d:	pop    esi
  4dd01e:	pop    ecx
  4dd01f:	ret
  4dd020:	call   0x427780
  4dd025:	mov    ecx,DWORD PTR [eax+0x4]
  4dd028:	call   0x4492c0
  4dd02d:	test   al,al
  4dd02f:	jne    0x4dd14a
  4dd035:	call   0x405360
  4dd03a:	push   0x0
  4dd03c:	mov    ecx,eax
  4dd03e:	call   0x4400b0
  4dd043:	xor    ecx,ecx
  4dd045:	cmp    eax,0xffffffff
  4dd048:	setne  cl
  4dd04b:	mov    ds:0x1392778,eax
  4dd050:	pop    esi
  4dd051:	add    ecx,0x2
  4dd054:	mov    DWORD PTR ds:0x139277c,ecx
  4dd05a:	pop    ecx
  4dd05b:	ret
  4dd05c:	call   0x408960
  4dd061:	mov    edx,DWORD PTR ds:0x1392778
  4dd067:	push   edx
  4dd068:	mov    ecx,eax
  4dd06a:	call   0x70e9a0
  4dd06f:	test   eax,eax
  4dd071:	jne    0x4dd14a
  4dd077:	mov    DWORD PTR ds:0x139277c,esi
  4dd07d:	pop    esi
  4dd07e:	pop    ecx
  4dd07f:	ret
  4dd080:	mov    eax,ds:0xbe1ea4
  4dd085:	test   eax,eax
  4dd087:	jne    0x4dd0a9
  4dd089:	push   0x1ac
  4dd08e:	call   0x404390
  4dd093:	add    esp,0x4
  4dd096:	test   eax,eax
  4dd098:	je     0x4dd0a2
  4dd09a:	mov    DWORD PTR [eax],0x7721bc
  4dd0a0:	jmp    0x4dd0a4
  4dd0a2:	xor    eax,eax
  4dd0a4:	mov    ds:0xbe1ea4,eax
  4dd0a9:	or     DWORD PTR [eax+0x11c],esi
  4dd0af:	call   0x405360
  4dd0b4:	mov    ecx,eax
  4dd0b6:	call   0x433b10
  4dd0bb:	mov    ecx,DWORD PTR ds:0xbd7670
  4dd0c1:	mov    esi,eax
  4dd0c3:	mov    eax,ds:0x8a9ba4
  4dd0c8:	fld    DWORD PTR [esi+0x7c]
  4dd0cb:	push   eax
  4dd0cc:	fstp   DWORD PTR [esp+0x8]
  4dd0d0:	call   0x6c5fd0
  4dd0d5:	fld    DWORD PTR [esp+0x4]
  4dd0d9:	fstp   DWORD PTR [eax+0x6c]
  4dd0dc:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dd0e2:	push   ecx
  4dd0e3:	mov    ecx,DWORD PTR ds:0xbd7670
  4dd0e9:	call   0x6c5fd0
  4dd0ee:	fld    DWORD PTR [esp+0x4]
  4dd0f2:	fstp   DWORD PTR [eax+0x7c]
  4dd0f5:	mov    edx,DWORD PTR [esi+0xd8]
  4dd0fb:	and    DWORD PTR [esi+0xdc],0xfffffffd
  4dd102:	mov    DWORD PTR [esi+0xd8],edx
  4dd108:	mov    eax,ds:0x8a9ba4
  4dd10d:	mov    ecx,DWORD PTR ds:0xbd7670
  4dd113:	push   0x38
  4dd115:	push   eax
  4dd116:	call   0x6c5fd0
  4dd11b:	mov    ecx,eax
  4dd11d:	call   0x528f40
  4dd122:	call   0x427780
  4dd127:	fld    DWORD PTR ds:0x77036c
  4dd12d:	push   0x1
  4dd12f:	push   0x0
  4dd131:	push   ecx
  4dd132:	mov    ecx,DWORD PTR [eax+0x4]
  4dd135:	fstp   DWORD PTR [esp]
  4dd138:	push   0x0
  4dd13a:	call   0x449a00
  4dd13f:	call   0x449480
  4dd144:	add    eax,0x4
  4dd147:	or     DWORD PTR [eax],0x4
  4dd14a:	pop    esi
  4dd14b:	pop    ecx
  4dd14c:	ret
