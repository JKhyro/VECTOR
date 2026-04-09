# VECTOR Character Consumer Boundary

## Purpose

VECTOR is the workspace-facing execution surface that consumes CORTEX-managed characters and components.

## What VECTOR owns

- workspace execution surfaces
- task interaction
- file, tool, and operator-user interaction surfaces
- child-program contracts used by the active workspace
- host-side presentation of character-driven work

## What VECTOR does not own

- character construction
- ARA composition rules
- lifecycle authority for the managed character
- PRISM personality governance
- ASCENT progression and achievement authority

## Native ownership boundary

Workspace runtime logic belongs in Native C first, with C++ allowed where it materially helps. Avalonia and C# remain the shell and interop layer only, through an explicit C ABI.

## Inputs

- `CORTEX -> VECTOR`: managed characters and components for execution
- `SYMBIOSIS -> VECTOR`: extracted OpenClaw, Lobster, and Hermes workspace behavior routed to its long-term home
