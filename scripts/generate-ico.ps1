param(
    [Parameter(Mandatory = $true)][string]$Svg,
    [Parameter(Mandatory = $true)][string]$Out,
    [string]$Rsvg = ""
)

$ErrorActionPreference = "Stop"

function Find-Rsvg {
    if ($Rsvg -and (Test-Path $Rsvg)) { return $Rsvg }
    foreach ($candidate in @(
        "C:\msys64\ucrt64\bin\rsvg-convert.exe",
        "C:\msys64\mingw64\bin\rsvg-convert.exe",
        "C:\msys64\mingw32\bin\rsvg-convert.exe"
    )) {
        if (Test-Path $candidate) { return $candidate }
    }
    $cmd = Get-Command rsvg-convert -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    throw "rsvg-convert not found. Install librsvg (e.g. mingw-w64-ucrt-x86_64-librsvg) or pass -Rsvg <path>."
}

function Write-Ico([System.Collections.ArrayList]$entries, [string]$outPath) {
    $ms = New-Object System.IO.MemoryStream
    $bw = New-Object System.IO.BinaryWriter($ms)

    $bw.Write([uint16]0)
    $bw.Write([uint16]1)
    $bw.Write([uint16]$entries.Count)

    $offset = 6 + (16 * $entries.Count)
    foreach ($e in $entries) {
        $bytes = [System.IO.File]::ReadAllBytes($e.path)
        $dim = if ($e.size -eq 256) { 0 } else { $e.size }
        $bw.Write([byte]$dim)
        $bw.Write([byte]$dim)
        $bw.Write([byte]0)
        $bw.Write([byte]0)
        $bw.Write([uint16]1)
        $bw.Write([uint16]32)
        $bw.Write([uint32]$bytes.Length)
        $bw.Write([uint32]$offset)
        $offset += $bytes.Length
    }
    foreach ($e in $entries) {
        $bw.Write([System.IO.File]::ReadAllBytes($e.path))
    }
    $bw.Flush()
    [System.IO.File]::WriteAllBytes($outPath, $ms.ToArray())
    $bw.Dispose()
    $ms.Dispose()
}

$rsvg = Find-Rsvg

$sizes = @(16, 24, 32, 48, 64, 128, 256)
$tmp = Join-Path $env:TEMP ("calculator-ico-" + [guid]::NewGuid().ToString("N"))
New-Item -ItemType Directory -Force -Path $tmp | Out-Null

try {
    $entries = New-Object System.Collections.ArrayList
    foreach ($size in $sizes) {
        $png = Join-Path $tmp "icon_$size.png"
        & $rsvg -w $size -h $size -o $png $Svg
        if ($LASTEXITCODE -ne 0) { throw "rsvg-convert failed rendering ${size}px" }
        $null = $entries.Add(@{ size = $size; path = $png })
    }
    Write-Ico $entries $Out
    Write-Host "Generated $Out ($($entries.Count) sizes)"
} finally {
    Remove-Item -Recurse -Force $tmp -ErrorAction SilentlyContinue
}
