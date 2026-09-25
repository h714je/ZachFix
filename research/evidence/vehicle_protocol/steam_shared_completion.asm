
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

004f5620 <.text+0xf4620>:
  4f5620:	mov    eax,ds:0xbe1ea4
  4f5625:	sub    esp,0x8
  4f5628:	test   eax,eax
  4f562a:	jne    0x4f564c
  4f562c:	push   0x1ac
  4f5631:	call   0x404390
  4f5636:	add    esp,0x4
  4f5639:	test   eax,eax
  4f563b:	je     0x4f5645
  4f563d:	mov    DWORD PTR [eax],0x7721bc
  4f5643:	jmp    0x4f5647
  4f5645:	xor    eax,eax
  4f5647:	mov    ds:0xbe1ea4,eax
  4f564c:	or     DWORD PTR [eax+0x11c],0x3
  4f5653:	mov    eax,ds:0x13928a4
  4f5658:	sub    eax,0x0
  4f565b:	push   esi
  4f565c:	je     0x4f5826
  4f5662:	sub    eax,0x1
  4f5665:	je     0x4f5809
  4f566b:	sub    eax,0x1
  4f566e:	jne    0x4f5b11
  4f5674:	mov    eax,ds:0x8a9ba4
  4f5679:	mov    ecx,DWORD PTR ds:0xbd7670
  4f567f:	push   ebx
  4f5680:	push   edi
  4f5681:	push   eax
  4f5682:	call   0x6c5fd0
  4f5687:	mov    eax,DWORD PTR [eax+0x654]
  4f568d:	add    eax,0xffffffbd
  4f5690:	cmp    eax,0x25
  4f5693:	ja     0x4f572c
  4f5699:	movzx  ecx,BYTE PTR [eax+0x4f5b50]
  4f56a0:	jmp    DWORD PTR [ecx*4+0x4f5b18]
  4f56a7:	mov    esi,0x1d
  4f56ac:	lea    edi,[esi+0x20]
  4f56af:	jmp    0x4f5734
  4f56b4:	mov    esi,0x1d
  4f56b9:	lea    edi,[esi+0x36]
  4f56bc:	jmp    0x4f5734
  4f56be:	mov    esi,0x1d
  4f56c3:	lea    edi,[esi+0x37]
  4f56c6:	jmp    0x4f5734
  4f56c8:	mov    esi,0x1d
  4f56cd:	lea    edi,[esi+0x38]
  4f56d0:	jmp    0x4f5734
  4f56d2:	mov    esi,0x1d
  4f56d7:	lea    edi,[esi+0x39]
  4f56da:	jmp    0x4f5734
  4f56dc:	mov    esi,0x1d
  4f56e1:	lea    edi,[esi+0x3a]
  4f56e4:	jmp    0x4f5734
  4f56e6:	mov    esi,0x1d
  4f56eb:	lea    edi,[esi+0x3b]
  4f56ee:	jmp    0x4f5734
  4f56f0:	mov    esi,0x1d
  4f56f5:	lea    edi,[esi+0x3c]
  4f56f8:	jmp    0x4f5734
  4f56fa:	mov    esi,0x1d
  4f56ff:	lea    edi,[esi+0x3d]
  4f5702:	jmp    0x4f5734
  4f5704:	mov    esi,0x1d
  4f5709:	lea    edi,[esi+0x3e]
  4f570c:	jmp    0x4f5734
  4f570e:	mov    esi,0x1d
  4f5713:	lea    edi,[esi+0x2e]
  4f5716:	jmp    0x4f5734
  4f5718:	mov    esi,0x1d
  4f571d:	lea    edi,[esi+0x21]
  4f5720:	jmp    0x4f5734
  4f5722:	mov    esi,0x1d
  4f5727:	lea    edi,[esi+0x3f]
  4f572a:	jmp    0x4f5734
  4f572c:	mov    esi,DWORD PTR [esp+0xc]
  4f5730:	mov    edi,DWORD PTR [esp+0xc]
  4f5734:	call   0x40a320
  4f5739:	mov    DWORD PTR [eax+0x8c5b8],esi
  4f573f:	call   0x40a320
  4f5744:	mov    esi,eax
  4f5746:	call   0x40a320
  4f574b:	mov    edx,DWORD PTR ds:0x8a9ba4
  4f5751:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5757:	lea    ebx,[esi+0x8c57c]
  4f575d:	mov    esi,DWORD PTR [eax+0x8c57c]
  4f5763:	push   edx
  4f5764:	call   0x6c5fd0
  4f5769:	mov    eax,ds:0x148132c
  4f576e:	test   eax,eax
  4f5770:	je     0x4f577a
  4f5772:	push   ebx
  4f5773:	push   edi
  4f5774:	push   esi
  4f5775:	call   eax
  4f5777:	add    esp,0xc
  4f577a:	mov    eax,DWORD PTR [esi+0x44]
  4f577d:	test   eax,eax
  4f577f:	je     0x4f5789
  4f5781:	push   ebx
  4f5782:	push   edi
  4f5783:	push   esi
  4f5784:	call   eax
  4f5786:	add    esp,0xc
  4f5789:	mov    eax,ds:0xbe1ea4
  4f578e:	pop    edi
  4f578f:	pop    ebx
  4f5790:	test   eax,eax
  4f5792:	jne    0x4f57b4
  4f5794:	push   0x1ac
  4f5799:	call   0x404390
  4f579e:	add    esp,0x4
  4f57a1:	test   eax,eax
  4f57a3:	je     0x4f57ad
  4f57a5:	mov    DWORD PTR [eax],0x7721bc
  4f57ab:	jmp    0x4f57af
  4f57ad:	xor    eax,eax
  4f57af:	mov    ds:0xbe1ea4,eax
  4f57b4:	and    DWORD PTR [eax+0x11c],0xfffffffc
  4f57bb:	mov    eax,ds:0xbe1ea4
  4f57c0:	or     DWORD PTR [eax+0x11c],0x4
  4f57c7:	mov    eax,ds:0x8a9ba4
  4f57cc:	mov    ecx,DWORD PTR ds:0xbd7670
  4f57d2:	push   eax
  4f57d3:	call   0x6c5fd0
  4f57d8:	or     DWORD PTR [eax+0x638],0x1
  4f57df:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4f57e5:	push   0x0
  4f57e7:	push   ecx
  4f57e8:	mov    ecx,DWORD PTR ds:0xbd7670
  4f57ee:	call   0x6c5fd0
  4f57f3:	mov    ecx,eax
  4f57f5:	call   0x528f40
  4f57fa:	mov    DWORD PTR ds:0x13928a4,0x0
  4f5804:	pop    esi
  4f5805:	add    esp,0x8
  4f5808:	ret
  4f5809:	call   0x40a320
  4f580e:	cmp    DWORD PTR [eax+0x8c5b8],0x20
  4f5815:	jne    0x4f5b11
  4f581b:	inc    DWORD PTR ds:0x13928a4
  4f5821:	pop    esi
  4f5822:	add    esp,0x8
  4f5825:	ret
  4f5826:	mov    edx,DWORD PTR ds:0x8a9ba4
  4f582c:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5832:	push   edx
  4f5833:	call   0x6c5fd0
  4f5838:	lea    esi,[eax+0x6c]
  4f583b:	call   0x40a320
  4f5840:	fld    DWORD PTR [eax+0x8c5a0]
  4f5846:	fstp   DWORD PTR [esp+0x4]
  4f584a:	fld    DWORD PTR [esp+0x4]
  4f584e:	fstp   DWORD PTR [esi]
  4f5850:	mov    eax,ds:0x8a9ba4
  4f5855:	mov    ecx,DWORD PTR ds:0xbd7670
  4f585b:	push   eax
  4f585c:	call   0x6c5fd0
  4f5861:	fld    DWORD PTR [esp+0x4]
  4f5865:	fstp   DWORD PTR [eax+0x7c]
  4f5868:	call   0x40a320
  4f586d:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4f5873:	push   ecx
  4f5874:	mov    ecx,DWORD PTR ds:0xbd7670
  4f587a:	lea    esi,[eax+0x8c580]
  4f5880:	call   0x6c5fd0
  4f5885:	mov    edx,DWORD PTR [esi]
  4f5887:	mov    DWORD PTR [eax+0x3b0],edx
  4f588d:	mov    ecx,DWORD PTR [esi+0x4]
  4f5890:	add    eax,0x3b0
  4f5895:	mov    DWORD PTR [eax+0x4],ecx
  4f5898:	mov    edx,DWORD PTR [esi+0x8]
  4f589b:	mov    DWORD PTR [eax+0x8],edx
  4f589e:	mov    ecx,DWORD PTR [esi+0xc]
  4f58a1:	mov    DWORD PTR [eax+0xc],ecx
  4f58a4:	mov    edx,DWORD PTR ds:0x8a9ba4
  4f58aa:	mov    ecx,DWORD PTR ds:0xbd7670
  4f58b0:	push   edx
  4f58b1:	call   0x6c5fd0
  4f58b6:	fld    DWORD PTR [eax+0x3b0]
  4f58bc:	mov    ecx,DWORD PTR ds:0xbd7670
  4f58c2:	fstp   QWORD PTR [esp+0x4]
  4f58c6:	lea    esi,[eax+0x3b0]
  4f58cc:	mov    eax,ds:0x8a9ba4
  4f58d1:	push   eax
  4f58d2:	call   0x6c5fd0
  4f58d7:	fld    DWORD PTR [eax+0x7c]
  4f58da:	push   ecx
  4f58db:	fstp   DWORD PTR [esp]
  4f58de:	call   0x409f20
  4f58e3:	fadd   st(0),st
  4f58e5:	add    esp,0x4
  4f58e8:	fsubr  QWORD PTR [esp+0x4]
  4f58ec:	fstp   DWORD PTR [esi]
  4f58ee:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4f58f4:	push   ecx
  4f58f5:	mov    ecx,DWORD PTR ds:0xbd7670
  4f58fb:	call   0x6c5fd0
  4f5900:	fld    DWORD PTR [eax+0x3b8]
  4f5906:	mov    edx,DWORD PTR ds:0x8a9ba4
  4f590c:	fstp   QWORD PTR [esp+0x4]
  4f5910:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5916:	lea    esi,[eax+0x3b8]
  4f591c:	push   edx
  4f591d:	call   0x6c5fd0
  4f5922:	fld    DWORD PTR [eax+0x7c]
  4f5925:	push   ecx
  4f5926:	fstp   DWORD PTR [esp]
  4f5929:	call   0x409f80
  4f592e:	fadd   st(0),st
  4f5930:	add    esp,0x4
  4f5933:	fsubr  QWORD PTR [esp+0x4]
  4f5937:	fstp   DWORD PTR [esi]
  4f5939:	mov    eax,ds:0x8a9ba4
  4f593e:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5944:	push   eax
  4f5945:	call   0x6c5fd0
  4f594a:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4f5950:	push   ecx
  4f5951:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5957:	lea    esi,[eax+0x3b0]
  4f595d:	call   0x6c5fd0
  4f5962:	mov    edx,DWORD PTR [esi]
  4f5964:	add    eax,0x58
  4f5967:	mov    DWORD PTR [eax],edx
  4f5969:	mov    ecx,DWORD PTR [esi+0x4]
  4f596c:	mov    DWORD PTR [eax+0x4],ecx
  4f596f:	mov    edx,DWORD PTR [esi+0x8]
  4f5972:	mov    DWORD PTR [eax+0x8],edx
  4f5975:	mov    ecx,DWORD PTR [esi+0xc]
  4f5978:	mov    DWORD PTR [eax+0xc],ecx
  4f597b:	mov    edx,DWORD PTR ds:0x8a9ba4
  4f5981:	mov    ecx,DWORD PTR ds:0xbd7670
  4f5987:	push   edx
  4f5988:	call   0x6c5fd0
  4f598d:	mov    ecx,eax
  4f598f:	call   0x4e3110
  4f5994:	mov    eax,ds:0x8a9ba4
  4f5999:	mov    ecx,DWORD PTR ds:0xbd7670
  4f599f:	push   eax
  4f59a0:	call   0x6c5fd0
  4f59a5:	fldz
  4f59a7:	fstp   DWORD PTR [eax+0x644]
  4f59ad:	call   0x520860
  4f59b2:	mov    eax,ds:0xbe1ea4
  4f59b7:	test   eax,eax
  4f59b9:	jne    0x4f59db
  4f59bb:	push   0x1ac
  4f59c0:	call   0x404390
  4f59c5:	add    esp,0x4
  4f59c8:	test   eax,eax
  4f59ca:	je     0x4f59d4
  4f59cc:	mov    DWORD PTR [eax],0x7721bc
  4f59d2:	jmp    0x4f59d6
  4f59d4:	xor    eax,eax
  4f59d6:	mov    ds:0xbe1ea4,eax
  4f59db:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4f59e1:	push   ecx
  4f59e2:	mov    ecx,DWORD PTR ds:0xbd7670
  4f59e8:	mov    esi,eax
  4f59ea:	call   0x6c5fd0
  4f59ef:	mov    edx,DWORD PTR [eax+0x58]
  4f59f2:	add    eax,0x58
  4f59f5:	mov    DWORD PTR [esi+0x1c],edx
  4f59f8:	mov    edx,DWORD PTR [eax+0x4]
  4f59fb:	lea    ecx,[esi+0x1c]
  4f59fe:	mov    DWORD PTR [ecx+0x4],edx
  4f5a01:	mov    edx,DWORD PTR [eax+0x8]
  4f5a04:	mov    DWORD PTR [ecx+0x8],edx
  4f5a07:	mov    eax,DWORD PTR [eax+0xc]
  4f5a0a:	mov    DWORD PTR [ecx+0xc],eax
  4f5a0d:	mov    eax,ds:0xbe1ea4
  4f5a12:	test   eax,eax
  4f5a14:	jne    0x4f5a36
  4f5a16:	push   0x1ac
  4f5a1b:	call   0x404390
  4f5a20:	add    esp,0x4
  4f5a23:	test   eax,eax
  4f5a25:	je     0x4f5a2f
  4f5a27:	mov    DWORD PTR [eax],0x7721bc
  4f5a2d:	jmp    0x4f5a31
  4f5a2f:	xor    eax,eax
  4f5a31:	mov    ds:0xbe1ea4,eax
  4f5a36:	fld    DWORD PTR [eax+0x20]
  4f5a39:	fadd   QWORD PTR ds:0x773748
  4f5a3f:	fstp   DWORD PTR [eax+0x20]
  4f5a42:	mov    ecx,DWORD PTR ds:0xbe1ea4
  4f5a48:	mov    edx,DWORD PTR [ecx+0x1c]
  4f5a4b:	lea    eax,[ecx+0x1c]
  4f5a4e:	mov    DWORD PTR [ecx+0xc],edx
  4f5a51:	mov    edx,DWORD PTR [eax+0x4]
  4f5a54:	add    ecx,0xc
  4f5a57:	mov    DWORD PTR [ecx+0x4],edx
  4f5a5a:	mov    edx,DWORD PTR [eax+0x8]
  4f5a5d:	mov    DWORD PTR [ecx+0x8],edx
  4f5a60:	mov    eax,DWORD PTR [eax+0xc]
  4f5a63:	mov    DWORD PTR [ecx+0xc],eax
  4f5a66:	mov    esi,DWORD PTR ds:0xbe1ea4
  4f5a6c:	add    esi,0xc
  4f5a6f:	call   0x40a320
  4f5a74:	fld    DWORD PTR [eax+0x8c5a0]
  4f5a7a:	fadd   QWORD PTR ds:0x76f640
  4f5a80:	push   ecx
  4f5a81:	fstp   DWORD PTR [esp+0x8]
  4f5a85:	fld    DWORD PTR [esp+0x8]
  4f5a89:	fstp   DWORD PTR [esp]
  4f5a8c:	call   0x409f20
  4f5a91:	fmul   QWORD PTR ds:0x773748
  4f5a97:	add    esp,0x4
  4f5a9a:	fadd   DWORD PTR [esi]
  4f5a9c:	fstp   DWORD PTR [esi]
  4f5a9e:	mov    eax,ds:0xbe1ea4
  4f5aa3:	test   eax,eax
  4f5aa5:	jne    0x4f5ac7
  4f5aa7:	push   0x1ac
  4f5aac:	call   0x404390
  4f5ab1:	add    esp,0x4
  4f5ab4:	test   eax,eax
  4f5ab6:	je     0x4f5ac0
  4f5ab8:	mov    DWORD PTR [eax],0x7721bc
  4f5abe:	jmp    0x4f5ac2
  4f5ac0:	xor    eax,eax
  4f5ac2:	mov    ds:0xbe1ea4,eax
  4f5ac7:	fld    DWORD PTR [eax+0x10]
  4f5aca:	fadd   QWORD PTR ds:0x7722c8
  4f5ad0:	fstp   DWORD PTR [eax+0x10]
  4f5ad3:	mov    esi,DWORD PTR ds:0xbe1ea4
  4f5ad9:	add    esi,0x14
  4f5adc:	call   0x40a320
  4f5ae1:	fld    DWORD PTR [eax+0x8c5a0]
  4f5ae7:	fadd   QWORD PTR ds:0x76f640
  4f5aed:	push   ecx
  4f5aee:	fstp   DWORD PTR [esp+0x8]
  4f5af2:	fld    DWORD PTR [esp+0x8]
  4f5af6:	fstp   DWORD PTR [esp]
  4f5af9:	call   0x409f80
  4f5afe:	fmul   QWORD PTR ds:0x773748
  4f5b04:	add    esp,0x4
  4f5b07:	fadd   DWORD PTR [esi]
  4f5b09:	fstp   DWORD PTR [esi]
  4f5b0b:	inc    DWORD PTR ds:0x13928a4
  4f5b11:	pop    esi
  4f5b12:	add    esp,0x8
  4f5b15:	ret
