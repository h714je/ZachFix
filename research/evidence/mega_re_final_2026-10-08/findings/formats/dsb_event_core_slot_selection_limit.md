# DSB Event-Core Slot Selection Limit

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for candidate-slot scan; DSB payload grammar remains `UNKNOWN`.

Paired `0070E820/0070E7C0`, called only by the DSB event-core dispatch wrapper, does not read a DSB payload header. It scans candidate slot indices `0..0x17F`, invokes paired event predicates `00448DB0/00448E00` and `00449410/00449460`, and returns the first matching index or `0xFFFF`.

Thus the typed DSB branch selects an existing event-core slot through engine state/predicates, then activates/dispatches it. The static branch exposes no DSB-local record grammar or header discriminator; detailed DSB format semantics remain `UNKNOWN` pending a lower-level loader/parser or runtime evidence.
