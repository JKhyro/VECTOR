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

## Main program child-program contract

- The governing desktop program is `vector-main-program`.
- The first two first-class child programs are `vector-left-menu-program` and `vector-main-interaction-program`.
- `vector-left-menu-program` owns the left-side menu region for navigation, launching, workspace selection, and thread selection.
- `vector-main-interaction-program` owns the main interaction space for sessions, turns, approvals, artifacts, and runtime interaction.
- The current Avalonia continuity shell should route around those two regions explicitly rather than leaving the host as one undifferentiated diagnostics surface.
- Useful upstream Codex session-turn behavior should land inside `vector-main-interaction-program` instead of taking over the full shell.
- Candidate follow-on child programs are `vector-context-inspector-program`, `vector-approval-review-program`, `vector-artifact-inspector-program`, and `vector-status-activity-rail-program`.
- Avalonia hosts those regions, but the Native C runtime remains the authority for the child-program contract and switching rule.
- The current host seam now renders dedicated left-menu and main-interaction regions from that runtime contract instead of treating the split as an implicit operator convention.

## Child-program ownership

### 1. FLUX/ANVIL execution-governance child lane

- Purpose: version-control and GitHub execution truth.
- Governing database lane: `FLUXBASE`.
- Owns: repositories, issues, pull requests, discussions, commits, project items, and related execution-governance records.
- Does not own: VECTOR private memory, code-sandbox truth, or chat transcripts as their primary authority.

### 2. VECTOR memory and active-management child lane

- Purpose: VECTOR durable memory, retrieval, RAG, recursion, and active workspace management state.
- Governing database lane: `DEVBASE`.
- Owns: persistent memory, memory retrieval state, recursive retrieval growth, active management state, the forward replacement path for SQLite-shaped app memory, and shared development/administration state across `VECTOR`, `FORGE`, and `ANVIL` where that state is not ANVIL local-git or GitHub replacement truth.
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
- A shared project/development-management or development/administration operation across `VECTOR`, `FORGE`, and `ANVIL` that is not VCS/GitHub execution truth should also target `DEVBASE`.
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
- The next concrete recovery-override and next-step contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats the DEVBASE execution outcome and state-transition record as governed input, defines the recovery-override shape, and defines the next-step record used by downstream VECTOR active-management recovery flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this recovery/next-step contract, not the authority for recovery logic or next-step truth.
- The next concrete escalation and delegation contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats the DEVBASE recovery override and next-step record as governed input, defines the escalation record shape, and defines the delegation envelope used by downstream VECTOR active-management escalation flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this escalation/delegation contract, not the authority for escalation logic or delegation truth.
- The next concrete approval-gate and operator-confirmation contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats the DEVBASE escalation record and delegation envelope as governed input, defines the approval-gate shape, and defines the operator-confirmation record used by downstream VECTOR approval and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this approval/confirmation contract, not the authority for approval logic or operator-confirmation truth.
- The next concrete approval-resolution and execution-authority contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats the DEVBASE approval-gate and operator-confirmation records as governed input, defines the approval-resolution shape, and defines the execution-authority record used by downstream VECTOR execution and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this approval-resolution/execution-authority contract, not the authority for resolution logic or execution-authority truth.
- The next concrete post-approval execution-plan and dispatch contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats the DEVBASE approval-resolution and execution-authority records as governed input, defines the execution-plan shape, and defines the dispatch envelope used by downstream VECTOR runnable-work and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this post-approval execution-plan/dispatch contract, not the authority for plan assembly or dispatch truth.
- The next concrete post-dispatch observation and feedback contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats the DEVBASE execution plan and dispatch envelope as governed input, defines the observation shape, and defines the feedback record used by downstream VECTOR execution-review and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this post-dispatch observation/feedback contract, not the authority for observation assembly or feedback truth.
- The next concrete feedback-resolution and plan-adjustment contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE dispatch observation and feedback records as governed input, defines the feedback-resolution shape, and defines the plan-adjustment record used by downstream VECTOR execution-adjustment and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this feedback-resolution/plan-adjustment contract, not the authority for resolution assembly or adjustment truth.
- The next concrete reprioritization and queue-mutation contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE feedback-resolution and plan-adjustment records as governed input, defines the reprioritization shape, and defines the queue-mutation record used by downstream VECTOR work ordering and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this reprioritization/queue-mutation contract, not the authority for reprioritization or queue truth.
- The next concrete queue-execution-readiness contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE reprioritization and queue-mutation records as governed input, defines the queue-execution shape, and defines the execution-readiness record used by downstream VECTOR execution gating and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this queue-execution-readiness contract, not the authority for queue execution or readiness truth.
- The next concrete ready-queue claim and execution-lease contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE queue-execution and execution-readiness records as governed input, defines the queue-claim shape, and defines the execution-lease record used by downstream VECTOR executable-ownership and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this ready-queue-claim/execution-lease contract, not the authority for queue claims or execution-lease truth.
- The next concrete lease-heartbeat and ownership-renewal contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE queue-claim and execution-lease records as governed input, defines the lease-heartbeat shape, and defines the ownership-renewal record used by downstream VECTOR continued-ownership and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this lease-heartbeat/ownership-renewal contract, not the authority for lease continuity or ownership-renewal truth.
- The next concrete lease-expiry and reclaim contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE lease-heartbeat and ownership-renewal records as governed input, defines the lease-expiry shape, and defines the ownership-reclaim record used by downstream VECTOR reclaim and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this lease-expiry/reclaim contract, not the authority for expiry or reclaim truth.
- The next concrete reclaimed-work requeue and fairness contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE lease-expiry and ownership-reclaim records as governed input, defines the requeue shape, and defines the fairness record used by downstream VECTOR safe re-entry and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this reclaimed-work-requeue/fairness contract, not the authority for requeue or fairness truth.
- The next concrete requeue outcome and queue-admission acknowledgment contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE requeue and fairness records as governed input, defines the requeue-outcome shape, and defines the queue-admission acknowledgment record used by downstream VECTOR queue re-entry and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this requeue-outcome/queue-admission contract, not the authority for admission truth.
- The next concrete queue-admission settlement and ready-queue handoff contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE requeue-outcome and queue-admission acknowledgment records as governed input, defines the settlement shape, and defines the ready-queue handoff record used by downstream VECTOR ready-queue ownership and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this queue-admission-settlement/ready-queue-handoff contract, not the authority for handoff truth.
- The next concrete ready-queue handoff confirmation and lease activation contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE queue-admission settlement and ready-queue handoff records as governed input, defines the handoff-confirmation shape, and defines the lease-activation record used by downstream VECTOR active-lease ownership and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this ready-queue-handoff-confirmation/lease-activation contract, not the authority for lease truth.
- The next concrete lease-activation heartbeat and live-lease continuity contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE ready-queue handoff confirmation and lease activation records as governed input, defines the heartbeat shape, and defines the live-lease continuity record used by downstream VECTOR active-lease continuity and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this lease-activation-heartbeat/live-lease-continuity contract, not the authority for live lease truth.
- The next concrete live-lease continuity loss and recovery contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE lease-activation heartbeat and live-lease continuity records as governed input, defines the continuity-loss shape, and defines the recovery record used by downstream VECTOR active-lease recovery and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this live-lease-continuity-loss/recovery contract, not the authority for recovery truth.
- The next concrete recovery outcome and live-lease reassignment contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE live-lease continuity loss and recovery records as governed input, defines the recovery-outcome shape, and defines the reassignment record used by downstream VECTOR active-lease reassignment and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this recovery-outcome/live-lease-reassignment contract, not the authority for reassignment truth.
- The next concrete reassignment confirmation and replacement lease activation contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE recovery-outcome and live-lease reassignment records as governed input, defines the reassignment-confirmation shape, and defines the replacement-lease-activation record used by downstream VECTOR replacement-lease ownership and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this reassignment-confirmation/replacement-lease-activation contract, not the authority for replacement-lease truth.
- The next concrete replacement-lease heartbeat and successor live-lease continuity contract is now also defined as a `DEVBASE`-owned VECTOR memory operation.
- That contract treats DEVBASE reassignment-confirmation and replacement-lease-activation records as governed input, defines the replacement-lease-heartbeat shape, and defines the successor-live-lease-continuity record used by downstream VECTOR successor-lease ownership and operator-review flow.
- `FLUXBASE`, `CODEBASE`, and `CHATBASE` remain reference lanes only for this replacement-lease-heartbeat/successor-live-lease-continuity contract, not the authority for successor continuity truth.

## Immediate next slice

- Move from DEVBASE decision-envelope, execution-outcome, state-transition, recovery-override, escalation, approval, approval-resolution, post-approval dispatch, post-dispatch feedback, feedback-resolution/plan-adjustment, reprioritization/queue-mutation, queue-execution-readiness, ready-queue claim plus execution-lease, lease-heartbeat plus ownership-renewal, lease-expiry plus reclaim, reclaimed-work requeue plus fairness, requeue outcome plus queue-admission acknowledgment, queue-admission settlement plus ready-queue handoff, ready-queue handoff confirmation plus lease activation, lease-activation heartbeat plus live-lease continuity, live-lease continuity loss plus recovery, recovery outcome plus live-lease reassignment, reassignment confirmation plus replacement lease activation, and replacement-lease heartbeat plus successor live-lease continuity contract definition into bounded live selection, citation, handoff, decision execution, state-transition execution, recovery execution, delegation execution, approval execution, approval-resolution execution, dispatch execution, feedback execution, adjustment execution, queue-mutation execution readiness, queue claim execution, live execution lease, live ownership renewal, live reclaim execution, live requeue execution, live queue-admission execution, live ready-queue handoff execution, live lease activation execution, live lease continuity execution, live lease recovery execution, live lease reassignment execution, live replacement lease activation execution, and live successor continuity execution without collapsing the named lane boundaries.
