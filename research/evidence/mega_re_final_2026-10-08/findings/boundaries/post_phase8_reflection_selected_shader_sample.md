# PGC006 — selected reflection constant → varying → projected texture sample

## Disposition

**CONNECTED at conditional selected Steam static scope.** The missing constant/bank/permutation/sampler bridge is no longer supported only by embedded names. Actual selected live permutation, resource/COM/device coherence, success, final visible sample, GOG parity and reflection-offset causality remain **UNKNOWN**. This is finite new consumer evidence, not a broad renderer/Phase7 reopening; renderer236/Phase8 twice-width correction remains authoritative.

Main CPU receipts: `audit/post_phase8_closure_2026-10-07/REFLECTION_SHADER_PRIMARY_V2.json` (1108 instructions/3473 bytes) and `REFLECTION_CONSTANT_PRIMARY.json` (17/41), all own-PE/original-ASM reachable-singleton instructions. `SHADER_TOKENS_V3.json` and selected `DATA_PRIMARY.json` ranges independently preserve three exact shader blobs and selected bank/header/offset cells. `scripts/inspect_post_phase8_shader_tokens.py` parses only those blobs in memory; no binary/shader modification.

## Typed bank → creation/index → selected bind

**VERIFIED selected operands; declared types reused from C0241:** CRdModel-compatible initialization offers bank00A25278 at member+40 (DiffuseV), bank00ADCE20 at+D0 (DiffuseP_RAIN), and separate00BC3C70 at+118 (DiffuseP_MIRR) to006E20A0. Selected headers respectively encode(0,256),(1,544),(1,1).

006E20A0 derives code address as`bank + DWORD[bank+8+4*index]`; header0 takes vertex creation/array+4 path, nonzero takes pixel/array+14 path.006E2570 publishes array/count metadata;006E22A0 uses current vertex-array count to select vertex/pixel bind path and006E7D60 obtains the indexed array cell. Create/bind results, same generation and valid array elements are not certified by intended count publication.

**VERIFIED conditional selector bridge:**006E6CB0's selected rain branch calls vertex helper006E7E10 on root+40 and pixel helper006E7EC0 on root+D0. The vertex helper weights its seven supplied numeric arguments by1,2,8,16,32,64,128; pixel helper weights six by1,2,4,8,16,32. Thus a finite **conditional example** with vertex args(0,0,0,0,1,0,0) and pixel args(0,0,0,1,0,0) selects VS32/PS8. The shown branch supplies the vertex fifth argument1 and obtains pixel fourth argument from root2B0. A selected material caller can request root2B0=1 through006E04C0. Other gates/parameters/root observations remain conditional; this is not proof a live draw selects those rows.

## CPU matrix/texture inputs → exact selected tokens

**VERIFIED:** bias006E07D0 supplies ECX=root+40 to006E0830, with offered register offset0 and count1. Adapter adds0xEF and requests four-register matrix staging through006E24F0→006E2360. Under the intended selected vertex-array state this is the vertex constant239..242 request. The inherited transpose, twice-width bias arithmetic and stage-selection qualifications are retained.

Main renderer selected calls bind half/quarter holders to texture stages5/6, then reach shader-selector/draw requests. Service acquisitions and state/COM episodes are independent observations, not a stable-root/GPU sample certificate.

**VERIFIED selected shader data:**

| Blob / conditional row | Exact address / length | Declared contract |
|---|---|---|
| DiffuseV bank row32, offset0x9D48 |00A2EFC0,992 bytes|vs_3_0;g_mLightProj constant239,count4;g_mViewProj245,count4|
| DiffuseP_RAIN bank row8, offset0x1A18 |00ADE838,776 bytes|ps_3_0;g_tReflect sampler5;g_tRefract sampler6;g_vMirror constant10;g_vAddColor13|
| Separate MIRR bank row0, offset0xC |00BC3C7C,444 bytes|ps_3_0;only diffuse sampler0 in its selected TEX contract|

VS DCL00A2F180 declares outputo4 as **TEXCOORD5**. DP4 instructions00A2F268/278/288/2D4 write itsx/y/z/w fromr0 and constants239/240/241/242. Source r0 is separately produced using view-projection constants245..248; do not equate world/object/camera positions merely from shader names.

PS DCL00ADE9D8 declares inputv2 as **TEXCOORD5**.00ADEA14/00ADEA24 tokens are projected TEXLD opcodes consumingv2 and samplers5/6 into temporaryr0/r1.00ADEA34 interpolates withconstant10.y, and00ADEA48 addsconstant13. CPU006E0D20 has separate root+D0 parameter paths via006E0D80/006E0DB0 to constant10/13, with their own offered offsets/parameters. This is concrete register/varying/sampler correspondence, not a string-only shader attribution.

**STRONG_INFERENCE:** selected bank/target/constant/token contract is an intended planar reflection/refraction sample path. Conditional static shader choice→compiled varying linkage→projected samplers is now continuous. Actual driver execution and visible output remain unobserved.

## Negative name discriminator / retained ceiling

**VERIFIED selected control:** the separately typed MIRR member's single blob declares/samples only diffuse sampler0. The candidate hypothesis that its name alone identifies the stage5/6 reflection consumer is **DISPROVEN at this blob scope**. No historical canonical MIRR claim is silently retired; the old composition report did not prove its reflection consumer role.

**UNKNOWN:** liveVS32/PS8 admission, all other544/256 rows, material meaning, current count/array/root/COM/device episodes, coherent constants/targets, successful Create/bind/draw/sample, address-mode/target freshness, GPU last use and complete opposite-build equivalence. No all-permutation or universal shader-success theorem.

The established W,W versus W,H bias difference remains **VERIFIED arithmetic**; contribution to the reported reflection-offset symptom remains **HYPOTHESIS**, with matched causal observation **UNKNOWN**. The c243 water-note story is separate. No fix/patch or runtime experiment is authorized or implied.

Reopen for a concrete currently needed different admitted permutation/material/constant source or matched symptom/current-target observation, not another shader-name/all-variant census. Shader blob hashes and raw token/register fields are bound in the receipts; finite bytecode decoding does not replace live results.
