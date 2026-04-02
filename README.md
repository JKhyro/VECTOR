# VECTOR

Active agent workspace product that supersedes CODEX in the SYNAPSE suite.

VECTOR is the forward workspace product direction being cloned from useful Codex App behavior while keeping a different long-term architecture and data model. Native C remains the intended core runtime ownership lane. Avalonia is the intended desktop host through explicit Native C interop, with C# limited to the minimum host and binding glue that path requires.

## Data plane

- `FLUXBASE` is the VCS and execution-governance lane for `FLUX` and related `ANVIL` work. It holds repository, issue, pull request, discussion, commit, and GitHub project-item state that needs durable operational truth.
- `DEVBASE` is the active development and memory lane for `VECTOR`. It replaces SQLite for governed persistent memory and is the forward expansion path for VECTOR memory, RAG, recursion, and active management beyond the upstream Codex App scope.
- `CODEBASE` stores code, libraries, and the functional code-development and sandbox surfaces used by `FORGE`.
- `CHATBASE` stores communication state between agents and humans, including Characters such as Symbiotes, Curators, Synths, and other Agents.

## Current direction

- current CODEX continuity still lives in the Codex Workbench line while VECTOR product direction is made explicit
- useful upstream `openai/codex` behavior, contracts, and UX can be assimilated aggressively
- upstream SQLite-shaped persistence is import/reference only where VECTOR now has a governed `DEVBASE` path
- VCS truth belongs in `FLUXBASE`, code-development truth belongs in `CODEBASE`, and communications belong in `CHATBASE`

## Active tracking

- `JKhyro/VECTOR#1` defines the VECTOR product boundary
- `JKhyro/VECTOR#2` tracks migration from current CODEX continuity surfaces
- `JKhyro/VECTOR#3` tracks runtime topology for child programs
- `JKhyro/VECTOR#4` tracks packaging and distribution

## Runtime topology

The first concrete topology slice now lives in [`docs/vector-runtime-topology-first-slice.md`](docs/vector-runtime-topology-first-slice.md).

That document now also records the completed DEVBASE memory steps: retrieval selection and memory-citation shape are treated as `DEVBASE`-owned contracts, the retrieved-context bundle plus active-management handoff shape is likewise `DEVBASE`-owned, the active-management decision-envelope plus retrieval-action shape follows the same rule, the execution-outcome plus state-transition shape is governed in the same DEVBASE contract lane, the recovery-override plus next-step shape follows that same DEVBASE rule, and the escalation plus delegation shape now follows that same DEVBASE rule before downstream runtime paths consume it.

That slice makes the first ownership map explicit:

- `FLUXBASE` for FLUX/ANVIL VCS and GitHub execution truth
- `DEVBASE` for VECTOR memory and active management replacing SQLite
- `CODEBASE` for code/library and FORGE sandbox execution
- `CHATBASE` for agent and human communications

GitHub Discussions are currently disabled for this repository, so execution truth for now lives in the repo README, issues, and the VECTOR project.
