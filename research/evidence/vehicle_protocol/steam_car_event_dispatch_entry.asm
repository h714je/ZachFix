
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

00544ab0 <.text+0x143ab0>:
  544ab0:	sub    esp,0x1d0
  544ab6:	mov    eax,ds:0xbd5ecc
  544abb:	xor    eax,esp
  544abd:	mov    DWORD PTR [esp+0x1cc],eax
  544ac4:	push   esi
  544ac5:	push   edi
  544ac6:	mov    edi,DWORD PTR [esp+0x1e0]
  544acd:	mov    esi,ecx
  544acf:	call   0x54a0a0
  544ad4:	movzx  eax,BYTE PTR [esp+0x1dc]
  544adc:	add    eax,0xfffffffd
  544adf:	cmp    eax,0x7f
  544ae2:	ja     0x5478dc
  544ae8:	movzx  eax,BYTE PTR [eax+0x547930]
  544aef:	push   ebx
  544af0:	push   ebp
  544af1:	jmp    DWORD PTR [eax*4+0x5478f8]
