
/mnt/data/pc_steam_controls/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

00504378 <.text+0x103378>:
  504378:	1d 78 5f 77 00       	sbb    eax,0x775f78
  50437d:	df e0                	fnstsw ax
  50437f:	f6 c4 41             	test   ah,0x41
  504382:	75 22                	jne    0x5043a6
  504384:	e8 97 5f f0 ff       	call   0x40a320
  504389:	68 00 00 80 00       	push   0x800000
  50438e:	8b c8                	mov    ecx,eax
  504390:	e8 db 03 f5 ff       	call   0x454770
  504395:	85 c0                	test   eax,eax
  504397:	74 0d                	je     0x5043a6
  504399:	6a 00                	push   0x0
  50439b:	6a 04                	push   0x4
  50439d:	8b ce                	mov    ecx,esi
  50439f:	e8 7c 94 ff ff       	call   0x4fd820
  5043a4:	eb 0b                	jmp    0x5043b1
  5043a6:	6a 00                	push   0x0
  5043a8:	6a 04                	push   0x4
  5043aa:	8b ce                	mov    ecx,esi
  5043ac:	e8 8f 94 ff ff       	call   0x4fd840
  5043b1:	e8 6a 5f f0 ff       	call   0x40a320
  5043b6:	8b 80 c8 c5 08 00    	mov    eax,DWORD PTR [eax+0x8c5c8]
  5043bc:	83 e0 04             	and    eax,0x4
  5043bf:	33 c9                	xor    ecx,ecx
  5043c1:	0b c1                	or     eax,ecx
  5043c3:	0f 84 91 00 00 00    	je     0x50445a
  5043c9:	a1 a4 9b 8a 00       	mov    eax,ds:0x8a9ba4
  5043ce:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  5043d4:	68 00 01 00 00       	push   0x100
  5043d9:	50                   	push   eax
  5043da:	e8 f1 1b 1c 00       	call   0x6c5fd0
  5043df:	8b c8                	mov    ecx,eax
  5043e1:	e8 7a aa ff ff       	call   0x4fee60
  5043e6:	85 c0                	test   eax,eax
  5043e8:	74 70                	je     0x50445a
  5043ea:	e8 31 5f f0 ff       	call   0x40a320
  5043ef:	68 00 20 00 00       	push   0x2000
  5043f4:	8b c8                	mov    ecx,eax
  5043f6:	e8 b5 03 f5 ff       	call   0x4547b0
  5043fb:	85 c0                	test   eax,eax
  5043fd:	74 5b                	je     0x50445a
  5043ff:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  504405:	51                   	push   ecx
  504406:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  50440c:	e8 bf 1b 1c 00       	call   0x6c5fd0
  504411:	d9 40 7c             	fld    DWORD PTR [eax+0x7c]
  504414:	dc 05 40 f6 76 00    	fadd   QWORD PTR ds:0x76f640
  50441a:	51                   	push   ecx
  50441b:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  50441f:	d9 44 24 0c          	fld    DWORD PTR [esp+0xc]
  504423:	d9 1c 24             	fstp   DWORD PTR [esp]
  504426:	e8 65 5a f0 ff       	call   0x409e90
  50442b:	8b 15 a4 9b 8a 00    	mov    edx,DWORD PTR ds:0x8a9ba4
  504431:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  504435:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  50443b:	83 c4 04             	add    esp,0x4
  50443e:	52                   	push   edx
  50443f:	e8 8c 1b 1c 00       	call   0x6c5fd0
  504444:	d9 44 24 08          	fld    DWORD PTR [esp+0x8]
  504448:	6a 0b                	push   0xb
  50444a:	d9 58 6c             	fstp   DWORD PTR [eax+0x6c]
  50444d:	8b ce                	mov    ecx,esi
  50444f:	e8 ec 4a 02 00       	call   0x528f40
  504454:	5f                   	pop    edi
  504455:	5e                   	pop    esi
  504456:	83 c4 08             	add    esp,0x8
  504459:	c3                   	ret
