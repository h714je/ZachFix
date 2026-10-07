# XCA CRdInterp Wrapper Limit

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for wrapper identity; detailed array consumers remain `UNKNOWN`.

Paired `006BEF00/006BEA10` performs no independent attachment logic: it calls paired CRdInterp reset `006B9FD0/006B9F20`, then calls the already mapped XCA1 initializer `006B9E70/006B9DC0`. It does not read the initialized arrays or expose their semantic consumer.

The current XCA object-side static path is bounded at parser initialization/wrapper reset. Detailed XCA array semantics require a new class-discriminated consumer or parser-side field evidence.
