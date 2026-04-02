# VECTOR Runtime Topology: First Slice

This document turns the current VECTOR doctrine into a first concrete runtime-topology slice.

It is intentionally bounded. It does not describe every future subsystem. It defines the first ownership map that keeps the named database lanes explicit while VECTOR continues to absorb useful Codex behavior.

## Core rule

- Native C owns the core runtime, process routing, child-program ownership map, and the governed database-lane contract.
- Avalonia owns the desktop shell, windows, presentation, and narrow managed host glue over the Native C runtime.
- C# stays limited to Avalonia composition, host bootstrap, and interop glue where needed.

## Parent shell

- The parent desktop shell is Avalonia-hosted.
- The parent shell does not own durable runtime truth.
- The parent shell asks the Native C runtime which lane and which child-program contract applies for a given operation.
- The parent shell renders state returned by the runtime instead of inventing separate ownership rules in managed code.

## Child-program ownership

### 1. FLUX/ANVIL execution-governance child lane

- Purpose: version-control and GitHub execution truth.
- Governing database lane: `FLUXBASE`.
- Owns: repositories, issues, pull requests, discussions, commits, project items, and related execution-governance records.
- Does not own: VECTOR private memory, code-sandbox truth, or chat transcripts as their primary authority.

### 2. VECTOR memory and active-management child lane

- Purpose: VECTOR durable memory, retrieval, RAG, recursion, and active workspace management state.
- Governing database lane: `DEVBASE`.
- Owns: persistent memory, memory retrieval state, recursive retrieval growth, active management state, and the forward replacement path for SQLite-shaped app memory.
- Does not own: VCS/GitHub truth, code sandbox truth, or communications as their primary authority.

### 3. FORGE code-development child lane

- Purpose: code/library storage and functional code-development execution.
- Governing database lane: `CODEBASE`.
- Owns: code storage, library storage, development artifacts, and the functional sandbox used by `FORGE`.
- Does not own: communications or durable VECTOR memory as their primary authority.

### 4. Communication and character-interaction child lane

- Purpose: communication between agents and humans.
- Governing database lane: `CHATBASE`.
- Owns: agent-agent and agent-human communication records, including Characters such as Symbiotes, Curators, Synths, and other Agents.
- Does not own: code-sandbox truth, VCS truth, or VECTOR private-memory truth as their primary authority.

## Routing contract

- The Native C runtime is the authority that maps an operation to one primary governed lane.
- Cross-lane work is allowed, but one lane must remain authoritative for the record being created or updated.
- A retrieval or workspace-management operation that would previously drift into SQLite should now target `DEVBASE`.
- A repository or GitHub execution operation should target `FLUXBASE`.
- A code-development or sandbox operation should target `CODEBASE`.
- A communication operation should target `CHATBASE`.

## Current bounded implementation implication

- Current CODEX continuity work may still expose bounded seams through the Codex Workbench line.
- Those seams should keep reporting `FLUXBASE`, `DEVBASE`, `CODEBASE`, and `CHATBASE` explicitly rather than collapsing state back into one local app store.
- Upstream `openai/codex` session and retrieval behavior can still be assimilated, but the authoritative lane for forward VECTOR memory remains `DEVBASE`, not SQLite.

## Completed next slice

- The first concrete retrieval-selection and memory-citation contract is now defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats pgvector hits as selection input, authoritative Postgres records as selection truth, and governed memory citations as the downstream shape consumed by RAG, recursion, and operator preview surfaces.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain cross-lane reference surfaces only for that contract, not the primary authority for memory selection or citation materialization.
- The next concrete retrieved-context bundle and active-management handoff contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE memory citations as governed input, assembles a retrieved-context bundle, and defines the handoff payload shape used by VECTOR active-management follow-on work.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this handoff contract, not the authority for bundle assembly or management handoff truth.
- The next concrete active-management decision-envelope and retrieval-action contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats the DEVBASE handoff payload as governed input, defines the active-management decision envelope, and defines the retrieval-action shape used by downstream VECTOR management and operator-review flows.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this decision/action contract, not the authority for decision assembly or retrieval-action truth.
- The next concrete execution-outcome and state-transition contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats the DEVBASE decision envelope and retrieval action as governed input, defines the execution-outcome shape, and defines the governed state-transition record used by downstream VECTOR active-management state.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this outcome/transition contract, not the authority for outcome materialization or state-transition truth.

## Immediate next slice

- Move from DEVBASE decision-envelope, execution-outcome, and state-transition contract definition into bounded live selection, citation, handoff, decision execution, and state-transition execution readiness without collapsing the named lane boundaries.
