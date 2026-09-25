
/mnt/data/work_physx/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

004ddba0 <.text+0xdcba0>:
  4ddba0:	e8 cb 3f 20 00       	call   0x6e1b70
  4ddba5:	8b 3d 10 9e bd 00    	mov    edi,DWORD PTR ds:0xbd9e10
  4ddbab:	e8 d0 3f 20 00       	call   0x6e1b80
  4ddbb0:	6a 04                	push   0x4
  4ddbb2:	6a 00                	push   0x0
  4ddbb4:	8b cf                	mov    ecx,edi
  4ddbb6:	e8 65 ad 22 00       	call   0x708920
  4ddbbb:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  4ddbbf:	8b d1                	mov    edx,ecx
  4ddbc1:	f7 da                	neg    edx
  4ddbc3:	3b c2                	cmp    eax,edx
  4ddbc5:	7d 0f                	jge    0x4ddbd6
  4ddbc7:	d9 05 84 5a 77 00    	fld    DWORD PTR ds:0x775a84
  4ddbcd:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  4ddbd1:	e9 da 00 00 00       	jmp    0x4ddcb0
  4ddbd6:	3b c1                	cmp    eax,ecx
  4ddbd8:	0f 8e d2 00 00 00    	jle    0x4ddcb0
  4ddbde:	d9 05 80 5a 77 00    	fld    DWORD PTR ds:0x775a80
  4ddbe4:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  4ddbe8:	e9 c3 00 00 00       	jmp    0x4ddcb0
  4ddbed:	e8 7e 3f 20 00       	call   0x6e1b70
  4ddbf2:	8b 3d 10 9e bd 00    	mov    edi,DWORD PTR ds:0xbd9e10
  4ddbf8:	e8 83 3f 20 00       	call   0x6e1b80
  4ddbfd:	0f b6 05 b4 77 bd 00 	movzx  eax,BYTE PTR ds:0xbd77b4
  4ddc04:	6a 00                	push   0x0
  4ddc06:	6a 00                	push   0x0
  4ddc08:	50                   	push   eax
  4ddc09:	8b cf                	mov    ecx,edi
  4ddc0b:	e8 d0 ad 22 00       	call   0x7089e0
  4ddc10:	d9 5c 24 18          	fstp   DWORD PTR [esp+0x18]
  4ddc14:	d9 05 f8 f5 76 00    	fld    DWORD PTR ds:0x76f5f8
  4ddc1a:	d9 44 24 18          	fld    DWORD PTR [esp+0x18]
  4ddc1e:	d8 d1                	fcom   st(1)
  4ddc20:	df e0                	fnstsw ax
  4ddc22:	dd 05 78 5a 77 00    	fld    QWORD PTR ds:0x775a78
  4ddc28:	f6 c4 05             	test   ah,0x5
  4ddc2b:	7a 0e                	jp     0x4ddc3b
  4ddc2d:	dd d9                	fstp   st(1)
  4ddc2f:	d9 c9                	fxch   st(1)
  4ddc31:	d9 5c 24 18          	fstp   DWORD PTR [esp+0x18]
  4ddc35:	d9 44 24 18          	fld    DWORD PTR [esp+0x18]
  4ddc39:	eb 26                	jmp    0x4ddc61
  4ddc3b:	dd da                	fstp   st(2)
  4ddc3d:	d9 e8                	fld1
  4ddc3f:	d8 d1                	fcom   st(1)
  4ddc41:	df e0                	fnstsw ax
  4ddc43:	f6 c4 05             	test   ah,0x5
  4ddc46:	7a 0c                	jp     0x4ddc54
  4ddc48:	dd d9                	fstp   st(1)
  4ddc4a:	d9 5c 24 18          	fstp   DWORD PTR [esp+0x18]
  4ddc4e:	d9 44 24 18          	fld    DWORD PTR [esp+0x18]
  4ddc52:	eb 36                	jmp    0x4ddc8a
  4ddc54:	dd d8                	fstp   st(0)
  4ddc56:	d9 ee                	fldz
  4ddc58:	d8 d9                	fcomp  st(1)
  4ddc5a:	df e0                	fnstsw ax
  4ddc5c:	f6 c4 41             	test   ah,0x41
  4ddc5f:	75 1e                	jne    0x4ddc7f
  4ddc61:	d9 c0                	fld    st(0)
  4ddc63:	dc c8                	fmul   st(0),st
  4ddc65:	d8 c9                	fmul   st,st(1)
  4ddc67:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddc6b:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddc6f:	d9 e1                	fabs
  4ddc71:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddc75:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddc79:	d8 ca                	fmul   st,st(2)
  4ddc7b:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  4ddc7f:	d9 ee                	fldz
  4ddc81:	d8 d9                	fcomp  st(1)
  4ddc83:	df e0                	fnstsw ax
  4ddc85:	f6 c4 05             	test   ah,0x5
  4ddc88:	7a 22                	jp     0x4ddcac
  4ddc8a:	d9 c0                	fld    st(0)
  4ddc8c:	dc c8                	fmul   st(0),st
  4ddc8e:	de c9                	fmulp  st(1),st
  4ddc90:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddc94:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddc98:	d9 e1                	fabs
  4ddc9a:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddc9e:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddca2:	d9 e0                	fchs
  4ddca4:	de c9                	fmulp  st(1),st
  4ddca6:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  4ddcaa:	eb 04                	jmp    0x4ddcb0
  4ddcac:	dd d8                	fstp   st(0)
  4ddcae:	dd d8                	fstp   st(0)
  4ddcb0:	d9 86 e0 04 00 00    	fld    DWORD PTR [esi+0x4e0]
  4ddcb6:	51                   	push   ecx
  4ddcb7:	d9 5c 24 1c          	fstp   DWORD PTR [esp+0x1c]
  4ddcbb:	d9 05 e0 ff 4a 01    	fld    DWORD PTR ds:0x14affe0
  4ddcc1:	dc 0d 70 5a 77 00    	fmul   QWORD PTR ds:0x775a70
  4ddcc7:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddccb:	d9 44 24 18          	fld    DWORD PTR [esp+0x18]
  4ddccf:	d8 64 24 1c          	fsub   DWORD PTR [esp+0x1c]
  4ddcd3:	d9 5c 24 24          	fstp   DWORD PTR [esp+0x24]
  4ddcd7:	d9 44 24 24          	fld    DWORD PTR [esp+0x24]
  4ddcdb:	d9 1c 24             	fstp   DWORD PTR [esp]
  4ddcde:	e8 7d c1 f2 ff       	call   0x409e60
  4ddce3:	d9 5c 24 24          	fstp   DWORD PTR [esp+0x24]
  4ddce7:	83 c4 04             	add    esp,0x4
  4ddcea:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddcee:	d9 c0                	fld    st(0)
  4ddcf0:	d9 e1                	fabs
  4ddcf2:	d9 5c 24 24          	fstp   DWORD PTR [esp+0x24]
  4ddcf6:	d9 44 24 24          	fld    DWORD PTR [esp+0x24]
  4ddcfa:	d9 44 24 1c          	fld    DWORD PTR [esp+0x1c]
  4ddcfe:	de d9                	fcompp
  4ddd00:	df e0                	fnstsw ax
  4ddd02:	f6 c4 01             	test   ah,0x1
  4ddd05:	75 08                	jne    0x4ddd0f
  4ddd07:	dd d8                	fstp   st(0)
  4ddd09:	d9 44 24 14          	fld    DWORD PTR [esp+0x14]
  4ddd0d:	eb 3d                	jmp    0x4ddd4c
  4ddd0f:	51                   	push   ecx
  4ddd10:	d9 1c 24             	fstp   DWORD PTR [esp]
  4ddd13:	e8 48 c1 f2 ff       	call   0x409e60
  4ddd18:	dd d8                	fstp   st(0)
  4ddd1a:	83 c4 04             	add    esp,0x4
  4ddd1d:	d9 ee                	fldz
  4ddd1f:	d8 5c 24 20          	fcomp  DWORD PTR [esp+0x20]
  4ddd23:	df e0                	fnstsw ax
  4ddd25:	d9 44 24 18          	fld    DWORD PTR [esp+0x18]
  4ddd29:	f6 c4 05             	test   ah,0x5
  4ddd2c:	7a 06                	jp     0x4ddd34
  4ddd2e:	d8 44 24 1c          	fadd   DWORD PTR [esp+0x1c]
  4ddd32:	eb 04                	jmp    0x4ddd38
  4ddd34:	d8 64 24 1c          	fsub   DWORD PTR [esp+0x1c]
  4ddd38:	d9 5c 24 18          	fstp   DWORD PTR [esp+0x18]
  4ddd3c:	51                   	push   ecx
  4ddd3d:	d9 44 24 1c          	fld    DWORD PTR [esp+0x1c]
  4ddd41:	d9 1c 24             	fstp   DWORD PTR [esp]
  4ddd44:	e8 17 c1 f2 ff       	call   0x409e60
  4ddd49:	83 c4 04             	add    esp,0x4
  4ddd4c:	d9 5c 24 20          	fstp   DWORD PTR [esp+0x20]
  4ddd50:	d9 44 24 20          	fld    DWORD PTR [esp+0x20]
  4ddd54:	d9 9e e0 04 00 00    	fstp   DWORD PTR [esi+0x4e0]
  4ddd5a:	84 db                	test   bl,bl
  4ddd5c:	74 37                	je     0x4ddd95
  4ddd5e:	8b                   	.byte 0x8b
  4ddd5f:	0d                   	.byte 0xd
