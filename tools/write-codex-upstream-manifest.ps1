param(
  [string]$OpenAiCodexRoot = "C:\Users\Allan\OneDrive\Documents\New project.references\openai-codex",
  [string]$OutputPath = "docs\openai-codex-upstream-manifest.json"
)

$ErrorActionPreference = "Stop"

function Get-GitValue {
  param(
    [string]$WorkingDirectory,
    [string[]]$Arguments
  )

  $previous = Get-Location
  try {
    Set-Location $WorkingDirectory
    $value = & git @Arguments
    if ($LASTEXITCODE -ne 0) {
      throw "git $($Arguments -join ' ') failed in $WorkingDirectory"
    }
    return ($value | Select-Object -First 1)
  } finally {
    Set-Location $previous
  }
}

if (-not (Test-Path -LiteralPath $OpenAiCodexRoot -PathType Container)) {
  throw "OpenAI Codex root not found: $OpenAiCodexRoot"
}

$root = (Resolve-Path -LiteralPath $OpenAiCodexRoot).Path
$commit = Get-GitValue -WorkingDirectory $root -Arguments @("rev-parse", "HEAD")
$branch = Get-GitValue -WorkingDirectory $root -Arguments @("rev-parse", "--abbrev-ref", "HEAD")

$components = @(
  @{ upstreamPath = "codex-rs/core"; vectorProgram = "vector-session-runtime-program"; assimilation = "translate"; priority = 1 },
  @{ upstreamPath = "codex-rs/app-server"; vectorProgram = "vector-main-program"; assimilation = "translate"; priority = 1 },
  @{ upstreamPath = "codex-rs/app-server-protocol"; vectorProgram = "native-abi-and-avalonia-interop"; assimilation = "copy-contracts"; priority = 1 },
  @{ upstreamPath = "codex-rs/app-server-client"; vectorProgram = "avalonia-host-interop"; assimilation = "translate-thin-client"; priority = 2 },
  @{ upstreamPath = "codex-rs/tui"; vectorProgram = "vector-main-interaction-program"; assimilation = "copy-behavior-not-ui"; priority = 1 },
  @{ upstreamPath = "codex-rs/cli"; vectorProgram = "vector-command-launch"; assimilation = "translate"; priority = 2 },
  @{ upstreamPath = "codex-rs/exec"; vectorProgram = "vector-tool-execution-program"; assimilation = "translate"; priority = 1 },
  @{ upstreamPath = "codex-rs/protocol"; vectorProgram = "vector-shared-protocol"; assimilation = "copy-contracts"; priority = 1 },
  @{ upstreamPath = "codex-rs/apply-patch"; vectorProgram = "vector-artifact-program"; assimilation = "translate"; priority = 1 },
  @{ upstreamPath = "codex-rs/git-utils"; vectorProgram = "fluxbase-anvil-integration"; assimilation = "reference-and-translate"; priority = 2 },
  @{ upstreamPath = "codex-rs/file-search"; vectorProgram = "vector-tool-execution-program"; assimilation = "translate"; priority = 2 },
  @{ upstreamPath = "codex-rs/sandboxing"; vectorProgram = "vector-sandbox-program"; assimilation = "translate-policy"; priority = 2 },
  @{ upstreamPath = "codex-rs/windows-sandbox-rs"; vectorProgram = "vector-sandbox-program"; assimilation = "translate-policy"; priority = 2 },
  @{ upstreamPath = "codex-rs/linux-sandbox"; vectorProgram = "vector-sandbox-program"; assimilation = "translate-policy"; priority = 2 },
  @{ upstreamPath = "codex-rs/config"; vectorProgram = "vector-settings-contract"; assimilation = "translate"; priority = 2 },
  @{ upstreamPath = "codex-rs/state"; vectorProgram = "devbase-backed-vector-state"; assimilation = "replace-persistence"; priority = 1 },
  @{ upstreamPath = "codex-rs/login"; vectorProgram = "vector-auth-boundary"; assimilation = "translate"; priority = 3 },
  @{ upstreamPath = "codex-rs/chatgpt"; vectorProgram = "vector-model-provider-boundary"; assimilation = "translate"; priority = 3 },
  @{ upstreamPath = "codex-rs/codex-client"; vectorProgram = "vector-model-provider-boundary"; assimilation = "translate"; priority = 3 },
  @{ upstreamPath = "codex-rs/mcp-server"; vectorProgram = "vector-connector-program"; assimilation = "optional-integration"; priority = 3 },
  @{ upstreamPath = "codex-rs/rmcp-client"; vectorProgram = "vector-connector-program"; assimilation = "optional-integration"; priority = 3 },
  @{ upstreamPath = "codex-cli"; vectorProgram = "reference-only"; assimilation = "legacy-reference"; priority = 4 },
  @{ upstreamPath = "docs"; vectorProgram = "vector-operator-docs"; assimilation = "copy-useful-docs"; priority = 2 }
)

$componentRows = foreach ($component in $components) {
  $absolutePath = Join-Path $root $component.upstreamPath
  [ordered]@{
    upstreamPath = $component.upstreamPath
    exists = Test-Path -LiteralPath $absolutePath
    vectorProgram = $component.vectorProgram
    assimilation = $component.assimilation
    priority = $component.priority
  }
}

$manifest = [ordered]@{
  generatedAt = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ss.000Z")
  upstream = [ordered]@{
    repository = "https://github.com/openai/codex"
    localPath = $root
    branch = $branch
    commit = $commit
  }
  vector = [ordered]@{
    repository = "https://github.com/JKhyro/VECTOR"
    targetRuntime = "Native C"
    desktopHost = "Avalonia"
    persistence = "Postgres plus pgvector via DEVBASE; upstream SQLite-shaped persistence is import/reference only"
    firstClassPrograms = @(
      "vector-main-program",
      "vector-left-menu-program",
      "vector-main-interaction-program"
    )
  }
  components = $componentRows
}

$output = if ([System.IO.Path]::IsPathRooted($OutputPath)) {
  $OutputPath
} else {
  Join-Path (Get-Location) $OutputPath
}

$outputDirectory = Split-Path -Parent $output
if (-not (Test-Path -LiteralPath $outputDirectory -PathType Container)) {
  New-Item -ItemType Directory -Path $outputDirectory | Out-Null
}

$manifest | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $output -Encoding UTF8
Write-Output $output
