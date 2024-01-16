import os
import sys
from export_tileset import ExportTiles
from export_fogofwar import ExportFog

"""
Exports tiles from the given PSD files/
"""

ROOT = os.path.join("content", "tiles")
# OUT_DIR = os.path.join("dist", "tiles")
OUT_DIR = sys.argv[1]

if __name__ == "__main__":
    format = sys.argv[2]
    os.makedirs(OUT_DIR, exist_ok=True)
    # Landscape and environment tiles:
    ExportTiles(OUT_DIR, format).export(os.path.join(ROOT, "WORK environment tiles.psd"))
    # NPC and player character tiles:
    ExportTiles(OUT_DIR, format).export(os.path.join(ROOT, "WORK character tiles.psd"))
    # tiles used by the game's editor:
    ExportTiles(OUT_DIR, format).export(os.path.join(ROOT, "WORK editor.psd"))
    # fog-of-war tiles:
    ExportFog(OUT_DIR, format).export(os.path.join(ROOT, "WORK fog of war.psd"))
