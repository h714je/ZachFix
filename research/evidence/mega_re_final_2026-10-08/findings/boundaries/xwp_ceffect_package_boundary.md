# XWP CEffect Package Boundary

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for package lookup/member resource readiness and registered CEffect creation; XWP grammar remains `UNKNOWN`.

Paired `00677CF0/00677C40` accepts an XWP package name and config input under a mutex-bearing owner. If the package is absent, it derives a path below `UPDATA/EFFECT/XWP/` and issues a resource request. After CRdData lookup, it recognizes literal package marker `ver1.5`, checks or requests referenced member resources, and materializes a `0x348` runtime object through paired `006750C0/00675010`.

The constructors directly write `CEffect::vftable`. The loader copies package/member configuration into this instance, assigns a callback, registers it through the active-object manager, and applies name-conditioned flags. This establishes XWP package resource -> configured, registered CEffect runtime object.

The member record layout, meaning of `ver1.5`, callback semantic role, and XWP binary grammar remain `UNKNOWN`.
