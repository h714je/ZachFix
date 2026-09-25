
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

00547f40 <.text+0x146f40>:
  547f40:	push   esi
  547f41:	mov    esi,ecx
  547f43:	mov    eax,DWORD PTR [esi+0x19ac]
  547f49:	test   eax,eax
  547f4b:	je     0x547f60
  547f4d:	push   eax
  547f4e:	call   0x74ee0b
  547f53:	add    esp,0x4
  547f56:	mov    DWORD PTR [esi+0x19ac],0x0
  547f60:	pop    esi
  547f61:	ret
