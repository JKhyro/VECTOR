# OpenAI Codex Component Assimilation Map

This is the first VECTOR-owned map from the cloned upstream `openai/codex` repository into the VECTOR desktop program model.

It exists because VECTOR is cloning and transforming Codex, not merely referencing it. The upstream source should be copied or translated where useful, then reshaped into VECTOR's Native C plus Avalonia target, with Postgres and pgvector replacing upstream SQLite-shaped persistence.

## Source Reference

- Local upstream clone: set `OPENAI_CODEX_ROOT`, pass `-OpenAiCodexRoot`, or place an `openai-codex` clone beside the VECTOR repository
- Current inspected upstream commit: `5bbfee69b`
- Upstream root surface: Codex CLI/runtime monorepo
- Desktop entry claim: upstream README routes the desktop experience through `codex app`

## VECTOR Program Model

VECTOR is not one monolith. It is a main desktop program that hosts separable child programs.

### Main Programs

- `vector-main-program`: the governing desktop shell and child-program router.
- `vector-left-menu-program`: the left-side navigation, launcher, workspace selector, thread selector, and operator context switcher.
- `vector-main-interaction-program`: the main conversation, turn, approval, artifact, and tool interaction surface.

### Required Follow-On Child Programs

- `vector-session-runtime-program`: session lifecycle, rollout/thread state, turn reads, and resumable execution.
- `vector-tool-execution-program`: shell commands, patch application, file search, and tool result hydration.
- `vector-approval-program`: approval requests, approval resolution, escalation gates, and operator confirmation.
- `vector-artifact-program`: generated files, patches, diffs, previews, and output attachments.
- `vector-memory-program`: DEVBASE-backed persistent memory, RAG, recursion, retrieval, and active management.
- `vector-sandbox-program`: execution sandbox policy and CODEBASE/FORGE code-development isolation.
- `vector-status-activity-program`: status rail, background activity, progress events, notifications, and diagnostics.

## Upstream Component Mapping

| Upstream component | VECTOR target | Assimilation rule |
| --- | --- | --- |
| `codex-rs/core` | `vector-session-runtime-program` and `vector-main-interaction-program` | Translate behavior and contracts out of Rust into Native C-owned runtime interfaces. |
| `codex-rs/app-server` | `vector-main-program` host boundary | Assimilate app/server protocol behavior, but do not preserve the upstream Rust server as the long-term authority. |
| `codex-rs/app-server-protocol` | Native C ABI plus Avalonia interop contracts | Treat protocol payloads as high-value source contracts for VECTOR child-program boundaries. |
| `codex-rs/app-server-client` | Avalonia host interop and diagnostics client | Translate only the client shapes needed by the shell; keep C# thin. |
| `codex-rs/tui` | `vector-main-interaction-program` | Copy interaction semantics, turn flow, approval patterns, and status behaviors, not the terminal UI implementation. |
| `codex-rs/cli` | VECTOR command launch and developer tooling | Preserve useful command model and launch semantics, then rehost behind VECTOR's desktop entrypoints. |
| `codex-rs/exec` | `vector-tool-execution-program` | Translate non-interactive execution semantics into governed tool execution. |
| `codex-rs/protocol` | Shared VECTOR protocol schema | Assimilate event/message/result shapes as native contracts. |
| `codex-rs/apply-patch` | `vector-artifact-program` and `vector-tool-execution-program` | Reuse patch semantics and result reporting; implement final execution in the native runtime. |
| `codex-rs/git-utils` | FLUXBASE/ANVIL integration | Treat as source behavior only; durable VCS truth belongs in FLUXBASE. |
| `codex-rs/file-search` | `vector-tool-execution-program` and `CODEBASE` | Translate search semantics into native file/codebase operations. |
| `codex-rs/sandboxing`, `codex-rs/windows-sandbox-rs`, `codex-rs/linux-sandbox` | `vector-sandbox-program` | Preserve policy concepts while implementing VECTOR-owned sandbox contracts. |
| `codex-rs/config` | VECTOR settings and profile contract | Translate config concepts into VECTOR settings without inheriting upstream storage defaults blindly. |
| `codex-rs/state` | `vector-session-runtime-program` and `DEVBASE` | Import state semantics; persistent memory and active management move to Postgres/pgvector-backed DEVBASE. |
| `codex-rs/login`, `codex-rs/chatgpt`, `codex-rs/codex-client` | authentication and model-provider boundary | Assimilate provider/session concepts, but isolate credentials and account state behind VECTOR policy. |
| `codex-rs/mcp-server`, `codex-rs/rmcp-client` | connector and tool integration child lanes | Preserve MCP concepts as optional integration programs, not as the main shell architecture. |
| `codex-cli` legacy TypeScript package | reference only | Use for historical CLI behavior and docs; do not build new VECTOR runtime on this legacy lane. |
| `docs` | VECTOR docs and operator behavior references | Copy useful operator wording and flow definitions into VECTOR-owned documentation as needed. |

## Persistence Conversion

VECTOR must not treat upstream SQLite-shaped storage as the final architecture.

- DEVBASE owns VECTOR working memory, persistent memory, RAG, recursion, active management, and shared development/admin state across VECTOR, FORGE, and ANVIL when that state is not VCS/GitHub truth.
- FLUXBASE owns VCS and execution-governance truth for FLUX and ANVIL-related repo, issue, PR, discussion, commit, and project-item records.
- CODEBASE owns code, libraries, and FORGE sandbox/development storage.
- CHATBASE owns human-agent and agent-agent communication records.

## Immediate Build Order

1. Create a repeatable upstream manifest from the local `openai-codex` clone.
2. Use the manifest to define which upstream directories are copied, translated, or reference-only.
3. Build the VECTOR child-program shell around `vector-left-menu-program` and `vector-main-interaction-program`.
4. Assimilate upstream session, turn, tool, approval, and artifact contracts into the main interaction child.
5. Replace upstream persistence paths with DEVBASE contracts before adding live RAG and recursion execution.
6. Add sandbox/tool execution as separate child programs rather than burying them inside the UI host.

## Current Boundary

This map is not a claim that VECTOR is runnable as an upstream Codex clone today. It is the first concrete decomposition required to make that happen without losing the Native C plus Avalonia target or the Postgres/pgvector data-plane direction.
