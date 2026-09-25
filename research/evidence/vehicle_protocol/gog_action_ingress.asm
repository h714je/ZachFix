
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

0050a130 <.text+0x109130>:
  50a130:	sub    esp,0x44
  50a133:	push   ebx
  50a134:	mov    bl,BYTE PTR [esp+0x4c]
  50a138:	push   ebp
  50a139:	push   esi
  50a13a:	mov    ebp,ecx
  50a13c:	cmp    bl,0x65
  50a13f:	je     0x50a197
  50a141:	cmp    bl,0x66
  50a144:	je     0x50a197
  50a146:	mov    eax,ds:0xbda000
  50a14b:	test   eax,eax
  50a14d:	jne    0x50a17a
  50a14f:	push   0x124d0
  50a154:	call   0x404370
  50a159:	mov    esi,eax
  50a15b:	add    esp,0x4
  50a15e:	test   esi,esi
  50a160:	je     0x50a171
  50a162:	mov    ecx,esi
  50a164:	call   0x40a240
  50a169:	mov    DWORD PTR [esi],0x76f6a4
  50a16f:	jmp    0x50a173
  50a171:	xor    esi,esi
  50a173:	mov    eax,esi
  50a175:	mov    ds:0xbda000,eax
  50a17a:	cmp    BYTE PTR [eax+0x120b7],0x0
  50a181:	je     0x50a18e
  50a183:	pop    esi
  50a184:	pop    ebp
  50a185:	xor    al,al
  50a187:	pop    ebx
  50a188:	add    esp,0x44
  50a18b:	ret    0x8
  50a18e:	cmp    BYTE PTR [eax+0x120c4],0x0
  50a195:	jne    0x50a183
  50a197:	test   DWORD PTR [ebp+0x638],0x200
  50a1a1:	jne    0x50a1b7
  50a1a3:	cmp    bl,0x5f
  50a1a6:	je     0x50a1b7
  50a1a8:	cmp    bl,0x60
  50a1ab:	je     0x50a1b7
  50a1ad:	cmp    bl,0x65
  50a1b0:	je     0x50a1b7
  50a1b2:	cmp    bl,0x66
  50a1b5:	jne    0x50a183
  50a1b7:	call   0x4ada60
  50a1bc:	test   BYTE PTR [eax+0x4],0x1
  50a1c0:	jne    0x50a183
  50a1c2:	call   0x405330
  50a1c7:	mov    ecx,eax
  50a1c9:	call   0x42d090
  50a1ce:	test   eax,eax
  50a1d0:	je     0x50a183
  50a1d2:	push   edi
  50a1d3:	mov    edi,DWORD PTR [esp+0x5c]
  50a1d7:	mov    ecx,DWORD PTR [edi]
  50a1d9:	test   ecx,ecx
  50a1db:	je     0x50a20f
  50a1dd:	movzx  eax,WORD PTR [ecx+0x2c]
  50a1e1:	cmp    ax,0xc
  50a1e5:	je     0x50a1ed
  50a1e7:	cmp    ax,0x10
  50a1eb:	jne    0x50a1fa
  50a1ed:	test   DWORD PTR [ecx+0x1c],0x20000
  50a1f4:	jne    0x50ab02
  50a1fa:	mov    eax,DWORD PTR [ecx+0xd8]
  50a200:	and    eax,0x40000000
  50a205:	xor    edx,edx
  50a207:	or     eax,edx
  50a209:	jne    0x50ab02
  50a20f:	call   0x4607d0
  50a214:	test   eax,eax
  50a216:	jl     0x50a252
  50a218:	call   0x40a2f0
  50a21d:	mov    ecx,eax
  50a21f:	call   0x450900
  50a224:	test   eax,eax
  50a226:	jne    0x50a252
  50a228:	call   0x40a2f0
  50a22d:	mov    eax,DWORD PTR [eax+0x834fe8]
  50a233:	mov    ecx,DWORD PTR [edi]
  50a235:	push   eax
  50a236:	call   0x4607d0
  50a23b:	push   eax
  50a23c:	mov    ecx,ebp
  50a23e:	call   0x460570
  50a243:	mov    ecx,eax
  50a245:	call   0x427470
  50a24a:	test   eax,eax
  50a24c:	je     0x50ab02
  50a252:	mov    esi,DWORD PTR [esp+0x58]
  50a256:	push   esi
  50a257:	mov    ecx,ebp
  50a259:	call   0x509fb0
  50a25e:	test   al,al
  50a260:	je     0x50ab02
  50a266:	mov    eax,DWORD PTR [ebp+0x654]
  50a26c:	cmp    eax,0x2e
  50a26f:	je     0x50ab02
  50a275:	cmp    eax,0x31
  50a278:	je     0x50ab02
  50a27e:	cmp    eax,0x2f
  50a281:	je     0x50ab02
  50a287:	cmp    eax,0x30
  50a28a:	je     0x50ab02
  50a290:	cmp    eax,0x14
  50a293:	jl     0x50a2ac
  50a295:	cmp    eax,0x2d
  50a298:	jge    0x50a2ac
  50a29a:	cmp    bl,0x65
  50a29d:	jb     0x50ab02
  50a2a3:	cmp    bl,0x66
  50a2a6:	ja     0x50ab02
  50a2ac:	push   0x0
  50a2ae:	push   0x800
  50a2b3:	mov    ecx,ebp
  50a2b5:	call   0x4fd930
  50a2ba:	or     eax,edx
  50a2bc:	je     0x50a3b6
  50a2c2:	movzx  eax,bl
  50a2c5:	add    eax,0xffffffd2
  50a2c8:	cmp    eax,0x14
  50a2cb:	ja     0x50a3b6
  50a2d1:	movzx  eax,BYTE PTR [eax+0x50b124]
  50a2d8:	jmp    DWORD PTR [eax*4+0x50b10c]
  50a2df:	mov    eax,DWORD PTR [ebp+0x660]
  50a2e5:	cmp    eax,0x57
  50a2e8:	je     0x50ab02
  50a2ee:	cmp    eax,0x58
  50a2f1:	je     0x50ab02
  50a2f7:	mov    eax,DWORD PTR [ebp+0x660]
  50a2fd:	cmp    eax,0x45
  50a300:	je     0x50ab02
  50a306:	cmp    eax,0x32
  50a309:	je     0x50ab02
  50a30f:	cmp    DWORD PTR [ebp+0x660],0x38
  50a316:	je     0x50ab02
  50a31c:	mov    eax,DWORD PTR [ebp+0x660]
  50a322:	cmp    eax,0x3f
  50a325:	je     0x50ab02
  50a32b:	cmp    eax,0x3c
  50a32e:	je     0x50ab02
  50a334:	cmp    eax,0x3d
  50a337:	je     0x50ab02
  50a33d:	cmp    eax,0x40
  50a340:	je     0x50ab02
  50a346:	cmp    eax,0x45
  50a349:	je     0x50ab02
  50a34f:	cmp    eax,0x4b
  50a352:	je     0x50ab02
  50a358:	cmp    eax,0x70
  50a35b:	je     0x50ab02
  50a361:	cmp    eax,0x4c
  50a364:	je     0x50ab02
  50a36a:	cmp    eax,0x4d
  50a36d:	je     0x50ab02
  50a373:	cmp    eax,0x4e
  50a376:	je     0x50ab02
  50a37c:	cmp    eax,0x3e
  50a37f:	je     0x50ab02
  50a385:	cmp    eax,0x4f
  50a388:	je     0x50ab02
  50a38e:	mov    eax,DWORD PTR [ebp+0x660]
  50a394:	cmp    eax,0x69
  50a397:	je     0x50ab02
  50a39d:	cmp    eax,0x4f
  50a3a0:	jne    0x50a3b6
  50a3a2:	call   0x40a2f0
  50a3a7:	mov    ecx,DWORD PTR [eax+0x8c5a4]
  50a3ad:	cmp    ecx,DWORD PTR [edi+0x28]
  50a3b0:	jl     0x50ab02
  50a3b6:	push   esi
  50a3b7:	mov    ecx,ebp
  50a3b9:	call   0x5092a0
  50a3be:	mov    ebx,eax
  50a3c0:	mov    DWORD PTR [esp+0x14],ebx
  50a3c4:	cmp    ebx,0x51
  50a3c7:	jne    0x50a3d1
  50a3c9:	mov    edx,DWORD PTR [edi]
  50a3cb:	mov    DWORD PTR [ebp+0x688],edx
  50a3d1:	cmp    BYTE PTR [esp+0x58],0x42
  50a3d6:	jne    0x50a3f0
  50a3d8:	mov    eax,DWORD PTR [ebp+0x654]
  50a3de:	sub    eax,0x2
  50a3e1:	je     0x50ab02
  50a3e7:	sub    eax,0x44
  50a3ea:	je     0x50ab02
  50a3f0:	cmp    ebx,0xffffffff
  50a3f3:	je     0x50ab02
  50a3f9:	cmp    DWORD PTR [ebp+0x654],ebx
  50a3ff:	jne    0x50a413
  50a401:	cmp    ebx,0x58
  50a404:	je     0x50a413
  50a406:	cmp    BYTE PTR [ebp+0xb4c],0x66
  50a40d:	jne    0x50ab02
  50a413:	push   0x0
  50a415:	push   0x800
  50a41a:	mov    ecx,ebp
  50a41c:	call   0x4fd8f0
  50a421:	mov    DWORD PTR [ebp+0x660],ebx
  50a427:	cmp    ebx,0x57
  50a42a:	je     0x50a43e
  50a42c:	cmp    ebx,0x58
  50a42f:	je     0x50a43e
  50a431:	call   0x40a2f0
  50a436:	lea    edi,[eax+0x8c57c]
  50a43c:	jmp    0x50a449
  50a43e:	call   0x40a2f0
  50a443:	lea    edi,[eax+0x838de8]
  50a449:	mov    esi,DWORD PTR [esp+0x5c]
  50a44d:	mov    ecx,0x12
  50a452:	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  50a454:	mov    al,BYTE PTR [ebp+0xb4c]
  50a45a:	cmp    al,0x65
  50a45c:	je     0x50a4b6
  50a45e:	cmp    al,0x66
  50a460:	je     0x50a4b6
  50a462:	cmp    ebx,0x57
  50a465:	je     0x50a47d
  50a467:	cmp    ebx,0x58
  50a46a:	je     0x50a47d
  50a46c:	call   0x40a2f0
  50a471:	mov    esi,DWORD PTR [esp+0x5c]
  50a475:	lea    edi,[eax+0x8c57c]
  50a47b:	jmp    0x50a4c7
  50a47d:	call   0x40a2f0
  50a482:	mov    esi,DWORD PTR [esp+0x5c]
  50a486:	lea    edi,[eax+0x838de8]
  50a48c:	mov    ecx,0x12
  50a491:	push   0x0
  50a493:	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  50a495:	push   0x1000
  50a49a:	mov    ecx,ebp
  50a49c:	call   0x4fd930
  50a4a1:	or     eax,edx
  50a4a3:	jne    0x50a4ce
  50a4a5:	call   0x40a2f0
  50a4aa:	mov    esi,DWORD PTR [esp+0x5c]
  50a4ae:	lea    edi,[eax+0x8c57c]
  50a4b4:	jmp    0x50a4c7
  50a4b6:	call   0x40a2f0
  50a4bb:	lea    esi,[ebp+0xb00]
  50a4c1:	lea    edi,[eax+0x838de8]
  50a4c7:	mov    ecx,0x12
  50a4cc:	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
