
/mnt/data/pc_steam_controls/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

0050445a <.text+0x10345a>:
  50445a:	e8 d1 d6 1d 00       	call   0x6e1b30
  50445f:	8b 3d 10 9e bd 00    	mov    edi,DWORD PTR ds:0xbd9e10
  504465:	e8 d6 d6 1d 00       	call   0x6e1b40
  50446a:	0f b6 05 b4 77 bd 00 	movzx  eax,BYTE PTR ds:0xbd77b4
  504471:	68 00 00 03 00       	push   0x30000
  504476:	6a 00                	push   0x0
  504478:	50                   	push   eax
  504479:	8b cf                	mov    ecx,edi
  50447b:	e8 90 44 20 00       	call   0x708910
  504480:	85 c0                	test   eax,eax
  504482:	74 0c                	je     0x504490
  504484:	68 00 04 00 00       	push   0x400
  504489:	8b ce                	mov    ecx,esi
  50448b:	e8 d0 a9 ff ff       	call   0x4fee60
  504490:	8b ce                	mov    ecx,esi
  504492:	e8 c9 af ff ff       	call   0x4ff460
  504497:	83 be 54 06 00 00 00 	cmp    DWORD PTR [esi+0x654],0x0
  50449e:	75 1d                	jne    0x5044bd
  5044a0:	f6 05 88 69 47 01 01 	test   BYTE PTR ds:0x1476988,0x1
  5044a7:	74 0d                	je     0x5044b6
  5044a9:	83 25 84 69 47 01 fe 	and    DWORD PTR ds:0x1476984,0xfffffffe
  5044b0:	5f                   	pop    edi
  5044b1:	5e                   	pop    esi
  5044b2:	83 c4 08             	add    esp,0x8
  5044b5:	c3                   	ret
  5044b6:	83 0d 84 69 47 01 01 	or     DWORD PTR ds:0x1476984,0x1
  5044bd:	5f                   	pop    edi
  5044be:	5e                   	pop    esi
  5044bf:	83 c4 08             	add    esp,0x8
  5044c2:	c3                   	ret
