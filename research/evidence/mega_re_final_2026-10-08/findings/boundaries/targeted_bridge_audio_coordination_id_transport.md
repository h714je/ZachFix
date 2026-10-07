# TBC009 — CSound coordination ID to row to Main-operand transport

**Result: CONNECTED; VERIFIED selected Steam packing/resolution/stored-operand path.** CSound q and Main T are different protocols/domains, not a theorem that their numeric values can never coincide.

## Exact missing relation

Known Main/Core/status protocols and prior HookChain downstream q lookup lacked this selected q producer and cross-domain transport. The new flow is `CSound request → q in caller S6C0 → independently acquired CSound full-ID row lookup → row B2C copied to S6C4 → later independently acquired Main token-taking consumer`. S's class and the value's current validity are intentionally unproved.

## Primary mechanics

`AUDIO_ID_PRIMARY.json` establishes:

- Wrapper `0046EDC0→0046DB40`; selected request checks code/selector≤0x406 and other gates. Row-selector prefix scans256 stride0x54 rows, requiring flag/sentinel conditions. Its fallback does not certify free-row success or termination.
- `004698A0` increments root+8 and resets to0 if bits outside the low byte remain. Packing is **`(((seq<<16)|selector)<<8)|index`**, with 32-bit shifts and unmasked OR operands. Producer stores this q at root+34+54*i, equivalent to **B+28** for B=root+C+54*i; sets row flags. After other calls it returns the **then-current B28**, not unconditionally the packing-helper EAX.
- `0046A770` rejects fullFFFFFFFF, uses q&FF for row arithmetic and requires the **full B28==q**. The post-mask FFFFFFFF comparison cannot reject low-byteFF; index255 is not excluded. This is not permanent identity across wrapping/reconstruction or validated allocation extent.
- `0054E615` stores request result S6C0. Following independent CSound acquisition, `0054E629` resolves it; `0054E62E` dereferences B2C without a NULL test and stores S6C4. Later `0054E90B` passes the then-current S6C4 to current Main `00720080`, which has its own token lookup/null-gated mutation.

## Ceiling

The captured request's explicit writes do **not** freshly initialize B2C. The selected windows do not prove S6C4 unchanged across the gap, coherent CSound/Main generations, non-sentinel/valid Main T, named cue/backend/loop semantics, audible success or lifetime. Keep q, B, B2C/Main-operand, T, K and status Q distinct. Unchecked dereference is a static instruction fact, not an observed fault without admitted inputs/history.

Primary is own-PE/original ASM, independently reviewed; A05 and HookChain121 limits reused. No GOG q producer or full audio census is claimed.
