#!/usr/bin/env python3
"""Build reader-facing Markdown navigation without editing canonical MegaRE evidence."""
from __future__ import annotations

import argparse
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MEGA = ROOT / 'evidence' / 'mega_re_final_2026-10-08'
BEGIN = '<!-- BEGIN AUTO RESEARCH NAV -->'
END = '<!-- END AUTO RESEARCH NAV -->'

AREAS = [
    ('engine', 'Engine, scheduling and resources', {
        'overview.md': 'Main execution spine and subsystem relationships.',
        'mega-re-final.md': 'Current checkpoint-260 architecture synthesis and correction limits.',
        'timing.md': 'QPC, gameplay, presentation and other timing domains.',
        'resource-loading.md': 'Resource queues and typed handoffs.',
        'animation.md': 'Model state/matrix producer and submission.',
        'effects.md': 'Effects, XWP, lifecycle and timing.',
        'audio.md': 'Audio request and status pipelines.',
        'crddebug.md': 'Developer-mode and debugging surfaces.',
        'phase4-mechanisms.md': 'Historical Phase-4 mechanism synthesis (superseded as current authority).',
    }),
    ('input', 'Input, camera and controls', {
        'README.md': 'PC physical acquisition, CInput and provider boundaries.',
        'camera-modes.md': 'Camera-family-specific state and aim logic.',
        'xbox-controls.md': 'Original-control functionality and PC restoration limits.',
    }),
    ('player', 'Player, gameplay states and vehicles', {
        'README.md': 'Player state machine and native action lifecycle.',
        'state-families.md': '137-slot state table and grouped handlers.',
        'action-protocol.md': 'Object-action packets and event domains.',
        'action_selector.md': 'Event/capability selector mapping.',
        'object-action-taxonomy.md': 'Receiver/producer-driven action taxonomy.',
        'vehicle.md': 'Vehicle enter/exit, steering and action protocol.',
        'cct-bridge.md': 'Player/CCT and actor-state reconciliation.',
    }),
    ('world', 'World, visibility and distance', {
        'README.md': 'Independent distance, activation and representation controls.',
        'frustum.md': 'Main visibility-frustum taxonomy.',
        'lod.md': 'Geometry LOD selection.',
        'residency.md': 'Alternate 3D residency and streaming.',
        'shadows.md': 'Dedicated shadow visibility frusta.',
    }),
    ('render', 'Rendering and restoration', {
        'README.md': 'What has been restored versus unresolved rendering work.',
        'depth.md': 'Depth formats and AO precision limits.',
        'color.md': 'ENV grading and output transfer.',
        'water.md': 'Water shading and shader differences.',
        'daynight.md': 'HOUSE_LIST day/night data correction.',
        'interior-visibility.md': 'Visibility volume and interior props.',
    }),
    ('ui', 'Native in-game UI', {'README.md': 'CRdObject task architecture and Pause integration.'}),
    ('save', 'Persistence and resume', {'README.md': 'dp.sav / GameRecord and safe resume boundaries.'}),
    ('physx', 'Physics', {'README.md': 'PhysX timing closure and retired production experiments.'}),
]

GUIDES = [
    ('README.md', 'Research atlas', 'First page and primary jump-off point.'),
    ('ARCHITECTURE_MAP.md', 'Visual architecture map', 'Two diagrammed engine/input overviews and distance domains.'),
    ('STATUS.md', 'Current status', 'Static checkpoint/coverage and engineering scope.'),
    ('READING_PATHS.md', 'Reading paths', 'Routes from ZachFix questions to evidence.'),
    ('EVIDENCE_GUIDE.md', 'Evidence guide', 'Status labels, build identity and promotion rules.'),
    ('GLOSSARY.md', 'Glossary', 'Terminology and scoped proof.'),
    ('methodology.md', 'Methodology', 'Receiver tracing, homology and census heuristics.'),
    ('unresolved.md', 'Unresolved targets', 'Open questions, chronological additions.'),
    ('disproven.md', 'Disproven/corrected', 'Rejected hypotheses and narrower surviving claims.'),
    ('STYLE_GUIDE.md', 'Style guide', 'How to extend the archive consistently.'),
]

READER_NOTES = {
    'methodology.md': 'Durable RE workflow rules; the 2026-10-04 snapshot remains useful even where the census has since advanced.',
    'unresolved.md': 'Open questions accumulate chronologically. Check the 2026-10-08 final frontier before acting on older work items.',
    'disproven.md': 'Negative constraints are intentionally preserved. A rejected interpretation does not invalidate every weaker observation.',
    'engine/phase4-mechanisms.md': 'Historical 2026-10-04 Phase-4 view. Use MegaRE final for the current static synthesis.',
    'engine/mega-re-final.md': 'The readable MegaRE synthesis. Check the primary findings and latest applicable corrections for the exact scope.',
    'engine/overview.md': 'The shortest detailed engine tour; build-specific addresses and static-vs-runtime limits still apply.',
    'engine/timing.md': 'Timing is composed of distinct islands. Do not generalize one delta or clock to every subsystem.',
    'engine/audio.md': 'Selected audio paths are mapped, not an exhaustive behavioral or surround-sound runtime proof.',
    'engine/effects.md': 'Object/effect lifecycle and fixed-delta evidence are not blanket runtime timing guarantees.',
    'engine/crddebug.md': 'Recovered debug surfaces are research findings, not automatically safe public options.',
    'input/README.md': 'Physical providers, logical action processing and CInput are different layers.',
    'input/camera-modes.md': 'Camera families and mode-2 precision need their own evidence and runtime validation.',
    'player/README.md': 'Keep action selector candidates, committed states, packet phases and native events separate.',
    'player/action-protocol.md': 'Packet fields and numeric domains are not interchangeable.',
    'player/action_selector.md': 'This is the full selector table; native event, candidate, committed gameplay state and packet status remain distinct.',
    'world/README.md': 'NPC, light/shadow, main frustum and world residency may use independent limits.',
    'world/shadows.md': 'Shadow frusta and main-camera frusta must not be conflated.',
    'render/README.md': 'Distinguish shipped restorations from research-only renderer hypotheses.',
    'ui/README.md': 'Native task evidence supports a development PoC; it does not establish a production-safe settings menu.',
    'save/README.md': 'Persistent GameRecord bytes are not equivalent to a live object graph.',
    'physx/README.md': 'Research retained; no general PhysX timing patch is approved for production.',
}


def heading(f: Path) -> str:
    for line in f.read_text(encoding='utf-8-sig').splitlines():
        if line.startswith('# '):
            return line[2:].strip().replace('|', r'\|')
    return f.stem.replace('_', ' ').replace('-', ' ')


def write(path: Path, text: str, check: bool) -> bool:
    text = text.rstrip() + '\n'
    if check:
        if not path.exists() or path.read_text('utf-8') != text:
            print('OUT OF DATE', path.relative_to(ROOT))
            return False
        return True
    path.write_text(text, 'utf-8')
    return True


def slugify(h: str) -> str:
    h = re.sub(r'<[^>]+>', '', h)
    h = re.sub(r'\[([^]]+)\]\([^)]+\)', r'\1', h)
    h = h.replace('`', '').replace('*', '').lower()
    h = re.sub(r'[^\w\- ]', '', h)
    return re.sub(r'\s+', '-', h.strip())


def headings_outside_code(s: str):
    in_fence = False
    used = {}
    for line in s.splitlines():
        if re.match(r'^\s*(```|~~~)', line):
            in_fence = not in_fence
            continue
        if in_fence:
            continue
        m = re.match(r'^## (.+?)\s*#*\s*$', line)
        if not m:
            continue
        label = m.group(1).strip()
        slug = slugify(label)
        count = used.get(slug, 0)
        used[slug] = count + 1
        yield label, slug + (f'-{count}' if count else '')


def add_navigation(path: Path, check: bool) -> bool:
    orig = path.read_text('utf-8')
    # Keep only the historical body. Existing generated navigation is repeatably replaced.
    clean = re.sub(r'\n?' + re.escape(BEGIN) + r'.*?' + re.escape(END) + r'\n*', '\n', orig, flags=re.S)
    top, _, body = clean.partition('\n')
    body = body.lstrip('\n')
    rel = path.relative_to(ROOT).as_posix()
    depth = len(path.relative_to(ROOT).parts) - 1
    pre = '../' * depth
    breadcrumb = f'[← Research atlas]({pre}README.md) · [Topics]({pre}INDEX.md) · [Open questions]({pre}unresolved.md)'
    note = READER_NOTES.get(rel, 'Readable research synthesis; follow the cited evidence for build-specific claims.')
    sections = list(headings_outside_code(body))
    nav = [BEGIN, breadcrumb, '', f'> **Reading note:** {note}', '']
    if 0 < len(sections) < 7:
        nav += ['**Jump to:** ' + ' · '.join(f'[{title}](#{anchor})' for title,anchor in sections)]
    elif sections:
        nav += [f'<details><summary><strong>On this page</strong> · {len(sections)} sections</summary>', '']
        nav += [f'- [{title}](#{anchor})' for title,anchor in sections]
        nav += ['', '</details>']
    nav += [END]
    new = top + '\n\n' + '\n'.join(nav) + '\n\n' + body.rstrip() + '\n'
    return write(path, new, check)


def make_curated_index(check: bool) -> bool:
    text = '''# All readable research pages

[← Research atlas](README.md) · [Question-based routes](READING_PATHS.md) · [Evidence library](evidence/INDEX.md)

The pages below are the **readable interpretation layer**. Source reports, maps, logs, assembly and ledgers remain under `evidence/`; their scopes take precedence over a summary.

## Orientation & research hygiene

| Page | What it gives you |
| :-- | :-- |
'''
    for f,title,desc in GUIDES:
        text += f'| [{title}]({f}) | {desc} |\n'
    for directory, label, names in AREAS:
        text += f'\n## {label}\n\n| Page | Why to open it |\n| :-- | :-- |\n'
        for fname, desc in names.items():
            path = ROOT / directory / fname
            if path.exists():
                text += f'| [{heading(path)}]({directory}/{fname}) | {desc} |\n'
    text += '\n## Evidence collections\n\n- [Evidence collection guide](evidence/INDEX.md)\n- [2026-10-08 MegaRE reading index](evidence/mega_re_final_2026-10-08/INDEX.md)\n- [Finding catalogue](evidence/mega_re_final_2026-10-08/FINDINGS_INDEX.md)\n- [44 maps and 24 reports](evidence/mega_re_final_2026-10-08/MAPS_REPORTS_INDEX.md)\n- [23 checkpoint references](evidence/mega_re_final_2026-10-08/CHECKPOINTS_INDEX.md)\n'
    return write(ROOT/'INDEX.md', text, check)


def make_evidence_index(check: bool) -> bool:
    p = ROOT/'evidence'
    text = '''# Evidence library | Source material, not guesses

[← Research atlas](../README.md) · [Evidence rules](../EVIDENCE_GUIDE.md) · [Reading paths](../READING_PATHS.md)

> [!IMPORTANT]
> **Primary findings are kept as historical records.** Do not treat a matching path, class name, or inferred role as a runtime proof. Many imported MegaRE documents were written for their original workspace and may mention paths not embedded in this smaller ZachFix evidence snapshot.

## Current imported MegaRE

| Open | Purpose |
| :-- | :-- |
| [MegaRE reading index](mega_re_final_2026-10-08/INDEX.md) | Best starting point for the final 2026-10-08 static import. |
| [Findings catalogue](mega_re_final_2026-10-08/FINDINGS_INDEX.md) | 281 primary findings by evidence family (286 files including category READMEs). |
| [Maps & reports](mega_re_final_2026-10-08/MAPS_REPORTS_INDEX.md) | All 44 current/final maps and 24 reports. |
| [Checkpoints](mega_re_final_2026-10-08/CHECKPOINTS_INDEX.md) | The retained 238–260 chronological sequence. |

## Focused ZachFix evidence

| Collection | Why it matters |
| :-- | :-- |
'''
    note={
      'aim_mode2_precision':'Exact mode-2 aiming, precision and reset-policy traces.',
      'ceffect_xbox_timing':'Selected original Xbox effect delta evidence.',
      'cinput_pipeline':'PC input staging and the Xbox same-update comparison.',
      'game_record':'Native save record byte-level reconstruction.',
      'game_time_precision':'QPC/x87 long-session precision failure and guard.',
      'legacy_joystick_polling':'WinMM joystick retry, handle leak and runtime cost.',
      'native_ui':'Native task/ABI, Pause integration, safety boundaries.',
      'physx_timing':'Physics timing closeout and runtime blockers.',
      'save_resume_contract':'Save/load lifecycle, adapters and corrective audit.',
      'vehicle_protocol':'Vehicle packet/state disassembly and extract protocol.',
      'xbox_only_controls':'Control features retained by PC state handlers.',
      'mega_re_census_2026-10-04':'Historical Phase-4 slice; superseded as current static reference.',
    }
    for directory in sorted(p.iterdir()):
        if not directory.is_dir() or directory.name == MEGA.name:continue
        readme=directory/'README.md'
        if readme.exists():
            text+=f'| [{heading(readme)}]({directory.name}/README.md) | {note.get(directory.name, "Focused evidence collection.")} |\n'
    text += '\nRaw `.asm`, `.json`, `.csv`, scripts and checksums stay beside their explanatory pages. Their machine-readable contents have not been redesigned.\n'
    return write(p/'INDEX.md',text,check)


def make_mega_index(check:bool)->bool:
    text='''# MegaRE | Final static archaeology, navigable edition

[← Research atlas](../../README.md) · [Readable synthesis](../../engine/mega-re-final.md) · [Evidence rules](../../EVIDENCE_GUIDE.md)

**Source:** preserved MegaRE snapshot imported on 2026-10-08 · **latest sequence:** 260 · **type:** static archaeology (no added runtime experiments).

> [!NOTE]
> **This page is a navigation overlay only.** The imported primary documents, their confidence, and their original wording are not rewritten. Where a report references primary files absent from this selective import, consult the original MegaRE workspace.

## Open the right kind of evidence

| Need | Go to |
| :-- | :-- |
| Whole-engine composition and flow | [Final engine flow](maps/FINAL_ENGINE_FLOW_MAP.md) · [Frame and lifecycle](maps/FRAME_AND_LIFECYCLE_PIPELINE.md) |
| Engine subsystem relationships | [Cross-subsystem dataflow](maps/CROSS_SUBSYSTEM_DATAFLOW.md) · [Subsystem roots](maps/SUBSYSTEM_ROOT_MAP.md) |
| Exact mechanism-level finding | [Findings catalogue](FINDINGS_INDEX.md) |
| Topic-specific maps | [Maps and reports index](MAPS_REPORTS_INDEX.md) |
| Address or object search | [Address map](reports/DP_ENGINE_ADDRESS_MAP_FINAL.md) · [Object model](reports/DP_OBJECT_MODEL_FINAL.md) |
| Cross-build comparison | [Steam/GOG correspondence](maps/STEAM_GOG_FLOW_CORRESPONDENCE.md) · [Homology report](reports/DP_CROSS_BUILD_HOMOLOGY_FINAL.md) |
| Structural-foundation repair | [Foundation map](maps/STRUCTURAL_FOUNDATION_MAP_2026-10-06.md) · [Repair report](reports/STRUCTURAL_FOUNDATION_REPAIR_2026-10-06.md) |
| Disproven claims | [Corrections report](reports/DP_RE_CORRECTIONS_FINAL.md) · [ZachFix corrections](../../disproven.md) |
| Where claims stop | [Unresolved archaeology](reports/DP_UNRESOLVED_ARCHAEOLOGY_FINAL.md) · [Coverage](reports/DP_EXECUTABLE_COVERAGE_FINAL.md) |
| How the conclusions evolved | [Checkpoint timeline](CHECKPOINTS_INDEX.md) |

## Publication boundary

| Preserved scope | Count |
| :-- | --: |
| Primary finding pages | 281 |
| Finding-category README pages | 5 |
| Architecture and flow map files (including README) | 44 |
| Canonical semantic ledgers | 11 |
| Reports (includes machine-readable control/publication receipts) | 24 |
| Retained checkpoints 238–260 | 23 |

`VERIFIED` establishes **selected bounded mechanics**, not exhaustive engine semantics. Historical 20,252-row function accounting and the repaired 21,871-entry structural census cannot be freely combined. [See status](../../STATUS.md).

## Storage and provenance

The primary [import README](README.md) describes the selected scope. The full archive (`MegaRE_Final`) includes audit/scratch/frozen dumps intentionally not duplicated here. `SHA256SUMS.txt` is retained with the imported source metadata.
'''
    return write(MEGA/'INDEX.md',text,check)


def format_name(p:Path)->str:
    title=heading(p)
    # Do not turn unverified confidence words into editorial tags.
    return title.replace('|',r'\|')


def list_mega(check:bool)->bool:
    good=True
    findings=MEGA/'findings'
    text='''# MegaRE primary findings | Complete catalogue

[← MegaRE index](INDEX.md) · [Readable synthesis](../../engine/mega-re-final.md) · [Maps & reports](MAPS_REPORTS_INDEX.md)

**281 finding pages**, plus five category README files (286 Markdown files total). Entries link directly to the untouched source documents. Use the title to locate a mechanism; the proof status must be read from the document, not guessed from a filename.

'''
    for dir in sorted(findings.iterdir()):
        if not dir.is_dir():continue
        files=sorted(p for p in dir.glob('*.md') if p.name!='README.md')
        text+=f'## {dir.name.title()} · {len(files)}\n\n| Primary finding | Source file |\n| :-- | :-- |\n'
        for p in files:
            text+=f'| [{format_name(p)}](findings/{dir.name}/{p.name}) | `{p.stem}` |\n'
        text+='\n'
    good &= write(MEGA/'FINDINGS_INDEX.md',text,check)
    text='''# MegaRE maps and reports | Source catalogue

[← MegaRE index](INDEX.md) · [Findings catalogue](FINDINGS_INDEX.md) · [Checkpoint timeline](CHECKPOINTS_INDEX.md)

Maps are readable topologies and dependency sketches; reports carry fuller reasoning, residual limits, corrections and coverage. Neither replaces the cited primary findings.

'''
    for dirname in ['maps','reports','ledgers']:
        d=MEGA/dirname
        files=sorted(p for p in d.iterdir() if p.is_file() and p.name!='README.md')
        text+=f'## {dirname.title()} · {len(files)} files\n\n| File | Type |\n| :-- | :-- |\n'
        for p in files:
            title=format_name(p) if p.suffix=='.md' else p.name
            text+=f'| [{title}]({dirname}/{p.name}) | `{p.suffix.lstrip(".") or "file"}` |\n'
        text+='\n'
    good &= write(MEGA/'MAPS_REPORTS_INDEX.md',text,check)
    cps=sorted((MEGA/'checkpoints').glob('*.md'))
    text='''# MegaRE checkpoint timeline | Retained source snapshots

[← MegaRE index](INDEX.md) · [Status](../../STATUS.md)

**23 retained checkpoints, 238–260.** These are historical research checkpoints, not new runtime validation or automatically promoted claims. For the current synthesis, start at [MegaRE final](../../engine/mega-re-final.md).

| Checkpoint | Source |
| :-- | :-- |
'''
    for p in cps:
        m=re.search(r'seq0*(\d+)',p.name,re.I)
        seq=m.group(1) if m else p.stem
        text+=f'| {seq} | [{format_name(p)}](checkpoints/{p.name}) |\n'
    good &= write(MEGA/'CHECKPOINTS_INDEX.md',text,check)
    return good


def do_main(check:bool)->bool:
    good=make_curated_index(check) & make_evidence_index(check) & make_mega_index(check) & list_mega(check)
    readable=[ROOT/f for f,_,_ in GUIDES if f in {'methodology.md','disproven.md','unresolved.md'}]
    for area,_,names in AREAS:
        readable += [ROOT/area/f for f in names]
    for p in readable:
        good &= add_navigation(p,check)
    return bool(good)

if __name__=='__main__':
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--check',action='store_true',help='verify that generated contents are current')
    args=ap.parse_args()
    result=do_main(args.check)
    print('Navigation/indexes OK' if result else 'Navigation/indexes need rebuilding')
    raise SystemExit(0 if result else 1)
