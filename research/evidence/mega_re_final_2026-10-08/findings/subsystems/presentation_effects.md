# Presentation/media effects subsystem — selected request/fade/latch mechanism

2026-10-03, Phase4 sequence41. Scoped reconstruction only.

## Selected architecture

Accepted C0134-C0136 establish CFadeManager's three retained CFade objects, typed callback/use and application-tail consumer. C0137 establishes distinct static CDemoMovie and CDemo roots. New C0162-C0164/BND-142–144 connect a selected Steam CDemoMovie request0x40 to an installed selector0 callback task, conditional event/private-state route, retained-CFade request7 numeric state and receiverless presentation latch00BE1EAC. The accepted tail tests/clears the latch and conditionally passes manager+4 CFade data to acquired CRdPrim.

The object/value roles remain separate: CDemoMovie M=008A7218; callback task T; acquired CRdMovie R with byte+4 gate; F_A/F_B/F_C are separate manager+4 loads at distinct stages; L=00BE1EAC; CRdPrim is renderer receiver. Shared manager/global locations do not prove pointer identity, freshness, lifetime, ownership or concurrency safety.

## Positive selected responsibilities

- M request producer005DF190/00428870 supplies literal0x40 and controls0/0/1/0 under BCD0==4; movie arm checks M+4 bit31, performs opaque prepublication calls, writes M+8 and bytes+1C..1F, and installs00427AF0 on returned task T through006BAB80. Initial synchronous event0 is not event1 delivery.
- Callback00427AF0 conditionally maps private M+14 state0 through raw table004287C8, current request/gates and R+4 byte0 to a CMessage continuation; returning path writes M+14=3 and issues NULL00449890 then F_B request7 [7,30.0,0,1]. It performs a separate later R+4 byte gate before receiverless latch call. Exact OS/query/media inputs and helper results are UNKNOWN.
- NULL helper selected stores F_A+19C/+1A0/+1A4=0 and +1A8=1. Request7 admits full request7 (unsigned<=8; no old state/source admission), increments +188, writes +164=7/+16C=30/+170=operational1/30, copies source quartet to +18C..+198 then snapshots +178=1 into+198 and advances+178 using live non-file-backed014AFFE0. It returns AL=1 only. +198 is the accepted consumer data field; +178 is separately post-stepped and not resnapshotted.
- Receiverless00449690 writes L=1. Accepted0044AC10 checks L, compares the separate draw guard before clearing L, then branches; nonzero guard can consume without drawing. On continuation it reloads manager+4 and copies+18C..+198 to acquired CRdPrim/006E3360.

## Scope / unknowns

This does not establish event delivery, request success, movie/fade completion, friendly mode meanings, F pointer equality or last-writer freshness, actual drawing/GPU result, scalar units/cadence, safe lifetime/free, all movie/fade/weather algorithms, GOG/Xbox correspondence or runtime behavior. The first untyped edges are outer BCD0 producer, 0040EDE0/0044E560/0045A560/00700260/helper effects, R+4 production and F retention/pointee continuity. The selected mechanism is conditional static evidence, not Phase4 closeout.

Primary detailed report: `findings/boundaries/presentation_movie_fade_latch_chain.md`. Accepted organizing report: `findings/boundaries/effects_organizing_roots_and_use.md`.
