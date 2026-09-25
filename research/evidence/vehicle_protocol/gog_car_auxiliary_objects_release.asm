
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

00540f30 <.text+0x13ff30>:
  540f30:	push   esi
  540f31:	push   edi
  540f32:	lea    esi,[ecx+0x1fe4]
  540f38:	mov    edi,0x4
  540f3d:	lea    ecx,[ecx+0x0]
  540f40:	mov    eax,DWORD PTR [esi]
  540f42:	mov    ecx,DWORD PTR ds:0xbd7670
  540f48:	push   eax
  540f49:	call   0x6c5ad0
  540f4e:	test   eax,eax
  540f50:	je     0x540f61
  540f52:	mov    edx,DWORD PTR [eax]
  540f54:	mov    ecx,eax
  540f56:	mov    eax,DWORD PTR [edx+0x30]
  540f59:	call   eax
  540f5b:	mov    DWORD PTR [esi],0xffffffff
  540f61:	add    esi,0x4
  540f64:	sub    edi,0x1
  540f67:	jne    0x540f40
  540f69:	pop    edi
  540f6a:	pop    esi
  540f6b:	ret
