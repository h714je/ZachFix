
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

005421d0 <.text+0x1411d0>:
  5421d0:	push   ebx
  5421d1:	mov    ebx,DWORD PTR [esp+0x8]
  5421d5:	mov    eax,DWORD PTR [ebx+0x3c]
  5421d8:	add    eax,0xfffffff1
  5421db:	push   esi
  5421dc:	mov    esi,ecx
  5421de:	cmp    eax,0x6
  5421e1:	ja     0x542343
  5421e7:	push   edi
  5421e8:	jmp    DWORD PTR [eax*4+0x542348]
  5421ef:	mov    eax,ds:0x8a9ba4
  5421f4:	mov    ecx,DWORD PTR ds:0xbd7670
  5421fa:	push   eax
  5421fb:	call   0x6c5fd0
  542200:	fld    DWORD PTR [eax+0x644]
  542206:	fstp   DWORD PTR [esp+0x10]
  54220a:	call   0x4051f0
  54220f:	fld    DWORD PTR [esp+0x10]
  542213:	mov    edi,DWORD PTR [esi]
  542215:	push   0x0
  542217:	push   0x0
  542219:	sub    esp,0x8
  54221c:	fstp   DWORD PTR [esp+0x4]
  542220:	mov    ecx,eax
  542222:	fldz
  542224:	fstp   DWORD PTR [esp]
  542227:	push   0x0
  542229:	push   0x29d3
  54222e:	call   0x6b2be0
  542233:	mov    edx,DWORD PTR [edi+0x54]
  542236:	push   eax
  542237:	mov    ecx,esi
  542239:	call   edx
  54223b:	or     DWORD PTR [esi+0x434],0x100
  542245:	mov    ecx,esi
  542247:	call   0x541520
  54224c:	mov    ecx,esi
  54224e:	call   0x547fe0
  542253:	fld1
  542255:	push   ecx
  542256:	fstp   DWORD PTR [esp]
  542259:	mov    ecx,esi
  54225b:	mov    BYTE PTR [esi+0x3a],0x0
  54225f:	call   0x6c3fc0
  542264:	pop    edi
  542265:	pop    esi
  542266:	pop    ebx
  542267:	ret    0x4
  54226a:	mov    eax,DWORD PTR [esi+0x18c]
  542270:	test   eax,eax
  542272:	je     0x542342
  542278:	lea    ecx,[esi+0x178]
  54227e:	test   ecx,ecx
  542280:	je     0x542342
  542286:	movzx  eax,WORD PTR [eax+0xa]
  54228a:	mov    DWORD PTR [esp+0x10],eax
  54228e:	pop    edi
  54228f:	fild   DWORD PTR [esp+0xc]
  542293:	fstp   DWORD PTR [esi+0x194]
  542299:	pop    esi
  54229a:	pop    ebx
  54229b:	ret    0x4
  54229e:	mov    ecx,DWORD PTR ds:0x8a9ba4
  5422a4:	push   ecx
  5422a5:	mov    ecx,DWORD PTR ds:0xbd7670
  5422ab:	call   0x6c5fd0
  5422b0:	fld    DWORD PTR [eax+0x644]
  5422b6:	fstp   DWORD PTR [esp+0x10]
  5422ba:	call   0x4051f0
  5422bf:	fld    DWORD PTR [esp+0x10]
  5422c3:	mov    edi,DWORD PTR [esi]
  5422c5:	push   0x0
  5422c7:	push   0x0
  5422c9:	sub    esp,0x8
  5422cc:	fstp   DWORD PTR [esp+0x4]
  5422d0:	mov    ecx,eax
  5422d2:	fldz
  5422d4:	fstp   DWORD PTR [esp]
  5422d7:	push   0x0
  5422d9:	push   0x29d4
  5422de:	call   0x6b2be0
  5422e3:	mov    edx,DWORD PTR [edi+0x54]
  5422e6:	push   eax
  5422e7:	mov    ecx,esi
  5422e9:	call   edx
  5422eb:	and    DWORD PTR [esi+0x434],0xfffffeff
  5422f5:	mov    ecx,esi
  5422f7:	call   0x540e60
  5422fc:	mov    ecx,esi
  5422fe:	call   0x547f40
  542303:	cmp    DWORD PTR [ebx+0x3c],0x12
  542307:	jne    0x542342
  542309:	jmp    0x542331
  54230b:	mov    eax,DWORD PTR [esi+0x18c]
  542311:	test   eax,eax
  542313:	je     0x542331
  542315:	lea    ecx,[esi+0x178]
  54231b:	test   ecx,ecx
  54231d:	je     0x542331
  54231f:	movzx  eax,WORD PTR [eax+0xa]
  542323:	mov    DWORD PTR [esp+0x10],eax
  542327:	fild   DWORD PTR [esp+0x10]
  54232b:	fstp   DWORD PTR [esi+0x194]
  542331:	fld1
  542333:	push   ecx
  542334:	mov    ecx,esi
  542336:	fstp   DWORD PTR [esp]
  542339:	mov    BYTE PTR [esi+0x3a],0x1
  54233d:	call   0x6c3fc0
  542342:	pop    edi
  542343:	pop    esi
  542344:	pop    ebx
  542345:	.byte 0xc2
