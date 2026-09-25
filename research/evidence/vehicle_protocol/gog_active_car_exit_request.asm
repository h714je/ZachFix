
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

004dd8a0 <.text+0xdc8a0>:
  4dd8a0:	push   0x1
  4dd8a2:	mov    eax,0x1000
  4dd8a7:	call   0x4dc3a0
  4dd8ac:	add    esp,0x4
  4dd8af:	test   eax,eax
  4dd8b1:	je     0x4dd9a3
  4dd8b7:	mov    eax,DWORD PTR [esi+0x434]
  4dd8bd:	shr    eax,0x14
  4dd8c0:	test   al,0x1
  4dd8c2:	jne    0x4dd9a3
  4dd8c8:	cmp    DWORD PTR [esi+0x12f4],0x0
  4dd8cf:	jne    0x4dd9a3
  4dd8d5:	mov    ecx,esi
  4dd8d7:	call   0x54a7d0
  4dd8dc:	test   al,al
  4dd8de:	je     0x4dd9a3
  4dd8e4:	fldz
  4dd8e6:	fcom   DWORD PTR [esi+0x134c]
  4dd8ec:	fnstsw ax
  4dd8ee:	test   ah,0x1
  4dd8f1:	jne    0x4dd9a5
  4dd8f7:	cmp    BYTE PTR ds:0x1476a1e,0x0
  4dd8fe:	jne    0x4dd9a5
  4dd904:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dd90a:	fstp   st(0)
  4dd90c:	push   0x40
  4dd90e:	push   ecx
  4dd90f:	mov    ecx,DWORD PTR ds:0xbd7670
  4dd915:	call   0x6c5ad0
  4dd91a:	mov    ecx,eax
  4dd91c:	call   0x4fef70
  4dd921:	test   eax,eax
  4dd923:	je     0x4dd961
  4dd925:	call   0x4494b0
  4dd92a:	and    DWORD PTR [eax+0x4],0xfffffffb
  4dd92e:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dd934:	mov    ecx,DWORD PTR ds:0xbd7670
  4dd93a:	add    eax,0x4
  4dd93d:	push   0x88
  4dd942:	push   edx
  4dd943:	mov    DWORD PTR ds:0x139277c,0x0
  4dd94d:	call   0x6c5ad0
  4dd952:	mov    ecx,eax
  4dd954:	call   0x529010
  4dd959:	pop    ebp
  4dd95a:	pop    edi
  4dd95b:	pop    ebx
  4dd95c:	pop    esi
  4dd95d:	add    esp,0x18
  4dd960:	ret
