"""Read-only, bounded PC protocol extraction. Requires GNU objdump, Python 3.

Usage: python extract_protocol.py DP_GOG.exe DP_STEAM.exe OUTPUT_DIRECTORY
No game code is executed. Branch-table bytes are read as data, not instructions.
"""
import argparse, hashlib, json, re, struct, subprocess
from pathlib import Path

class PE:
    def __init__(self,path):
        self.path=Path(path); self.data=self.path.read_bytes()
        h=struct.unpack_from('<I',self.data,0x3c)[0]
        self.base=struct.unpack_from('<I',self.data,h+52)[0]
        count=struct.unpack_from('<H',self.data,h+6)[0]
        size=struct.unpack_from('<H',self.data,h+20)[0]
        self.sections=[]
        for i in range(count):
            a=h+24+size+i*40
            vs,va,rs,rp=struct.unpack_from('<4I',self.data,a+8)
            self.sections.append((va,rs,rp))
    def read(self,va,n):
        r=va-self.base
        for v,s,p in self.sections:
            if v<=r and r+n<=v+s:return self.data[p+r-v:p+r-v+n]
        raise ValueError(hex(va))
    def u32(self,va):return struct.unpack('<I',self.read(va,4))[0]
    def dis(self,start,end):
        s=subprocess.check_output(['objdump','-d','-w','-Mintel','--no-show-raw-insn',
            f'--start-address={start}',f'--stop-address={end}',str(self.path)],text=True)
        return s

def selector(pe,delta):
    text=pe.dis(0x5092a0-delta,0x509788-delta)
    rows=[]
    for line in text.splitlines():
        m=re.match(r'\s*([0-9a-f]+):\s*(.*)',line)
        if m:rows.append((int(m[1],16),m[2]))
    ix={a:i for i,(a,t) in enumerate(rows)}
    result=[]
    for event in range(0x2d,0x7d):
        a=pe.u32(0x509788-delta+4*(event-0x2d)); i=ix[a]
        state=None; mask=None; trace=[]
        for _ in range(40):
            pc,t=rows[i];trace.append(f'{pc:08X}')
            if t.startswith('ret'):break
            m=re.fullmatch(r'push\s+0x([0-9a-f]+)',t)
            if m:mask=int(m[1],16)
            m=re.fullmatch(r'mov\s+esi,0x([0-9a-f]+)',t)
            if m:state=int(m[1],16)
            m=re.fullmatch(r'jmp\s+0x([0-9a-f]+)',t)
            if m:i=ix[int(m[1],16)]
            else:i+=1
        else:raise RuntimeError('Selector path limit')
        result.append({'event':f'{event:02X}','candidate':f'{state:02X}' if state is not None else None,
                       'gate_mask':f'0x{mask:X}' if mask is not None else None,'case_va':f'{a:08X}'})
    return result

def shared(pe,delta):
    out=[]
    for state in [0x43,0x44,*range(0x5a,0x64),0x66,0x68]:
        ix=pe.read(0x4f5c20-delta+state-0x43,1)[0]
        target=pe.u32(0x4f5be8-delta+ix*4)
        b=pe.read(target,8)
        if b[0]==0xbe and b[5:7]==b'\x8d\x7e':
            phase=struct.unpack_from('<I',b,1)[0];event=phase+struct.unpack('b',b[7:8])[0]
        else:phase=event=None
        out.append({'state':f'{state:02X}','phase':f'{phase:02X}' if phase is not None else None,
                    'event':f'{event:02X}' if event is not None else None,'case_va':f'{target:08X}'})
    return out

def main():
    ap=argparse.ArgumentParser();ap.add_argument('gog');ap.add_argument('steam');ap.add_argument('output');a=ap.parse_args()
    out=Path(a.output);out.mkdir(parents=True,exist_ok=True)
    report={}
    for name,path,delta in [('gog',a.gog,0),('steam',a.steam,0xd0)]:
        pe=PE(path)
        states=[pe.u32(0x8a9758+4*i) for i in range(137)]
        car_case=pe.u32(0x5479c8-delta+4*pe.read(0x547a00-delta+0x35-3,1)[0])
        refresh_case=pe.u32(0x534738-delta+4*pe.read(0x534798-delta+0x67,1)[0])
        assert car_case==0x544d6e-delta
        assert refresh_case==0x534173-delta
        report[name]={'sha256':hashlib.sha256(pe.data).hexdigest(),'states':[f'{x:08X}' for x in states],
                      'non_null':sum(bool(x) for x in states),'unique_non_null':len(set(states)-{0}),
                      'selector':selector(pe,delta),'shared_completion':shared(pe,delta),
                      'car_event35_dispatch_case':f'{car_case:08X}','player_event67_dispatch_case':f'{refresh_case:08X}',
                      'car_phase_cases':{f'{x:02X}':f'{pe.u32(0x542418-delta+4*(x-0xf)):08X}' for x in range(0xf,0x16)}}
        ranges={'action_selector':(0x5092a0,0x509788), 'action_ingress':(0x50a130,0x50a4ce),
                'packet_refresh':(0x509f90,0x509fab),'shared_completion':(0x4f56f0,0x4f5be6),
                'vehicle_exit_prelude':(0x4dd0b0,0x4dd21d), 'vehicle_entry':(0x4dde80,0x4de2a0),
                'vehicle_cleanup':(0x4de2a0,0x4de69a),'vehicle_hub38':(0x4de6a0,0x4dee6b),
                'car_packet_consumer':(0x5422a0,0x542416),'active_car_exit_request':(0x4dd8a0,0x4dd961),
                'player_packet_refresh_dispatch':(0x534173,0x5341a0),
                'vehicle_setup':(0x4dc4a0,0x4dcba0),'car_mode_enable':(0x4dcfa0,0x4dd020),
                'car_event_dispatch_entry':(0x544b80,0x544bc8),'car_event35_dispatch_case':(0x544d6e,0x544d88),
                'player_event_dispatch_entry':(0x52ec90,0x52ecbd),
                'car_auxiliary_release':(0x548010,0x548032),'car_auxiliary_create':(0x5480b0,0x548155),
                'car_auxiliary_objects_release':(0x540f30,0x540f6c)}
        for label,(start,end) in ranges.items():
            (out/f'{name}_{label}.asm').write_text(pe.dis(start-delta,end-delta))
    for key in ['selector','shared_completion']:
        clean=lambda xs:[{k:v for k,v in x.items() if k!='case_va'} for x in xs]
        assert clean(report['gog'][key])==clean(report['steam'][key]),key
    assert all((int(g,16)-int(s,16)==0xd0) if int(g,16) else int(s,16)==0
               for g,s in zip(report['gog']['states'],report['steam']['states']))
    report['verification']='GOG/Steam: 137 slots agree at the observed handler delta; selector outputs/gates and shared phase/event mappings agree. Local delta only, not a global address translation rule.'
    (out/'protocol.json').write_text(json.dumps(report,indent=2)+'\n')
    print(report['verification']);print('Non-null / unique:',report['gog']['non_null'],report['gog']['unique_non_null'])
    for x in report['gog']['shared_completion']:print(x)

if __name__=='__main__':main()
