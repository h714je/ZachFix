# CRdMovie organizing root above DirectShow

**Date:** 2026-10-02. **Outcome:** ADVANCE; the selected audio search uncovered a movie/media root, not a general gameplay-audio manager.

## Source contract

`AUDIO_DIRECTSHOW_GAME_OWNER`: paired direct acquiring/shutdown caller chains above known setup `00735D00/00735A10` and release `00736300/00736010`, included only with concrete receiver/global provenance. Exclude repeating WndProc event draining and middleware enumeration. Useful success: a game-owned organizing root and lifetime edge; negative condition: stop at first untyped owner. Existing H0240 notification correspondence is reused, not rediscovered.

## Positive root/lifetime evidence

`VERIFIED`: `00736010/00735D20` opens a named source through the global DirectShow graph, attaches a Sample Grabber and Null Filter, starts `IMediaControl`, and sets readiness `014B09E8` only on success. The graph/interface globals are **shared process globals**, not fields of a game-owned audio manager.

`VERIFIED`: their only direct higher caller is `00700260/00700280`, a CRdMovie method. It changes the requested path extension to `wmv`, prepares a wide path, calls source setup, and falls back to `UPDATA/MOVIE/TitleBG0.wmv` when readiness is zero. It allocates a separate `0x20` helper into receiver `+0x08`, initializes a `0x500 x 0x2D0` (1280 x 720) surface/helper, and sets receiver `+0x04` to 1. The concrete type of the helper is not assigned solely from this allocation.

`VERIFIED`: title-start pair `00641500/00641450` acquires `00BD9E48`, lazily allocates `0x0C`, writes the named `CSingleton<CRdMovie>` vtable `0076E838/0076E828`, retains the same pointer in ESI, and passes ESI as ECX to movie-start `00700260/00700280`. It also sets active-manager field `+0x6770` bit 0. This positively links the movie root to a native title/mode boundary; the full meaning of that manager flag is UNKNOWN. Other command contexts request POLD235/ADD00_US and related movie names; command taxonomy is outside this batch.

`VERIFIED`: explicit stop pair `006425F0/00642540` acquires the same named root and passes it as ECX to `00700480/007004A0`. Cleanup calls graph-stop `00736230/00735F40`, then supplies receiver `+0x08` as ECX to `006B59A0/006B58F0`, clears `+0x08`, and clears active byte `+0x04`. Graph-stop resets completion, conditionally stops IMediaControl, releases the retained interface globals, and calls `CoUninitialize`. The release helper invokes COM slot +8 then nulls each pointer. This is a positive session cleanup/reset, **not** evidence that the singleton allocation itself is freed.

`VERIFIED` at inspected method scope: `00700500/00700520` consumes the movie helper, copies 720 rows of `0x1400` bytes from the sample buffer into the surface with vertically reversed source indexing, polls completion, and calls the same object cleanup on completion. Its entire application-loop caller/mode placement is not established here. No live frame/audio cadence follows from this static update body.

## Root/interface table limits

Named `CRdMovie` and `CSingleton<CRdMovie>` tables each expose one function pointer before the next metadata word: Steam `0076E7B8/0076E838` slot 0 -> `004034B0`; GOG `0076E7A8/0076E828` slot 0 -> `00403010`. Their deletion-target semantics and actual singleton free invocation are not reanalyzed. The root is a 0x0C shell with observed active byte +4 and helper pointer +8, not a CRdObject registered active object. No handle registration or manager-owned delete relationship is fabricated.

## Correction / first missing edges

The earlier `directshow_audio_graph_initialize` canonical label and “audio graph globals” wording were overly narrow: current caller/path/surface/update evidence identifies **movie/media playback including audio interfaces**. Existing notification, event-code, and COM-interface mechanics remain valid. This does not disprove audio participation, but it does disprove using this path as evidence that a general gameplay-audio root was found. Preserve broad AUDIO as unresolved and add MEDIA_PLAYBACK as its own organizing-root family.

First missing edges: main game audio/music/SFX acquisition and manager boundary; full movie mode/application-update selection; movie singleton free caller and precise surface-helper type. Runtime media cadence remains B0007-blocked. Static media session cleanup is positive, while singleton lifetime is explicitly bounded to observed acquisition/stop uses—not globally claimed permanent.

## Primary references

- Source setup/release: Steam `inputs/decompiler/steam/DP_decompiled.c:514563-514609`, `:514610-514626`, `:514664-514689`, `:514990-515004`; GOG `inputs/decompiler/gog/DP_decompiled.c:411512-411558`, `:411559-411575`.
- Movie start/stop/update: Steam `inputs/decompiler/steam/DP_decompiled.c:479738-479827`, `:479848-479888`; GOG `inputs/decompiler/gog/DP_decompiled.c:376931-377020`, `:377041-377081`.
- Root acquisition: Steam `inputs/decompiler/steam/DP_decompiled.c:360339-360376`; GOG `inputs/decompiler/gog/DP_decompiled.c:278257-278294`. Raw result/ECX check: Steam `inputs/decompiler/steam/DP_full.asm:645542-645576`; GOG `inputs/decompiler/gog/DP_full.asm:494430-494464`.
- Explicit stop root receiver: Steam `inputs/decompiler/steam/DP_full.asm:646793-646811`; GOG `inputs/decompiler/gog/DP_full.asm:495681-495699`; cleanup field flow Steam `:868785-868799`, GOG `:691044-691058`.
- Graph call/fallback bytes: Steam `inputs/decompiler/steam/DP_full.asm:868725-868748`; GOG `inputs/decompiler/gog/DP_full.asm:690984-691007`.
- Table identity: build-specific `symbols.csv` CRdMovie/CSingleton<CRdMovie> symbols and raw `inputs/binaries/steam/DP_STEAM.exe` / `inputs/binaries/gog/DP_GOG.exe` at the table VAs above. Existing H0240/BND-077 is reused only for notification/interface identification.

## Red-team receiver check — strategic review, 2026-10-02

The movie-update class assignment was checked against its sole direct call context, not accepted from matching +8 fields alone. Paired `00401440` loads/acquires `00BD9E48`, writes the named singleton table on allocation, and passes that exact pointer in ECX to `00700500/00700520` at `004016FC`. Primary raw anchors: Steam `inputs/decompiler/steam/DP_full.asm:464-478`; GOG `inputs/decompiler/gog/DP_full.asm:464-478`. This corroborates the existing CRdMovie update identity. It does not establish the complete mode-gate/order relationship between `00401440` and frame root `00401A70`; that is the proposed later Phase 3 entry question, not new Phase 3 research.

## Successor card

`RESOURCE_XPC_PORTFOLIO_PRIMARY_RECHECK`, RESOURCE_FACTORY_OWNERSHIP: high payoff is a **distinct positive resource lifecycle** rather than more movie wrappers. New discriminator is a targeted provenance/integration test of the already selected archive -> callback-created CRdPicture -> manager +0x1C -> named LEVEL.XPC -> CLevel +0x164 chain, not reopening bounded parser/render scans. Resolve build-qualified references and raw descriptor commit/receiver flow, reject any implicit type transfer, and identify the first missing edge. Cost is one named chain. Negative value is a precise break in a foundational closeout portfolio. Deep GOG inspection only where identity/export boundaries affect this promotion; otherwise reuse established homologues at their proven scopes.
