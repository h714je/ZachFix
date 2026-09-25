
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

004dc4a0 <.text+0xdb4a0>:
  4dc4a0:	mov    eax,ds:0xbe1ea4
  4dc4a5:	push   ebp
  4dc4a6:	push   edi
  4dc4a7:	mov    ebp,ecx
  4dc4a9:	test   eax,eax
  4dc4ab:	jne    0x4dc4cd
  4dc4ad:	push   0x1ac
  4dc4b2:	call   0x404370
  4dc4b7:	add    esp,0x4
  4dc4ba:	test   eax,eax
  4dc4bc:	je     0x4dc4c6
  4dc4be:	mov    DWORD PTR [eax],0x7721ac
  4dc4c4:	jmp    0x4dc4c8
  4dc4c6:	xor    eax,eax
  4dc4c8:	mov    ds:0xbe1ea4,eax
  4dc4cd:	fldz
  4dc4cf:	mov    edi,DWORD PTR [esp+0xc]
  4dc4d3:	push   esi
  4dc4d4:	fstp   DWORD PTR [eax+0x80]
  4dc4da:	push   edi
  4dc4db:	call   0x405330
  4dc4e0:	mov    ecx,eax
  4dc4e2:	call   0x42d400
  4dc4e7:	call   0x40a2f0
  4dc4ec:	mov    DWORD PTR [eax+0x8c57c],edi
  4dc4f2:	mov    eax,ds:0x8a9ba4
  4dc4f7:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc4fd:	push   eax
  4dc4fe:	call   0x6c5ad0
  4dc503:	fldz
  4dc505:	push   0x0
  4dc507:	fstp   DWORD PTR [eax+0x8d4]
  4dc50d:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc513:	push   0x20
  4dc515:	push   ecx
  4dc516:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc51c:	call   0x6c5ad0
  4dc521:	mov    ecx,eax
  4dc523:	call   0x4fd910
  4dc528:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc52e:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc534:	push   0x0
  4dc536:	push   0x2
  4dc538:	push   edx
  4dc539:	call   0x6c5ad0
  4dc53e:	mov    ecx,eax
  4dc540:	call   0x4fd910
  4dc545:	mov    eax,ds:0x8a9ba4
  4dc54a:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc550:	push   0x0
  4dc552:	push   0x2000
  4dc557:	push   eax
  4dc558:	call   0x6c5ad0
  4dc55d:	mov    ecx,eax
  4dc55f:	call   0x4fd910
  4dc564:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc56a:	push   0x0
  4dc56c:	push   0x4000
  4dc571:	push   ecx
  4dc572:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc578:	call   0x6c5ad0
  4dc57d:	mov    ecx,eax
  4dc57f:	call   0x4fd910
  4dc584:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc58a:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc590:	push   0x0
  4dc592:	push   0x8000
  4dc597:	push   edx
  4dc598:	call   0x6c5ad0
  4dc59d:	mov    ecx,eax
  4dc59f:	call   0x4fd910
  4dc5a4:	mov    eax,ds:0x8a9ba4
  4dc5a9:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc5af:	push   0x0
  4dc5b1:	push   0x10000
  4dc5b6:	push   eax
  4dc5b7:	call   0x6c5ad0
  4dc5bc:	mov    ecx,eax
  4dc5be:	call   0x4fd910
  4dc5c3:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc5c9:	push   0x0
  4dc5cb:	push   0x20000
  4dc5d0:	push   ecx
  4dc5d1:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc5d7:	call   0x6c5ad0
  4dc5dc:	mov    ecx,eax
  4dc5de:	call   0x4fd910
  4dc5e3:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc5e9:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc5ef:	push   0x0
  4dc5f1:	push   0x1000000
  4dc5f6:	push   edx
  4dc5f7:	call   0x6c5ad0
  4dc5fc:	mov    ecx,eax
  4dc5fe:	call   0x4fd910
  4dc603:	push   0x0
  4dc605:	mov    eax,ds:0x8a9ba4
  4dc60a:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc610:	push   0x80
  4dc615:	push   eax
  4dc616:	call   0x6c5ad0
  4dc61b:	mov    ecx,eax
  4dc61d:	call   0x4fd910
  4dc622:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc628:	push   ecx
  4dc629:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc62f:	call   0x6c5ad0
  4dc634:	mov    ecx,eax
  4dc636:	call   0x4ffda0
  4dc63b:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc641:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc647:	push   edx
  4dc648:	call   0x6c5ad0
  4dc64d:	mov    eax,DWORD PTR [eax+0x84c]
  4dc653:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc659:	push   eax
  4dc65a:	call   0x6c5ad0
  4dc65f:	mov    esi,0x40000000
  4dc664:	test   eax,eax
  4dc666:	je     0x4dc69e
  4dc668:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc66e:	push   ecx
  4dc66f:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc675:	call   0x6c5ad0
  4dc67a:	mov    edx,DWORD PTR [eax+0x84c]
  4dc680:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc686:	push   edx
  4dc687:	call   0x6c5ad0
  4dc68c:	mov    ecx,DWORD PTR [eax+0xdc]
  4dc692:	or     DWORD PTR [eax+0xd8],esi
  4dc698:	mov    DWORD PTR [eax+0xdc],ecx
  4dc69e:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc6a4:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc6aa:	push   edx
  4dc6ab:	call   0x6c5ad0
  4dc6b0:	mov    eax,DWORD PTR [eax+0x848]
  4dc6b6:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc6bc:	push   eax
  4dc6bd:	call   0x6c5ad0
  4dc6c2:	test   eax,eax
  4dc6c4:	je     0x4dc6fc
  4dc6c6:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc6cc:	push   ecx
  4dc6cd:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc6d3:	call   0x6c5ad0
  4dc6d8:	mov    edx,DWORD PTR [eax+0x848]
  4dc6de:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc6e4:	push   edx
  4dc6e5:	call   0x6c5ad0
  4dc6ea:	mov    ecx,DWORD PTR [eax+0xdc]
  4dc6f0:	or     DWORD PTR [eax+0xd8],esi
  4dc6f6:	mov    DWORD PTR [eax+0xdc],ecx
  4dc6fc:	mov    eax,ds:0xbe1ea4
  4dc701:	test   eax,eax
  4dc703:	jne    0x4dc725
  4dc705:	push   0x1ac
  4dc70a:	call   0x404370
  4dc70f:	add    esp,0x4
  4dc712:	test   eax,eax
  4dc714:	je     0x4dc71e
  4dc716:	mov    DWORD PTR [eax],0x7721ac
  4dc71c:	jmp    0x4dc720
  4dc71e:	xor    eax,eax
  4dc720:	mov    ds:0xbe1ea4,eax
  4dc725:	fld    DWORD PTR [edi+0x7c]
  4dc728:	lea    esi,[eax+0x70]
  4dc72b:	fstp   DWORD PTR [esi]
  4dc72d:	mov    eax,ds:0xbe1ea4
  4dc732:	test   eax,eax
  4dc734:	jne    0x4dc756
  4dc736:	push   0x1ac
  4dc73b:	call   0x404370
  4dc740:	add    esp,0x4
  4dc743:	test   eax,eax
  4dc745:	je     0x4dc74f
  4dc747:	mov    DWORD PTR [eax],0x7721ac
  4dc74d:	jmp    0x4dc751
  4dc74f:	xor    eax,eax
  4dc751:	mov    ds:0xbe1ea4,eax
  4dc756:	fld    DWORD PTR [esi]
  4dc758:	fstp   DWORD PTR [eax+0x30]
  4dc75b:	mov    eax,DWORD PTR [edi+0x424]
  4dc761:	sub    eax,0x0
  4dc764:	je     0x4dc789
  4dc766:	sub    eax,0x1
  4dc769:	jne    0x4dc7bf
  4dc76b:	call   0x4051c0
  4dc770:	push   0x1
  4dc772:	push   0x258d
  4dc777:	mov    ecx,eax
  4dc779:	call   0x6b2be0
  4dc77e:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc784:	mov    esi,eax
  4dc786:	push   edx
  4dc787:	jmp    0x4dc7a4
  4dc789:	call   0x4051c0
  4dc78e:	push   0x1
  4dc790:	push   0x258b
  4dc795:	mov    ecx,eax
  4dc797:	call   0x6b2be0
  4dc79c:	mov    esi,eax
  4dc79e:	mov    eax,ds:0x8a9ba4
  4dc7a3:	push   eax
  4dc7a4:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc7aa:	call   0x6c5ad0
  4dc7af:	mov    DWORD PTR [eax+0x630],0x1
  4dc7b9:	mov    DWORD PTR [eax+0x628],esi
  4dc7bf:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc7c5:	push   ebx
  4dc7c6:	push   ecx
  4dc7c7:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc7cd:	call   0x6c5ad0
  4dc7d2:	mov    esi,eax
  4dc7d4:	add    esi,0x3b0
  4dc7da:	call   0x40a2f0
  4dc7df:	mov    edx,DWORD PTR [eax+0x8c580]
  4dc7e5:	mov    DWORD PTR [esi],edx
  4dc7e7:	mov    ecx,DWORD PTR [eax+0x8c584]
  4dc7ed:	mov    DWORD PTR [esi+0x4],ecx
  4dc7f0:	mov    edx,DWORD PTR [eax+0x8c588]
  4dc7f6:	mov    DWORD PTR [esi+0x8],edx
  4dc7f9:	mov    eax,DWORD PTR [eax+0x8c58c]
  4dc7ff:	mov    DWORD PTR [esi+0xc],eax
  4dc802:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc808:	push   ecx
  4dc809:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc80f:	call   0x6c5ad0
  4dc814:	mov    edx,DWORD PTR [esi]
  4dc816:	add    eax,0x58
  4dc819:	mov    DWORD PTR [eax],edx
  4dc81b:	mov    ecx,DWORD PTR [esi+0x4]
  4dc81e:	mov    DWORD PTR [eax+0x4],ecx
  4dc821:	mov    edx,DWORD PTR [esi+0x8]
  4dc824:	mov    DWORD PTR [eax+0x8],edx
  4dc827:	mov    ecx,DWORD PTR [esi+0xc]
  4dc82a:	push   0x0
  4dc82c:	mov    DWORD PTR [eax+0xc],ecx
  4dc82f:	push   0x0
  4dc831:	mov    ecx,edi
  4dc833:	call   0x6c3690
  4dc838:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc83e:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc844:	push   eax
  4dc845:	push   edx
  4dc846:	call   0x6c5ad0
  4dc84b:	add    eax,0x78
  4dc84e:	push   eax
  4dc84f:	call   0x6b6b80
  4dc854:	mov    eax,ds:0x8a9ba4
  4dc859:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc85f:	add    esp,0x8
  4dc862:	push   eax
  4dc863:	call   0x6c5ad0
  4dc868:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc86e:	push   ecx
  4dc86f:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc875:	lea    esi,[eax+0x78]
  4dc878:	call   0x6c5ad0
  4dc87d:	fldz
  4dc87f:	mov    edx,DWORD PTR [esi]
  4dc881:	mov    DWORD PTR [eax+0x68],edx
  4dc884:	mov    ecx,DWORD PTR [esi+0x4]
  4dc887:	add    eax,0x68
  4dc88a:	mov    DWORD PTR [eax+0x4],ecx
  4dc88d:	mov    edx,DWORD PTR [esi+0x8]
  4dc890:	mov    DWORD PTR [eax+0x8],edx
  4dc893:	mov    ecx,DWORD PTR [esi+0xc]
  4dc896:	push   ecx
  4dc897:	mov    DWORD PTR [eax+0xc],ecx
  4dc89a:	fstp   DWORD PTR [esp]
  4dc89d:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc8a3:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc8a9:	push   edx
  4dc8aa:	call   0x6c5ad0
  4dc8af:	mov    ecx,eax
  4dc8b1:	call   0x4e32c0
  4dc8b6:	mov    eax,ds:0x8a9ba4
  4dc8bb:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc8c1:	push   eax
  4dc8c2:	call   0x6c5ad0
  4dc8c7:	mov    ecx,DWORD PTR [eax+0xd8]
  4dc8cd:	and    DWORD PTR [eax+0xdc],0xfffffffd
  4dc8d4:	mov    DWORD PTR [eax+0xd8],ecx
  4dc8da:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc8e0:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc8e6:	push   edx
  4dc8e7:	call   0x6c5ad0
  4dc8ec:	and    DWORD PTR [eax+0x638],0xfffffffe
  4dc8f3:	or     DWORD PTR [edi+0x434],0x20000000
  4dc8fd:	xor    ebx,ebx
  4dc8ff:	cmp    BYTE PTR [esp+0x18],bl
  4dc903:	setne  bl
  4dc906:	lea    ebx,[ebx+ebx*1+0xf]
  4dc90a:	call   0x40a2f0
  4dc90f:	mov    DWORD PTR [eax+0x8c5b8],ebx
  4dc915:	call   0x40a2f0
  4dc91a:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc920:	mov    esi,eax
  4dc922:	mov    eax,ds:0x8a9ba4
  4dc927:	push   eax
  4dc928:	add    esi,0x8c57c
  4dc92e:	call   0x6c5ad0
  4dc933:	mov    eax,ds:0x148132c
  4dc938:	pop    ebx
  4dc939:	test   eax,eax
  4dc93b:	je     0x4dc946
  4dc93d:	push   esi
  4dc93e:	push   0x35
  4dc940:	push   edi
  4dc941:	call   eax
  4dc943:	add    esp,0xc
  4dc946:	mov    eax,DWORD PTR [edi+0x44]
  4dc949:	test   eax,eax
  4dc94b:	je     0x4dc956
  4dc94d:	push   esi
  4dc94e:	push   0x35
  4dc950:	push   edi
  4dc951:	call   eax
  4dc953:	add    esp,0xc
  4dc956:	call   0x40a2f0
  4dc95b:	mov    cx,WORD PTR [eax+0x9906c]
  4dc962:	pop    esi
  4dc963:	cmp    cx,WORD PTR [edi+0x2e]
  4dc967:	je     0x4dc973
  4dc969:	or     DWORD PTR [edi+0x434],0x2000
  4dc973:	call   0x40a2f0
  4dc978:	mov    dx,WORD PTR [edi+0x2e]
  4dc97c:	mov    ecx,edi
  4dc97e:	mov    WORD PTR [eax+0x9906c],dx
  4dc985:	call   0x54e3b0
  4dc98a:	push   edi
  4dc98b:	push   0x1
  4dc98d:	call   0x4ada60
  4dc992:	mov    ecx,eax
  4dc994:	call   0x4af140
  4dc999:	mov    eax,ds:0x8a9ba4
  4dc99e:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc9a4:	push   eax
  4dc9a5:	call   0x6c5ad0
  4dc9aa:	and    DWORD PTR [eax+0x638],0xffff7fff
  4dc9b4:	push   edi
  4dc9b5:	call   0x40b240
  4dc9ba:	add    esp,0x4
  4dc9bd:	xor    ecx,ecx
  4dc9bf:	pop    edi
  4dc9c0:	mov    WORD PTR [ebp+0xaaa],cx
  4dc9c7:	pop    ebp
  4dc9c8:	ret    0x8
  4dc9cb:	int3
  4dc9cc:	int3
  4dc9cd:	int3
  4dc9ce:	int3
  4dc9cf:	int3
  4dc9d0:	mov    eax,DWORD PTR [esp+0x8]
  4dc9d4:	sub    esp,0x30
  4dc9d7:	push   esi
  4dc9d8:	mov    esi,ecx
  4dc9da:	cmp    eax,0xffffffff
  4dc9dd:	jne    0x4dc9f2
  4dc9df:	call   0x40a2f0
  4dc9e4:	mov    eax,DWORD PTR [eax+0x8c57c]
  4dc9ea:	push   eax
  4dc9eb:	mov    ecx,esi
  4dc9ed:	call   0x4dbf60
  4dc9f2:	mov    DWORD PTR [esi+0x9f4],eax
  4dc9f8:	cmp    eax,0xffffffff
  4dc9fb:	je     0x4dcb94
  4dca01:	push   ebx
  4dca02:	push   ebp
  4dca03:	push   edi
  4dca04:	push   eax
  4dca05:	lea    eax,[esp+0x14]
  4dca09:	push   eax
  4dca0a:	mov    ecx,esi
  4dca0c:	call   0x460570
  4dca11:	mov    ecx,eax
  4dca13:	call   0x5e1fa0
  4dca18:	mov    ecx,DWORD PTR [esi+0x9f4]
  4dca1e:	push   ecx
  4dca1f:	lea    edx,[esp+0x34]
  4dca23:	push   edx
  4dca24:	mov    ecx,esi
  4dca26:	call   0x460570
  4dca2b:	mov    ecx,eax
  4dca2d:	call   0x5e1ab0
  4dca32:	fld    DWORD PTR [esp+0x10]
  4dca36:	mov    esi,DWORD PTR [esp+0x44]
  4dca3a:	fsub   DWORD PTR [esi+0x58]
  4dca3d:	mov    eax,ds:0x8a9ba4
  4dca42:	mov    ecx,DWORD PTR ds:0xbd7670
  4dca48:	push   eax
  4dca49:	fstp   DWORD PTR [esp+0x24]
  4dca4d:	fld    DWORD PTR [esp+0x18]
  4dca51:	fsub   DWORD PTR [esi+0x5c]
  4dca54:	fstp   DWORD PTR [esp+0x28]
  4dca58:	fld    DWORD PTR [esp+0x1c]
  4dca5c:	fsub   DWORD PTR [esi+0x60]
  4dca5f:	fstp   DWORD PTR [esp+0x2c]
  4dca63:	fld    DWORD PTR [esp+0x20]
  4dca67:	fsub   DWORD PTR [esi+0x64]
  4dca6a:	fstp   DWORD PTR [esp+0x30]
  4dca6e:	call   0x6c5ad0
  4dca73:	fld    DWORD PTR [eax+0x58]
  4dca76:	add    eax,0x58
  4dca79:	fadd   DWORD PTR [esp+0x20]
  4dca7d:	fstp   DWORD PTR [eax]
  4dca7f:	fld    DWORD PTR [eax+0x4]
  4dca82:	fadd   DWORD PTR [esp+0x24]
  4dca86:	fstp   DWORD PTR [eax+0x4]
  4dca89:	fld    DWORD PTR [eax+0x8]
  4dca8c:	fadd   DWORD PTR [esp+0x28]
  4dca90:	fstp   DWORD PTR [eax+0x8]
  4dca93:	fld    DWORD PTR [eax+0xc]
  4dca96:	fadd   DWORD PTR [esp+0x2c]
  4dca9a:	fstp   DWORD PTR [eax+0xc]
  4dca9d:	mov    ecx,DWORD PTR [esp+0x10]
  4dcaa1:	mov    edx,DWORD PTR [esp+0x14]
  4dcaa5:	fld    DWORD PTR [esp+0x30]
  4dcaa9:	mov    eax,DWORD PTR [esp+0x18]
  4dcaad:	mov    DWORD PTR [esi+0x58],ecx
  4dcab0:	mov    ecx,DWORD PTR [esp+0x1c]
  4dcab4:	mov    DWORD PTR [esi+0x5c],edx
  4dcab7:	mov    DWORD PTR [esi+0x60],eax
  4dcaba:	mov    DWORD PTR [esi+0x64],ecx
  4dcabd:	fsub   DWORD PTR [esi+0x78]
  4dcac0:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dcac6:	mov    ecx,DWORD PTR ds:0xbd7670
  4dcacc:	fstp   DWORD PTR [esp+0x20]
  4dcad0:	push   edx
  4dcad1:	fld    DWORD PTR [esp+0x38]
  4dcad5:	fsub   DWORD PTR [esi+0x7c]
  4dcad8:	fstp   DWORD PTR [esp+0x28]
  4dcadc:	fld    DWORD PTR [esp+0x3c]
  4dcae0:	fsub   DWORD PTR [esi+0x80]
  4dcae6:	fstp   DWORD PTR [esp+0x2c]
  4dcaea:	fld    DWORD PTR [esp+0x40]
  4dcaee:	fsub   DWORD PTR [esi+0x84]
  4dcaf4:	fstp   DWORD PTR [esp+0x30]
  4dcaf8:	call   0x6c5ad0
  4dcafd:	fld    DWORD PTR [eax+0x68]
  4dcb00:	add    eax,0x68
  4dcb03:	fadd   DWORD PTR [esp+0x20]
  4dcb07:	fstp   DWORD PTR [eax]
  4dcb09:	fld    DWORD PTR [eax+0x4]
  4dcb0c:	fadd   DWORD PTR [esp+0x24]
  4dcb10:	fstp   DWORD PTR [eax+0x4]
  4dcb13:	fld    DWORD PTR [eax+0x8]
  4dcb16:	fadd   DWORD PTR [esp+0x28]
  4dcb1a:	fstp   DWORD PTR [eax+0x8]
  4dcb1d:	fld    DWORD PTR [eax+0xc]
  4dcb20:	fadd   DWORD PTR [esp+0x2c]
  4dcb24:	fstp   DWORD PTR [eax+0xc]
  4dcb27:	mov    eax,ds:0x8a9ba4
  4dcb2c:	mov    ecx,DWORD PTR ds:0xbd7670
  4dcb32:	push   eax
  4dcb33:	call   0x6c5ad0
  4dcb38:	mov    ecx,DWORD PTR [eax+0x74]
  4dcb3b:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dcb41:	mov    edi,DWORD PTR [eax+0x68]
  4dcb44:	mov    ebx,DWORD PTR [eax+0x6c]
  4dcb47:	mov    ebp,DWORD PTR [eax+0x70]
  4dcb4a:	add    eax,0x68
  4dcb4d:	mov    DWORD PTR [esp+0x2c],ecx
  4dcb51:	mov    ecx,DWORD PTR ds:0xbd7670
  4dcb57:	push   edx
  4dcb58:	call   0x6c5ad0
  4dcb5d:	mov    ecx,DWORD PTR [esp+0x2c]
  4dcb61:	mov    DWORD PTR [eax+0x78],edi
  4dcb64:	mov    DWORD PTR [eax+0x7c],ebx
  4dcb67:	mov    DWORD PTR [eax+0x80],ebp
  4dcb6d:	add    eax,0x78
  4dcb70:	mov    DWORD PTR [eax+0xc],ecx
  4dcb73:	mov    edx,DWORD PTR [esi+0x68]
  4dcb76:	mov    eax,DWORD PTR [esi+0x6c]
  4dcb79:	mov    ecx,DWORD PTR [esi+0x70]
  4dcb7c:	mov    DWORD PTR [esi+0x78],edx
  4dcb7f:	mov    edx,DWORD PTR [esi+0x74]
  4dcb82:	pop    edi
  4dcb83:	mov    DWORD PTR [esi+0x7c],eax
  4dcb86:	pop    ebp
  4dcb87:	mov    DWORD PTR [esi+0x80],ecx
  4dcb8d:	mov    DWORD PTR [esi+0x84],edx
  4dcb93:	pop    ebx
  4dcb94:	pop    esi
  4dcb95:	add    esp,0x30
  4dcb98:	ret    0x8
  4dcb9b:	int3
  4dcb9c:	int3
  4dcb9d:	int3
  4dcb9e:	int3
  4dcb9f:	int3
