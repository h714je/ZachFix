
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

004dd7d0 <.text+0xdc7d0>:
  4dd7d0:	push   0x1
  4dd7d2:	mov    eax,0x1000
  4dd7d7:	call   0x4dc2d0
  4dd7dc:	add    esp,0x4
  4dd7df:	test   eax,eax
  4dd7e1:	je     0x4dd8d3
  4dd7e7:	mov    eax,DWORD PTR [esi+0x434]
  4dd7ed:	shr    eax,0x14
  4dd7f0:	test   al,0x1
  4dd7f2:	jne    0x4dd8d3
  4dd7f8:	cmp    DWORD PTR [esi+0x12f4],0x0
  4dd7ff:	jne    0x4dd8d3
  4dd805:	mov    ecx,esi
  4dd807:	call   0x54a700
  4dd80c:	test   al,al
  4dd80e:	je     0x4dd8d3
  4dd814:	fldz
  4dd816:	fcom   DWORD PTR [esi+0x134c]
  4dd81c:	fnstsw ax
  4dd81e:	test   ah,0x1
  4dd821:	jne    0x4dd8d5
  4dd827:	cmp    BYTE PTR ds:0x1476a1e,0x0
  4dd82e:	jne    0x4dd8d5
  4dd834:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dd83a:	fstp   st(0)
  4dd83c:	push   0x40
  4dd83e:	push   ecx
  4dd83f:	mov    ecx,DWORD PTR ds:0xbd7670
  4dd845:	call   0x6c5fd0
  4dd84a:	mov    ecx,eax
  4dd84c:	call   0x4feea0
  4dd851:	test   eax,eax
  4dd853:	je     0x4dd891
  4dd855:	call   0x449480
  4dd85a:	and    DWORD PTR [eax+0x4],0xfffffffb
  4dd85e:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dd864:	mov    ecx,DWORD PTR ds:0xbd7670
  4dd86a:	add    eax,0x4
  4dd86d:	push   0x88
  4dd872:	push   edx
  4dd873:	mov    DWORD PTR ds:0x139277c,0x0
  4dd87d:	call   0x6c5fd0
  4dd882:	mov    ecx,eax
  4dd884:	call   0x528f40
  4dd889:	pop    ebp
  4dd88a:	pop    edi
  4dd88b:	pop    ebx
  4dd88c:	pop    esi
  4dd88d:	add    esp,0x18
  4dd890:	ret
