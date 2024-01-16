import os
from psd_tools import PSDImage
from PIL import Image

FILE_FORMAT = "bmp"

LAYER_TYPES = {
    "wall": {
        "size": (32, 48),
    },
    "shadow": {
        "size": (32, 32),
    },
    "floor": {
        "size": (32, 32),
    },
    "player": {
        "size": (32, 32),
    },
    "editor": {
        "size": (32, 32),
    },
    "object": {
        "size": (32, 32),
    },
}


OVERRIDES = {
    "shadow B": {"offset": (0, 1)},
    "shadow R": {"offset": (1, 0)},
    "player shadow 00": {"offset": (0, 18)},
}

# TODO - REMOVE TRANSLATION!
TRANSLATION = {
    "wall brick 00 T": "wall brick 00 T",
    "wall brick 00 B": "wall brick 00 B",
    "wall brick 00 L": "wall brick 00 L",
    "wall brick 00 R": "wall brick 00 R",
    "wall brick 00 LR": "wall brick 00 LR",
    "wall brick 00 TB": "wall brick 00 TB",
    "wall brick 00 BL": "wall brick 00 BL",
    "wall brick 00 BR": "wall brick 00 BR",
    "wall brick 00 TL": "wall brick 00 TL",
    "wall brick 00 TR": "wall brick 00 TR",
    "wall brick 00 TBL": "wall brick 00 TBL",
    "wall brick 00 TLR": "wall brick 00 TLR",
    "wall brick 00 BLR": "wall brick 00 BLR",
    "wall brick 00 TBR": "wall brick 00 TBR",
    "wall brick 00 none": "wall brick 00 none",

    "floor cobble 00": "floor cobble 00",
    "floor blank 00": "floor blank 00",
    "floor wood_beams 00": "floor wood_beams 00",
    "floor default 00": "floor default 00",

    "shadow R": "shadow R",
    "shadow TL": "shadow TL",
    "shadow TB": "shadow TB",
    "shadow B": "shadow B",
    "shadow LR": "shadow LR",
    "shadow BR": "shadow BR",

    "player shadow 00": "player shadow 00",
    "player 00": "player 00",
    
    "editor player": "player start",

    "object swirl 00": "object swirl 00",
    "object swirl 01": "object swirl 01",
    "object swirl 02": "object swirl 02",
    "object swirl 03": "object swirl 03",
}


class ExportTiles:
    def __init__(self, out_dir, format):
        self.__out_dir = out_dir
        self.format = format

    def export(self, psd_file):
        return self.__export_tileset_layers(psd_file)

    def __export_tileset_layers(self, psd_file=None, psd=None, i=0):
        """
        Recursively exports all tile images from a PSD.
        Layers in the PSD must be named appropriately in order to be exported.
        """
        if psd is None:
            if not os.path.isfile(psd_file):
                print(f"ERROR: file {psd_file} not found")
                return False
            psd = PSDImage.open(psd_file)
        for layer in psd:
            if layer.kind == "pixel":
                ltype = layer.name.split(" ")[0]
                if ltype in LAYER_TYPES.keys():
                    if layer.name in TRANSLATION:

                        dim = (layer.width, layer.height)


                        name = TRANSLATION[layer.name]


                        if dim == LAYER_TYPES[ltype]["size"]:
                            img = layer.topil()
                        else:
                            img_orig = layer.topil()
                            img = Image.new(mode=img_orig.mode, size=LAYER_TYPES[ltype]["size"])
                            if layer.name in OVERRIDES:
                                offset = OVERRIDES[layer.name]["offset"]
                            else:
                                offset = (0, 0)
                            img.paste(img_orig, offset)
                        print("SAVING", layer.name, "/", name)
                        img.save(os.path.join(self.__out_dir, f"{name}.{self.format}"))
            elif layer.kind == "group":
                if i + 1 < 2:
                    # print("".zfill(i*2).replace("0"," "), "GROUP", layer.name, i)
                    self.__export_tileset_layers(psd=layer.descendants(), i=i + 1)
            else:
                raise Exception("unknown layer type found")
        return True
