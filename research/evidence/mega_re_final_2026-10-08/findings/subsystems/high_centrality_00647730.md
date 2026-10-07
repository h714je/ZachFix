# High-Centrality Dispatcher Pair `00647730` / `00647680`

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for native-task callback registration and initial generic task-event dispatch; broad dispatcher subsystem remains `STRONG_INFERENCE`.

## Direct-caller constraint

The supplied Steam/GOG call manifests report zero ordinary direct callers for Steam `00647730` and GOG `00647680`, despite 951 outgoing edges each. The xref manifests show no conventional entry-call site.

## Resolved indirect registration path

Targeted original-binary scans found one literal occurrence of each entry address in `.text`, not a `.rdata` table:

| Build | Raw registration block | Task factory | Callback helper | Dispatcher pointer |
|---|---|---|---|---|
| Steam | `0064D810` | `006C5930` | `006BAB80` | `00647730` |
| GOG | raw `0064D760`–`0064D7B1` (no Ghidra function boundary) | `006C5430` | `006BAAD0` | `00647680` |

The paired blocks gate on `0042D010/0042D090`, request a task object from the native task factory, set shared bytes `0147693C=1` and `014768F5=0`, push the dispatcher address, and call the paired helper.

`006BAB80/006BAAD0` stores its first stack argument at task field `+0x44`. The common helper (`00402870` Steam / `00402860` GOG) then invokes the global callback, if set, followed by task `+0x44` with `(task, 0, registration_arg)`. Thus `00647730/00647680` is immediately dispatched as a generic task callback at registration.

## Limits

This does not prove the callback's full event taxonomy, recurring task phase, CToolBox relationship, UI taxonomy, or final D3D9 ownership. The direct-call negative constraint remains valid; the correct mechanism is a task callback slot and generic event dispatch, not a normal call edge.

## Evidence

- Steam raw assembly `0064D810`–`0064D861`; GOG original binary disassembly `0064D760`–`0064D7B1`
- Steam/GOG callback helpers `006BAB80/006BAAD0`
- Steam/GOG common callback dispatch `00402870/00402860`
- `inputs/decompiler/steam/DP_decompiled.c:1545-1554,367720-367733,434804-434810`
- `inputs/decompiler/gog/DP_decompiled.c:1534-1543,332561-332567`
