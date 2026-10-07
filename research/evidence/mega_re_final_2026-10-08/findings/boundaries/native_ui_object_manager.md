# Native UI Task Factory to Object Manager Boundary

**Addresses:** Steam `006C5930`; GOG `006C5430`
**Evidence state:** `STRONG_INFERENCE` for task-factory/manager role; allocation and selector behavior are direct static facts.

## Direct evidence

Both build-specific functions switch on a selector byte and allocate one of four fixed object sizes:

- selector 0: `0x160` bytes, constructor `00402650`;
- selector 1: `0x3B0` bytes, constructor `00402A20` Steam / `00402A30` GOG;
- selector 2: `0x2B0` bytes, constructor `005F2630` Steam / `005F2560` GOG;
- selector 3: `0x218` bytes, constructor `00678FD0` Steam / `00679080` GOG.

The result is passed to a common registration helper (`006C55E0` Steam / `006C5AE0` GOG) with the caller-supplied arguments. The helper manipulates manager-linked lists and invokes a virtual function through the newly allocated object's vtable.

The functions have high reuse: 63 incoming call edges in Steam and 53 in GOG. The exact class names and all lifetime transitions are not present in this selector function alone.

## Interpretation

This is a strong candidate for a native task/object factory boundary. Selector 0's fixed allocation size and common registration path align with the prior native UI task report, but the current ledger intentionally does not assume that every selector is UI-owned or that selector 0 is the only path used by game menus.

## Open questions

- Recover the four constructor class identities and vtable addresses.
- Map manager list fields and cleanup/destructor ownership.
- Distinguish native UI tasks from other selector-based active objects.
- Confirm same-frame callback and removal behavior from the manager's update/render phases.

## Sequence25 required selected Steam correction — 2026-10-03

**VERIFIED selectednewscope, K0020:** actualtypedCMenu request usesSteam006C5930 selector1 ->3B0 allocation ->00402A30 rawCRdObjectModel0076E704 ->006C5AE0 registration. The historicalSteam00402A20/006C55E0 attribution above conflictswithrawcurrentbytes andis supersededONLYatthisselectedSteamboundary. Otherselectors/GOG labels are notrechecked ortransposed. OriginalPhase2 manager/registration milestone/oldbounds remainaccepted. Source: findings/boundaries/cmenu_organizing_task_use.md.
