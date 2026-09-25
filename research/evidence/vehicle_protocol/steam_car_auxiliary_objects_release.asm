
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

00540e60 <.text+0x13fe60>:
  540e60:	push   esi
  540e61:	push   edi
  540e62:	lea    esi,[ecx+0x1fe4]
  540e68:	mov    edi,0x4
  540e6d:	lea    ecx,[ecx+0x0]
  540e70:	mov    eax,DWORD PTR [esi]
  540e72:	mov    ecx,DWORD PTR ds:0xbd7670
  540e78:	push   eax
  540e79:	call   0x6c5fd0
  540e7e:	test   eax,eax
  540e80:	je     0x540e91
  540e82:	mov    edx,DWORD PTR [eax]
  540e84:	mov    ecx,eax
  540e86:	mov    eax,DWORD PTR [edx+0x30]
  540e89:	call   eax
  540e8b:	mov    DWORD PTR [esi],0xffffffff
  540e91:	add    esi,0x4
  540e94:	sub    edi,0x1
  540e97:	jne    0x540e70
  540e99:	pop    edi
  540e9a:	pop    esi
  540e9b:	ret
