# CRdInput available gated request-pair interface

## Primary scope

**VERIFIED C0305, Steam only:** main006E15F0[006E15F0,006E164B),33 owned instructions/91 bytes, exactPEASM/metadata/interval MATCH. Source scratch/phase6_main_input91_personal_seq0180.json. No helper, provider-method, caller, gap or runtime acquisition. C0302 supplies selected availableCRdInput getter-result receiver context; C0298 supplies its base-initialization recipe.

Saved incomingECX=R. If DWORD[R4]=0, the method skips all request/field-update instructions and leavesEAX=R at normalexit. Otherwise:
1. w2, the third stackDWORD, is logically shifted right6 and saved locally.
2. Pushw1 thenw0; call opaque006E1840; copy returnedEAX toECX; call opaque00735B80.
3. Push(w2>>6) thenw0; independently call006E1840 again; copy returnedEAX toECX; call opaque00735BB0.
4. Storew0 toDWORD[R8], store fourth stackword asfloat toRC, and normalexit withEAX=R/RET10.

The exact pre-helper stack operand placement is proved. It is not yet proved which words006E1840 consumes and which downstream method consumes them, or that both returnedprovider-like operands refer to one object/generation. The two directtargets are emitted directcalls, not observed virtual dispatch.

At the C0302 selected caller sites, the four prepared stackwords persist across the accepted getter's normalRET-without-immediate boundary. Composing that available ABI path withRET10 yields selectedw0=0,w1=FFFF-or8000,w2=0,w3=float10 at this method. This qualifies them as method-stack operands rather than inventing getter parameters. No physicalinput masks, motor values, duration units, currentdevice or backend semantics are established.

ConstructorC0298 writesR4=1/R8=0/RC=float0; this method readsR4 as nonzero gate and replacesR8/RC after two opaque requests. These are available same-field relationships, not proof of liveconstructor→caller chronology, currentinstance, successful effect, exclusive ownership or lifetime.

## Architectural interpretation and limit

**STRONG_INFERENCE:** CRdInput-compatible nonzero-gated external-request interface with remembered numeric/float state, rather than merely an unconditional local-field setter. No complete input/OS/backend service role or new manager is proved. R4 is an observed gate, not a named enabled policy; R8/RC are remembered operands, not typed controller/time fields.

First static stopping edge006E1613→006E1840 is potentially architecture-sensitive: its exact available result/source relationship could connect this CRdInput interface to establishedCInput/provider storage or expose anothermechanism. One finite helper/type-source discriminator is justified; do not blindly walk both downstream methods or infer the provider from names. Runtime object/generation, acquisition, synchronization, currentconfiguredstate, ownership, lifecycle andsafe retirement remain UNKNOWN.

RD_ROOT123's91-byte method mechanics are resolved at available-static scope; provider identity remains a possible finite architectural join, not automatically a demand for full hardware semantics.168NOT_READY remains; no re-review, Phase7, BND-244 or universalbuild parity.
