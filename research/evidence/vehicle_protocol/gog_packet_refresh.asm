
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

00509f90 <.text+0x108f90>:
  509f90:	push   esi
  509f91:	push   edi
  509f92:	call   0x40a2f0
  509f97:	mov    esi,DWORD PTR [esp+0x10]
  509f9b:	lea    edi,[eax+0x8c57c]
  509fa1:	mov    ecx,0x12
  509fa6:	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  509fa8:	pop    edi
  509fa9:	pop    esi
  509faa:	.byte 0xc2
