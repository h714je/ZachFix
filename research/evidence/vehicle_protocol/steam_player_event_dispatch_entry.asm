
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

0052ebc0 <.text+0x12dbc0>:
  52ebc0:	sub    esp,0x318
  52ebc6:	push   ebx
  52ebc7:	mov    ebx,DWORD PTR [esp+0x324]
  52ebce:	movzx  eax,bl
  52ebd1:	push   esi
  52ebd2:	cmp    eax,0x81
  52ebd7:	ja     0x53465e
  52ebdd:	movzx  eax,BYTE PTR [eax+0x5346c8]
  52ebe4:	push   ebp
  52ebe5:	push   edi
  52ebe6:	jmp    DWORD PTR [eax*4+0x534668]
