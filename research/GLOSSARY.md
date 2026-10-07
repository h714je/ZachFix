# Glossary | Language used throughout research

[← Research atlas](README.md) · [Evidence guide](EVIDENCE_GUIDE.md)

| Term | Meaning in these notes |
| :-- | :-- |
| **Build** | A specific binary/platform version. Steam and GOG PC addresses cannot be silently interchanged. |
| **GOG / Steam** | Distinct 32-bit PC executable builds, often compared to establish a shared mechanism. |
| **Xbox homolog** | A cross-version code or data correspondence; its strength depends on direct supporting evidence. |
| **Static evidence** | Decompiled code, assembly, bytes, layout or dataflow without asserting actual in-game runtime outcomes. |
| **Runtime evidence** | Observations under measured conditions, with logs, traces, reproduction or A/B comparisons. |
| **Scoped / bounded proof** | A result applies to a named location/path/sample; it does not prove completeness of the entire subsystem. |
| **Producer / consumer** | The code writing/creating a value versus the code reading/using it. An edge alone does not prove ownership. |
| **Owner / lifetime** | The entity responsible for allocating, retaining, replacing or freeing a resource. |
| **Entrypoint / root** | A reachable execution or object origin used to organize a search, not necessarily a full semantic function identification. |
| **Dispatcher** | A routine routing work to phases, objects, handlers or callbacks. One dispatcher may have multiple semantically different passes. |
| **CInput** | Native input processing/aggregation system; not equivalent to just a WinMM device snapshot. |
| **Action record** | The game's logical controller/action representation; distinct from physical SDL/WinMM inputs. |
| **CCT** | PhysX character controller. Keep its reconciliation and vehicle actor-state writes separate from ordinary scene steps. |
| **Frustum** | Camera visibility volume. Main-camera frusta, shadow frusta and other distance controls are independent domains. |
| **LOD / residency** | Geometry detail selection versus what remains loaded or represented in the world. |
| **GameRecord** | Persisted game state serialized into `dp.sav`; not a transparent dump of all transient runtime pointers. |
| **Native UI task** | In-engine `CRdObject`/callback-backed UI work, independent of ImGui and not automatically a stock `COption` slot. |
| **`VERIFIED` / `STRONG_INFERENCE`** | MegaRE confidence labels with **local** evidence ceilings, not global implementation readiness. |
| **Corrected / `DISPROVEN`** | An explicitly retired interpretation with a documented replacement or narrower surviving claim. |
| **Historical vs. repaired census** | Original function ledger and corrected structural-entry universe; do not mix their denominators. |

For byte/offset notation, numeric domains and how to avoid false interpretations, see [methodology](methodology.md) and [evidence guide](EVIDENCE_GUIDE.md).
