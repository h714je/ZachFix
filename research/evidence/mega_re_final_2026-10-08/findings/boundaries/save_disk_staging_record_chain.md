# Save disk → staged image → selected CGame record mechanism

**2026-10-03; Phase4 first evidence batch, Steam only. C0146–C0148 / BND-125–127.** VERIFIED selected request, state, pointer/count and data-consumer mechanics. UNKNOWN actual thread-start/recurrence, full format validity, unconditional load→commit→actor-creation chronology, lifetime/free and GOG. Phase3 remains closed at its accepted conditional-static scope.

## Contract and provenance

Selected seq29 SAVE_RECORD_STAGING_CHAIN: new typed disk/request/completion and staging/header/record consumers; exclude accepted getter/backup/factory rediscovery, full28-record field grammar, manual builders, allocation release and GOG. Prior C0125/C0126 prove shared CGame, backup/live regions and a conditional typed CPlayer consumer; reused only at that scope. C0007 broader image/schema remains secondary STRONG_INFERENCE.

Main traced CPreserve consumers and personally verified the bounded Opus I/O branch. Actual main consumer checker:477 instructions/1740 bytes. Opus checker locally rerun:805/2621,20 target checks,39 exact anchors,5 RTTI/table checks,11 raw PE imports; independent aligned orphan objdump73 instructions. Main independently inspected241/677 file/coordinator/completion instructions plus318/1084 root/request/typed callback instructions and raw imports/tables. Counts overlap and are not additive semantic coverage. One draft promotion label initially called write op4; corrected against raw table toop2/6 **before execution/promotion**, not an executable contradiction. No supplied evidence changed.

## 1. Typed producers, admission and retained request — VERIFIED

- `00409C90` acquires a0x74 allocation, passes it to `00409BD0`, publishes the returned pointer to **00BD9FF8**. Raw wrapper table0076F604 is **TSiHolder<CSaveData>**; base0076F5FC is **CSaveData**. Actual same-receiver construction installs a **TCallBackClass<CSaveData>** at manager+2C (table0076F5C4), saves the same manager at+30 and raw target **00409980 at+34**. This is not the CGame singleton, staging image, CPreserve or movie/audio root.
- `004094C0` asks acquired **CSysutil00BD9E0C** to admit manager+2C through `00707700`. Only Sysutil+C8==0 admits and retains that callback. It can retry up to1000 unsuccessful admission checks and return without a distinct admitted-status result; do not assume every request succeeds.
- After admission, manager lock+8 protects waiting for+28==0, then writes **+28=1, +3C=operation, +40=supplied buffer, +44=byte count, +5C=6, +64=1, +65=0**. The manager retains the pointer/count, not an independent deep-copy of the image or ownership transfer. Waiting for manager idle is separate from bounded Sysutil admission retries.

## 2. Conditional execution and protocol completion — VERIFIED

`00707790` samples Sysutil+C8 under its lock and conditionally invokes callback table+8. Raw0076F5C4+8=`004091D0`; adapter uses callback+4 as actual CSaveData ECX, target+8 as00409980, and pushes callback+0C. **Same-manager/value genealogy**, not a compatible-offset join.

The separately installed orphan **00409980** reads manager+28:1→2; on2 and+65==0 it calls `00409790` on that same manager. Raw coordinator table00409868 maps **3→read00409812;2/6→write0040982B;1→set+54;other/default selected arms return0**. Read loads EBX=manager+44 and EDI=manager+40 before `00408C80`. It sets+65 around physical dispatch, then clears it and returns actual EAX; void-looking decompiler output is not authoritative.

Callback publishes actual EAX into **manager+5C**, selected6→2 remap, then+28=3. A **later selected state3 callback** clears Sysutil+C8 through00707750 and manager+28/+64/+65, retaining result+5C. Result publication and busy clear are not one instruction or measured frame occurrence.

**Outer missing edge:** CSysutil ctor installs receiver/target00707790 into embedded+94 callback configuration, but indirect start/context dispatch at0070729A was not resolved to eventual invocation in this batch. Conditional callback→I/O→completion is positive; actual worker/thread activation, thread identity and cadence are UNKNOWN, not globally absent. No Phase3 obligation reopened.

**Export boundary:**004098F0 is the CSysutil getter endingRET00409976, then9 CC bytes;00409980 is distinct orphan callback endingRET4 at00409A31. calls.csv's empty caller at004099EE is export ownership loss, not a missing executable call or getter fallthrough. No exported function row is invented for the orphan.

## 3. File size is not successful/full read — VERIFIED

Raw PE imports establish CreateFileA/GetFileSize/ReadFile/CloseHandle. `00408B20` opens **savedata/dp.sav**, GENERIC_READ/share0/OPEN_EXISTING/attribute80. Invalid handle returns0. Valid handle's **GetFileSize(h,NULL) low DWORD F** controls temporary allocation and ReadFile count, supplies the temporary to the out-pointer, closes the handle and **returns F**. **ReadFile Boolean and bytes-read local are not checked** in this selected body; allocation-result/GetFileSize-error/high-DWORD gates are likewise not present here. No runtime consequence or whole-program safety claim.

`00408C80` rejects onlyF==0. Let supplied requested count=N:

- F==N: memcpyN from temporary to supplied buffer.
- F!=N: memcpy**F**, notmin(F,N), then a signed-offset loop writes **one zero byte atF−4, F, F+4,... while offset<N−4**. Not a full-tail memset or dword store, and not exact-size rejection.
- Free temporary; returnAL1 on the nonzero-size path.

Consequently selected op3 **protocol result0 means nonzero-file-size wrapper path**, not verified ReadFile success, transferred-byte count, exact image size, format validity or reliable completion. Result1 covers the zero-size path, including invalid-open/empty-file possibilities, not all OS errors. Write was sampled only for op2/6 contrast and does not establish complete writer/error policy.

## 4. Newly typed CPreserve and read/header context — VERIFIED

In-place **CPreserve00BE5970**, raw table0077306C/COL00876C50/TD008A827C, is constructed by00468F70 from initializer0076ACF0; actual ctor installs the table and constructs **CAutoSave at+28C**. Observed byte+57C gives a0x57D lower bound, notsizeof/allocation/free closure. Selected object task **00BE5964 is separate**:00468F20 requests selector0 from the existing scene root, installs00468E80. Callback event1 explicitly switches ECX to00BE5970 and invokes00468980; task creation/setter interfaces are prior scoped reuse.

CPreserve+160 numeric **5** selects embeddedcontext+1DC→004647F0. Its byte+8F states route:

| Context state | Mechanism |
|---|---|
|0|00464550 requests op3, **buffer00BE5EF0, count0x7A2620** through actual CSavedata root; then+8F=1|
|1|00464300 independently locked samples+64 and, only whenzero,+5C; raw result0→+8F=2,1/7→4,2→3,3→6,5→5;4/6 leaveunchanged|
|2|004646C0 consumes selected header/staging fields; sets+92=1,+8F=7|
|7|sets context+90=1|

**0x7A2620 is a scalar byte count**, not a pointer to the decompiler's coincidentally located XPC string. Source and physical consumer prove this. The caller advances its context after requesting even though request admission can fail; no actual live failure claim is made.

00464590 clears0x21C0 at scratch01388510, copies **0x48 dwords=0x120** from00BE5EF0, then:

| Staging/header source | Actual CGame destination |
|---|---|
|header+3C byte|+8C608|
|header+40,13 dwords|+82EB08|
|header+74,39 dwords|+82EB3C|
|header+110..+11C four dwords|+CFF1C..+CFF28|

These are numeric data meanings, not a verified version/chapter/time schema. No header validator is asserted by this copy routine. Selected004646C0 also consumes record0+A4..A7 through four distinct decrement-and-call paths; friendly domains remain UNKNOWN.

## 5. Separate direct-live record commit and downstream use — VERIFIED

CPreserve+160 numeric **3**, not5, selects the same embeddedcontext+1DC→00468800. On contextstate[0]==2/+1D==6, it invokes `00465170`, then+1D=7; subsequent7 setsstate[0]=3. **No automatic5→3 or read→commit chronology is proved.**

00465170 first setsCGame+83500C bit1. If **00BE1EB8==0**:

1. memcpy **0x45CC0** from **00BE6010=staging+0x120** directly to **same acquired CGame+8C568**.
2.27 iterations memcpy same stride from **00C2BCD0+i*45CC0** to **CGame+D2228+i*45CC0**; selected bound27*45CC0=75C840.

If gate!=0, copies are skipped and gate becomes3. This is **not backup+BE8→0061A830**. Arithmetic **120+28*45CC0=7A2620** matches selected request/consumer boundaries and establishes this image envelope/record stride, **not every field, version, compatibility policy or full save schema**.

Postcommit calls0044F980 on actual CGame ECX; its internals are deferred. Selected numeric byte/flag gates can change+8C608 to8, clearbit02000000, and set+835020 ifbit400000. It does **not unconditionally set live bit31**. Reused C0126 reads this same live byte block into an independently typed CPlayer under its bit31/+CADAgates; positive value genealogy, not an immediate commit→factory callstack or every-load recreation claim.

## Primary references and reproduction

- Producer: inputs/decompiler/steam/DP_full.asm:9382-9545;9791-9805;10050-10106;10278-10498;10524-10560;10633-10736;878387-878479. Outer context setters/registration:872843-872852;877990-878018;881031-881040.
- Consumer: same assembly:115232-115309;115420-115476;115526-115624;116265-116322;120450-120608;120924-121013;991429-991434. Supporting inputs/decompiler/steam/DP_decompiled.c:77189-77251;77292-77450;77924-78122.
- Raw RTTI/table/import/bytes: scripts/inspect_phase4_save_consumer.py; scripts/inspect_phase4_save_io_branch.py; scratch/phase4_save_consumer_primary.json; scratch/phase4_save_io_branch.json; main independent scratch/phase4_save_io_main_verify.json and scratch/phase4_save_io_root_main_verify.json. Raw bytes outrank decompiler shapes; receipts are derived checks, not independent runtime truth.

## Exact residual obligations

Actual Sysutil outer activation, CPreserve operation/commit policy, staging writer/build/version grammar, full28 record fields,000BE1EB8 gate meaning,0044F980 domain work, actual reconstruction chronology/wholeactorworld lifetime/free and GOG remain unvisited. Existing C0125/C0126/secondary corrections and original CEvent/CMessage/rawselector/handle/asset/runtime bounds survive. The newly typed root joins the root map; it does not close Phase4 by report creation.
