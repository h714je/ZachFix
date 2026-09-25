
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

0050a060 <.text+0x109060>:
  50a060:	sub    esp,0x44
  50a063:	push   ebx
  50a064:	mov    bl,BYTE PTR [esp+0x4c]
  50a068:	push   ebp
  50a069:	push   esi
  50a06a:	mov    ebp,ecx
  50a06c:	cmp    bl,0x65
  50a06f:	je     0x50a0c7
  50a071:	cmp    bl,0x66
  50a074:	je     0x50a0c7
  50a076:	mov    eax,ds:0xbda000
  50a07b:	test   eax,eax
  50a07d:	jne    0x50a0aa
  50a07f:	push   0x124d0
  50a084:	call   0x404390
  50a089:	mov    esi,eax
  50a08b:	add    esp,0x4
  50a08e:	test   esi,esi
  50a090:	je     0x50a0a1
  50a092:	mov    ecx,esi
  50a094:	call   0x40a270
  50a099:	mov    DWORD PTR [esi],0x76f6b4
  50a09f:	jmp    0x50a0a3
  50a0a1:	xor    esi,esi
  50a0a3:	mov    eax,esi
  50a0a5:	mov    ds:0xbda000,eax
  50a0aa:	cmp    BYTE PTR [eax+0x120b7],0x0
  50a0b1:	je     0x50a0be
  50a0b3:	pop    esi
  50a0b4:	pop    ebp
  50a0b5:	xor    al,al
  50a0b7:	pop    ebx
  50a0b8:	add    esp,0x44
  50a0bb:	ret    0x8
  50a0be:	cmp    BYTE PTR [eax+0x120c4],0x0
  50a0c5:	jne    0x50a0b3
  50a0c7:	test   DWORD PTR [ebp+0x638],0x200
  50a0d1:	jne    0x50a0e7
  50a0d3:	cmp    bl,0x5f
  50a0d6:	je     0x50a0e7
  50a0d8:	cmp    bl,0x60
  50a0db:	je     0x50a0e7
  50a0dd:	cmp    bl,0x65
  50a0e0:	je     0x50a0e7
  50a0e2:	cmp    bl,0x66
  50a0e5:	jne    0x50a0b3
  50a0e7:	call   0x4ad980
  50a0ec:	test   BYTE PTR [eax+0x4],0x1
  50a0f0:	jne    0x50a0b3
  50a0f2:	call   0x405360
  50a0f7:	mov    ecx,eax
  50a0f9:	call   0x42d010
  50a0fe:	test   eax,eax
  50a100:	je     0x50a0b3
  50a102:	push   edi
  50a103:	mov    edi,DWORD PTR [esp+0x5c]
  50a107:	mov    ecx,DWORD PTR [edi]
  50a109:	test   ecx,ecx
  50a10b:	je     0x50a13f
  50a10d:	movzx  eax,WORD PTR [ecx+0x2c]
  50a111:	cmp    ax,0xc
  50a115:	je     0x50a11d
  50a117:	cmp    ax,0x10
  50a11b:	jne    0x50a12a
  50a11d:	test   DWORD PTR [ecx+0x1c],0x20000
  50a124:	jne    0x50aa32
  50a12a:	mov    eax,DWORD PTR [ecx+0xd8]
  50a130:	and    eax,0x40000000
  50a135:	xor    edx,edx
  50a137:	or     eax,edx
  50a139:	jne    0x50aa32
  50a13f:	call   0x4607a0
  50a144:	test   eax,eax
  50a146:	jl     0x50a182
  50a148:	call   0x40a320
  50a14d:	mov    ecx,eax
  50a14f:	call   0x4508d0
  50a154:	test   eax,eax
  50a156:	jne    0x50a182
  50a158:	call   0x40a320
  50a15d:	mov    eax,DWORD PTR [eax+0x834fe8]
  50a163:	mov    ecx,DWORD PTR [edi]
  50a165:	push   eax
  50a166:	call   0x4607a0
  50a16b:	push   eax
  50a16c:	mov    ecx,ebp
  50a16e:	call   0x460540
  50a173:	mov    ecx,eax
  50a175:	call   0x427450
  50a17a:	test   eax,eax
  50a17c:	je     0x50aa32
  50a182:	mov    esi,DWORD PTR [esp+0x58]
  50a186:	push   esi
  50a187:	mov    ecx,ebp
  50a189:	call   0x509ee0
  50a18e:	test   al,al
  50a190:	je     0x50aa32
  50a196:	mov    eax,DWORD PTR [ebp+0x654]
  50a19c:	cmp    eax,0x2e
  50a19f:	je     0x50aa32
  50a1a5:	cmp    eax,0x31
  50a1a8:	je     0x50aa32
  50a1ae:	cmp    eax,0x2f
  50a1b1:	je     0x50aa32
  50a1b7:	cmp    eax,0x30
  50a1ba:	je     0x50aa32
  50a1c0:	cmp    eax,0x14
  50a1c3:	jl     0x50a1dc
  50a1c5:	cmp    eax,0x2d
  50a1c8:	jge    0x50a1dc
  50a1ca:	cmp    bl,0x65
  50a1cd:	jb     0x50aa32
  50a1d3:	cmp    bl,0x66
  50a1d6:	ja     0x50aa32
  50a1dc:	push   0x0
  50a1de:	push   0x800
  50a1e3:	mov    ecx,ebp
  50a1e5:	call   0x4fd860
  50a1ea:	or     eax,edx
  50a1ec:	je     0x50a2e6
  50a1f2:	movzx  eax,bl
  50a1f5:	add    eax,0xffffffd2
  50a1f8:	cmp    eax,0x14
  50a1fb:	ja     0x50a2e6
  50a201:	movzx  eax,BYTE PTR [eax+0x50b054]
  50a208:	jmp    DWORD PTR [eax*4+0x50b03c]
  50a20f:	mov    eax,DWORD PTR [ebp+0x660]
  50a215:	cmp    eax,0x57
  50a218:	je     0x50aa32
  50a21e:	cmp    eax,0x58
  50a221:	je     0x50aa32
  50a227:	mov    eax,DWORD PTR [ebp+0x660]
  50a22d:	cmp    eax,0x45
  50a230:	je     0x50aa32
  50a236:	cmp    eax,0x32
  50a239:	je     0x50aa32
  50a23f:	cmp    DWORD PTR [ebp+0x660],0x38
  50a246:	je     0x50aa32
  50a24c:	mov    eax,DWORD PTR [ebp+0x660]
  50a252:	cmp    eax,0x3f
  50a255:	je     0x50aa32
  50a25b:	cmp    eax,0x3c
  50a25e:	je     0x50aa32
  50a264:	cmp    eax,0x3d
  50a267:	je     0x50aa32
  50a26d:	cmp    eax,0x40
  50a270:	je     0x50aa32
  50a276:	cmp    eax,0x45
  50a279:	je     0x50aa32
  50a27f:	cmp    eax,0x4b
  50a282:	je     0x50aa32
  50a288:	cmp    eax,0x70
  50a28b:	je     0x50aa32
  50a291:	cmp    eax,0x4c
  50a294:	je     0x50aa32
  50a29a:	cmp    eax,0x4d
  50a29d:	je     0x50aa32
  50a2a3:	cmp    eax,0x4e
  50a2a6:	je     0x50aa32
  50a2ac:	cmp    eax,0x3e
  50a2af:	je     0x50aa32
  50a2b5:	cmp    eax,0x4f
  50a2b8:	je     0x50aa32
  50a2be:	mov    eax,DWORD PTR [ebp+0x660]
  50a2c4:	cmp    eax,0x69
  50a2c7:	je     0x50aa32
  50a2cd:	cmp    eax,0x4f
  50a2d0:	jne    0x50a2e6
  50a2d2:	call   0x40a320
  50a2d7:	mov    ecx,DWORD PTR [eax+0x8c5a4]
  50a2dd:	cmp    ecx,DWORD PTR [edi+0x28]
  50a2e0:	jl     0x50aa32
  50a2e6:	push   esi
  50a2e7:	mov    ecx,ebp
  50a2e9:	call   0x5091d0
  50a2ee:	mov    ebx,eax
  50a2f0:	mov    DWORD PTR [esp+0x14],ebx
  50a2f4:	cmp    ebx,0x51
  50a2f7:	jne    0x50a301
  50a2f9:	mov    edx,DWORD PTR [edi]
  50a2fb:	mov    DWORD PTR [ebp+0x688],edx
  50a301:	cmp    BYTE PTR [esp+0x58],0x42
  50a306:	jne    0x50a320
  50a308:	mov    eax,DWORD PTR [ebp+0x654]
  50a30e:	sub    eax,0x2
  50a311:	je     0x50aa32
  50a317:	sub    eax,0x44
  50a31a:	je     0x50aa32
  50a320:	cmp    ebx,0xffffffff
  50a323:	je     0x50aa32
  50a329:	cmp    DWORD PTR [ebp+0x654],ebx
  50a32f:	jne    0x50a343
  50a331:	cmp    ebx,0x58
  50a334:	je     0x50a343
  50a336:	cmp    BYTE PTR [ebp+0xb4c],0x66
  50a33d:	jne    0x50aa32
  50a343:	push   0x0
  50a345:	push   0x800
  50a34a:	mov    ecx,ebp
  50a34c:	call   0x4fd820
  50a351:	mov    DWORD PTR [ebp+0x660],ebx
  50a357:	cmp    ebx,0x57
  50a35a:	je     0x50a36e
  50a35c:	cmp    ebx,0x58
  50a35f:	je     0x50a36e
  50a361:	call   0x40a320
  50a366:	lea    edi,[eax+0x8c57c]
  50a36c:	jmp    0x50a379
  50a36e:	call   0x40a320
  50a373:	lea    edi,[eax+0x838de8]
  50a379:	mov    esi,DWORD PTR [esp+0x5c]
  50a37d:	mov    ecx,0x12
  50a382:	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  50a384:	mov    al,BYTE PTR [ebp+0xb4c]
  50a38a:	cmp    al,0x65
  50a38c:	je     0x50a3e6
  50a38e:	cmp    al,0x66
  50a390:	je     0x50a3e6
  50a392:	cmp    ebx,0x57
  50a395:	je     0x50a3ad
  50a397:	cmp    ebx,0x58
  50a39a:	je     0x50a3ad
  50a39c:	call   0x40a320
  50a3a1:	mov    esi,DWORD PTR [esp+0x5c]
  50a3a5:	lea    edi,[eax+0x8c57c]
  50a3ab:	jmp    0x50a3f7
  50a3ad:	call   0x40a320
  50a3b2:	mov    esi,DWORD PTR [esp+0x5c]
  50a3b6:	lea    edi,[eax+0x838de8]
  50a3bc:	mov    ecx,0x12
  50a3c1:	push   0x0
  50a3c3:	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  50a3c5:	push   0x1000
  50a3ca:	mov    ecx,ebp
  50a3cc:	call   0x4fd860
  50a3d1:	or     eax,edx
  50a3d3:	jne    0x50a3fe
  50a3d5:	call   0x40a320
  50a3da:	mov    esi,DWORD PTR [esp+0x5c]
  50a3de:	lea    edi,[eax+0x8c57c]
  50a3e4:	jmp    0x50a3f7
  50a3e6:	call   0x40a320
  50a3eb:	lea    esi,[ebp+0xb00]
  50a3f1:	lea    edi,[eax+0x838de8]
  50a3f7:	mov    ecx,0x12
  50a3fc:	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
