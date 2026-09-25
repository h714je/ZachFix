
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

00548010 <.text+0x147010>:
  548010:	push   esi
  548011:	mov    esi,ecx
  548013:	mov    eax,DWORD PTR [esi+0x19ac]
  548019:	test   eax,eax
  54801b:	je     0x548030
  54801d:	push   eax
  54801e:	call   0x74eb1b
  548023:	add    esp,0x4
  548026:	mov    DWORD PTR [esi+0x19ac],0x0
  548030:	pop    esi
  548031:	ret
