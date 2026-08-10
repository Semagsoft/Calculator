param(
    [string]$SourceDir = "build-msi/staging",
    [string]$Version = "1.01.0",
    [string]$Arch = "x64",
    [string]$Output = "package/Calculator-1.01-win64.msi"
)

$ErrorActionPreference = "Stop"

$RepoRoot = Split-Path $PSScriptRoot -Parent
if (-not [System.IO.Path]::IsPathRooted($SourceDir)) {
    $SourceDir = Join-Path $RepoRoot $SourceDir
}
if (-not [System.IO.Path]::IsPathRooted($Output)) {
    $Output = Join-Path $RepoRoot $Output
}

if (-not (Test-Path (Join-Path $SourceDir "Calculator.exe"))) {
    throw "Calculator.exe not found in $SourceDir"
}
if (-not (Get-Command windeployqt -ErrorAction SilentlyContinue)) {
    throw "windeployqt not found on PATH"
}
if (-not (Get-Command wix -ErrorAction SilentlyContinue)) {
    throw "wix not found on PATH (install with: dotnet tool install --global wix)"
}

Write-Host "==> Deploying Qt dependencies..."
& windeployqt --release --no-translations (Join-Path $SourceDir "Calculator.exe")
if ($LASTEXITCODE -ne 0) {
    throw "windeployqt failed with exit code $LASTEXITCODE"
}

Write-Host "==> Building MSI..."
& wix build -arch $Arch -d "Version=$Version" -d "SourceDir=$SourceDir" `
    (Join-Path $PSScriptRoot "calculator.wxs") -o $Output
if ($LASTEXITCODE -ne 0) {
    throw "wix build failed with exit code $LASTEXITCODE"
}

Write-Host "==> Done! MSI created: $Output"
