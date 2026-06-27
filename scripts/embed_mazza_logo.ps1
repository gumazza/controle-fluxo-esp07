param(
    [string]$Source = (Join-Path $PSScriptRoot "..\include\logo_mazza_src.png"),
    [string]$Output = (Join-Path $PSScriptRoot "..\include\web_logo.h"),
    [int]$Width = 140,
    [switch]$Raw
)

if (-not (Test-Path -LiteralPath $Source)) {
    Write-Error "Arquivo nao encontrado: $Source"
    exit 1
}

if ($Raw) {
    $bytes = [System.IO.File]::ReadAllBytes($Source)
}
else {
    Add-Type -AssemblyName System.Drawing

    $img = [System.Drawing.Image]::FromFile($Source)
    $scale = $Width / $img.Width
    $height = [int]($img.Height * $scale)
    $bmp = New-Object System.Drawing.Bitmap $Width, $height, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $g.CompositingQuality = [System.Drawing.Drawing2D.CompositingQuality]::HighQuality
    $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::HighQuality
    $g.Clear([System.Drawing.Color]::Transparent)
    $g.DrawImage($img, 0, 0, $Width, $height)
    $g.Dispose()
    $img.Dispose()

    $processed = Join-Path (Split-Path $Output) "logo_mazza.png"
    $bmp.Save($processed, [System.Drawing.Imaging.ImageFormat]::Png)
    $bmp.Dispose()

    $bytes = [System.IO.File]::ReadAllBytes($processed)
}

$sb = New-Object System.Text.StringBuilder

[void]$sb.AppendLine("#pragma once")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("#include <Arduino.h>")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("static const uint8_t LOGO_MAZZA_PNG[] PROGMEM = {")

$line = "    "

for ($i = 0; $i -lt $bytes.Length; $i++) {
    $line += ("0x{0:X2}, " -f $bytes[$i])

    if (($i + 1) % 16 -eq 0) {
        [void]$sb.AppendLine($line.TrimEnd())
        $line = "    "
    }
}

if ($line.Trim().Length -gt 0) {
    [void]$sb.AppendLine($line.TrimEnd().TrimEnd(','))
}

[void]$sb.AppendLine("};")
[void]$sb.AppendLine("")
[void]$sb.AppendLine(("static const size_t LOGO_MAZZA_PNG_LEN = {0};" -f $bytes.Length))

[System.IO.File]::WriteAllText($Output, $sb.ToString())

Write-Output "Logo embutida: $Output ($($bytes.Length) bytes)"
