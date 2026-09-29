# Génère les icônes des cartes (évolutions et objets posables) en PNG 256x256.
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
$evo = [System.Drawing.Color]::FromArgb(255, 70, 60, 170)
$item = [System.Drawing.Color]::FromArgb(255, 200, 100, 30)

# --- Évolutions ---
# 0 Double saut : deux chevrons vers le haut
$c = New-Canvas $evo; $g = $c[1]
$g.DrawLines($wpen, (P @(70,150, 128,95, 186,150)))
$g.DrawLines($wpen, (P @(70,200, 128,145, 186,200)))
$g.FillEllipse($white, 108, 40, 40, 40)
Save-Icon $c[0] $g 'T_Evo_DoubleJump'

# 1 Dash : flèche vers la droite + traits de vitesse
$c = New-Canvas $evo; $g = $c[1]
$g.FillPolygon($white, (P @(120,80, 205,128, 120,176, 120,148, 95,148, 95,108, 120,108)))
$g.DrawLine($thin, 40, 100, 80, 100); $g.DrawLine($thin, 30, 128, 80, 128); $g.DrawLine($thin, 40, 156, 80, 156)
Save-Icon $c[0] $g 'T_Evo_Dash'

# 2 Planer : parachute
$c = New-Canvas $evo; $g = $c[1]
$g.FillPie($white, 48, 50, 160, 120, 180, 180)
$g.DrawLine($thin, 56, 112, 128, 190); $g.DrawLine($thin, 200, 112, 128, 190); $g.DrawLine($thin, 128, 112, 128, 190)
$g.FillEllipse($white, 112, 180, 32, 32)
Save-Icon $c[0] $g 'T_Evo_Glide'

# 3 Vitesse : éclair
$c = New-Canvas $evo; $g = $c[1]
$g.FillPolygon($white, (P @(145,35, 70,140, 120,140, 100,220, 185,105, 135,105, 160,35)))
Save-Icon $c[0] $g 'T_Evo_Speed'

# 4 Course murale : mur à gauche + flèche qui longe le mur
$c = New-Canvas $evo; $g = $c[1]
$g.FillRectangle($white, 60, 50, 30, 160)
$g.DrawLine($wpen, 115, 190, 175, 80)
$g.FillPolygon($white, (P @(190,50, 195,110, 150,85)))
Save-Icon $c[0] $g 'T_Evo_WallRun'

# --- Objets posables ---
# 0 Plateforme
$c = New-Canvas $item; $g = $c[1]
$g.FillRectangle($white, 45, 115, 166, 34)
$g.DrawLine($thin, 70, 170, 70, 200); $g.DrawLine($thin, 186, 170, 186, 200)
Save-Icon $c[0] $g 'T_Item_Platform'

# 1 Mur (briques)
$c = New-Canvas $item; $g = $c[1]
$g.FillRectangle($white, 78, 45, 100, 166)
$bp = New-Object System.Drawing.Pen $item, 6
foreach ($y in 85, 125, 165) { $g.DrawLine($bp, 78, $y, 178, $y) }
$g.DrawLine($bp, 128, 45, 128, 85); $g.DrawLine($bp, 103, 85, 103, 125); $g.DrawLine($bp, 153, 85, 153, 125); $g.DrawLine($bp, 128, 125, 128, 165); $g.DrawLine($bp, 103, 165, 103, 211); $g.DrawLine($bp, 153, 165, 153, 211)
Save-Icon $c[0] $g 'T_Item_Wall'

# 2 Pics
$c = New-Canvas $item; $g = $c[1]
$g.FillRectangle($white, 45, 180, 166, 22)
foreach ($x in 45, 100, 155) { $g.FillPolygon($white, (P @($x,180, ($x + 28),70, ($x + 56),180))) }
Save-Icon $c[0] $g 'T_Item_Spikes'

# 3 Piston : bloc + tige + plaque + flèche
$c = New-Canvas $item; $g = $c[1]
$g.FillRectangle($white, 40, 95, 50, 66)
$g.FillRectangle($white, 90, 118, 60, 20)
$g.FillRectangle($white, 150, 75, 22, 106)
$g.FillPolygon($white, (P @(185,108, 220,128, 185,148)))
Save-Icon $c[0] $g 'T_Item_Piston'

# 4 Plateforme mobile : plateforme + flèches gauche/droite
$c = New-Canvas $item; $g = $c[1]
$g.FillRectangle($white, 75, 140, 106, 30)
$g.DrawLine($wpen, 60, 100, 196, 100)
$g.FillPolygon($white, (P @(40,100, 75,75, 75,125)))
$g.FillPolygon($white, (P @(216,100, 181,75, 181,125)))
Save-Icon $c[0] $g 'T_Item_MovingPlatform'

Get-ChildItem $outDir | Select-Object -ExpandProperty FullName
