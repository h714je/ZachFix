
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

004de5d0 <.text+0xdd5d0>:
  4de5d0:	push   ecx
  4de5d1:	push   ebx
  4de5d2:	push   ebp
  4de5d3:	push   esi
  4de5d4:	push   edi
  4de5d5:	call   0x40a320
  4de5da:	mov    esi,DWORD PTR [eax+0x8c57c]
  4de5e0:	mov    eax,ds:0xbe1ea4
  4de5e5:	test   eax,eax
  4de5e7:	jne    0x4de609
  4de5e9:	push   0x1ac
  4de5ee:	call   0x404390
  4de5f3:	add    esp,0x4
  4de5f6:	test   eax,eax
  4de5f8:	je     0x4de602
  4de5fa:	mov    DWORD PTR [eax],0x7721bc
  4de600:	jmp    0x4de604
  4de602:	xor    eax,eax
  4de604:	mov    ds:0xbe1ea4,eax
  4de609:	mov    ebp,0x3
  4de60e:	or     DWORD PTR [eax+0x11c],ebp
  4de614:	mov    eax,ds:0x8a9ba4
  4de619:	mov    ecx,DWORD PTR ds:0xbd7670
  4de61f:	push   eax
  4de620:	call   0x6c5fd0
  4de625:	mov    ebx,0x1
  4de62a:	mov    BYTE PTR [eax+0xa8c],bl
  4de630:	mov    eax,ds:0xbe1ea4
  4de635:	test   eax,eax
  4de637:	jne    0x4de659
  4de639:	push   0x1ac
  4de63e:	call   0x404390
  4de643:	add    esp,0x4
  4de646:	test   eax,eax
  4de648:	je     0x4de652
  4de64a:	mov    DWORD PTR [eax],0x7721bc
  4de650:	jmp    0x4de654
  4de652:	xor    eax,eax
  4de654:	mov    ds:0xbe1ea4,eax
  4de659:	fld    DWORD PTR ds:0x772270
  4de65f:	push   0x0
  4de661:	fstp   DWORD PTR [eax+0x124]
  4de667:	mov    ecx,DWORD PTR ds:0xbe1ea4
  4de66d:	add    ecx,0x1c
  4de670:	push   0x775a64
  4de675:	push   ecx
  4de676:	mov    ecx,esi
  4de678:	call   0x6c4060
  4de67d:	mov    eax,ds:0xbe1ea4
  4de682:	test   eax,eax
  4de684:	jne    0x4de6a6
  4de686:	push   0x1ac
  4de68b:	call   0x404390
  4de690:	add    esp,0x4
  4de693:	test   eax,eax
  4de695:	je     0x4de69f
  4de697:	mov    DWORD PTR [eax],0x7721bc
  4de69d:	jmp    0x4de6a1
  4de69f:	xor    eax,eax
  4de6a1:	mov    ds:0xbe1ea4,eax
  4de6a6:	fld    DWORD PTR [eax+0x20]
  4de6a9:	fadd   QWORD PTR ds:0x7703f0
  4de6af:	fstp   DWORD PTR [eax+0x20]
  4de6b2:	mov    ecx,DWORD PTR ds:0xbe1ea4
  4de6b8:	mov    edx,DWORD PTR [ecx+0x1c]
  4de6bb:	lea    eax,[ecx+0x1c]
  4de6be:	mov    DWORD PTR [ecx+0xc],edx
  4de6c1:	mov    edx,DWORD PTR [eax+0x4]
  4de6c4:	add    ecx,0xc
  4de6c7:	mov    DWORD PTR [ecx+0x4],edx
  4de6ca:	mov    edx,DWORD PTR [eax+0x8]
  4de6cd:	mov    DWORD PTR [ecx+0x8],edx
  4de6d0:	mov    eax,DWORD PTR [eax+0xc]
  4de6d3:	mov    DWORD PTR [ecx+0xc],eax
  4de6d6:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de6dc:	mov    edi,DWORD PTR ds:0xbe1ea4
  4de6e2:	push   ecx
  4de6e3:	mov    ecx,DWORD PTR ds:0xbd7670
  4de6e9:	add    edi,0xc
  4de6ec:	call   0x6c5fd0
  4de6f1:	fld    DWORD PTR [eax+0x7c]
  4de6f4:	fadd   QWORD PTR ds:0x7703a0
  4de6fa:	fstp   DWORD PTR [esp+0x10]
  4de6fe:	fld    DWORD PTR [esp+0x10]
  4de702:	fmul   QWORD PTR ds:0x76f650
  4de708:	call   0x74ed60
  4de70d:	mov    ecx,eax
  4de70f:	mov    eax,0xd646fa41
  4de714:	imul   ecx
  4de716:	sar    edx,0xa
  4de719:	mov    eax,edx
  4de71b:	shr    eax,0x1f
  4de71e:	add    eax,edx
  4de720:	imul   eax,eax,0x188b
  4de726:	add    ecx,eax
  4de728:	jns    0x4de730
  4de72a:	add    ecx,0x188b
  4de730:	fld    DWORD PTR [ecx*4+0x8880e0]
  4de737:	fmul   QWORD PTR ds:0x775ab0
  4de73d:	fadd   DWORD PTR [edi]
  4de73f:	fstp   DWORD PTR [edi]
  4de741:	mov    eax,ds:0xbe1ea4
  4de746:	test   eax,eax
  4de748:	jne    0x4de76a
  4de74a:	push   0x1ac
  4de74f:	call   0x404390
  4de754:	add    esp,0x4
  4de757:	test   eax,eax
  4de759:	je     0x4de763
  4de75b:	mov    DWORD PTR [eax],0x7721bc
  4de761:	jmp    0x4de765
  4de763:	xor    eax,eax
  4de765:	mov    ds:0xbe1ea4,eax
  4de76a:	fld    DWORD PTR [eax+0x10]
  4de76d:	fadd   QWORD PTR ds:0x775aa8
  4de773:	fstp   DWORD PTR [eax+0x10]
  4de776:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de77c:	mov    edi,DWORD PTR ds:0xbe1ea4
  4de782:	push   ecx
  4de783:	mov    ecx,DWORD PTR ds:0xbd7670
  4de789:	add    edi,0x14
  4de78c:	call   0x6c5fd0
  4de791:	fld    DWORD PTR [eax+0x7c]
  4de794:	fadd   QWORD PTR ds:0x7703a0
  4de79a:	fstp   DWORD PTR [esp+0x10]
  4de79e:	fld    DWORD PTR [esp+0x10]
  4de7a2:	fmul   QWORD PTR ds:0x76f650
  4de7a8:	call   0x74ed60
  4de7ad:	mov    ecx,eax
  4de7af:	mov    eax,0xd646fa41
  4de7b4:	imul   ecx
  4de7b6:	sar    edx,0xa
  4de7b9:	mov    eax,edx
  4de7bb:	shr    eax,0x1f
  4de7be:	add    eax,edx
  4de7c0:	imul   eax,eax,0x188b
  4de7c6:	add    ecx,eax
  4de7c8:	jns    0x4de7d0
  4de7ca:	add    ecx,0x188b
  4de7d0:	fld    DWORD PTR [ecx*4+0x88e310]
  4de7d7:	fmul   QWORD PTR ds:0x775ab0
  4de7dd:	fadd   DWORD PTR [edi]
  4de7df:	fstp   DWORD PTR [edi]
  4de7e1:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de7e7:	push   ecx
  4de7e8:	mov    ecx,DWORD PTR ds:0xbd7670
  4de7ee:	call   0x6c5fd0
  4de7f3:	mov    edi,DWORD PTR [eax+0x628]
  4de7f9:	call   0x4051f0
  4de7fe:	push   ebx
  4de7ff:	push   0x2471
  4de804:	mov    ecx,eax
  4de806:	call   0x6b2be0
  4de80b:	mov    ecx,DWORD PTR ds:0xbd7670
  4de811:	cmp    edi,eax
  4de813:	jne    0x4de995
  4de819:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de81f:	push   edx
  4de820:	call   0x6c5fd0
  4de825:	mov    eax,DWORD PTR [eax+0x1fc]
  4de82b:	mov    ecx,eax
  4de82d:	not    ecx
  4de82f:	test   cl,0x2
  4de832:	je     0x4ded95
  4de838:	test   al,0x10
  4de83a:	je     0x4ded95
  4de840:	call   0x405360
  4de845:	and    DWORD PTR [eax+0x4],0xffefffff
  4de84c:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de852:	mov    ecx,DWORD PTR ds:0xbd7670
  4de858:	add    eax,0x4
  4de85b:	push   0x4000000
  4de860:	push   0x0
  4de862:	push   edx
  4de863:	call   0x6c5fd0
  4de868:	mov    ecx,eax
  4de86a:	call   0x4fd840
  4de86f:	mov    eax,ds:0x8a9ba4
  4de874:	mov    ecx,DWORD PTR ds:0xbd7670
  4de87a:	push   0x4000
  4de87f:	push   0x0
  4de881:	push   eax
  4de882:	call   0x6c5fd0
  4de887:	mov    ecx,eax
  4de889:	call   0x4fd860
  4de88e:	or     eax,edx
  4de890:	je     0x4de8de
  4de892:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de898:	push   0x4000
  4de89d:	push   0x0
  4de89f:	push   ecx
  4de8a0:	mov    ecx,DWORD PTR ds:0xbd7670
  4de8a6:	call   0x6c5fd0
  4de8ab:	mov    ecx,eax
  4de8ad:	call   0x4fd840
  4de8b2:	call   0x405360
  4de8b7:	mov    ecx,eax
  4de8b9:	call   0x43fb40
  4de8be:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de8c4:	mov    ecx,DWORD PTR ds:0xbd7670
  4de8ca:	push   edx
  4de8cb:	call   0x6c5fd0
  4de8d0:	pop    edi
  4de8d1:	pop    esi
  4de8d2:	pop    ebp
  4de8d3:	mov    ecx,eax
  4de8d5:	pop    ebx
  4de8d6:	add    esp,0x4
  4de8d9:	jmp    0x4dddb0
  4de8de:	mov    eax,DWORD PTR [esi+0x424]
  4de8e4:	sub    eax,0x0
  4de8e7:	je     0x4de909
  4de8e9:	sub    eax,ebx
  4de8eb:	jne    0x4de93b
  4de8ed:	call   0x4051f0
  4de8f2:	push   ebx
  4de8f3:	push   0x258e
  4de8f8:	mov    ecx,eax
  4de8fa:	call   0x6b2be0
  4de8ff:	mov    edi,eax
  4de901:	mov    eax,ds:0x8a9ba4
  4de906:	push   eax
  4de907:	jmp    0x4de924
  4de909:	call   0x4051f0
  4de90e:	push   ebx
  4de90f:	push   0x258c
  4de914:	mov    ecx,eax
  4de916:	call   0x6b2be0
  4de91b:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de921:	mov    edi,eax
  4de923:	push   ecx
  4de924:	mov    ecx,DWORD PTR ds:0xbd7670
  4de92a:	call   0x6c5fd0
  4de92f:	mov    DWORD PTR [eax+0x630],ebx
  4de935:	mov    DWORD PTR [eax+0x628],edi
  4de93b:	call   0x40a320
  4de940:	mov    DWORD PTR [eax+0x8c5b8],0x10
  4de94a:	call   0x40a320
  4de94f:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de955:	mov    ecx,DWORD PTR ds:0xbd7670
  4de95b:	mov    edi,eax
  4de95d:	push   edx
  4de95e:	add    edi,0x8c57c
  4de964:	call   0x6c5fd0
  4de969:	mov    eax,ds:0x148132c
  4de96e:	test   eax,eax
  4de970:	je     0x4de97b
  4de972:	push   edi
  4de973:	push   0x35
  4de975:	push   esi
  4de976:	call   eax
  4de978:	add    esp,0xc
  4de97b:	mov    eax,DWORD PTR [esi+0x44]
  4de97e:	test   eax,eax
  4de980:	je     0x4ded95
  4de986:	push   edi
  4de987:	push   0x35
  4de989:	push   esi
  4de98a:	call   eax
  4de98c:	add    esp,0xc
  4de98f:	pop    edi
  4de990:	pop    esi
  4de991:	pop    ebp
  4de992:	pop    ebx
  4de993:	pop    ecx
  4de994:	ret
  4de995:	mov    eax,ds:0x8a9ba4
  4de99a:	push   eax
  4de99b:	call   0x6c5fd0
  4de9a0:	mov    edi,DWORD PTR [eax+0x628]
  4de9a6:	call   0x4051f0
  4de9ab:	push   ebx
  4de9ac:	push   0x258b
  4de9b1:	mov    ecx,eax
  4de9b3:	call   0x6b2be0
  4de9b8:	cmp    edi,eax
  4de9ba:	je     0x4decf7
  4de9c0:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de9c6:	push   ecx
  4de9c7:	mov    ecx,DWORD PTR ds:0xbd7670
  4de9cd:	call   0x6c5fd0
  4de9d2:	mov    edi,DWORD PTR [eax+0x628]
  4de9d8:	call   0x4051f0
  4de9dd:	push   ebx
  4de9de:	push   0x258d
  4de9e3:	mov    ecx,eax
  4de9e5:	call   0x6b2be0
  4de9ea:	cmp    edi,eax
  4de9ec:	je     0x4decf7
  4de9f2:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de9f8:	mov    ecx,DWORD PTR ds:0xbd7670
  4de9fe:	push   edx
  4de9ff:	call   0x6c5fd0
  4dea04:	mov    edi,DWORD PTR [eax+0x628]
  4dea0a:	call   0x4051f0
  4dea0f:	push   ebx
  4dea10:	push   0x258c
  4dea15:	mov    ecx,eax
  4dea17:	call   0x6b2be0
  4dea1c:	cmp    edi,eax
  4dea1e:	je     0x4deb95
  4dea24:	mov    eax,ds:0x8a9ba4
  4dea29:	mov    ecx,DWORD PTR ds:0xbd7670
  4dea2f:	push   eax
  4dea30:	call   0x6c5fd0
  4dea35:	mov    edi,DWORD PTR [eax+0x628]
  4dea3b:	call   0x4051f0
  4dea40:	push   ebx
  4dea41:	push   0x258e
  4dea46:	mov    ecx,eax
  4dea48:	call   0x6b2be0
  4dea4d:	cmp    edi,eax
  4dea4f:	je     0x4deb95
  4dea55:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dea5b:	push   0x2000000
  4dea60:	push   0x0
  4dea62:	push   ecx
  4dea63:	mov    ecx,DWORD PTR ds:0xbd7670
  4dea69:	call   0x6c5fd0
  4dea6e:	mov    ecx,eax
  4dea70:	call   0x4fd860
  4dea75:	or     eax,edx
  4dea77:	jne    0x4ded95
  4dea7d:	mov    edx,DWORD PTR ds:0x8a9ba4
  4dea83:	mov    ecx,DWORD PTR ds:0xbd7670
  4dea89:	push   edx
  4dea8a:	call   0x6c5fd0
  4dea8f:	fldz
  4dea91:	and    DWORD PTR [eax+0x63c],0xfffffffd
  4dea98:	fstp   DWORD PTR [esi+0x13dc]
  4dea9e:	and    DWORD PTR [esi+0x434],0xfffbffe8
  4deaa8:	mov    eax,ds:0x8a9ba4
  4deaad:	mov    ecx,DWORD PTR ds:0xbd7670
  4deab3:	push   0x80
  4deab8:	push   0x0
  4deaba:	push   eax
  4deabb:	call   0x6c5fd0
  4deac0:	mov    ecx,eax
  4deac2:	call   0x4fd840
  4deac7:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4deacd:	push   0x0
  4deacf:	push   0x400
  4dead4:	push   ecx
  4dead5:	mov    ecx,DWORD PTR ds:0xbd7670
  4deadb:	call   0x6c5fd0
  4deae0:	mov    ecx,eax
  4deae2:	call   0x4fd860
  4deae7:	or     eax,edx
  4deae9:	mov    ecx,DWORD PTR ds:0xbd7670
  4deaef:	je     0x4deb0b
  4deaf1:	mov    edx,DWORD PTR ds:0x8a9ba4
  4deaf7:	push   ebx
  4deaf8:	push   0x0
  4deafa:	push   0x0
  4deafc:	push   edx
  4deafd:	call   0x6c5fd0
  4deb02:	mov    ecx,eax
  4deb04:	call   0x4dccc0
  4deb09:	jmp    0x4deb6f
  4deb0b:	mov    eax,ds:0x8a9ba4
  4deb10:	push   0x0
  4deb12:	push   esi
  4deb13:	push   eax
  4deb14:	call   0x6c5fd0
  4deb19:	mov    ecx,eax
  4deb1b:	call   0x4dc3d0
  4deb20:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4deb26:	push   0x4000
  4deb2b:	push   0x0
  4deb2d:	push   ecx
  4deb2e:	mov    ecx,DWORD PTR ds:0xbd7670
  4deb34:	call   0x6c5fd0
  4deb39:	mov    ecx,eax
  4deb3b:	call   0x4fd820
  4deb40:	mov    edx,DWORD PTR ds:0x8a9ba4
  4deb46:	mov    ecx,DWORD PTR ds:0xbd7670
  4deb4c:	push   edx
  4deb4d:	call   0x6c5fd0
  4deb52:	mov    BYTE PTR [eax+0xa6a],bl
  4deb58:	mov    eax,ds:0x8a9ba4
  4deb5d:	mov    ecx,DWORD PTR ds:0xbd7670
  4deb63:	push   eax
  4deb64:	call   0x6c5fd0
  4deb69:	mov    DWORD PTR [eax+0xa6c],ebp
  4deb6f:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4deb75:	push   0x4000000
  4deb7a:	push   0x0
  4deb7c:	push   ecx
  4deb7d:	mov    ecx,DWORD PTR ds:0xbd7670
  4deb83:	call   0x6c5fd0
  4deb88:	mov    ecx,eax
  4deb8a:	call   0x4fd820
  4deb8f:	pop    edi
  4deb90:	pop    esi
  4deb91:	pop    ebp
  4deb92:	pop    ebx
  4deb93:	pop    ecx
  4deb94:	ret
  4deb95:	mov    edx,DWORD PTR ds:0x8a9ba4
  4deb9b:	mov    ecx,DWORD PTR ds:0xbd7670
  4deba1:	push   edx
  4deba2:	call   0x6c5fd0
  4deba7:	mov    ecx,DWORD PTR [eax+0xdc]
  4debad:	and    ecx,0x4
  4debb0:	xor    eax,eax
  4debb2:	or     eax,ecx
  4debb4:	je     0x4dec01
  4debb6:	mov    eax,ds:0x8a9ba4
  4debbb:	mov    ecx,DWORD PTR ds:0xbd7670
  4debc1:	push   0x0
  4debc3:	push   0x0
  4debc5:	push   0x0
  4debc7:	push   0x0
  4debc9:	push   0x0
  4debcb:	push   0x0
  4debcd:	push   eax
  4debce:	call   0x6c5fd0
  4debd3:	mov    ecx,eax
  4debd5:	call   0x6c2880
  4debda:	mov    ecx,DWORD PTR [esi+0x134]
  4debe0:	and    DWORD PTR [esi+0x130],0xfffffff8
  4debe7:	or     DWORD PTR [esi+0x130],ebp
  4debed:	and    DWORD PTR [esi+0x130],0xffffefff
  4debf7:	mov    edx,ecx
  4debf9:	mov    eax,edx
  4debfb:	mov    DWORD PTR [esi+0x134],eax
  4dec01:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dec07:	push   ecx
  4dec08:	mov    ecx,DWORD PTR ds:0xbd7670
  4dec0e:	call   0x6c5fd0
  4dec13:	mov    eax,DWORD PTR [eax+0x1fc]
  4dec19:	mov    edx,eax
  4dec1b:	not    edx
  4dec1d:	test   dl,0x2
  4dec20:	je     0x4ded95
  4dec26:	test   al,0x10
  4dec28:	je     0x4ded95
  4dec2e:	mov    eax,ds:0x8a9ba4
  4dec33:	mov    ecx,DWORD PTR ds:0xbd7670
  4dec39:	push   0x4000000
  4dec3e:	push   0x0
  4dec40:	push   eax
  4dec41:	call   0x6c5fd0
  4dec46:	mov    ecx,eax
  4dec48:	call   0x4fd840
  4dec4d:	call   0x40a320
  4dec52:	mov    DWORD PTR [eax+0x8c5b8],0x15
  4dec5c:	call   0x40a320
  4dec61:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4dec67:	mov    edi,eax
  4dec69:	push   ecx
  4dec6a:	mov    ecx,DWORD PTR ds:0xbd7670
  4dec70:	add    edi,0x8c57c
  4dec76:	call   0x6c5fd0
  4dec7b:	mov    eax,ds:0x148132c
  4dec80:	test   eax,eax
  4dec82:	je     0x4dec8d
  4dec84:	push   edi
  4dec85:	push   0x35
  4dec87:	push   esi
  4dec88:	call   eax
  4dec8a:	add    esp,0xc
  4dec8d:	mov    eax,DWORD PTR [esi+0x44]
  4dec90:	test   eax,eax
  4dec92:	je     0x4dec9d
  4dec94:	push   edi
  4dec95:	push   0x35
  4dec97:	push   esi
  4dec98:	call   eax
  4dec9a:	add    esp,0xc
  4dec9d:	call   0x405360
  4deca2:	mov    ecx,eax
  4deca4:	call   0x43fb90
  4deca9:	mov    edx,DWORD PTR ds:0x8a9ba4
  4decaf:	mov    ecx,DWORD PTR ds:0xbd7670
  4decb5:	push   edx
  4decb6:	call   0x6c5fd0
  4decbb:	mov    ecx,eax
  4decbd:	call   0x4de1d0
  4decc2:	mov    eax,ds:0x8a9ba4
  4decc7:	mov    ecx,DWORD PTR ds:0xbd7670
  4deccd:	push   eax
  4decce:	call   0x6c5fd0
  4decd3:	mov    BYTE PTR [eax+0xa6a],bl
  4decd9:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4decdf:	push   ecx
  4dece0:	mov    ecx,DWORD PTR ds:0xbd7670
  4dece6:	call   0x6c5fd0
  4deceb:	pop    edi
  4decec:	pop    esi
  4deced:	mov    DWORD PTR [eax+0xa6c],ebp
  4decf3:	pop    ebp
  4decf4:	pop    ebx
  4decf5:	pop    ecx
  4decf6:	ret
  4decf7:	call   0x405360
  4decfc:	or     DWORD PTR [eax+0x4],0x100000
  4ded03:	mov    edx,DWORD PTR ds:0x8a9ba4
  4ded09:	mov    ecx,DWORD PTR ds:0xbd7670
  4ded0f:	add    eax,0x4
  4ded12:	push   edx
  4ded13:	call   0x6c5fd0
  4ded18:	mov    eax,DWORD PTR [eax+0x1fc]
  4ded1e:	mov    ecx,eax
  4ded20:	not    ecx
  4ded22:	test   cl,0x2
  4ded25:	je     0x4ded95
  4ded27:	test   al,0x10
  4ded29:	je     0x4ded95
  4ded2b:	mov    edx,DWORD PTR ds:0x8a9ba4
  4ded31:	mov    ecx,DWORD PTR ds:0xbd7670
  4ded37:	push   0x4000
  4ded3c:	push   0x0
  4ded3e:	push   edx
  4ded3f:	call   0x6c5fd0
  4ded44:	mov    ecx,eax
  4ded46:	call   0x4fd820
  4ded4b:	call   0x4051f0
  4ded50:	push   ebx
  4ded51:	push   0x2471
  4ded56:	mov    ecx,eax
  4ded58:	call   0x6b2be0
  4ded5d:	mov    ecx,DWORD PTR ds:0xbd7670
  4ded63:	mov    esi,eax
  4ded65:	mov    eax,ds:0x8a9ba4
  4ded6a:	push   eax
  4ded6b:	call   0x6c5fd0
  4ded70:	mov    DWORD PTR [eax+0x628],esi
  4ded76:	mov    DWORD PTR [eax+0x630],ebx
  4ded7c:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4ded82:	push   ecx
  4ded83:	mov    ecx,DWORD PTR ds:0xbd7670
  4ded89:	call   0x6c5fd0
  4ded8e:	mov    BYTE PTR [eax+0xa8c],0x0
  4ded95:	pop    edi
  4ded96:	pop    esi
  4ded97:	pop    ebp
  4ded98:	pop    ebx
  4ded99:	pop    ecx
  4ded9a:	ret
