
/mnt/data/pc_gog_controls/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

00504448 <.text+0x103448>:
  504448:	1d 68 5f 77 00       	sbb    eax,0x775f68
  50444d:	df e0                	fnstsw ax
  50444f:	f6 c4 41             	test   ah,0x41
  504452:	75 22                	jne    0x504476
  504454:	e8 97 5e f0 ff       	call   0x40a2f0
  504459:	68 00 00 80 00       	push   0x800000
  50445e:	8b c8                	mov    ecx,eax
  504460:	e8 3b 03 f5 ff       	call   0x4547a0
  504465:	85 c0                	test   eax,eax
  504467:	74 0d                	je     0x504476
  504469:	6a 00                	push   0x0
  50446b:	6a 04                	push   0x4
  50446d:	8b ce                	mov    ecx,esi
  50446f:	e8 7c 94 ff ff       	call   0x4fd8f0
  504474:	eb 0b                	jmp    0x504481
  504476:	6a 00                	push   0x0
  504478:	6a 04                	push   0x4
  50447a:	8b ce                	mov    ecx,esi
  50447c:	e8 8f 94 ff ff       	call   0x4fd910
  504481:	e8 6a 5e f0 ff       	call   0x40a2f0
  504486:	8b 80 c8 c5 08 00    	mov    eax,DWORD PTR [eax+0x8c5c8]
  50448c:	83 e0 04             	and    eax,0x4
  50448f:	33 c9                	xor    ecx,ecx
  504491:	0b c1                	or     eax,ecx
  504493:	0f 84 91 00 00 00    	je     0x50452a
  504499:	a1 a4 9b 8a 00       	mov    eax,ds:0x8a9ba4
  50449e:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  5044a4:	68 00 01 00 00       	push   0x100
  5044a9:	50                   	push   eax
  5044aa:	e8 21 16 1c 00       	call   0x6c5ad0
  5044af:	8b c8                	mov    ecx,eax
  5044b1:	e8 7a aa ff ff       	call   0x4fef30
  5044b6:	85 c0                	test   eax,eax
  5044b8:	74 70                	je     0x50452a
  5044ba:	e8 31 5e f0 ff       	call   0x40a2f0
  5044bf:	68 00 20 00 00       	push   0x2000
  5044c4:	8b c8                	mov    ecx,eax
  5044c6:	e8 15 03 f5 ff       	call   0x4547e0
  5044cb:	85 c0                	test   eax,eax
  5044cd:	74 5b                	je     0x50452a
  5044cf:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  5044d5:	51                   	push   ecx
  5044d6:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  5044dc:	e8 ef 15 1c 00       	call   0x6c5ad0
  5044e1:	d9 40 7c             	fld    DWORD PTR [eax+0x7c]
  5044e4:	dc 05 30 f6 76 00    	fadd   QWORD PTR ds:0x76f630
  5044ea:	51                   	push   ecx
  5044eb:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  5044ef:	d9 44 24 0c          	fld    DWORD PTR [esp+0xc]
  5044f3:	d9 1c 24             	fstp   DWORD PTR [esp]
  5044f6:	e8 65 59 f0 ff       	call   0x409e60
  5044fb:	8b 15 a4 9b 8a 00    	mov    edx,DWORD PTR ds:0x8a9ba4
  504501:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  504505:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  50450b:	83 c4 04             	add    esp,0x4
  50450e:	52                   	push   edx
  50450f:	e8 bc 15 1c 00       	call   0x6c5ad0
  504514:	d9 44 24 08          	fld    DWORD PTR [esp+0x8]
  504518:	6a 0b                	push   0xb
  50451a:	d9 58 6c             	fstp   DWORD PTR [eax+0x6c]
  50451d:	8b ce                	mov    ecx,esi
  50451f:	e8 ec 4a 02 00       	call   0x529010
  504524:	5f                   	pop    edi
  504525:	5e                   	pop    esi
  504526:	83 c4 08             	add    esp,0x8
  504529:	c3                   	ret
