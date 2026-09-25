
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

00547fe0 <.text+0x146fe0>:
  547fe0:	push   0xffffffff
  547fe2:	push   0x766d4b
  547fe7:	mov    eax,fs:0x0
  547fed:	push   eax
  547fee:	push   ecx
  547fef:	push   esi
  547ff0:	push   edi
  547ff1:	mov    eax,ds:0xbd5ecc
  547ff6:	xor    eax,esp
  547ff8:	push   eax
  547ff9:	lea    eax,[esp+0x10]
  547ffd:	mov    fs:0x0,eax
  548003:	mov    esi,ecx
  548005:	mov    eax,DWORD PTR [esi+0x19ac]
  54800b:	test   eax,eax
  54800d:	je     0x548022
  54800f:	push   eax
  548010:	call   0x74ee0b
  548015:	add    esp,0x4
  548018:	mov    DWORD PTR [esi+0x19ac],0x0
  548022:	push   0x1c20
  548027:	call   0x73a8dc
  54802c:	mov    edi,eax
  54802e:	add    esp,0x4
  548031:	mov    DWORD PTR [esp+0xc],edi
  548035:	mov    DWORD PTR [esp+0x18],0x0
  54803d:	test   edi,edi
  54803f:	je     0x54806b
  548041:	push   0x547a70
  548046:	push   0x168
  54804b:	push   0x14
  54804d:	push   edi
  54804e:	call   0x401d90
  548053:	mov    DWORD PTR [esi+0x19ac],edi
  548059:	mov    ecx,DWORD PTR [esp+0x10]
  54805d:	mov    DWORD PTR fs:0x0,ecx
  548064:	pop    ecx
  548065:	pop    edi
  548066:	pop    esi
  548067:	add    esp,0x10
  54806a:	ret
  54806b:	xor    eax,eax
  54806d:	mov    DWORD PTR [esi+0x19ac],eax
  548073:	mov    ecx,DWORD PTR [esp+0x10]
  548077:	mov    DWORD PTR fs:0x0,ecx
  54807e:	pop    ecx
  54807f:	pop    edi
  548080:	pop    esi
  548081:	add    esp,0x10
  548084:	ret
