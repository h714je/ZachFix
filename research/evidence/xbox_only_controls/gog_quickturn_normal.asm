
/mnt/data/pc_gog_controls/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

00504077 <.text+0x103077>:
  504077:	6a 00                	push   0x0
  504079:	6a 04                	push   0x4
  50407b:	8b ce                	mov    ecx,esi
  50407d:	e8 ae 98 ff ff       	call   0x4fd930
  504082:	0b c2                	or     eax,edx
  504084:	0f 84 91 00 00 00    	je     0x50411b
  50408a:	a1 a4 9b 8a 00       	mov    eax,ds:0x8a9ba4
  50408f:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  504095:	68 00 01 00 00       	push   0x100
  50409a:	50                   	push   eax
  50409b:	e8 30 1a 1c 00       	call   0x6c5ad0
  5040a0:	8b c8                	mov    ecx,eax
  5040a2:	e8 89 ae ff ff       	call   0x4fef30
  5040a7:	85 c0                	test   eax,eax
  5040a9:	74 70                	je     0x50411b
  5040ab:	e8 40 62 f0 ff       	call   0x40a2f0
  5040b0:	68 00 01 00 00       	push   0x100
  5040b5:	8b c8                	mov    ecx,eax
  5040b7:	e8 24 07 f5 ff       	call   0x4547e0
  5040bc:	85 c0                	test   eax,eax
  5040be:	74 5b                	je     0x50411b
  5040c0:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  5040c6:	51                   	push   ecx
  5040c7:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  5040cd:	e8 fe 19 1c 00       	call   0x6c5ad0
  5040d2:	d9 40 7c             	fld    DWORD PTR [eax+0x7c]
  5040d5:	dc 05 30 f6 76 00    	fadd   QWORD PTR ds:0x76f630
  5040db:	51                   	push   ecx
  5040dc:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  5040e0:	d9 44 24 0c          	fld    DWORD PTR [esp+0xc]
  5040e4:	d9 1c 24             	fstp   DWORD PTR [esp]
  5040e7:	e8 74 5d f0 ff       	call   0x409e60
  5040ec:	8b 15 a4 9b 8a 00    	mov    edx,DWORD PTR ds:0x8a9ba4
  5040f2:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  5040f6:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  5040fc:	83 c4 04             	add    esp,0x4
  5040ff:	52                   	push   edx
  504100:	e8 cb 19 1c 00       	call   0x6c5ad0
  504105:	d9 44 24 08          	fld    DWORD PTR [esp+0x8]
  504109:	6a 0b                	push   0xb
  50410b:	d9 58 6c             	fstp   DWORD PTR [eax+0x6c]
  50410e:	8b ce                	mov    ecx,esi
  504110:	e8 fb 4e 02 00       	call   0x529010
  504115:	5f                   	pop    edi
  504116:	5e                   	pop    esi
  504117:	83 c4 08             	add    esp,0x8
  50411a:	c3                   	ret
