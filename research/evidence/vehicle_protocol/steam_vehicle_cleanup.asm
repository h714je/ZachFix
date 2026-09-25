
inputs/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

004de1d0 <.text+0xdd1d0>:
  4de1d0:	sub    esp,0x50
  4de1d3:	push   esi
  4de1d4:	mov    esi,ecx
  4de1d6:	call   0x405360
  4de1db:	mov    ecx,eax
  4de1dd:	call   0x42d010
  4de1e2:	test   eax,eax
  4de1e4:	je     0x4de1ed
  4de1e6:	mov    ecx,esi
  4de1e8:	call   0x4dcf50
  4de1ed:	push   edi
  4de1ee:	call   0x405360
  4de1f3:	mov    ecx,eax
  4de1f5:	call   0x433b10
  4de1fa:	mov    ecx,DWORD PTR ds:0xbd7670
  4de200:	mov    edi,eax
  4de202:	mov    eax,ds:0x8a9ba4
  4de207:	push   0x0
  4de209:	push   eax
  4de20a:	call   0x6c5fd0
  4de20f:	add    eax,0x58
  4de212:	push   eax
  4de213:	mov    ecx,edi
  4de215:	call   0x540ea0
  4de21a:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de220:	push   ecx
  4de221:	mov    ecx,DWORD PTR ds:0xbd7670
  4de227:	call   0x6c5fd0
  4de22c:	mov    edx,DWORD PTR [eax+0x58]
  4de22f:	add    eax,0x58
  4de232:	mov    DWORD PTR [esp+0x8],edx
  4de236:	mov    ecx,DWORD PTR [eax+0x4]
  4de239:	mov    DWORD PTR [esp+0xc],ecx
  4de23d:	mov    edx,DWORD PTR [eax+0x8]
  4de240:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de246:	mov    DWORD PTR [esp+0x10],edx
  4de24a:	mov    eax,DWORD PTR [eax+0xc]
  4de24d:	push   ecx
  4de24e:	mov    ecx,DWORD PTR ds:0xbd7670
  4de254:	mov    DWORD PTR [esp+0x18],eax
  4de258:	call   0x6c5fd0
  4de25d:	fld    DWORD PTR [eax+0x80]
  4de263:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de269:	push   ecx
  4de26a:	mov    ecx,DWORD PTR ds:0xbd7670
  4de270:	fstp   DWORD PTR [esp]
  4de273:	push   edx
  4de274:	call   0x6c5fd0
  4de279:	fld    DWORD PTR [eax+0x78]
  4de27c:	mov    eax,ds:0x8a9ba4
  4de281:	push   ecx
  4de282:	mov    ecx,DWORD PTR ds:0xbd7670
  4de288:	fstp   DWORD PTR [esp]
  4de28b:	push   eax
  4de28c:	call   0x6c5fd0
  4de291:	fld    DWORD PTR [eax+0x7c]
  4de294:	push   ecx
  4de295:	lea    ecx,[esp+0x24]
  4de299:	fstp   DWORD PTR [esp]
  4de29c:	push   ecx
  4de29d:	call   0x73a9e2
  4de2a2:	lea    edx,[esp+0x18]
  4de2a6:	push   edx
  4de2a7:	push   0x14812c8
  4de2ac:	lea    eax,[esp+0x10]
  4de2b0:	push   eax
  4de2b1:	call   0x73a9ee
  4de2b6:	fld    DWORD PTR [esp+0x8]
  4de2ba:	fld    QWORD PTR ds:0x772a00
  4de2c0:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de2c6:	fmul   st(1),st
  4de2c8:	push   ecx
  4de2c9:	mov    ecx,DWORD PTR ds:0xbd7670
  4de2cf:	fxch   st(1)
  4de2d1:	fstp   DWORD PTR [esp+0xc]
  4de2d5:	fld    DWORD PTR [esp+0x10]
  4de2d9:	fmul   st,st(1)
  4de2db:	fstp   DWORD PTR [esp+0x10]
  4de2df:	fld    DWORD PTR [esp+0x14]
  4de2e3:	fmul   st,st(1)
  4de2e5:	fstp   DWORD PTR [esp+0x14]
  4de2e9:	fmul   DWORD PTR [esp+0x18]
  4de2ed:	fstp   DWORD PTR [esp+0x18]
  4de2f1:	call   0x6c5fd0
  4de2f6:	fld    DWORD PTR [esp+0x8]
  4de2fa:	add    eax,0x58
  4de2fd:	fadd   DWORD PTR [eax]
  4de2ff:	fstp   DWORD PTR [eax]
  4de301:	fld    DWORD PTR [eax+0x4]
  4de304:	fadd   DWORD PTR [esp+0xc]
  4de308:	fstp   DWORD PTR [eax+0x4]
  4de30b:	fld    DWORD PTR [eax+0x8]
  4de30e:	fadd   DWORD PTR [esp+0x10]
  4de312:	fstp   DWORD PTR [eax+0x8]
  4de315:	fld    DWORD PTR [eax+0xc]
  4de318:	fadd   DWORD PTR [esp+0x14]
  4de31c:	fstp   DWORD PTR [eax+0xc]
  4de31f:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de325:	mov    ecx,DWORD PTR ds:0xbd7670
  4de32b:	push   edx
  4de32c:	call   0x6c5fd0
  4de331:	mov    ecx,DWORD PTR ds:0xbd7670
  4de337:	lea    esi,[eax+0x58]
  4de33a:	mov    eax,ds:0x8a9ba4
  4de33f:	push   eax
  4de340:	call   0x6c5fd0
  4de345:	mov    ecx,DWORD PTR [esi]
  4de347:	mov    DWORD PTR [eax+0x3b0],ecx
  4de34d:	mov    edx,DWORD PTR [esi+0x4]
  4de350:	add    eax,0x3b0
  4de355:	mov    DWORD PTR [eax+0x4],edx
  4de358:	mov    ecx,DWORD PTR [esi+0x8]
  4de35b:	mov    DWORD PTR [eax+0x8],ecx
  4de35e:	mov    edx,DWORD PTR [esi+0xc]
  4de361:	mov    DWORD PTR [eax+0xc],edx
  4de364:	mov    eax,ds:0x8a9ba4
  4de369:	mov    ecx,DWORD PTR ds:0xbd7670
  4de36f:	push   eax
  4de370:	call   0x6c5fd0
  4de375:	mov    ecx,eax
  4de377:	call   0x4e3110
  4de37c:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de382:	push   ecx
  4de383:	mov    ecx,DWORD PTR ds:0xbd7670
  4de389:	call   0x6c5fd0
  4de38e:	or     DWORD PTR [eax+0x638],0x1
  4de395:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de39b:	mov    ecx,DWORD PTR ds:0xbd7670
  4de3a1:	push   0x0
  4de3a3:	push   0x400
  4de3a8:	push   edx
  4de3a9:	call   0x6c5fd0
  4de3ae:	mov    ecx,eax
  4de3b0:	call   0x4fd840
  4de3b5:	call   0x405360
  4de3ba:	mov    ecx,eax
  4de3bc:	call   0x42d010
  4de3c1:	test   eax,eax
  4de3c3:	je     0x4de443
  4de3c5:	mov    eax,ds:0x8a9ba4
  4de3ca:	mov    ecx,DWORD PTR ds:0xbd7670
  4de3d0:	push   eax
  4de3d1:	call   0x6c5fd0
  4de3d6:	fldz
  4de3d8:	fstp   DWORD PTR [eax+0x644]
  4de3de:	push   0x0
  4de3e0:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de3e6:	push   ecx
  4de3e7:	mov    ecx,DWORD PTR ds:0xbd7670
  4de3ed:	call   0x6c5fd0
  4de3f2:	mov    ecx,eax
  4de3f4:	call   0x528f40
  4de3f9:	call   0x520860
  4de3fe:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de404:	mov    ecx,DWORD PTR ds:0xbd7670
  4de40a:	push   edx
  4de40b:	call   0x6c5fd0
  4de410:	mov    ecx,DWORD PTR ds:0xbd7670
  4de416:	mov    esi,eax
  4de418:	mov    eax,ds:0x8a9ba4
  4de41d:	push   eax
  4de41e:	call   0x6c5fd0
  4de423:	fldz
  4de425:	mov    edx,DWORD PTR [esi]
  4de427:	mov    eax,DWORD PTR [eax+0x628]
  4de42d:	mov    edx,DWORD PTR [edx+0x54]
  4de430:	push   0x0
  4de432:	push   0x1
  4de434:	sub    esp,0x8
  4de437:	fst    DWORD PTR [esp+0x4]
  4de43b:	mov    ecx,esi
  4de43d:	fstp   DWORD PTR [esp]
  4de440:	push   eax
  4de441:	call   edx
  4de443:	mov    eax,ds:0x8a9ba4
  4de448:	mov    ecx,DWORD PTR ds:0xbd7670
  4de44e:	push   eax
  4de44f:	call   0x6c5fd0
  4de454:	and    DWORD PTR [eax+0x20],0xffffffc7
  4de458:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de45e:	push   ecx
  4de45f:	mov    ecx,DWORD PTR ds:0xbd7670
  4de465:	call   0x6c5fd0
  4de46a:	mov    edx,DWORD PTR [eax+0xdc]
  4de470:	and    DWORD PTR [eax+0xd8],0x7fffffff
  4de47a:	mov    DWORD PTR [eax+0xdc],edx
  4de480:	call   0x40a320
  4de485:	mov    DWORD PTR [eax+0x8c5b8],0x12
  4de48f:	call   0x40a320
  4de494:	mov    ecx,DWORD PTR ds:0xbd7670
  4de49a:	mov    esi,eax
  4de49c:	mov    eax,ds:0x8a9ba4
  4de4a1:	push   eax
  4de4a2:	add    esi,0x8c57c
  4de4a8:	call   0x6c5fd0
  4de4ad:	mov    eax,ds:0x148132c
  4de4b2:	test   eax,eax
  4de4b4:	je     0x4de4bf
  4de4b6:	push   esi
  4de4b7:	push   0x35
  4de4b9:	push   edi
  4de4ba:	call   eax
  4de4bc:	add    esp,0xc
  4de4bf:	mov    eax,DWORD PTR [edi+0x44]
  4de4c2:	test   eax,eax
  4de4c4:	je     0x4de4cf
  4de4c6:	push   esi
  4de4c7:	push   0x35
  4de4c9:	push   edi
  4de4ca:	call   eax
  4de4cc:	add    esp,0xc
  4de4cf:	mov    eax,ds:0xbe1ea4
  4de4d4:	pop    edi
  4de4d5:	test   eax,eax
  4de4d7:	jne    0x4de4f9
  4de4d9:	push   0x1ac
  4de4de:	call   0x404390
  4de4e3:	add    esp,0x4
  4de4e6:	test   eax,eax
  4de4e8:	je     0x4de4f2
  4de4ea:	mov    DWORD PTR [eax],0x7721bc
  4de4f0:	jmp    0x4de4f4
  4de4f2:	xor    eax,eax
  4de4f4:	mov    ds:0xbe1ea4,eax
  4de4f9:	or     DWORD PTR [eax+0x11c],0x4
  4de500:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de506:	push   ecx
  4de507:	mov    ecx,DWORD PTR ds:0xbd7670
  4de50d:	call   0x6c5fd0
  4de512:	mov    ecx,DWORD PTR [eax+0xdc]
  4de518:	and    ecx,0x4
  4de51b:	xor    eax,eax
  4de51d:	or     eax,ecx
  4de51f:	jne    0x4de53d
  4de521:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de527:	mov    ecx,DWORD PTR ds:0xbd7670
  4de52d:	push   edx
  4de52e:	call   0x6c5fd0
  4de533:	or     DWORD PTR [eax+0x638],0x8000
  4de53d:	mov    eax,ds:0x8a9ba4
  4de542:	mov    ecx,DWORD PTR ds:0xbd7670
  4de548:	push   eax
  4de549:	call   0x6c5fd0
  4de54e:	fldz
  4de550:	fstp   DWORD PTR [eax+0x78]
  4de553:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de559:	push   ecx
  4de55a:	mov    ecx,DWORD PTR ds:0xbd7670
  4de560:	call   0x6c5fd0
  4de565:	fldz
  4de567:	fstp   DWORD PTR [eax+0x80]
  4de56d:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de573:	mov    ecx,DWORD PTR ds:0xbd7670
  4de579:	push   edx
  4de57a:	call   0x6c5fd0
  4de57f:	mov    ecx,DWORD PTR ds:0xbd7670
  4de585:	lea    esi,[eax+0x78]
  4de588:	mov    eax,ds:0x8a9ba4
  4de58d:	push   eax
  4de58e:	call   0x6c5fd0
  4de593:	mov    ecx,DWORD PTR [esi]
  4de595:	mov    DWORD PTR [eax+0x68],ecx
  4de598:	mov    edx,DWORD PTR [esi+0x4]
  4de59b:	add    eax,0x68
  4de59e:	mov    DWORD PTR [eax+0x4],edx
  4de5a1:	mov    ecx,DWORD PTR [esi+0x8]
  4de5a4:	mov    DWORD PTR [eax+0x8],ecx
  4de5a7:	mov    edx,DWORD PTR [esi+0xc]
  4de5aa:	mov    DWORD PTR [eax+0xc],edx
  4de5ad:	mov    eax,ds:0x8a9ba4
  4de5b2:	mov    ecx,DWORD PTR ds:0xbd7670
  4de5b8:	push   eax
  4de5b9:	call   0x6c5fd0
  4de5be:	or     DWORD PTR [eax+0x63c],0x40
  4de5c5:	pop    esi
  4de5c6:	add    esp,0x50
  4de5c9:	ret
