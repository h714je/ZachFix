
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

004de6a0 <.text+0xdd6a0>:
  4de6a0:	push   ecx
  4de6a1:	push   ebx
  4de6a2:	push   ebp
  4de6a3:	push   esi
  4de6a4:	push   edi
  4de6a5:	call   0x40a2f0
  4de6aa:	mov    esi,DWORD PTR [eax+0x8c57c]
  4de6b0:	mov    eax,ds:0xbe1ea4
  4de6b5:	test   eax,eax
  4de6b7:	jne    0x4de6d9
  4de6b9:	push   0x1ac
  4de6be:	call   0x404370
  4de6c3:	add    esp,0x4
  4de6c6:	test   eax,eax
  4de6c8:	je     0x4de6d2
  4de6ca:	mov    DWORD PTR [eax],0x7721ac
  4de6d0:	jmp    0x4de6d4
  4de6d2:	xor    eax,eax
  4de6d4:	mov    ds:0xbe1ea4,eax
  4de6d9:	mov    ebp,0x3
  4de6de:	or     DWORD PTR [eax+0x11c],ebp
  4de6e4:	mov    eax,ds:0x8a9ba4
  4de6e9:	mov    ecx,DWORD PTR ds:0xbd7670
  4de6ef:	push   eax
  4de6f0:	call   0x6c5ad0
  4de6f5:	mov    ebx,0x1
  4de6fa:	mov    BYTE PTR [eax+0xa8c],bl
  4de700:	mov    eax,ds:0xbe1ea4
  4de705:	test   eax,eax
  4de707:	jne    0x4de729
  4de709:	push   0x1ac
  4de70e:	call   0x404370
  4de713:	add    esp,0x4
  4de716:	test   eax,eax
  4de718:	je     0x4de722
  4de71a:	mov    DWORD PTR [eax],0x7721ac
  4de720:	jmp    0x4de724
  4de722:	xor    eax,eax
  4de724:	mov    ds:0xbe1ea4,eax
  4de729:	fld    DWORD PTR ds:0x772260
  4de72f:	push   0x0
  4de731:	fstp   DWORD PTR [eax+0x124]
  4de737:	mov    ecx,DWORD PTR ds:0xbe1ea4
  4de73d:	add    ecx,0x1c
  4de740:	push   0x775a54
  4de745:	push   ecx
  4de746:	mov    ecx,esi
  4de748:	call   0x6c3b70
  4de74d:	mov    eax,ds:0xbe1ea4
  4de752:	test   eax,eax
  4de754:	jne    0x4de776
  4de756:	push   0x1ac
  4de75b:	call   0x404370
  4de760:	add    esp,0x4
  4de763:	test   eax,eax
  4de765:	je     0x4de76f
  4de767:	mov    DWORD PTR [eax],0x7721ac
  4de76d:	jmp    0x4de771
  4de76f:	xor    eax,eax
  4de771:	mov    ds:0xbe1ea4,eax
  4de776:	fld    DWORD PTR [eax+0x20]
  4de779:	fadd   QWORD PTR ds:0x7703e0
  4de77f:	fstp   DWORD PTR [eax+0x20]
  4de782:	mov    ecx,DWORD PTR ds:0xbe1ea4
  4de788:	mov    edx,DWORD PTR [ecx+0x1c]
  4de78b:	lea    eax,[ecx+0x1c]
  4de78e:	mov    DWORD PTR [ecx+0xc],edx
  4de791:	mov    edx,DWORD PTR [eax+0x4]
  4de794:	add    ecx,0xc
  4de797:	mov    DWORD PTR [ecx+0x4],edx
  4de79a:	mov    edx,DWORD PTR [eax+0x8]
  4de79d:	mov    DWORD PTR [ecx+0x8],edx
  4de7a0:	mov    eax,DWORD PTR [eax+0xc]
  4de7a3:	mov    DWORD PTR [ecx+0xc],eax
  4de7a6:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de7ac:	mov    edi,DWORD PTR ds:0xbe1ea4
  4de7b2:	push   ecx
  4de7b3:	mov    ecx,DWORD PTR ds:0xbd7670
  4de7b9:	add    edi,0xc
  4de7bc:	call   0x6c5ad0
  4de7c1:	fld    DWORD PTR [eax+0x7c]
  4de7c4:	fadd   QWORD PTR ds:0x770390
  4de7ca:	fstp   DWORD PTR [esp+0x10]
  4de7ce:	fld    DWORD PTR [esp+0x10]
  4de7d2:	fmul   QWORD PTR ds:0x76f640
  4de7d8:	call   0x74ea70
  4de7dd:	mov    ecx,eax
  4de7df:	mov    eax,0xd646fa41
  4de7e4:	imul   ecx
  4de7e6:	sar    edx,0xa
  4de7e9:	mov    eax,edx
  4de7eb:	shr    eax,0x1f
  4de7ee:	add    eax,edx
  4de7f0:	imul   eax,eax,0x188b
  4de7f6:	add    ecx,eax
  4de7f8:	jns    0x4de800
  4de7fa:	add    ecx,0x188b
  4de800:	fld    DWORD PTR [ecx*4+0x8880e0]
  4de807:	fmul   QWORD PTR ds:0x775aa0
  4de80d:	fadd   DWORD PTR [edi]
  4de80f:	fstp   DWORD PTR [edi]
  4de811:	mov    eax,ds:0xbe1ea4
  4de816:	test   eax,eax
  4de818:	jne    0x4de83a
  4de81a:	push   0x1ac
  4de81f:	call   0x404370
  4de824:	add    esp,0x4
  4de827:	test   eax,eax
  4de829:	je     0x4de833
  4de82b:	mov    DWORD PTR [eax],0x7721ac
  4de831:	jmp    0x4de835
  4de833:	xor    eax,eax
  4de835:	mov    ds:0xbe1ea4,eax
  4de83a:	fld    DWORD PTR [eax+0x10]
  4de83d:	fadd   QWORD PTR ds:0x775a98
  4de843:	fstp   DWORD PTR [eax+0x10]
  4de846:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de84c:	mov    edi,DWORD PTR ds:0xbe1ea4
  4de852:	push   ecx
  4de853:	mov    ecx,DWORD PTR ds:0xbd7670
  4de859:	add    edi,0x14
  4de85c:	call   0x6c5ad0
  4de861:	fld    DWORD PTR [eax+0x7c]
  4de864:	fadd   QWORD PTR ds:0x770390
  4de86a:	fstp   DWORD PTR [esp+0x10]
  4de86e:	fld    DWORD PTR [esp+0x10]
  4de872:	fmul   QWORD PTR ds:0x76f640
  4de878:	call   0x74ea70
  4de87d:	mov    ecx,eax
  4de87f:	mov    eax,0xd646fa41
  4de884:	imul   ecx
  4de886:	sar    edx,0xa
  4de889:	mov    eax,edx
  4de88b:	shr    eax,0x1f
  4de88e:	add    eax,edx
  4de890:	imul   eax,eax,0x188b
  4de896:	add    ecx,eax
  4de898:	jns    0x4de8a0
  4de89a:	add    ecx,0x188b
  4de8a0:	fld    DWORD PTR [ecx*4+0x88e310]
  4de8a7:	fmul   QWORD PTR ds:0x775aa0
  4de8ad:	fadd   DWORD PTR [edi]
  4de8af:	fstp   DWORD PTR [edi]
  4de8b1:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de8b7:	push   ecx
  4de8b8:	mov    ecx,DWORD PTR ds:0xbd7670
  4de8be:	call   0x6c5ad0
  4de8c3:	mov    edi,DWORD PTR [eax+0x628]
  4de8c9:	call   0x4051c0
  4de8ce:	push   ebx
  4de8cf:	push   0x2471
  4de8d4:	mov    ecx,eax
  4de8d6:	call   0x6b2be0
  4de8db:	mov    ecx,DWORD PTR ds:0xbd7670
  4de8e1:	cmp    edi,eax
  4de8e3:	jne    0x4dea65
  4de8e9:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de8ef:	push   edx
  4de8f0:	call   0x6c5ad0
  4de8f5:	mov    eax,DWORD PTR [eax+0x1fc]
  4de8fb:	mov    ecx,eax
  4de8fd:	not    ecx
  4de8ff:	test   cl,0x2
  4de902:	je     0x4dee65
  4de908:	test   al,0x10
  4de90a:	je     0x4dee65
  4de910:	call   0x405330
  4de915:	and    DWORD PTR [eax+0x4],0xffefffff
  4de91c:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de922:	mov    ecx,DWORD PTR ds:0xbd7670
  4de928:	add    eax,0x4
  4de92b:	push   0x4000000
  4de930:	push   0x0
  4de932:	push   edx
  4de933:	call   0x6c5ad0
  4de938:	mov    ecx,eax
  4de93a:	call   0x4fd910
  4de93f:	mov    eax,ds:0x8a9ba4
  4de944:	mov    ecx,DWORD PTR ds:0xbd7670
  4de94a:	push   0x4000
  4de94f:	push   0x0
  4de951:	push   eax
  4de952:	call   0x6c5ad0
  4de957:	mov    ecx,eax
  4de959:	call   0x4fd930
  4de95e:	or     eax,edx
  4de960:	je     0x4de9ae
  4de962:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de968:	push   0x4000
  4de96d:	push   0x0
  4de96f:	push   ecx
  4de970:	mov    ecx,DWORD PTR ds:0xbd7670
  4de976:	call   0x6c5ad0
  4de97b:	mov    ecx,eax
  4de97d:	call   0x4fd910
  4de982:	call   0x405330
  4de987:	mov    ecx,eax
  4de989:	call   0x43fbc0
  4de98e:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de994:	mov    ecx,DWORD PTR ds:0xbd7670
  4de99a:	push   edx
  4de99b:	call   0x6c5ad0
  4de9a0:	pop    edi
  4de9a1:	pop    esi
  4de9a2:	pop    ebp
  4de9a3:	mov    ecx,eax
  4de9a5:	pop    ebx
  4de9a6:	add    esp,0x4
  4de9a9:	jmp    0x4dde80
  4de9ae:	mov    eax,DWORD PTR [esi+0x424]
  4de9b4:	sub    eax,0x0
  4de9b7:	je     0x4de9d9
  4de9b9:	sub    eax,ebx
  4de9bb:	jne    0x4dea0b
  4de9bd:	call   0x4051c0
  4de9c2:	push   ebx
  4de9c3:	push   0x258e
  4de9c8:	mov    ecx,eax
  4de9ca:	call   0x6b2be0
  4de9cf:	mov    edi,eax
  4de9d1:	mov    eax,ds:0x8a9ba4
  4de9d6:	push   eax
  4de9d7:	jmp    0x4de9f4
  4de9d9:	call   0x4051c0
  4de9de:	push   ebx
  4de9df:	push   0x258c
  4de9e4:	mov    ecx,eax
  4de9e6:	call   0x6b2be0
  4de9eb:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de9f1:	mov    edi,eax
  4de9f3:	push   ecx
  4de9f4:	mov    ecx,DWORD PTR ds:0xbd7670
  4de9fa:	call   0x6c5ad0
  4de9ff:	mov    DWORD PTR [eax+0x630],ebx
  4dea05:	mov    DWORD PTR [eax+0x628],edi
  4dea0b:	call   0x40a2f0
  4dea10:	mov    DWORD PTR [eax+0x8c5b8],0x10
  4dea1a:	call   0x40a2f0
  4dea1f:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dea25:	mov    ecx,DWORD PTR ds:0xbd7670
  4dea2b:	mov    edi,eax
  4dea2d:	push   edx
  4dea2e:	add    edi,0x8c57c
  4dea34:	call   0x6c5ad0
  4dea39:	mov    eax,ds:0x148132c
  4dea3e:	test   eax,eax
  4dea40:	je     0x4dea4b
  4dea42:	push   edi
  4dea43:	push   0x35
  4dea45:	push   esi
  4dea46:	call   eax
  4dea48:	add    esp,0xc
  4dea4b:	mov    eax,DWORD PTR [esi+0x44]
  4dea4e:	test   eax,eax
  4dea50:	je     0x4dee65
  4dea56:	push   edi
  4dea57:	push   0x35
  4dea59:	push   esi
  4dea5a:	call   eax
  4dea5c:	add    esp,0xc
  4dea5f:	pop    edi
  4dea60:	pop    esi
  4dea61:	pop    ebp
  4dea62:	pop    ebx
  4dea63:	pop    ecx
  4dea64:	ret
  4dea65:	mov    eax,ds:0x8a9ba4
  4dea6a:	push   eax
  4dea6b:	call   0x6c5ad0
  4dea70:	mov    edi,DWORD PTR [eax+0x628]
  4dea76:	call   0x4051c0
  4dea7b:	push   ebx
  4dea7c:	push   0x258b
  4dea81:	mov    ecx,eax
  4dea83:	call   0x6b2be0
  4dea88:	cmp    edi,eax
  4dea8a:	je     0x4dedc7
  4dea90:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dea96:	push   ecx
  4dea97:	mov    ecx,DWORD PTR ds:0xbd7670
  4dea9d:	call   0x6c5ad0
  4deaa2:	mov    edi,DWORD PTR [eax+0x628]
  4deaa8:	call   0x4051c0
  4deaad:	push   ebx
  4deaae:	push   0x258d
  4deab3:	mov    ecx,eax
  4deab5:	call   0x6b2be0
  4deaba:	cmp    edi,eax
  4deabc:	je     0x4dedc7
  4deac2:	mov    edx,DWORD PTR ds:0x8a9ba4
  4deac8:	mov    ecx,DWORD PTR ds:0xbd7670
  4deace:	push   edx
  4deacf:	call   0x6c5ad0
  4dead4:	mov    edi,DWORD PTR [eax+0x628]
  4deada:	call   0x4051c0
  4deadf:	push   ebx
  4deae0:	push   0x258c
  4deae5:	mov    ecx,eax
  4deae7:	call   0x6b2be0
  4deaec:	cmp    edi,eax
  4deaee:	je     0x4dec65
  4deaf4:	mov    eax,ds:0x8a9ba4
  4deaf9:	mov    ecx,DWORD PTR ds:0xbd7670
  4deaff:	push   eax
  4deb00:	call   0x6c5ad0
  4deb05:	mov    edi,DWORD PTR [eax+0x628]
  4deb0b:	call   0x4051c0
  4deb10:	push   ebx
  4deb11:	push   0x258e
  4deb16:	mov    ecx,eax
  4deb18:	call   0x6b2be0
  4deb1d:	cmp    edi,eax
  4deb1f:	je     0x4dec65
  4deb25:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4deb2b:	push   0x2000000
  4deb30:	push   0x0
  4deb32:	push   ecx
  4deb33:	mov    ecx,DWORD PTR ds:0xbd7670
  4deb39:	call   0x6c5ad0
  4deb3e:	mov    ecx,eax
  4deb40:	call   0x4fd930
  4deb45:	or     eax,edx
  4deb47:	jne    0x4dee65
  4deb4d:	mov    edx,DWORD PTR ds:0x8a9ba4
  4deb53:	mov    ecx,DWORD PTR ds:0xbd7670
  4deb59:	push   edx
  4deb5a:	call   0x6c5ad0
  4deb5f:	fldz
  4deb61:	and    DWORD PTR [eax+0x63c],0xfffffffd
  4deb68:	fstp   DWORD PTR [esi+0x13dc]
  4deb6e:	and    DWORD PTR [esi+0x434],0xfffbffe8
  4deb78:	mov    eax,ds:0x8a9ba4
  4deb7d:	mov    ecx,DWORD PTR ds:0xbd7670
  4deb83:	push   0x80
  4deb88:	push   0x0
  4deb8a:	push   eax
  4deb8b:	call   0x6c5ad0
  4deb90:	mov    ecx,eax
  4deb92:	call   0x4fd910
  4deb97:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4deb9d:	push   0x0
  4deb9f:	push   0x400
  4deba4:	push   ecx
  4deba5:	mov    ecx,DWORD PTR ds:0xbd7670
  4debab:	call   0x6c5ad0
  4debb0:	mov    ecx,eax
  4debb2:	call   0x4fd930
  4debb7:	or     eax,edx
  4debb9:	mov    ecx,DWORD PTR ds:0xbd7670
  4debbf:	je     0x4debdb
  4debc1:	mov    edx,DWORD PTR ds:0x8a9ba4
  4debc7:	push   ebx
  4debc8:	push   0x0
  4debca:	push   0x0
  4debcc:	push   edx
  4debcd:	call   0x6c5ad0
  4debd2:	mov    ecx,eax
  4debd4:	call   0x4dcd90
  4debd9:	jmp    0x4dec3f
  4debdb:	mov    eax,ds:0x8a9ba4
  4debe0:	push   0x0
  4debe2:	push   esi
  4debe3:	push   eax
  4debe4:	call   0x6c5ad0
  4debe9:	mov    ecx,eax
  4debeb:	call   0x4dc4a0
  4debf0:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4debf6:	push   0x4000
  4debfb:	push   0x0
  4debfd:	push   ecx
  4debfe:	mov    ecx,DWORD PTR ds:0xbd7670
  4dec04:	call   0x6c5ad0
  4dec09:	mov    ecx,eax
  4dec0b:	call   0x4fd8f0
  4dec10:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dec16:	mov    ecx,DWORD PTR ds:0xbd7670
  4dec1c:	push   edx
  4dec1d:	call   0x6c5ad0
  4dec22:	mov    BYTE PTR [eax+0xa6a],bl
  4dec28:	mov    eax,ds:0x8a9ba4
  4dec2d:	mov    ecx,DWORD PTR ds:0xbd7670
  4dec33:	push   eax
  4dec34:	call   0x6c5ad0
  4dec39:	mov    DWORD PTR [eax+0xa6c],ebp
  4dec3f:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dec45:	push   0x4000000
  4dec4a:	push   0x0
  4dec4c:	push   ecx
  4dec4d:	mov    ecx,DWORD PTR ds:0xbd7670
  4dec53:	call   0x6c5ad0
  4dec58:	mov    ecx,eax
  4dec5a:	call   0x4fd8f0
  4dec5f:	pop    edi
  4dec60:	pop    esi
  4dec61:	pop    ebp
  4dec62:	pop    ebx
  4dec63:	pop    ecx
  4dec64:	ret
  4dec65:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dec6b:	mov    ecx,DWORD PTR ds:0xbd7670
  4dec71:	push   edx
  4dec72:	call   0x6c5ad0
  4dec77:	mov    ecx,DWORD PTR [eax+0xdc]
  4dec7d:	and    ecx,0x4
  4dec80:	xor    eax,eax
  4dec82:	or     eax,ecx
  4dec84:	je     0x4decd1
  4dec86:	mov    eax,ds:0x8a9ba4
  4dec8b:	mov    ecx,DWORD PTR ds:0xbd7670
  4dec91:	push   0x0
  4dec93:	push   0x0
  4dec95:	push   0x0
  4dec97:	push   0x0
  4dec99:	push   0x0
  4dec9b:	push   0x0
  4dec9d:	push   eax
  4dec9e:	call   0x6c5ad0
  4deca3:	mov    ecx,eax
  4deca5:	call   0x6c2390
  4decaa:	mov    ecx,DWORD PTR [esi+0x134]
  4decb0:	and    DWORD PTR [esi+0x130],0xfffffff8
  4decb7:	or     DWORD PTR [esi+0x130],ebp
  4decbd:	and    DWORD PTR [esi+0x130],0xffffefff
  4decc7:	mov    edx,ecx
  4decc9:	mov    eax,edx
  4deccb:	mov    DWORD PTR [esi+0x134],eax
  4decd1:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4decd7:	push   ecx
  4decd8:	mov    ecx,DWORD PTR ds:0xbd7670
  4decde:	call   0x6c5ad0
  4dece3:	mov    eax,DWORD PTR [eax+0x1fc]
  4dece9:	mov    edx,eax
  4deceb:	not    edx
  4deced:	test   dl,0x2
  4decf0:	je     0x4dee65
  4decf6:	test   al,0x10
  4decf8:	je     0x4dee65
  4decfe:	mov    eax,ds:0x8a9ba4
  4ded03:	mov    ecx,DWORD PTR ds:0xbd7670
  4ded09:	push   0x4000000
  4ded0e:	push   0x0
  4ded10:	push   eax
  4ded11:	call   0x6c5ad0
  4ded16:	mov    ecx,eax
  4ded18:	call   0x4fd910
  4ded1d:	call   0x40a2f0
  4ded22:	mov    DWORD PTR [eax+0x8c5b8],0x15
  4ded2c:	call   0x40a2f0
  4ded31:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4ded37:	mov    edi,eax
  4ded39:	push   ecx
  4ded3a:	mov    ecx,DWORD PTR ds:0xbd7670
  4ded40:	add    edi,0x8c57c
  4ded46:	call   0x6c5ad0
  4ded4b:	mov    eax,ds:0x148132c
  4ded50:	test   eax,eax
  4ded52:	je     0x4ded5d
  4ded54:	push   edi
  4ded55:	push   0x35
  4ded57:	push   esi
  4ded58:	call   eax
  4ded5a:	add    esp,0xc
  4ded5d:	mov    eax,DWORD PTR [esi+0x44]
  4ded60:	test   eax,eax
  4ded62:	je     0x4ded6d
  4ded64:	push   edi
  4ded65:	push   0x35
  4ded67:	push   esi
  4ded68:	call   eax
  4ded6a:	add    esp,0xc
  4ded6d:	call   0x405330
  4ded72:	mov    ecx,eax
  4ded74:	call   0x43fc10
  4ded79:	mov    edx,DWORD PTR ds:0x8a9ba4
  4ded7f:	mov    ecx,DWORD PTR ds:0xbd7670
  4ded85:	push   edx
  4ded86:	call   0x6c5ad0
  4ded8b:	mov    ecx,eax
  4ded8d:	call   0x4de2a0
  4ded92:	mov    eax,ds:0x8a9ba4
  4ded97:	mov    ecx,DWORD PTR ds:0xbd7670
  4ded9d:	push   eax
  4ded9e:	call   0x6c5ad0
  4deda3:	mov    BYTE PTR [eax+0xa6a],bl
  4deda9:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dedaf:	push   ecx
  4dedb0:	mov    ecx,DWORD PTR ds:0xbd7670
  4dedb6:	call   0x6c5ad0
  4dedbb:	pop    edi
  4dedbc:	pop    esi
  4dedbd:	mov    DWORD PTR [eax+0xa6c],ebp
  4dedc3:	pop    ebp
  4dedc4:	pop    ebx
  4dedc5:	pop    ecx
  4dedc6:	ret
  4dedc7:	call   0x405330
  4dedcc:	or     DWORD PTR [eax+0x4],0x100000
  4dedd3:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dedd9:	mov    ecx,DWORD PTR ds:0xbd7670
  4deddf:	add    eax,0x4
  4dede2:	push   edx
  4dede3:	call   0x6c5ad0
  4dede8:	mov    eax,DWORD PTR [eax+0x1fc]
  4dedee:	mov    ecx,eax
  4dedf0:	not    ecx
  4dedf2:	test   cl,0x2
  4dedf5:	je     0x4dee65
  4dedf7:	test   al,0x10
  4dedf9:	je     0x4dee65
  4dedfb:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dee01:	mov    ecx,DWORD PTR ds:0xbd7670
  4dee07:	push   0x4000
  4dee0c:	push   0x0
  4dee0e:	push   edx
  4dee0f:	call   0x6c5ad0
  4dee14:	mov    ecx,eax
  4dee16:	call   0x4fd8f0
  4dee1b:	call   0x4051c0
  4dee20:	push   ebx
  4dee21:	push   0x2471
  4dee26:	mov    ecx,eax
  4dee28:	call   0x6b2be0
  4dee2d:	mov    ecx,DWORD PTR ds:0xbd7670
  4dee33:	mov    esi,eax
  4dee35:	mov    eax,ds:0x8a9ba4
  4dee3a:	push   eax
  4dee3b:	call   0x6c5ad0
  4dee40:	mov    DWORD PTR [eax+0x628],esi
  4dee46:	mov    DWORD PTR [eax+0x630],ebx
  4dee4c:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dee52:	push   ecx
  4dee53:	mov    ecx,DWORD PTR ds:0xbd7670
  4dee59:	call   0x6c5ad0
  4dee5e:	mov    BYTE PTR [eax+0xa8c],0x0
  4dee65:	pop    edi
  4dee66:	pop    esi
  4dee67:	pop    ebp
  4dee68:	pop    ebx
  4dee69:	pop    ecx
  4dee6a:	ret
