
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

004de2a0 <.text+0xdd2a0>:
  4de2a0:	sub    esp,0x50
  4de2a3:	push   esi
  4de2a4:	mov    esi,ecx
  4de2a6:	call   0x405330
  4de2ab:	mov    ecx,eax
  4de2ad:	call   0x42d090
  4de2b2:	test   eax,eax
  4de2b4:	je     0x4de2bd
  4de2b6:	mov    ecx,esi
  4de2b8:	call   0x4dd020
  4de2bd:	push   edi
  4de2be:	call   0x405330
  4de2c3:	mov    ecx,eax
  4de2c5:	call   0x433b90
  4de2ca:	mov    ecx,DWORD PTR ds:0xbd7670
  4de2d0:	mov    edi,eax
  4de2d2:	mov    eax,ds:0x8a9ba4
  4de2d7:	push   0x0
  4de2d9:	push   eax
  4de2da:	call   0x6c5ad0
  4de2df:	add    eax,0x58
  4de2e2:	push   eax
  4de2e3:	mov    ecx,edi
  4de2e5:	call   0x540f70
  4de2ea:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de2f0:	push   ecx
  4de2f1:	mov    ecx,DWORD PTR ds:0xbd7670
  4de2f7:	call   0x6c5ad0
  4de2fc:	mov    edx,DWORD PTR [eax+0x58]
  4de2ff:	add    eax,0x58
  4de302:	mov    DWORD PTR [esp+0x8],edx
  4de306:	mov    ecx,DWORD PTR [eax+0x4]
  4de309:	mov    DWORD PTR [esp+0xc],ecx
  4de30d:	mov    edx,DWORD PTR [eax+0x8]
  4de310:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de316:	mov    DWORD PTR [esp+0x10],edx
  4de31a:	mov    eax,DWORD PTR [eax+0xc]
  4de31d:	push   ecx
  4de31e:	mov    ecx,DWORD PTR ds:0xbd7670
  4de324:	mov    DWORD PTR [esp+0x18],eax
  4de328:	call   0x6c5ad0
  4de32d:	fld    DWORD PTR [eax+0x80]
  4de333:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de339:	push   ecx
  4de33a:	mov    ecx,DWORD PTR ds:0xbd7670
  4de340:	fstp   DWORD PTR [esp]
  4de343:	push   edx
  4de344:	call   0x6c5ad0
  4de349:	fld    DWORD PTR [eax+0x78]
  4de34c:	mov    eax,ds:0x8a9ba4
  4de351:	push   ecx
  4de352:	mov    ecx,DWORD PTR ds:0xbd7670
  4de358:	fstp   DWORD PTR [esp]
  4de35b:	push   eax
  4de35c:	call   0x6c5ad0
  4de361:	fld    DWORD PTR [eax+0x7c]
  4de364:	push   ecx
  4de365:	lea    ecx,[esp+0x24]
  4de369:	fstp   DWORD PTR [esp]
  4de36c:	push   ecx
  4de36d:	call   0x73a6e2
  4de372:	lea    edx,[esp+0x18]
  4de376:	push   edx
  4de377:	push   0x14812c8
  4de37c:	lea    eax,[esp+0x10]
  4de380:	push   eax
  4de381:	call   0x73a6ee
  4de386:	fld    DWORD PTR [esp+0x8]
  4de38a:	fld    QWORD PTR ds:0x7729f0
  4de390:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de396:	fmul   st(1),st
  4de398:	push   ecx
  4de399:	mov    ecx,DWORD PTR ds:0xbd7670
  4de39f:	fxch   st(1)
  4de3a1:	fstp   DWORD PTR [esp+0xc]
  4de3a5:	fld    DWORD PTR [esp+0x10]
  4de3a9:	fmul   st,st(1)
  4de3ab:	fstp   DWORD PTR [esp+0x10]
  4de3af:	fld    DWORD PTR [esp+0x14]
  4de3b3:	fmul   st,st(1)
  4de3b5:	fstp   DWORD PTR [esp+0x14]
  4de3b9:	fmul   DWORD PTR [esp+0x18]
  4de3bd:	fstp   DWORD PTR [esp+0x18]
  4de3c1:	call   0x6c5ad0
  4de3c6:	fld    DWORD PTR [esp+0x8]
  4de3ca:	add    eax,0x58
  4de3cd:	fadd   DWORD PTR [eax]
  4de3cf:	fstp   DWORD PTR [eax]
  4de3d1:	fld    DWORD PTR [eax+0x4]
  4de3d4:	fadd   DWORD PTR [esp+0xc]
  4de3d8:	fstp   DWORD PTR [eax+0x4]
  4de3db:	fld    DWORD PTR [eax+0x8]
  4de3de:	fadd   DWORD PTR [esp+0x10]
  4de3e2:	fstp   DWORD PTR [eax+0x8]
  4de3e5:	fld    DWORD PTR [eax+0xc]
  4de3e8:	fadd   DWORD PTR [esp+0x14]
  4de3ec:	fstp   DWORD PTR [eax+0xc]
  4de3ef:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de3f5:	mov    ecx,DWORD PTR ds:0xbd7670
  4de3fb:	push   edx
  4de3fc:	call   0x6c5ad0
  4de401:	mov    ecx,DWORD PTR ds:0xbd7670
  4de407:	lea    esi,[eax+0x58]
  4de40a:	mov    eax,ds:0x8a9ba4
  4de40f:	push   eax
  4de410:	call   0x6c5ad0
  4de415:	mov    ecx,DWORD PTR [esi]
  4de417:	mov    DWORD PTR [eax+0x3b0],ecx
  4de41d:	mov    edx,DWORD PTR [esi+0x4]
  4de420:	add    eax,0x3b0
  4de425:	mov    DWORD PTR [eax+0x4],edx
  4de428:	mov    ecx,DWORD PTR [esi+0x8]
  4de42b:	mov    DWORD PTR [eax+0x8],ecx
  4de42e:	mov    edx,DWORD PTR [esi+0xc]
  4de431:	mov    DWORD PTR [eax+0xc],edx
  4de434:	mov    eax,ds:0x8a9ba4
  4de439:	mov    ecx,DWORD PTR ds:0xbd7670
  4de43f:	push   eax
  4de440:	call   0x6c5ad0
  4de445:	mov    ecx,eax
  4de447:	call   0x4e31e0
  4de44c:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de452:	push   ecx
  4de453:	mov    ecx,DWORD PTR ds:0xbd7670
  4de459:	call   0x6c5ad0
  4de45e:	or     DWORD PTR [eax+0x638],0x1
  4de465:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de46b:	mov    ecx,DWORD PTR ds:0xbd7670
  4de471:	push   0x0
  4de473:	push   0x400
  4de478:	push   edx
  4de479:	call   0x6c5ad0
  4de47e:	mov    ecx,eax
  4de480:	call   0x4fd910
  4de485:	call   0x405330
  4de48a:	mov    ecx,eax
  4de48c:	call   0x42d090
  4de491:	test   eax,eax
  4de493:	je     0x4de513
  4de495:	mov    eax,ds:0x8a9ba4
  4de49a:	mov    ecx,DWORD PTR ds:0xbd7670
  4de4a0:	push   eax
  4de4a1:	call   0x6c5ad0
  4de4a6:	fldz
  4de4a8:	fstp   DWORD PTR [eax+0x644]
  4de4ae:	push   0x0
  4de4b0:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de4b6:	push   ecx
  4de4b7:	mov    ecx,DWORD PTR ds:0xbd7670
  4de4bd:	call   0x6c5ad0
  4de4c2:	mov    ecx,eax
  4de4c4:	call   0x529010
  4de4c9:	call   0x520930
  4de4ce:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de4d4:	mov    ecx,DWORD PTR ds:0xbd7670
  4de4da:	push   edx
  4de4db:	call   0x6c5ad0
  4de4e0:	mov    ecx,DWORD PTR ds:0xbd7670
  4de4e6:	mov    esi,eax
  4de4e8:	mov    eax,ds:0x8a9ba4
  4de4ed:	push   eax
  4de4ee:	call   0x6c5ad0
  4de4f3:	fldz
  4de4f5:	mov    edx,DWORD PTR [esi]
  4de4f7:	mov    eax,DWORD PTR [eax+0x628]
  4de4fd:	mov    edx,DWORD PTR [edx+0x54]
  4de500:	push   0x0
  4de502:	push   0x1
  4de504:	sub    esp,0x8
  4de507:	fst    DWORD PTR [esp+0x4]
  4de50b:	mov    ecx,esi
  4de50d:	fstp   DWORD PTR [esp]
  4de510:	push   eax
  4de511:	call   edx
  4de513:	mov    eax,ds:0x8a9ba4
  4de518:	mov    ecx,DWORD PTR ds:0xbd7670
  4de51e:	push   eax
  4de51f:	call   0x6c5ad0
  4de524:	and    DWORD PTR [eax+0x20],0xffffffc7
  4de528:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de52e:	push   ecx
  4de52f:	mov    ecx,DWORD PTR ds:0xbd7670
  4de535:	call   0x6c5ad0
  4de53a:	mov    edx,DWORD PTR [eax+0xdc]
  4de540:	and    DWORD PTR [eax+0xd8],0x7fffffff
  4de54a:	mov    DWORD PTR [eax+0xdc],edx
  4de550:	call   0x40a2f0
  4de555:	mov    DWORD PTR [eax+0x8c5b8],0x12
  4de55f:	call   0x40a2f0
  4de564:	mov    ecx,DWORD PTR ds:0xbd7670
  4de56a:	mov    esi,eax
  4de56c:	mov    eax,ds:0x8a9ba4
  4de571:	push   eax
  4de572:	add    esi,0x8c57c
  4de578:	call   0x6c5ad0
  4de57d:	mov    eax,ds:0x148132c
  4de582:	test   eax,eax
  4de584:	je     0x4de58f
  4de586:	push   esi
  4de587:	push   0x35
  4de589:	push   edi
  4de58a:	call   eax
  4de58c:	add    esp,0xc
  4de58f:	mov    eax,DWORD PTR [edi+0x44]
  4de592:	test   eax,eax
  4de594:	je     0x4de59f
  4de596:	push   esi
  4de597:	push   0x35
  4de599:	push   edi
  4de59a:	call   eax
  4de59c:	add    esp,0xc
  4de59f:	mov    eax,ds:0xbe1ea4
  4de5a4:	pop    edi
  4de5a5:	test   eax,eax
  4de5a7:	jne    0x4de5c9
  4de5a9:	push   0x1ac
  4de5ae:	call   0x404370
  4de5b3:	add    esp,0x4
  4de5b6:	test   eax,eax
  4de5b8:	je     0x4de5c2
  4de5ba:	mov    DWORD PTR [eax],0x7721ac
  4de5c0:	jmp    0x4de5c4
  4de5c2:	xor    eax,eax
  4de5c4:	mov    ds:0xbe1ea4,eax
  4de5c9:	or     DWORD PTR [eax+0x11c],0x4
  4de5d0:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de5d6:	push   ecx
  4de5d7:	mov    ecx,DWORD PTR ds:0xbd7670
  4de5dd:	call   0x6c5ad0
  4de5e2:	mov    ecx,DWORD PTR [eax+0xdc]
  4de5e8:	and    ecx,0x4
  4de5eb:	xor    eax,eax
  4de5ed:	or     eax,ecx
  4de5ef:	jne    0x4de60d
  4de5f1:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de5f7:	mov    ecx,DWORD PTR ds:0xbd7670
  4de5fd:	push   edx
  4de5fe:	call   0x6c5ad0
  4de603:	or     DWORD PTR [eax+0x638],0x8000
  4de60d:	mov    eax,ds:0x8a9ba4
  4de612:	mov    ecx,DWORD PTR ds:0xbd7670
  4de618:	push   eax
  4de619:	call   0x6c5ad0
  4de61e:	fldz
  4de620:	fstp   DWORD PTR [eax+0x78]
  4de623:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4de629:	push   ecx
  4de62a:	mov    ecx,DWORD PTR ds:0xbd7670
  4de630:	call   0x6c5ad0
  4de635:	fldz
  4de637:	fstp   DWORD PTR [eax+0x80]
  4de63d:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de643:	mov    ecx,DWORD PTR ds:0xbd7670
  4de649:	push   edx
  4de64a:	call   0x6c5ad0
  4de64f:	mov    ecx,DWORD PTR ds:0xbd7670
  4de655:	lea    esi,[eax+0x78]
  4de658:	mov    eax,ds:0x8a9ba4
  4de65d:	push   eax
  4de65e:	call   0x6c5ad0
  4de663:	mov    ecx,DWORD PTR [esi]
  4de665:	mov    DWORD PTR [eax+0x68],ecx
  4de668:	mov    edx,DWORD PTR [esi+0x4]
  4de66b:	add    eax,0x68
  4de66e:	mov    DWORD PTR [eax+0x4],edx
  4de671:	mov    ecx,DWORD PTR [esi+0x8]
  4de674:	mov    DWORD PTR [eax+0x8],ecx
  4de677:	mov    edx,DWORD PTR [esi+0xc]
  4de67a:	mov    DWORD PTR [eax+0xc],edx
  4de67d:	mov    eax,ds:0x8a9ba4
  4de682:	mov    ecx,DWORD PTR ds:0xbd7670
  4de688:	push   eax
  4de689:	call   0x6c5ad0
  4de68e:	or     DWORD PTR [eax+0x63c],0x40
  4de695:	pop    esi
  4de696:	add    esp,0x50
  4de699:	ret
