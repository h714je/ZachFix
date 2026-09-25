
/mnt/data/pc_steam_controls/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

00524bf0 <.text+0x123bf0>:
  524bf0:	83 ec 18             	sub    esp,0x18
  524bf3:	56                   	push   esi
  524bf4:	be 1b 00 00 00       	mov    esi,0x1b
  524bf9:	e8 22 57 ee ff       	call   0x40a320
  524bfe:	8b c8                	mov    ecx,eax
  524c00:	e8 cb bc f2 ff       	call   0x4508d0
  524c05:	85 c0                	test   eax,eax
  524c07:	75 03                	jne    0x524c0c
  524c09:	8d 70 39             	lea    esi,[eax+0x39]
  524c0c:	a1 a4 9b 8a 00       	mov    eax,ds:0x8a9ba4
  524c11:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524c17:	50                   	push   eax
  524c18:	e8 b3 13 1a 00       	call   0x6c5fd0
  524c1d:	8b c8                	mov    ecx,eax
  524c1f:	e8 8c 9e 00 00       	call   0x52eab0
  524c24:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  524c2a:	03 c6                	add    eax,esi
  524c2c:	8d 34 80             	lea    esi,[eax+eax*4]
  524c2f:	51                   	push   ecx
  524c30:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524c36:	03 f6                	add    esi,esi
  524c38:	03 f6                	add    esi,esi
  524c3a:	e8 91 13 1a 00       	call   0x6c5fd0
  524c3f:	8b 90 94 06 00 00    	mov    edx,DWORD PTR [eax+0x694]
  524c45:	a1 a4 9b 8a 00       	mov    eax,ds:0x8a9ba4
  524c4a:	d9 44 32 08          	fld    DWORD PTR [edx+esi*1+0x8]
  524c4e:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524c54:	d9 5c 24 10          	fstp   DWORD PTR [esp+0x10]
  524c58:	50                   	push   eax
  524c59:	e8 72 13 1a 00       	call   0x6c5fd0
  524c5e:	d9 40 6c             	fld    DWORD PTR [eax+0x6c]
  524c61:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  524c67:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  524c6b:	51                   	push   ecx
  524c6c:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524c72:	e8 59 13 1a 00       	call   0x6c5fd0
  524c77:	d9 40 7c             	fld    DWORD PTR [eax+0x7c]
  524c7a:	d9 5c 24 04          	fstp   DWORD PTR [esp+0x4]
  524c7e:	51                   	push   ecx
  524c7f:	d9 44 24 14          	fld    DWORD PTR [esp+0x14]
  524c83:	dc 0d d0 f6 76 00    	fmul   QWORD PTR ds:0x76f6d0
  524c89:	d8 0d e0 ff 4a 01    	fmul   DWORD PTR ds:0x14affe0
  524c8f:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  524c93:	d9 44 24 10          	fld    DWORD PTR [esp+0x10]
  524c97:	d8 64 24 08          	fsub   DWORD PTR [esp+0x8]
  524c9b:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  524c9f:	d9 44 24 14          	fld    DWORD PTR [esp+0x14]
  524ca3:	d9 1c 24             	fstp   DWORD PTR [esp]
  524ca6:	e8 e5 51 ee ff       	call   0x409e90
  524cab:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  524caf:	83 c4 04             	add    esp,0x4
  524cb2:	d9 44 24 10          	fld    DWORD PTR [esp+0x10]
  524cb6:	5e                   	pop    esi
  524cb7:	d9 c0                	fld    st(0)
  524cb9:	d9 e1                	fabs
  524cbb:	d9 5c 24 10          	fstp   DWORD PTR [esp+0x10]
  524cbf:	d9 44 24 10          	fld    DWORD PTR [esp+0x10]
  524cc3:	d9 44 24 04          	fld    DWORD PTR [esp+0x4]
  524cc7:	de d9                	fcompp
  524cc9:	df e0                	fnstsw ax
  524ccb:	f6 c4 01             	test   ah,0x1
  524cce:	75 08                	jne    0x524cd8
  524cd0:	dd d8                	fstp   st(0)
  524cd2:	d9 44 24 08          	fld    DWORD PTR [esp+0x8]
  524cd6:	eb 3b                	jmp    0x524d13
  524cd8:	51                   	push   ecx
  524cd9:	d9 1c 24             	fstp   DWORD PTR [esp]
  524cdc:	e8 af 51 ee ff       	call   0x409e90
  524ce1:	dd d8                	fstp   st(0)
  524ce3:	83 c4 04             	add    esp,0x4
  524ce6:	d9 ee                	fldz
  524ce8:	d8 5c 24 0c          	fcomp  DWORD PTR [esp+0xc]
  524cec:	df e0                	fnstsw ax
  524cee:	d9 04 24             	fld    DWORD PTR [esp]
  524cf1:	f6 c4 05             	test   ah,0x5
  524cf4:	7a 06                	jp     0x524cfc
  524cf6:	d8 44 24 04          	fadd   DWORD PTR [esp+0x4]
  524cfa:	eb 04                	jmp    0x524d00
  524cfc:	d8 64 24 04          	fsub   DWORD PTR [esp+0x4]
  524d00:	d9 1c 24             	fstp   DWORD PTR [esp]
  524d03:	51                   	push   ecx
  524d04:	d9 44 24 04          	fld    DWORD PTR [esp+0x4]
  524d08:	d9 1c 24             	fstp   DWORD PTR [esp]
  524d0b:	e8 80 51 ee ff       	call   0x409e90
  524d10:	83 c4 04             	add    esp,0x4
  524d13:	8b 15 a4 9b 8a 00    	mov    edx,DWORD PTR ds:0x8a9ba4
  524d19:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  524d1d:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524d23:	52                   	push   edx
  524d24:	e8 a7 12 1a 00       	call   0x6c5fd0
  524d29:	d9 44 24 0c          	fld    DWORD PTR [esp+0xc]
  524d2d:	d9 58 7c             	fstp   DWORD PTR [eax+0x7c]
  524d30:	a1 a4 9b 8a 00       	mov    eax,ds:0x8a9ba4
  524d35:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524d3b:	50                   	push   eax
  524d3c:	e8 8f 12 1a 00       	call   0x6c5fd0
  524d41:	d9 40 7c             	fld    DWORD PTR [eax+0x7c]
  524d44:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  524d4a:	dd 5c 24 10          	fstp   QWORD PTR [esp+0x10]
  524d4e:	51                   	push   ecx
  524d4f:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524d55:	e8 76 12 1a 00       	call   0x6c5fd0
  524d5a:	d9 40 6c             	fld    DWORD PTR [eax+0x6c]
  524d5d:	dc 5c 24 10          	fcomp  QWORD PTR [esp+0x10]
  524d61:	df e0                	fnstsw ax
  524d63:	f6 c4 44             	test   ah,0x44
  524d66:	7a 33                	jp     0x524d9b
  524d68:	8b 15 a4 9b 8a 00    	mov    edx,DWORD PTR ds:0x8a9ba4
  524d6e:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524d74:	52                   	push   edx
  524d75:	e8 56 12 1a 00       	call   0x6c5fd0
  524d7a:	0f be 80 11 0a 00 00 	movsx  eax,BYTE PTR [eax+0xa11]
  524d81:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  524d87:	50                   	push   eax
  524d88:	51                   	push   ecx
  524d89:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524d8f:	e8 3c 12 1a 00       	call   0x6c5fd0
  524d94:	8b c8                	mov    ecx,eax
  524d96:	e8 a5 41 00 00       	call   0x528f40
  524d9b:	83 c4 18             	add    esp,0x18
  524d9e:	c3                   	ret
  524d9f:	cc                   	int3
