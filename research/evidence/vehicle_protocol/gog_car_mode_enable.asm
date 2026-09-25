
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

004dcfa0 <.text+0xdbfa0>:
  4dcfa0:	push   esi
  4dcfa1:	push   edi
  4dcfa2:	call   0x40a2f0
  4dcfa7:	mov    esi,DWORD PTR [eax+0x8c57c]
  4dcfad:	mov    ecx,DWORD PTR [esi+0x160]
  4dcfb3:	push   0x775a3c
  4dcfb8:	call   0x417c10
  4dcfbd:	mov    edi,DWORD PTR [esi+0x160]
  4dcfc3:	mov    ecx,DWORD PTR [edi+0x4]
  4dcfc6:	mov    edx,DWORD PTR [ecx+0x78]
  4dcfc9:	test   edx,edx
  4dcfcb:	je     0x4dcfcf
  4dcfcd:	add    edx,ecx
  4dcfcf:	mov    edi,DWORD PTR [edi+0x4]
  4dcfd2:	xor    ecx,ecx
  4dcfd4:	cmp    BYTE PTR [edi+0x37],cl
  4dcfd7:	jbe    0x4dd01a
  4dcfd9:	add    edx,0x28
  4dcfdc:	lea    esp,[esp+0x0]
  4dcfe0:	movzx  edi,BYTE PTR [edx]
  4dcfe3:	cmp    eax,edi
  4dcfe5:	je     0x4dcfff
  4dcfe7:	mov    edi,DWORD PTR [esi+0x160]
  4dcfed:	mov    edi,DWORD PTR [edi+0x4]
  4dcff0:	movzx  edi,BYTE PTR [edi+0x37]
  4dcff4:	inc    ecx
  4dcff5:	add    edx,0x30
  4dcff8:	cmp    ecx,edi
  4dcffa:	jl     0x4dcfe0
  4dcffc:	pop    edi
  4dcffd:	pop    esi
  4dcffe:	ret
  4dcfff:	or     DWORD PTR [esi+0x434],0x8000
  4dd009:	mov    ecx,esi
  4dd00b:	call   0x54e550
  4dd010:	or     DWORD PTR [esi+0x434],0x8000000
  4dd01a:	pop    edi
  4dd01b:	pop    esi
  4dd01c:	ret
  4dd01d:	int3
  4dd01e:	int3
  4dd01f:	int3
