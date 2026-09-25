
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

005480b0 <.text+0x1470b0>:
  5480b0:	push   0xffffffff
  5480b2:	push   0x766a8b
  5480b7:	mov    eax,fs:0x0
  5480bd:	push   eax
  5480be:	push   ecx
  5480bf:	push   esi
  5480c0:	push   edi
  5480c1:	mov    eax,ds:0xbd5ecc
  5480c6:	xor    eax,esp
  5480c8:	push   eax
  5480c9:	lea    eax,[esp+0x10]
  5480cd:	mov    fs:0x0,eax
  5480d3:	mov    esi,ecx
  5480d5:	mov    eax,DWORD PTR [esi+0x19ac]
  5480db:	test   eax,eax
  5480dd:	je     0x5480f2
  5480df:	push   eax
  5480e0:	call   0x74eb1b
  5480e5:	add    esp,0x4
  5480e8:	mov    DWORD PTR [esi+0x19ac],0x0
  5480f2:	push   0x1c20
  5480f7:	call   0x73a5dc
  5480fc:	mov    edi,eax
  5480fe:	add    esp,0x4
  548101:	mov    DWORD PTR [esp+0xc],edi
  548105:	mov    DWORD PTR [esp+0x18],0x0
  54810d:	test   edi,edi
  54810f:	je     0x54813b
  548111:	push   0x547b40
  548116:	push   0x168
  54811b:	push   0x14
  54811d:	push   edi
  54811e:	call   0x401d90
  548123:	mov    DWORD PTR [esi+0x19ac],edi
  548129:	mov    ecx,DWORD PTR [esp+0x10]
  54812d:	mov    DWORD PTR fs:0x0,ecx
  548134:	pop    ecx
  548135:	pop    edi
  548136:	pop    esi
  548137:	add    esp,0x10
  54813a:	ret
  54813b:	xor    eax,eax
  54813d:	mov    DWORD PTR [esi+0x19ac],eax
  548143:	mov    ecx,DWORD PTR [esp+0x10]
  548147:	mov    DWORD PTR fs:0x0,ecx
  54814e:	pop    ecx
  54814f:	pop    edi
  548150:	pop    esi
  548151:	add    esp,0x10
  548154:	ret
