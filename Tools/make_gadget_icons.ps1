# Génère les icônes des objets utilisables (gadgets) en PNG 256x256, même style que make_icons.ps1.
Add-Type -AssemblyName System.Drawing
$outDir = Join-Path $PSScriptRoot 'Icons'
New-Item -ItemType Directory -Force $outDir | Out-Null

function New-Canvas([System.Drawing.Color]$bg) {
    $bmp = New-Object System.Drawing.Bitmap 256, 256
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = 'AntiAlias'
    $g.Clear([System.Drawing.Color]::Transparent)
    $brush = New-Object System.Drawing.SolidBrush $bg
    $g.FillEllipse($brush, 8, 8, 240, 240)
    return @($bmp, $g)
}
function Save-Icon($bmp, $g, $name) {
    $g.Dispose()
    $bmp.Save((Join-Path $outDir "$name.png"), [System.Drawing.Imaging.ImageFormat]::Png)
    $bmp.Dispose()
}
function P([float[]]$xy) {
    $pts = @()
    for ($i = 0; $i -lt $xy.Length; $i += 2) { $pts += New-Object System.Drawing.PointF $xy[$i], $xy[$i + 1] }
    return ,$pts
}
$white = [System.Drawing.Brushes]::White
$wpen = New-Object System.Drawing.Pen ([System.Drawing.Color]::White), 14
$wpen.StartCap = 'Round'; $wpen.EndCap = 'Round'; $wpen.LineJoin = 'Round'
$thin = New-Object System.Drawing.Pen ([System.Drawing.Color]::White), 8
$thin.StartCap = 'Round'; $thin.EndCap = 'Round'
$gad = [System.Drawing.Color]::FromArgb(255, 20, 130, 140)
$bgPen = New-Object System.Drawing.Pen $gad, 8

# 0 Tromblon : canon évasé tiré vers la droite, flèche de recul vers la gauche
$c = New-Canvas $gad; $g = $c[1]
$g.FillRectangle($white, 70, 105, 90, 26)
$g.FillPolygon($white, (P @(160,105, 200,85, 200,151, 160,131)))
$g.FillPolygon($white, (P @(70,118, 95,131, 85,175, 60,170)))
$g.DrawLine($thin, 212, 100, 232, 88); $g.DrawLine($thin, 214, 118, 238, 118); $g.DrawLine($thin, 212, 136, 232, 148)
$g.DrawLine($wpen, 40, 205, 130, 205)
$g.FillPolygon($white, (P @(22,205, 52,185, 52,225)))
Save-Icon $c[0] $g 'T_Gadget_Blunderbuss'

# 1 Grappin : crochet en haut, corde qui descend
$c = New-Canvas $gad; $g = $c[1]
$g.DrawLine($wpen, 128, 150, 128, 75)
$g.DrawLines($wpen, (P @(128,85, 88,62, 78,95)))
$g.DrawLines($wpen, (P @(128,85, 168,62, 178,95)))
$g.DrawLine($wpen, 128, 75, 128, 40)
$g.DrawEllipse($thin, 116, 148, 24, 24)
$rope = New-Object System.Drawing.Pen ([System.Drawing.Color]::White), 6
$rope.DashStyle = 'Dash'
$g.DrawBezier($rope, 128, 172, 150, 195, 95, 205, 115, 232)
Save-Icon $c[0] $g 'T_Gadget_Grapple'

# 2 Fumigène : grenade + nuages
$c = New-Canvas $gad; $g = $c[1]
$g.FillEllipse($white, 60, 120, 80, 95)
$g.FillRectangle($white, 85, 100, 30, 25)
$g.DrawLine($thin, 115, 105, 140, 92)
foreach ($e in @(@(130,55,60,50), @(165,80,55,45), @(150,40,45,40), @(185,55,40,35))) { $g.FillEllipse($white, $e[0], $e[1], $e[2], $e[3]) }
Save-Icon $c[0] $g 'T_Gadget_Smoke'

# 3 Dynamite : 3 bâtons + mèche + étincelle
$c = New-Canvas $gad; $g = $c[1]
foreach ($x in 78, 113, 148) { $g.FillRectangle($white, $x, 110, 30, 100) }
$g.DrawLine($bgPen, 70, 140, 186, 140); $g.DrawLine($bgPen, 70, 180, 186, 180)
$g.DrawBezier($thin, 128, 110, 128, 80, 160, 90, 170, 60)
$g.FillPolygon($white, (P @(170,35, 178,52, 196,52, 182,63, 188,80, 170,70, 152,80, 158,63, 144,52, 162,52)))
Save-Icon $c[0] $g 'T_Gadget_Dynamite'

# 4 Ressort : zigzag + flèche vers le haut
$c = New-Canvas $gad; $g = $c[1]
$g.DrawLines($wpen, (P @(80,215, 176,195, 80,175, 176,155, 80,135, 176,115)))
$g.FillRectangle($white, 70, 212, 116, 16)
$g.DrawLine($wpen, 128, 95, 128, 55)
$g.FillPolygon($white, (P @(128,28, 158,62, 98,62)))
Save-Icon $c[0] $g 'T_Gadget_Spring'

# 5 Glu : flaque + gouttes
$c = New-Canvas $gad; $g = $c[1]
$g.FillEllipse($white, 45, 165, 166, 50)
$g.FillEllipse($white, 90, 150, 70, 40)
foreach ($d in @(@(95,60), @(150,90), @(120,115))) {
    $x = $d[0]; $y = $d[1]
    $g.FillPolygon($white, (P @($x,($y - 30), ($x + 14),($y - 4), ($x - 14),($y - 4))))
    $g.FillEllipse($white, ($x - 15), ($y - 16), 30, 30)
}
Save-Icon $c[0] $g 'T_Gadget_Glue'

Get-ChildItem $outDir -Filter 'T_Gadget_*' | Select-Object -ExpandProperty FullName
