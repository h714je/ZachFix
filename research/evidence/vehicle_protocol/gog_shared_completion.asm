
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

004f56f0 <.text+0xf46f0>:
  4f56f0:	mov    eax,ds:0xbe1ea4
  4f56f5:	sub    esp,0x8
  4f56f8:	test   eax,eax
  4f56fa:	jne    0x4f571c
  4f56fc:	push   0x1ac
  4f5701:	call   0x404370
  4f5706:	add    esp,0x4
  4f5709:	test   eax,eax
  4f570b:	je     0x4f5715
  4f570d:	mov    DWORD PTR [eax],0x7721ac
  4f5713:	jmp    0x4f5717
  4f5715:	xor    eax,eax
  4f5717:	mov    ds:0xbe1ea4,eax
  4f571c:	or     DWORD PTR [eax+0x11c],0x3
  4f5723:	mov    eax,ds:0x13928a4
  4f5728:	sub    eax,0x0
  4f572b:	push   esi
  4f572c:	je     0x4f58f6
  4f5732:	sub    eax,0x1
  4f5735:	je     0x4f58d9
  4f573b:	sub    eax,0x1
  4f573e:	jne    0x4f5be1
  4f5744:	mov    eax,ds:0x8a9ba4
  4f5749:	mov    ecx,DWORD PTR ds:0xbd7670
  4f574f:	push   ebx
  4f5750:	push   edi
  4f5751:	push   eax
  4f5752:	call   0x6c5ad0
  4f5757:	mov    eax,DWORD PTR [eax+0x654]
  4f575d:	add    eax,0xffffffbd
  4f5760:	cmp    eax,0x25
  4f5763:	ja     0x4f57fc
  4f5769:	movzx  ecx,BYTE PTR [eax+0x4f5c20]
  4f5770:	jmp    DWORD PTR [ecx*4+0x4f5be8]
  4f5777:	mov    esi,0x1d
  4f577c:	lea    edi,[esi+0x20]
  4f577f:	jmp    0x4f5804
  4f5784:	mov    esi,0x1d
  4f5789:	lea    edi,[esi+0x36]
  4f578c:	jmp    0x4f5804
  4f578e:	mov    esi,0x1d
  4f5793:	lea    edi,[esi+0x37]
  4f5796:	jmp    0x4f5804
  4f5798:	mov    esi,0x1d
  4f579d:	lea    edi,[esi+0x38]
  4f57a0:	jmp    0x4f5804
  4f57a2:	mov    esi,0x1d
  4f57a7:	lea    edi,[esi+0x39]
  4f57aa:	jmp    0x4f5804
  4f57ac:	mov    esi,0x1d
  4f57b1:	lea    edi,[esi+0x3a]
  4f57b4:	jmp    0x4f5804
  4f57b6:	mov    esi,0x1d
  4f57bb:	lea    edi,[esi+0x3b]
  4f57be:	jmp    0x4f5804
  4f57c0:	mov    esi,0x1d
  4f57c5:	lea    edi,[esi+0x3c]
  4f57c8:	jmp    0x4f5804
  4f57ca:	mov    esi,0x1d
  4f57cf:	lea    edi,[esi+0x3d]
  4f57d2:	jmp    0x4f5804
  4f57d4:	mov    esi,0x1d
  4f57d9:	lea    edi,[esi+0x3e]
  4f57dc:	jmp    0x4f5804
  4f57de:	mov    esi,0x1d
  4f57e3:	lea    edi,[esi+0x2e]
  4f57e6:	jmp    0x4f5804
  4f57e8:	mov    esi,0x1d
  4f57ed:	lea    edi,[esi+0x21]
  4f57f0:	jmp    0x4f5804
  4f57f2:	mov    esi,0x1d
  4f57f7:	lea    edi,[esi+0x3f]
  4f57fa:	jmp    0x4f5804
  4f57fc:	mov    esi,DWORD PTR [esp+0xc]
  4f5800:	mov    edi,DWORD PTR [esp+0xc]
  4f5804:	call   0x40a2f0
  4f5809:	mov    DWORD PTR [eax+0x8c5b8],esi
  4f580f:	call   0x40a2f0
  4f5814:	mov    esi,eax
  4f5816:	call   0x40a2f0
  4f581b:	mov    edx,DWORD PTR ds:0x8a9ba4
  4f5821:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5827:	lea    ebx,[esi+0x8c57c]
  4f582d:	mov    esi,DWORD PTR [eax+0x8c57c]
  4f5833:	push   edx
  4f5834:	call   0x6c5ad0
  4f5839:	mov    eax,ds:0x148132c
  4f583e:	test   eax,eax
  4f5840:	je     0x4f584a
  4f5842:	push   ebx
  4f5843:	push   edi
  4f5844:	push   esi
  4f5845:	call   eax
  4f5847:	add    esp,0xc
  4f584a:	mov    eax,DWORD PTR [esi+0x44]
  4f584d:	test   eax,eax
  4f584f:	je     0x4f5859
  4f5851:	push   ebx
  4f5852:	push   edi
  4f5853:	push   esi
  4f5854:	call   eax
  4f5856:	add    esp,0xc
  4f5859:	mov    eax,ds:0xbe1ea4
  4f585e:	pop    edi
  4f585f:	pop    ebx
  4f5860:	test   eax,eax
  4f5862:	jne    0x4f5884
  4f5864:	push   0x1ac
  4f5869:	call   0x404370
  4f586e:	add    esp,0x4
  4f5871:	test   eax,eax
  4f5873:	je     0x4f587d
  4f5875:	mov    DWORD PTR [eax],0x7721ac
  4f587b:	jmp    0x4f587f
  4f587d:	xor    eax,eax
  4f587f:	mov    ds:0xbe1ea4,eax
  4f5884:	and    DWORD PTR [eax+0x11c],0xfffffffc
  4f588b:	mov    eax,ds:0xbe1ea4
  4f5890:	or     DWORD PTR [eax+0x11c],0x4
  4f5897:	mov    eax,ds:0x8a9ba4
  4f589c:	mov    ecx,DWORD PTR ds:0xbd7670
  4f58a2:	push   eax
  4f58a3:	call   0x6c5ad0
  4f58a8:	or     DWORD PTR [eax+0x638],0x1
  4f58af:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4f58b5:	push   0x0
  4f58b7:	push   ecx
  4f58b8:	mov    ecx,DWORD PTR ds:0xbd7670
  4f58be:	call   0x6c5ad0
  4f58c3:	mov    ecx,eax
  4f58c5:	call   0x529010
  4f58ca:	mov    DWORD PTR ds:0x13928a4,0x0
  4f58d4:	pop    esi
  4f58d5:	add    esp,0x8
  4f58d8:	ret
  4f58d9:	call   0x40a2f0
  4f58de:	cmp    DWORD PTR [eax+0x8c5b8],0x20
  4f58e5:	jne    0x4f5be1
  4f58eb:	inc    DWORD PTR ds:0x13928a4
  4f58f1:	pop    esi
  4f58f2:	add    esp,0x8
  4f58f5:	ret
  4f58f6:	mov    edx,DWORD PTR ds:0x8a9ba4
  4f58fc:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5902:	push   edx
  4f5903:	call   0x6c5ad0
  4f5908:	lea    esi,[eax+0x6c]
  4f590b:	call   0x40a2f0
  4f5910:	fld    DWORD PTR [eax+0x8c5a0]
  4f5916:	fstp   DWORD PTR [esp+0x4]
  4f591a:	fld    DWORD PTR [esp+0x4]
  4f591e:	fstp   DWORD PTR [esi]
  4f5920:	mov    eax,ds:0x8a9ba4
  4f5925:	mov    ecx,DWORD PTR ds:0xbd7670
  4f592b:	push   eax
  4f592c:	call   0x6c5ad0
  4f5931:	fld    DWORD PTR [esp+0x4]
  4f5935:	fstp   DWORD PTR [eax+0x7c]
  4f5938:	call   0x40a2f0
  4f593d:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4f5943:	push   ecx
  4f5944:	mov    ecx,DWORD PTR ds:0xbd7670
  4f594a:	lea    esi,[eax+0x8c580]
  4f5950:	call   0x6c5ad0
  4f5955:	mov    edx,DWORD PTR [esi]
  4f5957:	mov    DWORD PTR [eax+0x3b0],edx
  4f595d:	mov    ecx,DWORD PTR [esi+0x4]
  4f5960:	add    eax,0x3b0
  4f5965:	mov    DWORD PTR [eax+0x4],ecx
  4f5968:	mov    edx,DWORD PTR [esi+0x8]
  4f596b:	mov    DWORD PTR [eax+0x8],edx
  4f596e:	mov    ecx,DWORD PTR [esi+0xc]
  4f5971:	mov    DWORD PTR [eax+0xc],ecx
  4f5974:	mov    edx,DWORD PTR ds:0x8a9ba4
  4f597a:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5980:	push   edx
  4f5981:	call   0x6c5ad0
  4f5986:	fld    DWORD PTR [eax+0x3b0]
  4f598c:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5992:	fstp   QWORD PTR [esp+0x4]
  4f5996:	lea    esi,[eax+0x3b0]
  4f599c:	mov    eax,ds:0x8a9ba4
  4f59a1:	push   eax
  4f59a2:	call   0x6c5ad0
  4f59a7:	fld    DWORD PTR [eax+0x7c]
  4f59aa:	push   ecx
  4f59ab:	fstp   DWORD PTR [esp]
  4f59ae:	call   0x409ef0
  4f59b3:	fadd   st(0),st
  4f59b5:	add    esp,0x4
  4f59b8:	fsubr  QWORD PTR [esp+0x4]
  4f59bc:	fstp   DWORD PTR [esi]
  4f59be:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4f59c4:	push   ecx
  4f59c5:	mov    ecx,DWORD PTR ds:0xbd7670
  4f59cb:	call   0x6c5ad0
  4f59d0:	fld    DWORD PTR [eax+0x3b8]
  4f59d6:	mov    edx,DWORD PTR ds:0x8a9ba4
  4f59dc:	fstp   QWORD PTR [esp+0x4]
  4f59e0:	mov    ecx,DWORD PTR ds:0xbd7670
  4f59e6:	lea    esi,[eax+0x3b8]
  4f59ec:	push   edx
  4f59ed:	call   0x6c5ad0
  4f59f2:	fld    DWORD PTR [eax+0x7c]
  4f59f5:	push   ecx
  4f59f6:	fstp   DWORD PTR [esp]
  4f59f9:	call   0x409f50
  4f59fe:	fadd   st(0),st
  4f5a00:	add    esp,0x4
  4f5a03:	fsubr  QWORD PTR [esp+0x4]
  4f5a07:	fstp   DWORD PTR [esi]
  4f5a09:	mov    eax,ds:0x8a9ba4
  4f5a0e:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5a14:	push   eax
  4f5a15:	call   0x6c5ad0
  4f5a1a:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4f5a20:	push   ecx
  4f5a21:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5a27:	lea    esi,[eax+0x3b0]
  4f5a2d:	call   0x6c5ad0
  4f5a32:	mov    edx,DWORD PTR [esi]
  4f5a34:	add    eax,0x58
  4f5a37:	mov    DWORD PTR [eax],edx
  4f5a39:	mov    ecx,DWORD PTR [esi+0x4]
  4f5a3c:	mov    DWORD PTR [eax+0x4],ecx
  4f5a3f:	mov    edx,DWORD PTR [esi+0x8]
  4f5a42:	mov    DWORD PTR [eax+0x8],edx
  4f5a45:	mov    ecx,DWORD PTR [esi+0xc]
  4f5a48:	mov    DWORD PTR [eax+0xc],ecx
  4f5a4b:	mov    edx,DWORD PTR ds:0x8a9ba4
  4f5a51:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5a57:	push   edx
  4f5a58:	call   0x6c5ad0
  4f5a5d:	mov    ecx,eax
  4f5a5f:	call   0x4e31e0
  4f5a64:	mov    eax,ds:0x8a9ba4
  4f5a69:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5a6f:	push   eax
  4f5a70:	call   0x6c5ad0
  4f5a75:	fldz
  4f5a77:	fstp   DWORD PTR [eax+0x644]
  4f5a7d:	call   0x520930
  4f5a82:	mov    eax,ds:0xbe1ea4
  4f5a87:	test   eax,eax
  4f5a89:	jne    0x4f5aab
  4f5a8b:	push   0x1ac
  4f5a90:	call   0x404370
  4f5a95:	add    esp,0x4
  4f5a98:	test   eax,eax
  4f5a9a:	je     0x4f5aa4
  4f5a9c:	mov    DWORD PTR [eax],0x7721ac
  4f5aa2:	jmp    0x4f5aa6
  4f5aa4:	xor    eax,eax
  4f5aa6:	mov    ds:0xbe1ea4,eax
  4f5aab:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4f5ab1:	push   ecx
  4f5ab2:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5ab8:	mov    esi,eax
  4f5aba:	call   0x6c5ad0
  4f5abf:	mov    edx,DWORD PTR [eax+0x58]
  4f5ac2:	add    eax,0x58
  4f5ac5:	mov    DWORD PTR [esi+0x1c],edx
  4f5ac8:	mov    edx,DWORD PTR [eax+0x4]
  4f5acb:	lea    ecx,[esi+0x1c]
  4f5ace:	mov    DWORD PTR [ecx+0x4],edx
  4f5ad1:	mov    edx,DWORD PTR [eax+0x8]
  4f5ad4:	mov    DWORD PTR [ecx+0x8],edx
  4f5ad7:	mov    eax,DWORD PTR [eax+0xc]
  4f5ada:	mov    DWORD PTR [ecx+0xc],eax
  4f5add:	mov    eax,ds:0xbe1ea4
  4f5ae2:	test   eax,eax
  4f5ae4:	jne    0x4f5b06
  4f5ae6:	push   0x1ac
  4f5aeb:	call   0x404370
  4f5af0:	add    esp,0x4
  4f5af3:	test   eax,eax
  4f5af5:	je     0x4f5aff
  4f5af7:	mov    DWORD PTR [eax],0x7721ac
  4f5afd:	jmp    0x4f5b01
  4f5aff:	xor    eax,eax
  4f5b01:	mov    ds:0xbe1ea4,eax
  4f5b06:	fld    DWORD PTR [eax+0x20]
  4f5b09:	fadd   QWORD PTR ds:0x773738
  4f5b0f:	fstp   DWORD PTR [eax+0x20]
  4f5b12:	mov    ecx,DWORD PTR ds:0xbe1ea4
  4f5b18:	mov    edx,DWORD PTR [ecx+0x1c]
  4f5b1b:	lea    eax,[ecx+0x1c]
  4f5b1e:	mov    DWORD PTR [ecx+0xc],edx
  4f5b21:	mov    edx,DWORD PTR [eax+0x4]
  4f5b24:	add    ecx,0xc
  4f5b27:	mov    DWORD PTR [ecx+0x4],edx
  4f5b2a:	mov    edx,DWORD PTR [eax+0x8]
  4f5b2d:	mov    DWORD PTR [ecx+0x8],edx
  4f5b30:	mov    eax,DWORD PTR [eax+0xc]
  4f5b33:	mov    DWORD PTR [ecx+0xc],eax
  4f5b36:	mov    esi,DWORD PTR ds:0xbe1ea4
  4f5b3c:	add    esi,0xc
  4f5b3f:	call   0x40a2f0
  4f5b44:	fld    DWORD PTR [eax+0x8c5a0]
  4f5b4a:	fadd   QWORD PTR ds:0x76f630
  4f5b50:	push   ecx
  4f5b51:	fstp   DWORD PTR [esp+0x8]
  4f5b55:	fld    DWORD PTR [esp+0x8]
  4f5b59:	fstp   DWORD PTR [esp]
  4f5b5c:	call   0x409ef0
  4f5b61:	fmul   QWORD PTR ds:0x773738
  4f5b67:	add    esp,0x4
  4f5b6a:	fadd   DWORD PTR [esi]
  4f5b6c:	fstp   DWORD PTR [esi]
  4f5b6e:	mov    eax,ds:0xbe1ea4
  4f5b73:	test   eax,eax
  4f5b75:	jne    0x4f5b97
  4f5b77:	push   0x1ac
  4f5b7c:	call   0x404370
  4f5b81:	add    esp,0x4
  4f5b84:	test   eax,eax
  4f5b86:	je     0x4f5b90
  4f5b88:	mov    DWORD PTR [eax],0x7721ac
  4f5b8e:	jmp    0x4f5b92
  4f5b90:	xor    eax,eax
  4f5b92:	mov    ds:0xbe1ea4,eax
  4f5b97:	fld    DWORD PTR [eax+0x10]
  4f5b9a:	fadd   QWORD PTR ds:0x7722b8
  4f5ba0:	fstp   DWORD PTR [eax+0x10]
  4f5ba3:	mov    esi,DWORD PTR ds:0xbe1ea4
  4f5ba9:	add    esi,0x14
  4f5bac:	call   0x40a2f0
  4f5bb1:	fld    DWORD PTR [eax+0x8c5a0]
  4f5bb7:	fadd   QWORD PTR ds:0x76f630
  4f5bbd:	push   ecx
  4f5bbe:	fstp   DWORD PTR [esp+0x8]
  4f5bc2:	fld    DWORD PTR [esp+0x8]
  4f5bc6:	fstp   DWORD PTR [esp]
  4f5bc9:	call   0x409f50
  4f5bce:	fmul   QWORD PTR ds:0x773738
  4f5bd4:	add    esp,0x4
  4f5bd7:	fadd   DWORD PTR [esi]
  4f5bd9:	fstp   DWORD PTR [esi]
  4f5bdb:	inc    DWORD PTR ds:0x13928a4
  4f5be1:	pop    esi
  4f5be2:	add    esp,0x8
  4f5be5:	ret
