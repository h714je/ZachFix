# XMD Virtual +0x4C Target Constraint

**Date:** 2026-10-01
**Evidence state:** `VERIFIED` for indirect handoff; target semantics `UNKNOWN`.

`006BE6E0` performs XMD animation, transform, and bounds preparation, then invokes `(**(code **)(*this + 0x4C))()` before publishing object state. The supplied function/xref manifests do not resolve this virtual call to a concrete target or final D3D9 draw routine. The object owner is used across many resource/actor setup paths, so slot `+0x4C` must remain an indirect lifecycle/render handoff candidate until a class-specific vtable instance is identified.

This negative constraint prevents treating `006BE6E0` or `006C1850` as the final renderer solely because they precede the virtual call.
