# VECTOR Main Child Program: First Slice

This document defines the first bounded decomposition of the main VECTOR desktop program.

It is intentionally narrow. It does not try to name every future subsystem. It makes the first two operator-facing child programs explicit so useful upstream Codex behavior can be assimilated without collapsing the surrounding VECTOR shell.

## Main rule

- `vector-main-program` is the governing desktop program.
- Native C defines the child-program contract and region ownership.
- Avalonia hosts the desktop shell and renders the child-program regions returned by the Native C runtime.
- Upstream Codex session-turn behavior should land inside the main interaction child program, not take over the entire VECTOR shell.

## First child programs

### 1. Left-side menu program

- Program id: `vector-left-menu-program`
- Region: left-side menu
- Purpose: navigation, launching, workspace selection, thread selection, and operator context switching
- Ownership rule: this program chooses where the operator is in VECTOR, but it does not own the downstream runtime truth for the active work

### 2. Main interaction program

- Program id: `vector-main-interaction-program`
- Region: main interaction space
- Purpose: sessions, turns, approvals, artifacts, and runtime interaction
- Ownership rule: this is the primary landing zone for assimilated Codex App interaction behavior inside VECTOR

## Integration rule

- The left-side menu and main interaction space are separable child programs, but they remain governed under the main VECTOR program.
- The parent shell should be able to switch, refresh, and compose those child programs without redefining their ownership in managed code.
- The child-program contract should stay queryable from the Native C runtime so the shell and later child programs consume one governing truth.
- The current Avalonia continuity shell should render the left-side menu as the explicit navigation rail and the main interaction space as the explicit payload/status region so the decomposition is visible in the host itself.

## Candidate next child programs

- `vector-context-inspector-program`
- `vector-approval-review-program`
- `vector-artifact-inspector-program`
- `vector-status-activity-rail-program`

These are candidates, not yet first-class runtime programs in the same way as the left-side menu and main interaction space.

## Current implementation implication

- The current usable repo lane is still the Codex Workbench Electron prototype.
- The Native C plus Avalonia lane now owns the `vector_main_child_program_contract` that defines the first child-program boundary for VECTOR.
- The Avalonia host now renders dedicated left-side menu and main interaction program regions from that runtime contract instead of leaving the split implicit in one generic payload pane.
- This slice is a structure and routing step, not a claim that the upstream `openai/codex` product base is already fully merged into VECTOR.
