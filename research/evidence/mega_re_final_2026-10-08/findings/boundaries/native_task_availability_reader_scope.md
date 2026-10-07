# Native-Task Availability Reader Scope

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired read gates; task lifetime implications remain `UNKNOWN`.

Paired `005019B0/00501A80` read `0147693C` only inside a narrow action/resource gate: when a per-index condition is active, availability is zero, mode is not one, and the index lies in the observed range, the helper returns `-2`. The bodies do not call `006BAB80/006BAAD0`, active-object registration, or retirement.

Paired `00537270/00537340` lazily use `CSingleton<CCamera>` and return before their subsequent camera/state work when availability is nonzero, alongside other global gate checks. They also contain no callback setter, registration, or retirement call.

Together with registration (`0064D810` / raw GOG counterpart) setting the byte and paired transition reset roots clearing it, this establishes a shared availability gate with action/camera consumers. It does not prove replacement, destruction, or scheduling of the native task.

Evidence: paired decompilation/xrefs for `005019B0/00501A80`, `00537270/00537340`, `0064D810`, `00642640/00642590`; native-task findings.
