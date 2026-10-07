# XMD Animation to Object Render Handoff

**Date:** 2026-10-01
**Evidence state:** `STRONG_INFERENCE` for render/object lifecycle handoff; exact virtual-slot renderer semantics remain unresolved.

Steam `006BE6E0` is a high-fan-in object/resource setup path:

- Stores two resource/object inputs at owner fields `+0x58/+0x59` and allocates/zeros `0xA0`-stride animation records at `+0x79` plus auxiliary `0x40`-stride state at `+0x7A`.
- Executes the full animation preparation sequence: `006BE460`, `006BABF0`, `006C1430(0)`, and `006C2020(0)`.
- Executes object post-processing (`006C3EC0`, `006BEB90`, `006C3E20`) and then invokes the owner virtual slot `+0x4C` before final cleanup/state publication.
- The function is called from many actor/resource setup paths, making it a central handoff between XMD-derived animation state and object lifecycle/render preparation.

The virtual `+0x4C` call is the first concrete post-bounds object handoff in this chain, but the target class, slot semantics, and final D3D9 draw path remain open.

## Sequence26 required primary supersession — K0021, 2026-10-03

**VERIFIED selectedSteam correction:** findings/boundaries/animation_model_state_submission.md proves class-anchored006C1430 update onCRdObjectModel/CPlayer,1E4 statebuffer/1E8 matrix-output ratherthan demonstratedtimerfields,base98/additional200/optional350-338-35C andconditionalsame-model1E8-to-packet consume. In006BE6E0, olddecompilerindices58/59/79/7A normalize tobyte160/164/1E4/1E8. CRdObjectModel own4C=00401D10 RET isnotdraw; otherclasses/acceptedCLevel4C stayseparate. Originalseedtext remainshistory; fullalgorithms/latestpose-perframe/free/GOG unknown. No acceptedPhase2 milestone regrade.
