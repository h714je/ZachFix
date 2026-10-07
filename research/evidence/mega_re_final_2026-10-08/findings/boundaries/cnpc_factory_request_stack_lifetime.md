# CNpcKiller Factory Request Stack Lifetime

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for representative paired direct caller construction and immediate factory consumption.

The `+0x30` byte consumed by Steam `004AB200` / GOG `004AB2E0` belongs to a transient, caller-owned factory request record rather than a CNpcKiller or CNpcRecord object.

Paired direct caller examples construct a `0x40` stack-local record, clear it, populate request fields, invoke the shared request initializer (`004B76D0` Steam / `004B77B0` GOG), and immediately pass the local address to the source-type selector mapper:

- paired `00435920` / `004359A0` build a request from current event/input state;
- paired event-interpreter case `0x2B` builds a request from a configuration-table entry including position/orientation fields.

The mapper reads only request byte `+0x30`, maps value `7` to selector `0x08`, and forwards the request pointer to the selector-factory wrapper. Successful factory completion registers the resulting outer CNpcKiller; no direct caller stores or retains the request pointer after the synchronous mapper call in the reviewed paths.

This establishes a call-local factory-request lifetime for the examined producer paths. It does not establish the semantic class name of the record, the gameplay meaning of its type byte, every possible producer family, or any independent CNpcRecord registration/ownership.
