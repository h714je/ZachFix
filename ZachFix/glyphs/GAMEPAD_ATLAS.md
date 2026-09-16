# ZachFix gamepad atlas template

Technical template for Deadly Premonition / ZachFix gamepad glyph themes.

## Geometry

- Atlas: `1024 x 1024`
- Base cell: `128 x 128`
- Rows `R0` through `R6`: ordinary `128 x 128` cells
- Row `R7`: four logical `256 x 128` regions:
  - `R7 C0-1` left stick left
  - `R7 C2-3` left stick right
  - `R7 C4-5` right stick left
  - `R7 C6-7` right stick right

Coordinates use **row, column** order.

## Confirmed mapping

| Region | Meaning |
|---|---|
| R0 C0 | A / Cross |
| R0 C1 | B / Circle |
| R0 C2 | X / Square |
| R0 C3 | Y / Triangle |
| R0 C4 | D-pad |
| R0 C5 | D-pad Up |
| R0 C6 | D-pad Down |
| R0 C7 | D-pad Left |
| R1 C0 | D-pad Right |
| R1 C1 | Left Stick |
| R1 C2 | Right Stick |
| R1 C3-C6 | Loading animation frames 1-4 |
| R1 C7 | Start / Menu / Options / Plus |
| R2 C0 | Select / View / Create / Minus |
| R2 C1 | LB / L1 / L |
| R2 C2 | RB / R1 / R |
| R2 C3 | LT / L2 / ZL |
| R2 C4 | RT / R2 / ZR |
| R2 C5 | L3 |
| R2 C6 | R3 |
| R2 C7 | Up-Down |
| R3 C0 | Left-Right |
| R3 C1 | Continue / scroll-down prompt |
| R7 C0-1 | Left stick, left |
| R7 C2-3 | Left stick, right |
| R7 C4-5 | Right stick, left |
| R7 C6-7 | Right stick, right |

Rows 5 and 6 are believed to be unused legacy PlayStation Move remnants.

Other cells remain intentionally marked `UNKNOWN / LEGACY?` until observed in-game.

## Theme authoring note

The currently validated visual fit for bundled Kenney themes is approximately
`120 x 120` visible content inside a `128 x 128` cell, leaving about 4 px padding
per side. Preserve the atlas UV geometry even when using higher-resolution source art.
