
/mnt/data/pc_gog_controls/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

0050452a <.text+0x10352a>:
  50452a:	e8 41 d6 1d 00       	call   0x6e1b70
  50452f:	8b 3d 10 9e bd 00    	mov    edi,DWORD PTR ds:0xbd9e10
  504535:	e8 46 d6 1d 00       	call   0x6e1b80
  50453a:	0f b6 05 b4 77 bd 00 	movzx  eax,BYTE PTR ds:0xbd77b4
  504541:	68 00 00 03 00       	push   0x30000
  504546:	6a 00                	push   0x0
  504548:	50                   	push   eax
  504549:	8b cf                	mov    ecx,edi
  50454b:	e8 70 43 20 00       	call   0x7088c0
  504550:	85 c0                	test   eax,eax
  504552:	74 0c                	je     0x504560
  504554:	68 00 04 00 00       	push   0x400
  504559:	8b ce                	mov    ecx,esi
  50455b:	e8 d0 a9 ff ff       	call   0x4fef30
  504560:	8b ce                	mov    ecx,esi
  504562:	e8 c9 af ff ff       	call   0x4ff530
  504567:	83 be 54 06 00 00 00 	cmp    DWORD PTR [esi+0x654],0x0
  50456e:	75 1d                	jne    0x50458d
  504570:	f6 05 88 69 47 01 01 	test   BYTE PTR ds:0x1476988,0x1
  504577:	74 0d                	je     0x504586
  504579:	83 25 84 69 47 01 fe 	and    DWORD PTR ds:0x1476984,0xfffffffe
  504580:	5f                   	pop    edi
  504581:	5e                   	pop    esi
  504582:	83 c4 08             	add    esp,0x8
  504585:	c3                   	ret
  504586:	83 0d 84 69 47 01 01 	or     DWORD PTR ds:0x1476984,0x1
  50458d:	5f                   	pop    edi
  50458e:	5e                   	pop    esi
  50458f:	83 c4 08             	add    esp,0x8
  504592:	c3                   	ret
