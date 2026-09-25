
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

004dc3d0 <.text+0xdb3d0>:
  4dc3d0:	mov    eax,ds:0xbe1ea4
  4dc3d5:	push   ebp
  4dc3d6:	push   edi
  4dc3d7:	mov    ebp,ecx
  4dc3d9:	test   eax,eax
  4dc3db:	jne    0x4dc3fd
  4dc3dd:	push   0x1ac
  4dc3e2:	call   0x404390
  4dc3e7:	add    esp,0x4
  4dc3ea:	test   eax,eax
  4dc3ec:	je     0x4dc3f6
  4dc3ee:	mov    DWORD PTR [eax],0x7721bc
  4dc3f4:	jmp    0x4dc3f8
  4dc3f6:	xor    eax,eax
  4dc3f8:	mov    ds:0xbe1ea4,eax
  4dc3fd:	fldz
  4dc3ff:	mov    edi,DWORD PTR [esp+0xc]
  4dc403:	push   esi
  4dc404:	fstp   DWORD PTR [eax+0x80]
  4dc40a:	push   edi
  4dc40b:	call   0x405360
  4dc410:	mov    ecx,eax
  4dc412:	call   0x42d380
  4dc417:	call   0x40a320
  4dc41c:	mov    DWORD PTR [eax+0x8c57c],edi
  4dc422:	mov    eax,ds:0x8a9ba4
  4dc427:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc42d:	push   eax
  4dc42e:	call   0x6c5fd0
  4dc433:	fldz
  4dc435:	push   0x0
  4dc437:	fstp   DWORD PTR [eax+0x8d4]
  4dc43d:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc443:	push   0x20
  4dc445:	push   ecx
  4dc446:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc44c:	call   0x6c5fd0
  4dc451:	mov    ecx,eax
  4dc453:	call   0x4fd840
  4dc458:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc45e:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc464:	push   0x0
  4dc466:	push   0x2
  4dc468:	push   edx
  4dc469:	call   0x6c5fd0
  4dc46e:	mov    ecx,eax
  4dc470:	call   0x4fd840
  4dc475:	mov    eax,ds:0x8a9ba4
  4dc47a:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc480:	push   0x0
  4dc482:	push   0x2000
  4dc487:	push   eax
  4dc488:	call   0x6c5fd0
  4dc48d:	mov    ecx,eax
  4dc48f:	call   0x4fd840
  4dc494:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc49a:	push   0x0
  4dc49c:	push   0x4000
  4dc4a1:	push   ecx
  4dc4a2:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc4a8:	call   0x6c5fd0
  4dc4ad:	mov    ecx,eax
  4dc4af:	call   0x4fd840
  4dc4b4:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc4ba:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc4c0:	push   0x0
  4dc4c2:	push   0x8000
  4dc4c7:	push   edx
  4dc4c8:	call   0x6c5fd0
  4dc4cd:	mov    ecx,eax
  4dc4cf:	call   0x4fd840
  4dc4d4:	mov    eax,ds:0x8a9ba4
  4dc4d9:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc4df:	push   0x0
  4dc4e1:	push   0x10000
  4dc4e6:	push   eax
  4dc4e7:	call   0x6c5fd0
  4dc4ec:	mov    ecx,eax
  4dc4ee:	call   0x4fd840
  4dc4f3:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc4f9:	push   0x0
  4dc4fb:	push   0x20000
  4dc500:	push   ecx
  4dc501:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc507:	call   0x6c5fd0
  4dc50c:	mov    ecx,eax
  4dc50e:	call   0x4fd840
  4dc513:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc519:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc51f:	push   0x0
  4dc521:	push   0x1000000
  4dc526:	push   edx
  4dc527:	call   0x6c5fd0
  4dc52c:	mov    ecx,eax
  4dc52e:	call   0x4fd840
  4dc533:	push   0x0
  4dc535:	mov    eax,ds:0x8a9ba4
  4dc53a:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc540:	push   0x80
  4dc545:	push   eax
  4dc546:	call   0x6c5fd0
  4dc54b:	mov    ecx,eax
  4dc54d:	call   0x4fd840
  4dc552:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc558:	push   ecx
  4dc559:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc55f:	call   0x6c5fd0
  4dc564:	mov    ecx,eax
  4dc566:	call   0x4ffcd0
  4dc56b:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc571:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc577:	push   edx
  4dc578:	call   0x6c5fd0
  4dc57d:	mov    eax,DWORD PTR [eax+0x84c]
  4dc583:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc589:	push   eax
  4dc58a:	call   0x6c5fd0
  4dc58f:	mov    esi,0x40000000
  4dc594:	test   eax,eax
  4dc596:	je     0x4dc5ce
  4dc598:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc59e:	push   ecx
  4dc59f:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc5a5:	call   0x6c5fd0
  4dc5aa:	mov    edx,DWORD PTR [eax+0x84c]
  4dc5b0:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc5b6:	push   edx
  4dc5b7:	call   0x6c5fd0
  4dc5bc:	mov    ecx,DWORD PTR [eax+0xdc]
  4dc5c2:	or     DWORD PTR [eax+0xd8],esi
  4dc5c8:	mov    DWORD PTR [eax+0xdc],ecx
  4dc5ce:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc5d4:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc5da:	push   edx
  4dc5db:	call   0x6c5fd0
  4dc5e0:	mov    eax,DWORD PTR [eax+0x848]
  4dc5e6:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc5ec:	push   eax
  4dc5ed:	call   0x6c5fd0
  4dc5f2:	test   eax,eax
  4dc5f4:	je     0x4dc62c
  4dc5f6:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc5fc:	push   ecx
  4dc5fd:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc603:	call   0x6c5fd0
  4dc608:	mov    edx,DWORD PTR [eax+0x848]
  4dc60e:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc614:	push   edx
  4dc615:	call   0x6c5fd0
  4dc61a:	mov    ecx,DWORD PTR [eax+0xdc]
  4dc620:	or     DWORD PTR [eax+0xd8],esi
  4dc626:	mov    DWORD PTR [eax+0xdc],ecx
  4dc62c:	mov    eax,ds:0xbe1ea4
  4dc631:	test   eax,eax
  4dc633:	jne    0x4dc655
  4dc635:	push   0x1ac
  4dc63a:	call   0x404390
  4dc63f:	add    esp,0x4
  4dc642:	test   eax,eax
  4dc644:	je     0x4dc64e
  4dc646:	mov    DWORD PTR [eax],0x7721bc
  4dc64c:	jmp    0x4dc650
  4dc64e:	xor    eax,eax
  4dc650:	mov    ds:0xbe1ea4,eax
  4dc655:	fld    DWORD PTR [edi+0x7c]
  4dc658:	lea    esi,[eax+0x70]
  4dc65b:	fstp   DWORD PTR [esi]
  4dc65d:	mov    eax,ds:0xbe1ea4
  4dc662:	test   eax,eax
  4dc664:	jne    0x4dc686
  4dc666:	push   0x1ac
  4dc66b:	call   0x404390
  4dc670:	add    esp,0x4
  4dc673:	test   eax,eax
  4dc675:	je     0x4dc67f
  4dc677:	mov    DWORD PTR [eax],0x7721bc
  4dc67d:	jmp    0x4dc681
  4dc67f:	xor    eax,eax
  4dc681:	mov    ds:0xbe1ea4,eax
  4dc686:	fld    DWORD PTR [esi]
  4dc688:	fstp   DWORD PTR [eax+0x30]
  4dc68b:	mov    eax,DWORD PTR [edi+0x424]
  4dc691:	sub    eax,0x0
  4dc694:	je     0x4dc6b9
  4dc696:	sub    eax,0x1
  4dc699:	jne    0x4dc6ef
  4dc69b:	call   0x4051f0
  4dc6a0:	push   0x1
  4dc6a2:	push   0x258d
  4dc6a7:	mov    ecx,eax
  4dc6a9:	call   0x6b2be0
  4dc6ae:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc6b4:	mov    esi,eax
  4dc6b6:	push   edx
  4dc6b7:	jmp    0x4dc6d4
  4dc6b9:	call   0x4051f0
  4dc6be:	push   0x1
  4dc6c0:	push   0x258b
  4dc6c5:	mov    ecx,eax
  4dc6c7:	call   0x6b2be0
  4dc6cc:	mov    esi,eax
  4dc6ce:	mov    eax,ds:0x8a9ba4
  4dc6d3:	push   eax
  4dc6d4:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc6da:	call   0x6c5fd0
  4dc6df:	mov    DWORD PTR [eax+0x630],0x1
  4dc6e9:	mov    DWORD PTR [eax+0x628],esi
  4dc6ef:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc6f5:	push   ebx
  4dc6f6:	push   ecx
  4dc6f7:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc6fd:	call   0x6c5fd0
  4dc702:	mov    esi,eax
  4dc704:	add    esi,0x3b0
  4dc70a:	call   0x40a320
  4dc70f:	mov    edx,DWORD PTR [eax+0x8c580]
  4dc715:	mov    DWORD PTR [esi],edx
  4dc717:	mov    ecx,DWORD PTR [eax+0x8c584]
  4dc71d:	mov    DWORD PTR [esi+0x4],ecx
  4dc720:	mov    edx,DWORD PTR [eax+0x8c588]
  4dc726:	mov    DWORD PTR [esi+0x8],edx
  4dc729:	mov    eax,DWORD PTR [eax+0x8c58c]
  4dc72f:	mov    DWORD PTR [esi+0xc],eax
  4dc732:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc738:	push   ecx
  4dc739:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc73f:	call   0x6c5fd0
  4dc744:	mov    edx,DWORD PTR [esi]
  4dc746:	add    eax,0x58
  4dc749:	mov    DWORD PTR [eax],edx
  4dc74b:	mov    ecx,DWORD PTR [esi+0x4]
  4dc74e:	mov    DWORD PTR [eax+0x4],ecx
  4dc751:	mov    edx,DWORD PTR [esi+0x8]
  4dc754:	mov    DWORD PTR [eax+0x8],edx
  4dc757:	mov    ecx,DWORD PTR [esi+0xc]
  4dc75a:	push   0x0
  4dc75c:	mov    DWORD PTR [eax+0xc],ecx
  4dc75f:	push   0x0
  4dc761:	mov    ecx,edi
  4dc763:	call   0x6c3b80
  4dc768:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc76e:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc774:	push   eax
  4dc775:	push   edx
  4dc776:	call   0x6c5fd0
  4dc77b:	add    eax,0x78
  4dc77e:	push   eax
  4dc77f:	call   0x6b6c30
  4dc784:	mov    eax,ds:0x8a9ba4
  4dc789:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc78f:	add    esp,0x8
  4dc792:	push   eax
  4dc793:	call   0x6c5fd0
  4dc798:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dc79e:	push   ecx
  4dc79f:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc7a5:	lea    esi,[eax+0x78]
  4dc7a8:	call   0x6c5fd0
  4dc7ad:	fldz
  4dc7af:	mov    edx,DWORD PTR [esi]
  4dc7b1:	mov    DWORD PTR [eax+0x68],edx
  4dc7b4:	mov    ecx,DWORD PTR [esi+0x4]
  4dc7b7:	add    eax,0x68
  4dc7ba:	mov    DWORD PTR [eax+0x4],ecx
  4dc7bd:	mov    edx,DWORD PTR [esi+0x8]
  4dc7c0:	mov    DWORD PTR [eax+0x8],edx
  4dc7c3:	mov    ecx,DWORD PTR [esi+0xc]
  4dc7c6:	push   ecx
  4dc7c7:	mov    DWORD PTR [eax+0xc],ecx
  4dc7ca:	fstp   DWORD PTR [esp]
  4dc7cd:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc7d3:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc7d9:	push   edx
  4dc7da:	call   0x6c5fd0
  4dc7df:	mov    ecx,eax
  4dc7e1:	call   0x4e31f0
  4dc7e6:	mov    eax,ds:0x8a9ba4
  4dc7eb:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc7f1:	push   eax
  4dc7f2:	call   0x6c5fd0
  4dc7f7:	mov    ecx,DWORD PTR [eax+0xd8]
  4dc7fd:	and    DWORD PTR [eax+0xdc],0xfffffffd
  4dc804:	mov    DWORD PTR [eax+0xd8],ecx
  4dc80a:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc810:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc816:	push   edx
  4dc817:	call   0x6c5fd0
  4dc81c:	and    DWORD PTR [eax+0x638],0xfffffffe
  4dc823:	or     DWORD PTR [edi+0x434],0x20000000
  4dc82d:	xor    ebx,ebx
  4dc82f:	cmp    BYTE PTR [esp+0x18],bl
  4dc833:	setne  bl
  4dc836:	lea    ebx,[ebx+ebx*1+0xf]
  4dc83a:	call   0x40a320
  4dc83f:	mov    DWORD PTR [eax+0x8c5b8],ebx
  4dc845:	call   0x40a320
  4dc84a:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc850:	mov    esi,eax
  4dc852:	mov    eax,ds:0x8a9ba4
  4dc857:	push   eax
  4dc858:	add    esi,0x8c57c
  4dc85e:	call   0x6c5fd0
  4dc863:	mov    eax,ds:0x148132c
  4dc868:	pop    ebx
  4dc869:	test   eax,eax
  4dc86b:	je     0x4dc876
  4dc86d:	push   esi
  4dc86e:	push   0x35
  4dc870:	push   edi
  4dc871:	call   eax
  4dc873:	add    esp,0xc
  4dc876:	mov    eax,DWORD PTR [edi+0x44]
  4dc879:	test   eax,eax
  4dc87b:	je     0x4dc886
  4dc87d:	push   esi
  4dc87e:	push   0x35
  4dc880:	push   edi
  4dc881:	call   eax
  4dc883:	add    esp,0xc
  4dc886:	call   0x40a320
  4dc88b:	mov    cx,WORD PTR [eax+0x9906c]
  4dc892:	pop    esi
  4dc893:	cmp    cx,WORD PTR [edi+0x2e]
  4dc897:	je     0x4dc8a3
  4dc899:	or     DWORD PTR [edi+0x434],0x2000
  4dc8a3:	call   0x40a320
  4dc8a8:	mov    dx,WORD PTR [edi+0x2e]
  4dc8ac:	mov    ecx,edi
  4dc8ae:	mov    WORD PTR [eax+0x9906c],dx
  4dc8b5:	call   0x54e2e0
  4dc8ba:	push   edi
  4dc8bb:	push   0x1
  4dc8bd:	call   0x4ad980
  4dc8c2:	mov    ecx,eax
  4dc8c4:	call   0x4af060
  4dc8c9:	mov    eax,ds:0x8a9ba4
  4dc8ce:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc8d4:	push   eax
  4dc8d5:	call   0x6c5fd0
  4dc8da:	and    DWORD PTR [eax+0x638],0xffff7fff
  4dc8e4:	push   edi
  4dc8e5:	call   0x40b270
  4dc8ea:	add    esp,0x4
  4dc8ed:	xor    ecx,ecx
  4dc8ef:	pop    edi
  4dc8f0:	mov    WORD PTR [ebp+0xaaa],cx
  4dc8f7:	pop    ebp
  4dc8f8:	ret    0x8
  4dc8fb:	int3
  4dc8fc:	int3
  4dc8fd:	int3
  4dc8fe:	int3
  4dc8ff:	int3
  4dc900:	mov    eax,DWORD PTR [esp+0x8]
  4dc904:	sub    esp,0x30
  4dc907:	push   esi
  4dc908:	mov    esi,ecx
  4dc90a:	cmp    eax,0xffffffff
  4dc90d:	jne    0x4dc922
  4dc90f:	call   0x40a320
  4dc914:	mov    eax,DWORD PTR [eax+0x8c57c]
  4dc91a:	push   eax
  4dc91b:	mov    ecx,esi
  4dc91d:	call   0x4dbe90
  4dc922:	mov    DWORD PTR [esi+0x9f4],eax
  4dc928:	cmp    eax,0xffffffff
  4dc92b:	je     0x4dcac4
  4dc931:	push   ebx
  4dc932:	push   ebp
  4dc933:	push   edi
  4dc934:	push   eax
  4dc935:	lea    eax,[esp+0x14]
  4dc939:	push   eax
  4dc93a:	mov    ecx,esi
  4dc93c:	call   0x460540
  4dc941:	mov    ecx,eax
  4dc943:	call   0x5e1ed0
  4dc948:	mov    ecx,DWORD PTR [esi+0x9f4]
  4dc94e:	push   ecx
  4dc94f:	lea    edx,[esp+0x34]
  4dc953:	push   edx
  4dc954:	mov    ecx,esi
  4dc956:	call   0x460540
  4dc95b:	mov    ecx,eax
  4dc95d:	call   0x5e19e0
  4dc962:	fld    DWORD PTR [esp+0x10]
  4dc966:	mov    esi,DWORD PTR [esp+0x44]
  4dc96a:	fsub   DWORD PTR [esi+0x58]
  4dc96d:	mov    eax,ds:0x8a9ba4
  4dc972:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc978:	push   eax
  4dc979:	fstp   DWORD PTR [esp+0x24]
  4dc97d:	fld    DWORD PTR [esp+0x18]
  4dc981:	fsub   DWORD PTR [esi+0x5c]
  4dc984:	fstp   DWORD PTR [esp+0x28]
  4dc988:	fld    DWORD PTR [esp+0x1c]
  4dc98c:	fsub   DWORD PTR [esi+0x60]
  4dc98f:	fstp   DWORD PTR [esp+0x2c]
  4dc993:	fld    DWORD PTR [esp+0x20]
  4dc997:	fsub   DWORD PTR [esi+0x64]
  4dc99a:	fstp   DWORD PTR [esp+0x30]
  4dc99e:	call   0x6c5fd0
  4dc9a3:	fld    DWORD PTR [eax+0x58]
  4dc9a6:	add    eax,0x58
  4dc9a9:	fadd   DWORD PTR [esp+0x20]
  4dc9ad:	fstp   DWORD PTR [eax]
  4dc9af:	fld    DWORD PTR [eax+0x4]
  4dc9b2:	fadd   DWORD PTR [esp+0x24]
  4dc9b6:	fstp   DWORD PTR [eax+0x4]
  4dc9b9:	fld    DWORD PTR [eax+0x8]
  4dc9bc:	fadd   DWORD PTR [esp+0x28]
  4dc9c0:	fstp   DWORD PTR [eax+0x8]
  4dc9c3:	fld    DWORD PTR [eax+0xc]
  4dc9c6:	fadd   DWORD PTR [esp+0x2c]
  4dc9ca:	fstp   DWORD PTR [eax+0xc]
  4dc9cd:	mov    ecx,DWORD PTR [esp+0x10]
  4dc9d1:	mov    edx,DWORD PTR [esp+0x14]
  4dc9d5:	fld    DWORD PTR [esp+0x30]
  4dc9d9:	mov    eax,DWORD PTR [esp+0x18]
  4dc9dd:	mov    DWORD PTR [esi+0x58],ecx
  4dc9e0:	mov    ecx,DWORD PTR [esp+0x1c]
  4dc9e4:	mov    DWORD PTR [esi+0x5c],edx
  4dc9e7:	mov    DWORD PTR [esi+0x60],eax
  4dc9ea:	mov    DWORD PTR [esi+0x64],ecx
  4dc9ed:	fsub   DWORD PTR [esi+0x78]
  4dc9f0:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dc9f6:	mov    ecx,DWORD PTR ds:0xbd7670
  4dc9fc:	fstp   DWORD PTR [esp+0x20]
  4dca00:	push   edx
  4dca01:	fld    DWORD PTR [esp+0x38]
  4dca05:	fsub   DWORD PTR [esi+0x7c]
  4dca08:	fstp   DWORD PTR [esp+0x28]
  4dca0c:	fld    DWORD PTR [esp+0x3c]
  4dca10:	fsub   DWORD PTR [esi+0x80]
  4dca16:	fstp   DWORD PTR [esp+0x2c]
  4dca1a:	fld    DWORD PTR [esp+0x40]
  4dca1e:	fsub   DWORD PTR [esi+0x84]
  4dca24:	fstp   DWORD PTR [esp+0x30]
  4dca28:	call   0x6c5fd0
  4dca2d:	fld    DWORD PTR [eax+0x68]
  4dca30:	add    eax,0x68
  4dca33:	fadd   DWORD PTR [esp+0x20]
  4dca37:	fstp   DWORD PTR [eax]
  4dca39:	fld    DWORD PTR [eax+0x4]
  4dca3c:	fadd   DWORD PTR [esp+0x24]
  4dca40:	fstp   DWORD PTR [eax+0x4]
  4dca43:	fld    DWORD PTR [eax+0x8]
  4dca46:	fadd   DWORD PTR [esp+0x28]
  4dca4a:	fstp   DWORD PTR [eax+0x8]
  4dca4d:	fld    DWORD PTR [eax+0xc]
  4dca50:	fadd   DWORD PTR [esp+0x2c]
  4dca54:	fstp   DWORD PTR [eax+0xc]
  4dca57:	mov    eax,ds:0x8a9ba4
  4dca5c:	mov    ecx,DWORD PTR ds:0xbd7670
  4dca62:	push   eax
  4dca63:	call   0x6c5fd0
  4dca68:	mov    ecx,DWORD PTR [eax+0x74]
  4dca6b:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dca71:	mov    edi,DWORD PTR [eax+0x68]
  4dca74:	mov    ebx,DWORD PTR [eax+0x6c]
  4dca77:	mov    ebp,DWORD PTR [eax+0x70]
  4dca7a:	add    eax,0x68
  4dca7d:	mov    DWORD PTR [esp+0x2c],ecx
  4dca81:	mov    ecx,DWORD PTR ds:0xbd7670
  4dca87:	push   edx
  4dca88:	call   0x6c5fd0
  4dca8d:	mov    ecx,DWORD PTR [esp+0x2c]
  4dca91:	mov    DWORD PTR [eax+0x78],edi
  4dca94:	mov    DWORD PTR [eax+0x7c],ebx
  4dca97:	mov    DWORD PTR [eax+0x80],ebp
  4dca9d:	add    eax,0x78
  4dcaa0:	mov    DWORD PTR [eax+0xc],ecx
  4dcaa3:	mov    edx,DWORD PTR [esi+0x68]
  4dcaa6:	mov    eax,DWORD PTR [esi+0x6c]
  4dcaa9:	mov    ecx,DWORD PTR [esi+0x70]
  4dcaac:	mov    DWORD PTR [esi+0x78],edx
  4dcaaf:	mov    edx,DWORD PTR [esi+0x74]
  4dcab2:	pop    edi
  4dcab3:	mov    DWORD PTR [esi+0x7c],eax
  4dcab6:	pop    ebp
  4dcab7:	mov    DWORD PTR [esi+0x80],ecx
  4dcabd:	mov    DWORD PTR [esi+0x84],edx
  4dcac3:	pop    ebx
  4dcac4:	pop    esi
  4dcac5:	add    esp,0x30
  4dcac8:	ret    0x8
  4dcacb:	int3
  4dcacc:	int3
  4dcacd:	int3
  4dcace:	int3
  4dcacf:	int3
