param(
    [Parameter(Mandatory = $true)]
    [string]$CortexHostPath,

    [Parameter(Mandatory = $true)]
    [string]$VectorHostPath,

    [string]$WorkDir = (Join-Path $env:TEMP "symbiosis-cortex-vector-installed-smoke"),

    [string]$StateFile
)

if (-not (Test-Path -LiteralPath $CortexHostPath)) {
    Write-Error "cortex_host not found: $CortexHostPath"
    exit 1
}

if (-not (Test-Path -LiteralPath $VectorHostPath)) {
    Write-Error "vector_host not found: $VectorHostPath"
    exit 1
}

New-Item -ItemType Directory -Path $WorkDir -Force | Out-Null

if ([string]::IsNullOrWhiteSpace($StateFile)) {
    $StateFile = Join-Path $WorkDir "cortex_host_demo.state"
}

& $CortexHostPath demo $StateFile
if ($LASTEXITCODE -ne 0) {
    Write-Error "cortex_host demo failed for state file: $StateFile"
    exit $LASTEXITCODE
}

if (-not (Test-Path -LiteralPath $StateFile)) {
    Write-Error "Expected state file was not created: $StateFile"
    exit 1
}

$assignOutput = & $VectorHostPath assign-state $StateFile tool-execution apply-patch 2>&1 | Out-String
$exitCode = $LASTEXITCODE

if ($exitCode -ne 0) {
    Write-Error "vector_host assign-state failed with exit code: $exitCode"
    Write-Error $assignOutput
    exit $exitCode
}

$required = @(
    "status=ok",
    "program=vector-tool-execution-program",
    "helper=host-demo-helper",
    "character=character-alpha",
    "component=component-active",
    "subagent=subagent-host-demo"
)

foreach ($token in $required) {
    if ($assignOutput -notlike "*$token*") {
        Write-Error "Expected token missing: $token"
        Write-Error $assignOutput
        exit 2
    }
}

Write-Output $assignOutput
Write-Host "Installed CORTEX/VECTOR smoke passed."
exit 0
