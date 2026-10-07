# World resource residency — typed representation policy

**2026-10-03; Phase4 sequence45; Steam PC.** C0167-C0168 / BND-148-BND-150. This is a bounded mechanism map, not world-lifetime closure.

**VERIFIED conditional chain:** `0040D0D0 -> 005F0A50 -> 005EEC20` selected desired-mode arm; fallback `0074EE20` with exact literal `0077A4A4` and selector `0x46` enters `005EB600 -> 005E7620` CObject construction; returned object remains ESI for dword `+0x444=1` and exact `005C8720` call. `005EB600` builds a local descriptor; `005E7620` conditionally copies descriptor `+0x50` to object byte `+0x416` when descriptor `+0x58` bit `0x100` is set. `005C8720` gates `+0x416/current+0x12/desired+0x444`, performs paired `006B2C70` accessor calls, calls `006BE6E0` with same object receiver and commits low-byte desired `+0x444` to current `+0x12`.

**UNKNOWN:** incoming context/class, 0074EE20 meaning/reachability, 005EA200 value semantics, descriptor/index validity, pair/resource identities, ownership/last-use/retirement/unload, CLevel equivalence, final draw, all 75 pairs, parser grammar, cadence, GOG/Xbox/runtime. `005C87F0` is an interior Steam label, not a function spelling. Generated C uses a different textual branch order; matched labels agree with the raw call-local control scalars at005F0A50. Provisional contradiction withdrawn before promotion. No setup-success claim from void 006BE6E0 or current-field commit.

Primary report: `findings/boundaries/world_object_representation_policy.md`. Main replay: `scripts/inspect_phase4_world_representation_main.py` / `scratch/seq45_world_main_receipt.json`; consumer branch: `scratch/seq45_world_consumer_branch.md` and receipt. Existing C0035/C0045/C0050/C0085/C0114/C0119/C0120 reused only exact scope.
