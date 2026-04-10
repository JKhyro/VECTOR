# VECTOR

VECTOR is the workspace-facing execution surface in the `SYNAPSE` suite.

VECTOR consumes CORTEX-managed characters and agent components and exposes them in productive workspace flows, child programs, and operator or user interaction surfaces. It supersedes the old CODEX product direction without inheriting a managed-first runtime model.

## Boundary

- `CORTEX` owns character construction, ARA composition, lifecycle, and component governance.
- `VECTOR` owns workspace-facing execution, task interaction, file and tool surfaces, and the runtime contract that lets managed characters do useful work.
- `SYMBIOSIS` routes harvested OpenClaw, Lobster, and Hermes behavior here when the capability is fundamentally workspace-facing rather than character-defining.

## Implementation direction

- Native C is the primary runtime ownership lane.
- C++ is acceptable where it materially improves the native runtime or tooling.
- Avalonia is the intended desktop host through explicit native interop.
- C# stays limited to the thinnest host, binding, and packaging glue that the Avalonia shell requires.

## Data plane

- `FLUXBASE` is the VCS and execution-governance lane for `FLUX` and related `ANVIL` work.
- `DEVBASE` is the active development and working-memory lane for `VECTOR`.
- `DEVBASE` is also the shared development and administration lane across `VECTOR`, `FORGE`, and `ANVIL` for project and development state that is not primarily VCS or GitHub truth.
- `CODEBASE` stores code, libraries, and the functional code-development and sandbox surfaces used by `FORGE`.
- `CHATBASE` stores communication state between agents and humans, including Characters such as Symbiotes, Curators, Synths, and other Agents.

## Current direction

- current CODEX continuity still lives in the Codex Workbench line while VECTOR product direction is made explicit
- useful upstream `openai/codex` behavior, contracts, and UX can be assimilated aggressively
- upstream `openai/codex` assimilation is now tracked as a concrete component map rather than a vague reference; the first map ties upstream crates and folders to VECTOR child programs
- upstream SQLite-shaped persistence is import/reference only where VECTOR now has a governed `DEVBASE` path
- VCS truth belongs in `FLUXBASE`, code-development truth belongs in `CODEBASE`, and communications belong in `CHATBASE`
- ANVIL replacement-oriented local-git and GitHub execution truth still belongs in `FLUXBASE` and the corresponding repo/project/issue/PR/discussion surfaces rather than in `DEVBASE`
- the upstream `openai/codex` clone is present as a real reference base, but VECTOR itself is not yet a merged upstream-based runnable product surface
- the current usable repo lane remains the Codex Workbench Electron prototype while Native C plus Avalonia and upstream-base assimilation continue
- the main VECTOR desktop program now treats the left-side menu and main interaction space as first-class child programs under a Native C-governed contract
- the Avalonia continuity shell now routes explicitly around those child-program regions instead of keeping the host as a flat diagnostics surface
- the Avalonia host now renders those two child-program regions directly from the runtime contract instead of leaving them implicit in one generic pane
- the main interaction child now defaults to a bounded upstream Codex-style conversation-summary, selected-turn detail, tool/payload preview, execution-result and approval-resolution continuity, approval/artifact continuity, thread-read, and turn-lifecycle preview, so useful Codex app interaction behavior is starting to land inside VECTOR without claiming a full upstream-base merge

## Active tracking

- `JKhyro/VECTOR#1` defines the VECTOR product boundary.
- `JKhyro/VECTOR#2` tracks migration from current CODEX continuity surfaces.
- `JKhyro/VECTOR#3` tracks runtime topology for child programs.
- `JKhyro/VECTOR#4` tracks packaging and distribution.

## Runtime topology

The first concrete topology slice lives in [`docs/vector-runtime-topology-first-slice.md`](docs/vector-runtime-topology-first-slice.md).

The first bounded main-program decomposition lives in [`docs/vector-main-child-program-first-slice.md`](docs/vector-main-child-program-first-slice.md).

The first Native C child-program ABI lives in [`include/vector/child_program_runtime.h`](include/vector/child_program_runtime.h). It exposes ABI/schema versions, first-class child-program descriptors, workspace-region routing, stable status names, and CORTEX character/component reference fields for the main interaction program.

The first upstream Codex component assimilation map lives in [`docs/openai-codex-component-assimilation-map.md`](docs/openai-codex-component-assimilation-map.md), with a generated local upstream manifest at [`docs/openai-codex-upstream-manifest.json`](docs/openai-codex-upstream-manifest.json).

The explicit CORTEX-to-VECTOR execution boundary now lives in [`docs/vector-character-consumer-boundary.md`](docs/vector-character-consumer-boundary.md).

## Native Build

Configure, build, and run the native child-program runtime tests with:

```powershell
cmake -S . -B "$env:LOCALAPPDATA\VECTOR\child-program-runtime-build"
cmake --build "$env:LOCALAPPDATA\VECTOR\child-program-runtime-build" --config Debug
ctest --test-dir "$env:LOCALAPPDATA\VECTOR\child-program-runtime-build" -C Debug --output-on-failure
```

## Upstream Manifest

Regenerate the local OpenAI Codex component manifest from the cloned source with:

```powershell
$env:OPENAI_CODEX_ROOT = "C:\path\to\openai-codex"
powershell -ExecutionPolicy Bypass -File tools\write-codex-upstream-manifest.ps1
```

You can also pass the source clone directly with `-OpenAiCodexRoot C:\path\to\openai-codex`. If neither is provided, the script checks for a sibling `openai-codex` clone beside the VECTOR repository.

GitHub Discussions are currently disabled for this repository, so execution truth for now lives in the repo README, issues, and the VECTOR project.
