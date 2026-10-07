# SAVE_GAMERECORD — Phase4 selected persistence/staging reconstruction

**2026-10-03; Steam new mechanism scope; C0146–C0148/BND-125–127.** Positive primary request → retained state/pointer/count → conditional typed callback execution → protocol result → selected header/record consumption. This is not complete save/load reconstruction, safe file validation, whole ownership or Phase4 closeout.

## Responsibilities and object/data split

- **CGame00BDA004**: shared application state, with existing backup+BE8/live+8C568 record blocks and conditional typed CPlayer consumption (C0125/C0126 reused). Not a standalone save manager.
- **TSiHolder<CSaveData>00BD9FF8**: concrete selected numeric request, retained supplied pointer/count, callback state/result/busy protocol. Not the staging payload itself.
- **CSysutil00BD9E0C**: selected exclusive callback admission/retention/invocation/clear at+C8. Outer actual activation/recurrence remains UNKNOWN.
- **CPreserve00BE5970**: in-place typed operation/context dispatch; independent object task00BE5964 switches to this root on event1. CAutoSave is embedded+28C; manual/automatic policy not reconstructed by the name.
- **00BE5EF0**: selected staging image; requested0x7A2620=header120+28*45CC0. Header scratch01388510 is distinct from file temporary and CGame records.

## Primary producer → mechanism → consumer

CPreserve selected readcontext requests op3/buffer00BE5EF0/count7A2620 → CSaveData admission through Sysutil+C8 → manager+28/+3C/+40/+44 and busy/result fields → selected same-manager callback executes disk read → result+5C then later busyclear+64 → CPreserve poll/header consumer. Its separately selected commit copies record0 directly into CGame live+8C568 and27 tails into+D2228, not backup+BE8. Same live block is read by prior independently typed CPlayer consumer under its bit31/record-byte guards. Actual read→commit mode change and later creator timing remain UNKNOWN.

## Concrete limits affecting reliance

- Protocol result0 is **not successful full ReadFile or exact-size validation**: raw reader returns file size, ignores Boolean/bytesread; mismatch copies actualF and performs sparse one-byte zeros.
- Request can return after1000 unsuccessful admission checks; caller's local advance is not proof of admitted/executed work.
- CPreserve readoperation5 and commitoperation3 are independent selected paths; no automatic chronology is inferred.
- Commit00BE1EB8 gate can skip copies; recordbit31 is not unconditionally set. Full modes/header/version/field grammar, transient actor/world reconstruction, staging lifetime, invoked allocation release and GOG remain unvisited.
- C0007 broader schema/C0004 fullplayer-state extent, inherited corrections and old static/runtime bounds remain exact. Unknown is not a reviewed unavailable-evidence exception.

## Evidence and next discriminator

Canonical detail: findings/boundaries/save_disk_staging_record_chain.md, with primary source lines, raw RTTI/import tables, actual Opus/main checks and export-orphan boundaries. Class/function/global/subsystem/boundary ledgers cross-link that report. No fixes/hooks/patches/runtime modification or supplied-evidence change.

Next high-information save sources: typed staging-builder/header producer, CPreserve operation/commit selector producer, concrete outer Sysutil invocation or actual transition-to-player creation. Do not repeat getter/backup/factory/callback type discovery. Other Phase4 families still need mechanism work; periodically reassess globally.
