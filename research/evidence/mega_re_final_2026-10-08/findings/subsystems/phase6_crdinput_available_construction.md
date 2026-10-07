# CRdInput available base initialization and singleton publication

## Qualified evidence

**VERIFIED C0298, Steam only:** fresh-primary172 replay of sealed170 own006E1750[006E1750,006E1783)17 instructions/51 bytes, retained168 getter00405270[00405270,004052E0)35/112, and retained direct table/RTTI raw records. Source: scratch/phase6_main_crdinput_retained_qualification_seq0172.json. Exact PE bytes, ASM line/text, geometry and direct call target match; no new acquisition or gap/callee widening.

Incoming initializer ECX is saved as R. Direct stores: R0=00826CDC at006E175A; R4=DWORD1 at006E1763; RC=x87 float0 at006E176F; R8=DWORD0 at006E1775. Final EAX=R at006E177C. No calls or global writes in this own body. The observed direct write endpoint is R+0x10, not sizeof or independently proven valid runtime extent.

Base-table00826CDC has retained COL0087C14C, TD00885D40, CHD008743EC and self descriptor008743D0. The raw declaration is CRdInput at mdisp0. Slot extent/method semantics were not acquired.

At accepted getter004052B7, saved nonzero allocation-result ESI is copied to ECX;004052B9 calls this exact initializer. Its own body does not change ESI. On normal return004052BE installs0076E830 at the same ESI+0;004052C8/CA copies ESI to EAX and publishes it to00BD9E44. The base constructor return is not used to select another receiver. Retained wrapper RTTI declares CSingleton<CRdInput> and CRdInput both at0, sharing descriptor008743D0/TD00885D40. This is an available same-receiver base-init → wrapper-table → cache-publication recipe.

## Identity distinctions

| Layer | State / scope |
|---|---|
|Cache identity|VERIFIED static00BD9E44 read/publication instructions; current value UNKNOWN/non-file-backed.|
|Table identity|VERIFIED raw00826CDC base and0076E830 wrapper declarations; not current table.|
|Initialized receiver|VERIFIED incomingECX and getter's saved allocation result are the same operand in this available normal path.|
|Service role|UNKNOWN pending selected consumer543 qualification; no OS/backend role from spelling.|
|Ownership/lifetime|UNKNOWN; publication is not exclusive ownership or safe retirement.|
|Current instance/generation continuity|UNKNOWN; no runtime sample, joined chronology or guard against replacement.|
|Runtime acquisition|UNKNOWN actual execution; available request10 is not actual allocation size/success.|
|Lifecycle responsibility|UNKNOWN; constructor recipe does not identify activation, scheduling, shutdown or final user.|

**UNKNOWN relationship to C0293:** separate00BD9E54/request7DC/00403360 acquisition has different cache/initializer coordinates and no qualified join. Keep separate in the map. This is neither proof that the mechanisms are the same service/interface/generation nor a universal runtime-disjointness guarantee.

## Architectural value and stopping edge

This refines the available CRdInput cache's construction geometry (convergence B), not a new root/subsystem or complete input architecture. RD_ROOT123's constructor subquestion is resolved at available-static scope. Its independent placement question remains finite: selected consumer0044E120 own543. Separate RD_ROOT59 initializer80 and largeWorld/Event concerns remain active. No new readiness verdict, boundary/homology, BND-244 or Phase7.

All C0281–C0297 qualifications and155/158 correction histories remain intact. Source-only170 has first canonical promotion here after fresh personal qualification; stopped170 agents receive none.
