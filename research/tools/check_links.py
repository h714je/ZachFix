#!/usr/bin/env python3
"""Check links and in-page anchors in reader-facing pages and generated indexes."""
from __future__ import annotations

import argparse
import re
from pathlib import Path
from urllib.parse import unquote

ROOT = Path(__file__).resolve().parents[1]
MEGA = ROOT / 'evidence' / 'mega_re_final_2026-10-08'


def without_code(s: str) -> str:
    s = re.sub(r'(?ms)^\s*```.*?^\s*```[^\n]*$', '', s)
    return s


def slugs(s: str) -> set[str]:
    r = set();seen={}
    for line in without_code(s).splitlines():
        m=re.match(r'^#{1,6}\s+(.+?)\s*#*$',line)
        if not m:continue
        title=m.group(1).lower().replace('`','').replace('*','')
        title=re.sub(r'<[^>]*>','',title)
        title=re.sub(r'\[([^]]+)\]\([^)]+\)',r'\1',title)
        title=re.sub(r'[^\w\- ]','',title)
        title=re.sub(r'\s+','-',title.strip())
        n=seen.get(title,0);seen[title]=n+1
        r.add(title+(('-'+str(n)) if n else ''))
    return r


def validate(paths, check_anchors):
    problems=[]; links=0
    for f in paths:
        raw=without_code(f.read_text('utf-8'))
        for match in re.finditer(r'(?<!!)\[[^\]\n]*(?:\\\][^\]\n]*)?\]\(([^)\n]+)\)',raw):
            u=match.group(1).strip().split(' "')[0]
            if not u or re.match(r'^(https?://|mailto:|//|data:)',u):continue
            if u.startswith('<') and u.endswith('>'):u=u[1:-1]
            filepart, _, anchor=u.partition('#')
            target=(f.parent/unquote(filepart)).resolve() if filepart else f
            links+=1
            if not target.is_relative_to(ROOT):continue
            if not target.exists():
                problems.append(f'{f.relative_to(ROOT)}: missing {u}')
            elif check_anchors and anchor and target.suffix=='.md':
                if unquote(anchor) not in slugs(target.read_text('utf-8-sig')):
                    problems.append(f'{f.relative_to(ROOT)}: anchor missing {u}')
    return links,problems


def paths_readable():
    base=[p for p in ROOT.rglob('*.md') if not p.is_relative_to(ROOT/'evidence')]
    base += [p for p in (ROOT/'evidence').glob('INDEX.md')]
    base += [p for p in MEGA.glob('*INDEX.md')]
    base += [MEGA/'README.md']
    return sorted(set(base))


if __name__=='__main__':
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--anchors',action='store_true',help='also inspect local markdown anchor fragments')
    a=ap.parse_args()
    paths=paths_readable()
    links,problems=validate(paths,a.anchors)
    print(f'Checked {len(paths)} reader-facing Markdown files; {links} local links.')
    for problem in problems:print('FAIL',problem)
    print('PASS' if not problems else f'FAIL: {len(problems)} broken links or anchors')
    raise SystemExit(bool(problems))
