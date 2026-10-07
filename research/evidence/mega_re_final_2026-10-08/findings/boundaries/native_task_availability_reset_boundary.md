# Native Task Availability Reset Boundary

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for paired grouped reset writes; task-specific reset semantics remain `UNKNOWN`.

Steam `00642640` and GOG `00642590` are structurally paired high-fanout roots and the only recovered direct writers of native-task availability byte `0147693C` outside task registration. Two paired write sites clear it:

| Steam | GOG | Direct grouped writes |
|---:|---:|---|
| `00643007` | `00642F57` | clears `0147693C` with `01476A1E`, `01470F24`, scene/input flags, and transition globals |
| `00644498` | `006443E8` | repeats the same clear batch after state/UI helper calls and before continuation dispatch |

The surrounding function is a large state-transition root with a major switch; the second reset sits in a branch that updates `014736D8/014736DC`, invokes scene/input helpers, clears scene-availability flags, and then resumes subsequent control flow.

Neither bounded write neighborhood directly:

- writes the selector-0 task callback field `+0x44`;
- calls `006BAB80/006BAAD0`;
- calls CRdHandleUtil release;
- calls generic retirement; or
- resolves an object through the task handle.

Thus `0147693C` is a shared availability gate cleared during a broader paired state-transition/reset batch, not direct evidence that the native task is replaced or retired. The single recovered direct caller of these large roots is raw Steam `00474857` / GOG `00474967`, but both sites are orphan forwarding thunks: each pushes its three stack arguments, calls the paired root, and returns. They expose no selector, manager root, callback field, or ownership state. This is a bounded export-level negative result; application/event ownership cannot be recovered from this call edge without a higher-level indirect-table or runtime source.
