
/mnt/data/pc_gog_controls/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

00524cc0 <.text+0x123cc0>:
  524cc0:	83 ec 18             	sub    esp,0x18
  524cc3:	56                   	push   esi
  524cc4:	be 1b 00 00 00       	mov    esi,0x1b
  524cc9:	e8 22 56 ee ff       	call   0x40a2f0
  524cce:	8b c8                	mov    ecx,eax
  524cd0:	e8 2b bc f2 ff       	call   0x450900
  524cd5:	85 c0                	test   eax,eax
  524cd7:	75 03                	jne    0x524cdc
  524cd9:	8d 70 39             	lea    esi,[eax+0x39]
  524cdc:	a1 a4 9b 8a 00       	mov    eax,ds:0x8a9ba4
  524ce1:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524ce7:	50                   	push   eax
  524ce8:	e8 e3 0d 1a 00       	call   0x6c5ad0
  524ced:	8b c8                	mov    ecx,eax
  524cef:	e8 8c 9e 00 00       	call   0x52eb80
  524cf4:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  524cfa:	03 c6                	add    eax,esi
  524cfc:	8d 34 80             	lea    esi,[eax+eax*4]
  524cff:	51                   	push   ecx
  524d00:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524d06:	03 f6                	add    esi,esi
  524d08:	03 f6                	add    esi,esi
  524d0a:	e8 c1 0d 1a 00       	call   0x6c5ad0
  524d0f:	8b 90 94 06 00 00    	mov    edx,DWORD PTR [eax+0x694]
  524d15:	a1 a4 9b 8a 00       	mov    eax,ds:0x8a9ba4
  524d1a:	d9 44 32 08          	fld    DWORD PTR [edx+esi*1+0x8]
  524d1e:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524d24:	d9 5c 24 10          	fstp   DWORD PTR [esp+0x10]
  524d28:	50                   	push   eax
  524d29:	e8 a2 0d 1a 00       	call   0x6c5ad0
  524d2e:	d9 40 6c             	fld    DWORD PTR [eax+0x6c]
  524d31:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  524d37:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  524d3b:	51                   	push   ecx
  524d3c:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524d42:	e8 89 0d 1a 00       	call   0x6c5ad0
  524d47:	d9 40 7c             	fld    DWORD PTR [eax+0x7c]
  524d4a:	d9 5c 24 04          	fstp   DWORD PTR [esp+0x4]
  524d4e:	51                   	push   ecx
  524d4f:	d9 44 24 14          	fld    DWORD PTR [esp+0x14]
  524d53:	dc 0d c0 f6 76 00    	fmul   QWORD PTR ds:0x76f6c0
  524d59:	d8 0d e0 ff 4a 01    	fmul   DWORD PTR ds:0x14affe0
  524d5f:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  524d63:	d9 44 24 10          	fld    DWORD PTR [esp+0x10]
  524d67:	d8 64 24 08          	fsub   DWORD PTR [esp+0x8]
  524d6b:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  524d6f:	d9 44 24 14          	fld    DWORD PTR [esp+0x14]
  524d73:	d9 1c 24             	fstp   DWORD PTR [esp]
  524d76:	e8 e5 50 ee ff       	call   0x409e60
  524d7b:	d9 5c 24 14          	fstp   DWORD PTR [esp+0x14]
  524d7f:	83 c4 04             	add    esp,0x4
  524d82:	d9 44 24 10          	fld    DWORD PTR [esp+0x10]
  524d86:	5e                   	pop    esi
  524d87:	d9 c0                	fld    st(0)
  524d89:	d9 e1                	fabs
  524d8b:	d9 5c 24 10          	fstp   DWORD PTR [esp+0x10]
  524d8f:	d9 44 24 10          	fld    DWORD PTR [esp+0x10]
  524d93:	d9 44 24 04          	fld    DWORD PTR [esp+0x4]
  524d97:	de d9                	fcompp
  524d99:	df e0                	fnstsw ax
  524d9b:	f6 c4 01             	test   ah,0x1
  524d9e:	75 08                	jne    0x524da8
  524da0:	dd d8                	fstp   st(0)
  524da2:	d9 44 24 08          	fld    DWORD PTR [esp+0x8]
  524da6:	eb 3b                	jmp    0x524de3
  524da8:	51                   	push   ecx
  524da9:	d9 1c 24             	fstp   DWORD PTR [esp]
  524dac:	e8 af 50 ee ff       	call   0x409e60
  524db1:	dd d8                	fstp   st(0)
  524db3:	83 c4 04             	add    esp,0x4
  524db6:	d9 ee                	fldz
  524db8:	d8 5c 24 0c          	fcomp  DWORD PTR [esp+0xc]
  524dbc:	df e0                	fnstsw ax
  524dbe:	d9 04 24             	fld    DWORD PTR [esp]
  524dc1:	f6 c4 05             	test   ah,0x5
  524dc4:	7a 06                	jp     0x524dcc
  524dc6:	d8 44 24 04          	fadd   DWORD PTR [esp+0x4]
  524dca:	eb 04                	jmp    0x524dd0
  524dcc:	d8 64 24 04          	fsub   DWORD PTR [esp+0x4]
  524dd0:	d9 1c 24             	fstp   DWORD PTR [esp]
  524dd3:	51                   	push   ecx
  524dd4:	d9 44 24 04          	fld    DWORD PTR [esp+0x4]
  524dd8:	d9 1c 24             	fstp   DWORD PTR [esp]
  524ddb:	e8 80 50 ee ff       	call   0x409e60
  524de0:	83 c4 04             	add    esp,0x4
  524de3:	8b 15 a4 9b 8a 00    	mov    edx,DWORD PTR ds:0x8a9ba4
  524de9:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  524ded:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524df3:	52                   	push   edx
  524df4:	e8 d7 0c 1a 00       	call   0x6c5ad0
  524df9:	d9 44 24 0c          	fld    DWORD PTR [esp+0xc]
  524dfd:	d9 58 7c             	fstp   DWORD PTR [eax+0x7c]
  524e00:	a1 a4 9b 8a 00       	mov    eax,ds:0x8a9ba4
  524e05:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524e0b:	50                   	push   eax
  524e0c:	e8 bf 0c 1a 00       	call   0x6c5ad0
  524e11:	d9 40 7c             	fld    DWORD PTR [eax+0x7c]
  524e14:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  524e1a:	dd 5c 24 10          	fstp   QWORD PTR [esp+0x10]
  524e1e:	51                   	push   ecx
  524e1f:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524e25:	e8 a6 0c 1a 00       	call   0x6c5ad0
  524e2a:	d9 40 6c             	fld    DWORD PTR [eax+0x6c]
  524e2d:	dc 5c 24 10          	fcomp  QWORD PTR [esp+0x10]
  524e31:	df e0                	fnstsw ax
  524e33:	f6 c4 44             	test   ah,0x44
  524e36:	7a 33                	jp     0x524e6b
  524e38:	8b 15 a4 9b 8a 00    	mov    edx,DWORD PTR ds:0x8a9ba4
  524e3e:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524e44:	52                   	push   edx
  524e45:	e8 86 0c 1a 00       	call   0x6c5ad0
  524e4a:	0f be 80 11 0a 00 00 	movsx  eax,BYTE PTR [eax+0xa11]
  524e51:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  524e57:	50                   	push   eax
  524e58:	51                   	push   ecx
  524e59:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  524e5f:	e8 6c 0c 1a 00       	call   0x6c5ad0
  524e64:	8b c8                	mov    ecx,eax
  524e66:	e8 a5 41 00 00       	call   0x529010
  524e6b:	83 c4 18             	add    esp,0x18
  524e6e:	c3                   	ret
  524e6f:	cc                   	int3
