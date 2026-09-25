
inputs/gog/DP_GOG.exe:     file format pei-i386


Disassembly of section .text:

004dde80 <.text+0xdce80>:
  4dde80:	sub    esp,0x90
  4dde86:	push   ebx
  4dde87:	push   esi
  4dde88:	push   edi
  4dde89:	mov    ebx,ecx
  4dde8b:	call   0x40a2f0
  4dde90:	mov    esi,DWORD PTR [eax+0x8c57c]
  4dde96:	mov    eax,ds:0x8a9ba4
  4dde9b:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddea1:	push   eax
  4ddea2:	call   0x6c5ad0
  4ddea7:	fldz
  4ddea9:	fstp   DWORD PTR [eax+0x644]
  4ddeaf:	call   0x4051c0
  4ddeb4:	push   0x1
  4ddeb6:	push   0x2590
  4ddebb:	mov    ecx,eax
  4ddebd:	call   0x6b2be0
  4ddec2:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4ddec8:	push   ecx
  4ddec9:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddecf:	mov    edi,eax
  4dded1:	call   0x6c5ad0
  4dded6:	push   0x4000
  4ddedb:	mov    DWORD PTR [eax+0x628],edi
  4ddee1:	mov    DWORD PTR [eax+0x630],0x1
  4ddeeb:	mov    edx,DWORD PTR ds:0x8a9ba4
  4ddef1:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddef7:	push   0x0
  4ddef9:	push   edx
  4ddefa:	call   0x6c5ad0
  4ddeff:	mov    ecx,eax
  4ddf01:	call   0x4fd910
  4ddf06:	mov    eax,ds:0x8a9ba4
  4ddf0b:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddf11:	push   0x0
  4ddf13:	push   0x400
  4ddf18:	push   eax
  4ddf19:	call   0x6c5ad0
  4ddf1e:	mov    ecx,eax
  4ddf20:	call   0x4fd8f0
  4ddf25:	mov    ecx,DWORD PTR ds:0x8a9ba4
  4ddf2b:	push   0x87
  4ddf30:	push   ecx
  4ddf31:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddf37:	call   0x6c5ad0
  4ddf3c:	mov    ecx,eax
  4ddf3e:	call   0x529010
  4ddf43:	mov    edx,DWORD PTR ds:0x8a9ba4
  4ddf49:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddf4f:	push   edx
  4ddf50:	call   0x6c5ad0
  4ddf55:	or     DWORD PTR [eax+0x63c],0x40
  4ddf5c:	mov    eax,ds:0x8a9ba4
  4ddf61:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddf67:	push   eax
  4ddf68:	call   0x6c5ad0
  4ddf6d:	mov    ecx,DWORD PTR [eax+0xdc]
  4ddf73:	or     DWORD PTR [eax+0xd8],0x80000000
  4ddf7d:	mov    DWORD PTR [eax+0xdc],ecx
  4ddf83:	mov    edx,DWORD PTR ds:0x8a9ba4
  4ddf89:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddf8f:	push   edx
  4ddf90:	call   0x6c5ad0
  4ddf95:	and    DWORD PTR [eax+0x638],0xffff7fff
  4ddf9f:	mov    eax,ds:0x8a9ba4
  4ddfa4:	mov    ecx,DWORD PTR ds:0xbd7670
  4ddfaa:	push   eax
  4ddfab:	call   0x6c5ad0
  4ddfb0:	mov    edi,0xfffffffe
  4ddfb5:	and    DWORD PTR [eax+0x638],edi
  4ddfbb:	mov    eax,ds:0xbe1ea4
  4ddfc0:	test   eax,eax
  4ddfc2:	jne    0x4ddfe4
  4ddfc4:	push   0x1ac
  4ddfc9:	call   0x404370
  4ddfce:	add    esp,0x4
  4ddfd1:	test   eax,eax
  4ddfd3:	je     0x4ddfdd
  4ddfd5:	mov    DWORD PTR [eax],0x7721ac
  4ddfdb:	jmp    0x4ddfdf
  4ddfdd:	xor    eax,eax
  4ddfdf:	mov    ds:0xbe1ea4,eax
  4ddfe4:	and    DWORD PTR [eax+0x11c],0xfffffffd
  4ddfeb:	mov    eax,ds:0xbe1ea4
  4ddff0:	and    DWORD PTR [eax+0x11c],edi
  4ddff6:	mov    eax,ds:0xbe1ea4
  4ddffb:	or     DWORD PTR [eax+0x11c],0x8
  4de002:	mov    DWORD PTR [ebx+0x9f4],0xffffffff
  4de00c:	mov    ecx,DWORD PTR [esi+0x58]
  4de00f:	mov    edx,DWORD PTR [esi+0x5c]
  4de012:	mov    eax,DWORD PTR [esi+0x60]
  4de015:	mov    DWORD PTR [esi+0x3b0],ecx
  4de01b:	mov    ecx,DWORD PTR [esi+0x64]
  4de01e:	mov    DWORD PTR [esi+0x3b4],edx
  4de024:	mov    DWORD PTR [esi+0x3b8],eax
  4de02a:	mov    DWORD PTR [esi+0x3bc],ecx
  4de030:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de036:	mov    ecx,DWORD PTR ds:0xbd7670
  4de03c:	push   edx
  4de03d:	call   0x6c5ad0
  4de042:	fld    DWORD PTR [eax+0x58]
  4de045:	fsub   DWORD PTR [esi+0x58]
  4de048:	add    eax,0x58
  4de04b:	push   ecx
  4de04c:	fstp   DWORD PTR [esp+0x10]
  4de050:	fld    DWORD PTR [eax+0x4]
  4de053:	fsub   DWORD PTR [esi+0x5c]
  4de056:	fstp   DWORD PTR [esp+0x14]
  4de05a:	fld    DWORD PTR [eax+0x8]
  4de05d:	fsub   DWORD PTR [esi+0x60]
  4de060:	fstp   DWORD PTR [esp+0x18]
  4de064:	fld    DWORD PTR [eax+0xc]
  4de067:	lea    eax,[esp+0x20]
  4de06b:	fsub   DWORD PTR [esi+0x64]
  4de06e:	fstp   DWORD PTR [esp+0x1c]
  4de072:	fldz
  4de074:	fst    DWORD PTR [esp+0x58]
  4de078:	fst    DWORD PTR [esp+0x54]
  4de07c:	fst    DWORD PTR [esp+0x50]
  4de080:	fst    DWORD PTR [esp+0x4c]
  4de084:	fst    DWORD PTR [esp+0x44]
  4de088:	fst    DWORD PTR [esp+0x40]
  4de08c:	fst    DWORD PTR [esp+0x3c]
  4de090:	fst    DWORD PTR [esp+0x38]
  4de094:	fst    DWORD PTR [esp+0x30]
  4de098:	fst    DWORD PTR [esp+0x2c]
  4de09c:	fst    DWORD PTR [esp+0x28]
  4de0a0:	fstp   DWORD PTR [esp+0x24]
  4de0a4:	fld1
  4de0a6:	fst    DWORD PTR [esp+0x5c]
  4de0aa:	fst    DWORD PTR [esp+0x48]
  4de0ae:	fst    DWORD PTR [esp+0x34]
  4de0b2:	fstp   DWORD PTR [esp+0x20]
  4de0b6:	fld    DWORD PTR [esi+0x7c]
  4de0b9:	fchs
  4de0bb:	fstp   DWORD PTR [esp]
  4de0be:	push   eax
  4de0bf:	call   0x73a6f4
  4de0c4:	mov    edx,DWORD PTR [esp+0x10]
  4de0c8:	mov    ecx,DWORD PTR [esp+0xc]
  4de0cc:	mov    eax,DWORD PTR [esp+0x14]
  4de0d0:	mov    DWORD PTR [esp+0x8c],ecx
  4de0d7:	mov    ecx,DWORD PTR [esp+0x18]
  4de0db:	mov    DWORD PTR [esp+0x90],edx
  4de0e2:	lea    edx,[esp+0x1c]
  4de0e6:	mov    DWORD PTR [esp+0x94],eax
  4de0ed:	push   edx
  4de0ee:	lea    eax,[esp+0x60]
  4de0f2:	mov    DWORD PTR [esp+0x9c],ecx
  4de0f9:	push   eax
  4de0fa:	mov    ecx,eax
  4de0fc:	push   ecx
  4de0fd:	call   0x73a6d0
  4de102:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de108:	mov    ecx,DWORD PTR ds:0xbd7670
  4de10e:	push   0x0
  4de110:	push   0x0
  4de112:	push   0x0
  4de114:	push   0x0
  4de116:	push   0x775a30
  4de11b:	push   esi
  4de11c:	push   edx
  4de11d:	call   0x6c5ad0
  4de122:	mov    ecx,eax
  4de124:	call   0x6c2390
  4de129:	mov    eax,DWORD PTR [esi+0x134]
  4de12f:	and    DWORD PTR [esi+0x130],0xfffffff8
  4de136:	or     DWORD PTR [esi+0x130],0x1000
  4de140:	mov    ecx,eax
  4de142:	mov    DWORD PTR [esi+0x134],ecx
  4de148:	call   0x40a2f0
  4de14d:	mov    DWORD PTR [eax+0x8c5b8],0x11
  4de157:	call   0x40a2f0
  4de15c:	mov    edx,DWORD PTR ds:0x8a9ba4
  4de162:	mov    ecx,DWORD PTR ds:0xbd7670
  4de168:	mov    edi,eax
  4de16a:	push   edx
  4de16b:	add    edi,0x8c57c
  4de171:	call   0x6c5ad0
  4de176:	mov    eax,ds:0x148132c
  4de17b:	test   eax,eax
  4de17d:	je     0x4de188
  4de17f:	push   edi
  4de180:	push   0x35
  4de182:	push   esi
  4de183:	call   eax
  4de185:	add    esp,0xc
  4de188:	mov    eax,DWORD PTR [esi+0x44]
  4de18b:	test   eax,eax
  4de18d:	je     0x4de198
  4de18f:	push   edi
  4de190:	push   0x35
  4de192:	push   esi
  4de193:	call   eax
  4de195:	add    esp,0xc
  4de198:	call   0x40a2f0
  4de19d:	mov    DWORD PTR [eax+0x8c5b8],0x13
  4de1a7:	call   0x40a2f0
  4de1ac:	mov    ecx,DWORD PTR ds:0xbd7670
  4de1b2:	mov    edi,eax
  4de1b4:	mov    eax,ds:0x8a9ba4
  4de1b9:	push   eax
  4de1ba:	add    edi,0x8c57c
  4de1c0:	call   0x6c5ad0
  4de1c5:	mov    eax,ds:0x148132c
  4de1ca:	test   eax,eax
  4de1cc:	je     0x4de1d7
  4de1ce:	push   edi
  4de1cf:	push   0x35
  4de1d1:	push   esi
  4de1d2:	call   eax
  4de1d4:	add    esp,0xc
  4de1d7:	mov    eax,DWORD PTR [esi+0x44]
  4de1da:	test   eax,eax
  4de1dc:	je     0x4de1e7
  4de1de:	push   edi
  4de1df:	push   0x35
  4de1e1:	push   esi
  4de1e2:	call   eax
  4de1e4:	add    esp,0xc
  4de1e7:	mov    eax,ds:0xbe1ea4
  4de1ec:	test   eax,eax
  4de1ee:	jne    0x4de210
  4de1f0:	push   0x1ac
  4de1f5:	call   0x404370
  4de1fa:	add    esp,0x4
  4de1fd:	test   eax,eax
  4de1ff:	je     0x4de209
  4de201:	mov    DWORD PTR [eax],0x7721ac
  4de207:	jmp    0x4de20b
  4de209:	xor    eax,eax
  4de20b:	mov    ds:0xbe1ea4,eax
  4de210:	cmp    DWORD PTR [eax+0x150],0x0
  4de217:	jne    0x4de222
  4de219:	push   0x1
  4de21b:	mov    ecx,esi
  4de21d:	call   0x54a050
  4de222:	mov    ecx,ebx
  4de224:	call   0x4dcfa0
  4de229:	push   esi
  4de22a:	push   0x1
  4de22c:	call   0x4183f0
  4de231:	mov    ecx,eax
  4de233:	call   0x46f140
  4de238:	call   0x405330
  4de23d:	mov    ecx,eax
  4de23f:	call   0x42d380
  4de244:	call   0x40a2f0
  4de249:	mov    cx,WORD PTR [eax+0x9906c]
  4de250:	cmp    cx,WORD PTR [esi+0x2e]
  4de254:	je     0x4de260
  4de256:	or     DWORD PTR [esi+0x434],0x2000
  4de260:	call   0x40a2f0
  4de265:	mov    dx,WORD PTR [esi+0x2e]
  4de269:	mov    ecx,esi
  4de26b:	mov    WORD PTR [eax+0x9906c],dx
  4de272:	call   0x54e3b0
  4de277:	fld    DWORD PTR ds:0x7740c4
  4de27d:	pop    edi
  4de27e:	fstp   DWORD PTR [esi+0x134c]
  4de284:	pop    esi
  4de285:	mov    DWORD PTR [ebx+0x660],0x0
  4de28f:	pop    ebx
  4de290:	add    esp,0x90
  4de296:	ret
  4de297:	int3
  4de298:	int3
  4de299:	int3
  4de29a:	int3
  4de29b:	int3
  4de29c:	int3
  4de29d:	int3
  4de29e:	int3
  4de29f:	int3
