# Génère les versions grises des icônes d'objets utilisables (T_Gadget_*_Used.png),
# affichées dans le panneau OBJETS du HUD quand un objet a déjà servi pendant la manche.
# Nécessite Python 3 et Pillow (pip install pillow). À lancer après make_gadget_icons.ps1.
from pathlib import Path

from PIL import Image, ImageEnhance

ICONS_DIR = Path(__file__).parent / "Icons"
NAMES = ["Blunderbuss", "Grapple", "Smoke", "Dynamite", "Spring", "Glue"]

for name in NAMES:
    icon = Image.open(ICONS_DIR / f"T_Gadget_{name}.png").convert("RGBA")
    r, g, b, alpha = icon.split()
    gray = Image.merge("RGB", (r, g, b)).convert("L")
    gray = ImageEnhance.Brightness(gray).enhance(0.75)
    Image.merge("RGBA", (gray, gray, gray, alpha)).save(ICONS_DIR / f"T_Gadget_{name}_Used.png")
    print(f"T_Gadget_{name}_Used.png")
