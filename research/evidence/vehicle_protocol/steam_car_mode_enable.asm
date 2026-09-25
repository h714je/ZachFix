
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

004dced0 <.text+0xdbed0>:
  4dced0:	push   esi
  4dced1:	push   edi
  4dced2:	call   0x40a320
  4dced7:	mov    esi,DWORD PTR [eax+0x8c57c]
  4dcedd:	mov    ecx,DWORD PTR [esi+0x160]
  4dcee3:	push   0x775a4c
  4dcee8:	call   0x417c40
  4dceed:	mov    edi,DWORD PTR [esi+0x160]
  4dcef3:	mov    ecx,DWORD PTR [edi+0x4]
  4dcef6:	mov    edx,DWORD PTR [ecx+0x78]
  4dcef9:	test   edx,edx
  4dcefb:	je     0x4dceff
  4dcefd:	add    edx,ecx
  4dceff:	mov    edi,DWORD PTR [edi+0x4]
  4dcf02:	xor    ecx,ecx
  4dcf04:	cmp    BYTE PTR [edi+0x37],cl
  4dcf07:	jbe    0x4dcf4a
  4dcf09:	add    edx,0x28
  4dcf0c:	lea    esp,[esp+0x0]
  4dcf10:	movzx  edi,BYTE PTR [edx]
  4dcf13:	cmp    eax,edi
  4dcf15:	je     0x4dcf2f
  4dcf17:	mov    edi,DWORD PTR [esi+0x160]
  4dcf1d:	mov    edi,DWORD PTR [edi+0x4]
  4dcf20:	movzx  edi,BYTE PTR [edi+0x37]
  4dcf24:	inc    ecx
  4dcf25:	add    edx,0x30
  4dcf28:	cmp    ecx,edi
  4dcf2a:	jl     0x4dcf10
  4dcf2c:	pop    edi
  4dcf2d:	pop    esi
  4dcf2e:	ret
  4dcf2f:	or     DWORD PTR [esi+0x434],0x8000
  4dcf39:	mov    ecx,esi
  4dcf3b:	call   0x54e480
  4dcf40:	or     DWORD PTR [esi+0x434],0x8000000
  4dcf4a:	pop    edi
  4dcf4b:	pop    esi
  4dcf4c:	ret
  4dcf4d:	int3
  4dcf4e:	int3
  4dcf4f:	int3
