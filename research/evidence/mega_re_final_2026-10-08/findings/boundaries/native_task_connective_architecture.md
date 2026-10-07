# Native Task Connective Architecture

**Date:** 2026-10-01  
**Evidence state:** `VERIFIED` for direct registration, callback table field, paired selector structure, and helper invocation; `STRONG_INFERENCE` for the presentation-oriented task subsystem grouping.

## Work Packages B–D — state and callback structure

`00647730/00647680` are paired callbacks whose top-level control is a 32-bit current selector at `01474CF8`; observed values include `0`, `1`, `3`, `4`, `6`, `9`, `0x0B`–`0x0E`, `0x5A`–`0x5C`, `0x62`, `0x65`, `0x66`, `99`, and `100`. Companion `01474CFC` is assigned transition values then copied to `01474CF8`, establishing a current/deferred selector pattern.

Other directly paired state:

| State | Direct structural role |
|---|---|
| `014768C0` | scalar gate compared to `0` and `5` during branch setup |
| `014768C8` | scalar sub-selector/index passed to repeated access helpers |
| `014768F5` | byte sub-selector initialized to zero by task registration |
| `0147693C` | byte registration/availability flag set by registration initializer and tested/cleared by external code |

Important external `0147693C` users are Steam `005019B0`, `00537270`, `00642640` and paired GOG `00501A80`, `00537340`, `00642590`. The supplied xref datasets do not enumerate `01474CF8/CFC` normally; their direct callback access is therefore established from paired source/assembly, not inferred xref totals.

The callback repeatedly calls `0045C9F0/0045CA20`, which is a paired high-fanin recursive helper taking position/flag inputs and an identifier-like integer. The callback immediately invokes paired `0040A3A0/0040A370` before these calls; that helper lazily constructs `CSingleton<CMessage>` at `00BDA000`. The presentation helper requires `this+0x34/+0x38/+0x3C`, consults that singleton, invokes a `0045Axxx/0045Bxxx` helper family, can recursively invoke itself, and returns a byte result.

Paired `0045A590/0045A5C0` exposes a concrete CMessage record interface: it indexes `this+0x44` as an offset table beginning at `+0x0C`, stores the selected entry pointer at `this+0x48`, reads the entry's leading 16-bit category, and normalizes category ranges to an ordinary result or sentinels `-1`, `-2`, and `-3`. Adjacent helpers read record flags and variable-length `0xFFFF`-terminated sequences through the same offset table. Paired CMessage constructors `0040A270/0040A240` initialize defaults but do not populate this table.

Selection methods `0045B370/0045B3A0` take a record index, reset CMessage playback/display fields around `+0x120A0`–`+0x120CD`, rebuild selected-entry cache `+0x48`, and read the selected record header. Under observed availability/category gates they call external paired request helper `004113B0/00411380` or `0046AFE0/0046B0D0`, then store the returned handle-like value at CMessage `+0x120C0`.

`004113B0/00411380` is not a CMessage method: it operates on an external object request field at `+0x454`, releases a previous value through `0046B0E0/0046B1D0`, and submits a new request through paired `0046ED90/0046EE80`. The backend validates an input object handle, constructs a request/transform descriptor, calls paired external submit `007202D0/0071FFE0`, allocates a `0x54`-stride entry, encodes an identity using its cursor and request ID, and returns a handle-like value. Completion and release are delegated through `0046B160/0046B250` and `0046B0E0/0046B1D0`. Direct caller census shows only CMessage selection and paired `0043E8D0/0043E950` call this helper; `0067A840` is instead a distinct direct client of the request backend. `0043E8D0/0043E950` is a paired CEvent command-interpreter routine: it advances a stream cursor through virtual parse slots `+0x34/+0x38`, branches on decoded command values, lazily creates `CSingleton<CEvent>` at `00BD9E58`, and updates command-specific event fields.

Paired `00447B50/00447BD0` is the directly recovered opcode dispatcher for that interpreter family: it snapshots parser context `+0x18` to `+0x1C`, clears `+0x68/+0x6C`, reads stream byte `+4`, bounds it to `0xCC` values after bias `-2`, then dispatches through a compressed byte table and jump table. Opcode byte `0x80` reaches `0043E8D0/0043E950`. A separate paired mode-filtered dispatcher `004481F0/00448270` selectively permits opcodes by context byte `+0x5D`. The compressed table (`00448120/004481A0`) and jump table (`00447FBC/0044803C`) are static code-region data: supplied xrefs show only dispatcher consumption, with no producer/write reference.

Paired `0042D5B0/0042D630` directly initializes the concrete CEvent singleton at `00BD9E58`: CEvent vtable, embedded CRdCriticalSection, parser/control fields, and a 0x32-entry `0xFFFFFFFF` handle array. Adjacent `0042D9A0/0042DA20` advances CEvent stream pointer `+0x18` through variable-length records. The observed layout is: leading byte; two 16-bit fields; signed flag byte; signed 16-bit relative offset to a trailing 16-bit field. Decoded branches reach ItemManager and event helper paths, including lazy `CSingleton<CItemManager>` creation. Candidate `0042B1C0/0042B240` does not load this stream: it preserves `+0x18` across an external helper call while resetting mode byte `+0x5D`. This is a verified CEvent stream-state-to-item/event boundary, though the initial stream buffer producer remains indirect/unknown from the current static source universe. This establishes a shared event-command-to-request-handle client alongside the CMessage presentation client. The backend/owner class and submitted subsystem are not yet proven. This is a verified message-record-to-external-request-handle boundary. Exact category semantics and table population remain `UNKNOWN`.

## Work Package E — static placement

Proven relationships:

```text
004F5029 -> 0064D810 -> selector-0 active task -> task+0x44 = 00647730
                                          -> common callback seam(event 0)
00401A70 -> 006C5FF0 / 006C5AF0 (generic active-object dispatch)
```

Both paths use the active-object manager architecture, but no direct call from scheduler `00401A70` or generic dispatcher `006C5FF0/006C5AF0` to this callback is recovered. The task is registered as an active object; its callback is indirect through `+0x44`. This supports a sibling/registered-task relationship, not a proven execution order or cadence.

## Work Package F — bounded interface

The exposed task interface is a callback field, not a newly recovered vtable:

| Field / seam | Evidence |
|---|---|
| task `+0x44` | `006BAB80/006BAAD0` stores an arbitrary callback pointer |
| callback dispatch | same helper immediately invokes common seam `00402870/00402860` with event code `0` |
| task factory | selector-0 creates a `0x160` CRdObject-derived registered task |

No further callback-table enumeration is justified from current static evidence. The callback setter has many callers, so it is generic infrastructure rather than a single-owner interface.

## Work Package G — organizing roots

| Root | Evidence-supported relationship |
|---|---|
| `00BD7670` | active-object manager passed to selector-0 task factory; task registration root |
| task `+0x44` | callback identity and initial event dispatch boundary |
| `01474CF8/CFC` | private current/deferred callback selector state |
| `0147693C` | registration/availability byte shared with external Steam/GOG functions |
| `00BDA000` | lazily initialized `CSingleton<CMessage>` consulted by `0045C9F0/0045CA20` |
| `00BDBCD0` | command-sequence scalar mutated by `0045C9F0/0045CA20` under observed identifier/state gates |

## Explicit limits

- No runtime callback cadence, ordering relative to generic dispatcher, or task lifetime duration is established.
- No semantic name for the `01474xxx` region, selector values, or presentation identifiers is promoted.
- No direct unregister callback or task-specific teardown has been recovered; generic active-object retirement remains the known lifecycle mechanism.
- The native task subsystem is structurally connected to `CSingleton<CMessage>` and a presentation helper family, but broad UI ownership remains `STRONG_INFERENCE`.
