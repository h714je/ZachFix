
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

00544b80 <.text+0x143b80>:
  544b80:	sub    esp,0x1d0
  544b86:	mov    eax,ds:0xbd5ecc
  544b8b:	xor    eax,esp
  544b8d:	mov    DWORD PTR [esp+0x1cc],eax
  544b94:	push   esi
  544b95:	push   edi
  544b96:	mov    edi,DWORD PTR [esp+0x1e0]
  544b9d:	mov    esi,ecx
  544b9f:	call   0x54a170
  544ba4:	movzx  eax,BYTE PTR [esp+0x1dc]
  544bac:	add    eax,0xfffffffd
  544baf:	cmp    eax,0x7f
  544bb2:	ja     0x5479ac
  544bb8:	movzx  eax,BYTE PTR [eax+0x547a00]
  544bbf:	push   ebx
  544bc0:	push   ebp
  544bc1:	jmp    DWORD PTR [eax*4+0x5479c8]
