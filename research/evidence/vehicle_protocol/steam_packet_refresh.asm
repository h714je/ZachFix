
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

00509ec0 <.text+0x108ec0>:
  509ec0:	push   esi
  509ec1:	push   edi
  509ec2:	call   0x40a320
  509ec7:	mov    esi,DWORD PTR [esp+0x10]
  509ecb:	lea    edi,[eax+0x8c57c]
  509ed1:	mov    ecx,0x12
  509ed6:	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  509ed8:	pop    edi
  509ed9:	pop    esi
  509eda:	.byte 0xc2
