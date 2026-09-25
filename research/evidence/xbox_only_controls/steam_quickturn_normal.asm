
/mnt/data/pc_steam_controls/DP_STEAM.exe:     file format pei-i386


Disassembly of section .text:

00503fb0 <.text+0x102fb0>:
  503fb0:	ff                   	(bad)
  503fb1:	ff 0b                	dec    DWORD PTR [ebx]
  503fb3:	c2 0f 84             	ret    0x840f
  503fb6:	91                   	xchg   ecx,eax
  503fb7:	00 00                	add    BYTE PTR [eax],al
  503fb9:	00 a1 a4 9b 8a 00    	add    BYTE PTR [ecx+0x8a9ba4],ah
  503fbf:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  503fc5:	68 00 01 00 00       	push   0x100
  503fca:	50                   	push   eax
  503fcb:	e8 00 20 1c 00       	call   0x6c5fd0
  503fd0:	8b c8                	mov    ecx,eax
  503fd2:	e8 89 ae ff ff       	call   0x4fee60
  503fd7:	85 c0                	test   eax,eax
  503fd9:	74 70                	je     0x50404b
  503fdb:	e8 40 63 f0 ff       	call   0x40a320
  503fe0:	68 00 01 00 00       	push   0x100
  503fe5:	8b c8                	mov    ecx,eax
  503fe7:	e8 c4 07 f5 ff       	call   0x4547b0
  503fec:	85 c0                	test   eax,eax
  503fee:	74 5b                	je     0x50404b
  503ff0:	8b 0d a4 9b 8a 00    	mov    ecx,DWORD PTR ds:0x8a9ba4
  503ff6:	51                   	push   ecx
  503ff7:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  503ffd:	e8 ce 1f 1c 00       	call   0x6c5fd0
  504002:	d9 40 7c             	fld    DWORD PTR [eax+0x7c]
  504005:	dc 05 40 f6 76 00    	fadd   QWORD PTR ds:0x76f640
  50400b:	51                   	push   ecx
  50400c:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  504010:	d9 44 24 0c          	fld    DWORD PTR [esp+0xc]
  504014:	d9 1c 24             	fstp   DWORD PTR [esp]
  504017:	e8 74 5e f0 ff       	call   0x409e90
  50401c:	8b 15 a4 9b 8a 00    	mov    edx,DWORD PTR ds:0x8a9ba4
  504022:	d9 5c 24 0c          	fstp   DWORD PTR [esp+0xc]
  504026:	8b 0d 70 76 bd 00    	mov    ecx,DWORD PTR ds:0xbd7670
  50402c:	83 c4 04             	add    esp,0x4
  50402f:	52                   	push   edx
  504030:	e8 9b 1f 1c 00       	call   0x6c5fd0
  504035:	d9 44 24 08          	fld    DWORD PTR [esp+0x8]
  504039:	6a 0b                	push   0xb
  50403b:	d9 58 6c             	fstp   DWORD PTR [eax+0x6c]
  50403e:	8b ce                	mov    ecx,esi
  504040:	e8 fb 4e 02 00       	call   0x528f40
  504045:	5f                   	pop    edi
  504046:	5e                   	pop    esi
  504047:	83 c4 08             	add    esp,0x8
  50404a:	c3                   	ret
