
/mnt/data/work_physx/steam/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

004ddad0 <.text+0xdcad0>:
  4ddad0:	e8 5b 40 20 00       	call   0x6e1b30
  4ddad5:	8b 3d 10 9e bd 00    	mov    edi,DWORD PTR ds:0xbd9e10
  4ddadb:	e8 60 40 20 00       	call   0x6e1b40
  4ddae0:	6a 04                	push   0x4
  4ddae2:	6a 00                	push   0x0
  4ddae4:	8b cf                	mov    ecx,edi
  4ddae6:	e8 85 ae 22 00       	call   0x708970
  4ddaeb:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  4ddaef:	8b d1                	mov    edx,ecx
  4ddaf1:	f7 da                	neg    edx
  4ddaf3:	3b c2                	cmp    eax,edx
  4ddaf5:	7d 0f                	jge    0x4ddb06
  4ddaf7:	d9 05 94 5a 77 00    	fld    DWORD PTR ds:0x775a94
  4ddafd:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  4ddb01:	e9 da 00 00 00       	jmp    0x4ddbe0
  4ddb06:	3b c1                	cmp    eax,ecx
  4ddb08:	0f 8e d2 00 00 00    	jle    0x4ddbe0
  4ddb0e:	d9 05 90 5a 77 00    	fld    DWORD PTR ds:0x775a90
  4ddb14:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  4ddb18:	e9 c3 00 00 00       	jmp    0x4ddbe0
  4ddb1d:	e8 0e 40 20 00       	call   0x6e1b30
  4ddb22:	8b 3d 10 9e bd 00    	mov    edi,DWORD PTR ds:0xbd9e10
  4ddb28:	e8 13 40 20 00       	call   0x6e1b40
  4ddb2d:	0f b6 05 b4 77 bd 00 	movzx  eax,BYTE PTR ds:0xbd77b4
  4ddb34:	6a 00                	push   0x0
  4ddb36:	6a 00                	push   0x0
  4ddb38:	50                   	push   eax
  4ddb39:	8b cf                	mov    ecx,edi
  4ddb3b:	e8 f0 ae 22 00       	call   0x708a30
  4ddb40:	d9 5c 24 18          	fstp   DWORD PTR [esp+0x18]
  4ddb44:	d9 05 08 f6 76 00    	fld    DWORD PTR ds:0x76f608
  4ddb4a:	d9 44 24 18          	fld    DWORD PTR [esp+0x18]
  4ddb4e:	d8 d1                	fcom   st(1)
  4ddb50:	df e0                	fnstsw ax
  4ddb52:	dd 05 88 5a 77 00    	fld    QWORD PTR ds:0x775a88
  4ddb58:	f6 c4 05             	test   ah,0x5
  4ddb5b:	7a 0e                	jp     0x4ddb6b
  4ddb5d:	dd d9                	fstp   st(1)
  4ddb5f:	d9 c9                	fxch   st(1)
  4ddb61:	d9 5c 24 18          	fstp   DWORD PTR [esp+0x18]
  4ddb65:	d9 44 24 18          	fld    DWORD PTR [esp+0x18]
  4ddb69:	eb 26                	jmp    0x4ddb91
  4ddb6b:	dd da                	fstp   st(2)
  4ddb6d:	d9 e8                	fld1
  4ddb6f:	d8 d1                	fcom   st(1)
  4ddb71:	df e0                	fnstsw ax
  4ddb73:	f6 c4 05             	test   ah,0x5
  4ddb76:	7a 0c                	jp     0x4ddb84
  4ddb78:	dd d9                	fstp   st(1)
  4ddb7a:	d9 5c 24 18          	fstp   DWORD PTR [esp+0x18]
  4ddb7e:	d9 44 24 18          	fld    DWORD PTR [esp+0x18]
  4ddb82:	eb 36                	jmp    0x4ddbba
  4ddb84:	dd d8                	fstp   st(0)
  4ddb86:	d9 ee                	fldz
  4ddb88:	d8 d9                	fcomp  st(1)
  4ddb8a:	df e0                	fnstsw ax
  4ddb8c:	f6 c4 41             	test   ah,0x41
  4ddb8f:	75 1e                	jne    0x4ddbaf
  4ddb91:	d9 c0                	fld    st(0)
  4ddb93:	dc c8                	fmul   st(0),st
  4ddb95:	d8 c9                	fmul   st,st(1)
  4ddb97:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddb9b:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddb9f:	d9 e1                	fabs
  4ddba1:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddba5:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddba9:	d8 ca                	fmul   st,st(2)
  4ddbab:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  4ddbaf:	d9 ee                	fldz
  4ddbb1:	d8 d9                	fcomp  st(1)
  4ddbb3:	df e0                	fnstsw ax
  4ddbb5:	f6 c4 05             	test   ah,0x5
  4ddbb8:	7a 22                	jp     0x4ddbdc
  4ddbba:	d9 c0                	fld    st(0)
  4ddbbc:	dc c8                	fmul   st(0),st
  4ddbbe:	de c9                	fmulp  st(1),st
  4ddbc0:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddbc4:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddbc8:	d9 e1                	fabs
  4ddbca:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddbce:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddbd2:	d9 e0                	fchs
  4ddbd4:	de c9                	fmulp  st(1),st
  4ddbd6:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  4ddbda:	eb 04                	jmp    0x4ddbe0
  4ddbdc:	dd d8                	fstp   st(0)
  4ddbde:	dd d8                	fstp   st(0)
  4ddbe0:	d9 86 e0 04 00 00    	fld    DWORD PTR [esi+0x4e0]
  4ddbe6:	51                   	push   ecx
  4ddbe7:	d9 5c 24 1c          	fstp   DWORD PTR [esp+0x1c]
  4ddbeb:	d9 05 e0 ff 4a 01    	fld    DWORD PTR ds:0x14affe0
  4ddbf1:	dc 0d 80 5a 77 00    	fmul   QWORD PTR ds:0x775a80
  4ddbf7:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddbfb:	d9 44 24 18          	fld    DWORD PTR [esp+0x18]
  4ddbff:	d8 64 24 1c          	fsub   DWORD PTR [esp+0x1c]
  4ddc03:	d9 5c 24 24          	fstp   DWORD PTR [esp+0x24]
  4ddc07:	d9 44 24 24          	fld    DWORD PTR [esp+0x24]
  4ddc0b:	d9 1c 24             	fstp   DWORD PTR [esp]
  4ddc0e:	e8 7d c2 f2 ff       	call   0x409e90
  4ddc13:	d9 5c 24 24          	fstp   DWORD PTR [esp+0x24]
  4ddc17:	83 c4 04             	add    esp,0x4
  4ddc1a:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddc1e:	d9 c0                	fld    st(0)
  4ddc20:	d9 e1                	fabs
  4ddc22:	d9 5c 24 24          	fstp   DWORD PTR [esp+0x24]
  4ddc26:	d9 44 24 24          	fld    DWORD PTR [esp+0x24]
  4ddc2a:	d9 44 24 1c          	fld    DWORD PTR [esp+0x1c]
  4ddc2e:	de d9                	fcompp
  4ddc30:	df e0                	fnstsw ax
  4ddc32:	f6 c4 01             	test   ah,0x1
  4ddc35:	75 08                	jne    0x4ddc3f
  4ddc37:	dd d8                	fstp   st(0)
  4ddc39:	d9 44 24 14          	fld    DWORD PTR [esp+0x14]
  4ddc3d:	eb 3d                	jmp    0x4ddc7c
  4ddc3f:	51                   	push   ecx
  4ddc40:	d9 1c 24             	fstp   DWORD PTR [esp]
  4ddc43:	e8 48 c2 f2 ff       	call   0x409e90
  4ddc48:	dd d8                	fstp   st(0)
  4ddc4a:	83 c4 04             	add    esp,0x4
  4ddc4d:	d9 ee                	fldz
  4ddc4f:	d8 5c 24 20          	fcomp  DWORD PTR [esp+0x20]
  4ddc53:	df e0                	fnstsw ax
  4ddc55:	d9 44 24 18          	fld    DWORD PTR [esp+0x18]
  4ddc59:	f6 c4 05             	test   ah,0x5
  4ddc5c:	7a 06                	jp     0x4ddc64
  4ddc5e:	d8 44 24 1c          	fadd   DWORD PTR [esp+0x1c]
  4ddc62:	eb 04                	jmp    0x4ddc68
  4ddc64:	d8 64 24 1c          	fsub   DWORD PTR [esp+0x1c]
  4ddc68:	d9 5c 24 18          	fstp   DWORD PTR [esp+0x18]
  4ddc6c:	51                   	push   ecx
  4ddc6d:	d9 44 24 1c          	fld    DWORD PTR [esp+0x1c]
  4ddc71:	d9 1c 24             	fstp   DWORD PTR [esp]
  4ddc74:	e8 17 c2 f2 ff       	call   0x409e90
  4ddc79:	83 c4 04             	add    esp,0x4
  4ddc7c:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddc80:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddc84:	d9 9e e0 04 00 00    	fstp   DWORD PTR [esi+0x4e0]
  4ddc8a:	84 db                	test   bl,bl
  4ddc8c:	74 37                	je     0x4ddcc5
  4ddc8e:	8b                   	.byte 0x8b
  4ddc8f:	0d                   	.byte 0xd
