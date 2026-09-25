
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

005422a0 <.text+0x1412a0>:
  5422a0:	push   ebx
  5422a1:	mov    ebx,DWORD PTR [esp+0x8]
  5422a5:	mov    eax,DWORD PTR [ebx+0x3c]
  5422a8:	add    eax,0xfffffff1
  5422ab:	push   esi
  5422ac:	mov    esi,ecx
  5422ae:	cmp    eax,0x6
  5422b1:	ja     0x542413
  5422b7:	push   edi
  5422b8:	jmp    DWORD PTR [eax*4+0x542418]
  5422bf:	mov    eax,ds:0x8a9ba4
  5422c4:	mov    ecx,DWORD PTR ds:0xbd7670
  5422ca:	push   eax
  5422cb:	call   0x6c5ad0
  5422d0:	fld    DWORD PTR [eax+0x644]
  5422d6:	fstp   DWORD PTR [esp+0x10]
  5422da:	call   0x4051c0
  5422df:	fld    DWORD PTR [esp+0x10]
  5422e3:	mov    edi,DWORD PTR [esi]
  5422e5:	push   0x0
  5422e7:	push   0x0
  5422e9:	sub    esp,0x8
  5422ec:	fstp   DWORD PTR [esp+0x4]
  5422f0:	mov    ecx,eax
  5422f2:	fldz
  5422f4:	fstp   DWORD PTR [esp]
  5422f7:	push   0x0
  5422f9:	push   0x29d3
  5422fe:	call   0x6b2be0
  542303:	mov    edx,DWORD PTR [edi+0x54]
  542306:	push   eax
  542307:	mov    ecx,esi
  542309:	call   edx
  54230b:	or     DWORD PTR [esi+0x434],0x100
  542315:	mov    ecx,esi
  542317:	call   0x5415f0
  54231c:	mov    ecx,esi
  54231e:	call   0x5480b0
  542323:	fld1
  542325:	push   ecx
  542326:	fstp   DWORD PTR [esp]
  542329:	mov    ecx,esi
  54232b:	mov    BYTE PTR [esi+0x3a],0x0
  54232f:	call   0x6c3ad0
  542334:	pop    edi
  542335:	pop    esi
  542336:	pop    ebx
  542337:	ret    0x4
  54233a:	mov    eax,DWORD PTR [esi+0x18c]
  542340:	test   eax,eax
  542342:	je     0x542412
  542348:	lea    ecx,[esi+0x178]
  54234e:	test   ecx,ecx
  542350:	je     0x542412
  542356:	movzx  eax,WORD PTR [eax+0xa]
  54235a:	mov    DWORD PTR [esp+0x10],eax
  54235e:	pop    edi
  54235f:	fild   DWORD PTR [esp+0xc]
  542363:	fstp   DWORD PTR [esi+0x194]
  542369:	pop    esi
  54236a:	pop    ebx
  54236b:	ret    0x4
  54236e:	mov    ecx,DWORD PTR ds:0x8a9ba4
  542374:	push   ecx
  542375:	mov    ecx,DWORD PTR ds:0xbd7670
  54237b:	call   0x6c5ad0
  542380:	fld    DWORD PTR [eax+0x644]
  542386:	fstp   DWORD PTR [esp+0x10]
  54238a:	call   0x4051c0
  54238f:	fld    DWORD PTR [esp+0x10]
  542393:	mov    edi,DWORD PTR [esi]
  542395:	push   0x0
  542397:	push   0x0
  542399:	sub    esp,0x8
  54239c:	fstp   DWORD PTR [esp+0x4]
  5423a0:	mov    ecx,eax
  5423a2:	fldz
  5423a4:	fstp   DWORD PTR [esp]
  5423a7:	push   0x0
  5423a9:	push   0x29d4
  5423ae:	call   0x6b2be0
  5423b3:	mov    edx,DWORD PTR [edi+0x54]
  5423b6:	push   eax
  5423b7:	mov    ecx,esi
  5423b9:	call   edx
  5423bb:	and    DWORD PTR [esi+0x434],0xfffffeff
  5423c5:	mov    ecx,esi
  5423c7:	call   0x540f30
  5423cc:	mov    ecx,esi
  5423ce:	call   0x548010
  5423d3:	cmp    DWORD PTR [ebx+0x3c],0x12
  5423d7:	jne    0x542412
  5423d9:	jmp    0x542401
  5423db:	mov    eax,DWORD PTR [esi+0x18c]
  5423e1:	test   eax,eax
  5423e3:	je     0x542401
  5423e5:	lea    ecx,[esi+0x178]
  5423eb:	test   ecx,ecx
  5423ed:	je     0x542401
  5423ef:	movzx  eax,WORD PTR [eax+0xa]
  5423f3:	mov    DWORD PTR [esp+0x10],eax
  5423f7:	fild   DWORD PTR [esp+0x10]
  5423fb:	fstp   DWORD PTR [esi+0x194]
  542401:	fld1
  542403:	push   ecx
  542404:	mov    ecx,esi
  542406:	fstp   DWORD PTR [esp]
  542409:	mov    BYTE PTR [esi+0x3a],0x1
  54240d:	call   0x6c3ad0
  542412:	pop    edi
  542413:	pop    esi
  542414:	pop    ebx
  542415:	.byte 0xc2
