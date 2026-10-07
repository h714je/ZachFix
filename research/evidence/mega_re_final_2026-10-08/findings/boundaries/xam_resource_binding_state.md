# XAM Resource Binding State Transition

**Date:** 2026-10-02  
**Evidence state:** `VERIFIED` for paired resource-binding state mechanics; resource subtype remains `UNKNOWN`.

Paired `006B9020/006B8F70`, reached from CObjectSpecies XAM receiver `006C0480/006BFF90`, manage a compact resource-binding state:

- when incoming nonzero resource differs from current `+0x14`, subtract prior tracked size from global `01481328`, free prior owned block `+0x04`, and clear size/index fields `+0x08/+0x0C`;
- snapshot previous resource/control fields at `+0x18/+0x20`;
- set current resource `+0x14`, conditionally set control/resource field `+0x1C`, and write companion fields `+0x24/+0x2C/+0x30/+0x34`.

This verifies that CObjectSpecies XAM application enters a generic resource-binding state transition that replaces previous resource-owned storage when the resource identity changes. The fields do not prove XAM grammar or a dedicated XAM class.
