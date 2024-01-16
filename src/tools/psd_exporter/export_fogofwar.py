import os
from psd_tools import PSDImage
from PIL import Image


class ExportFog:
    LAYERS = ['C1', 'C2', 'C3', 'C4', 'C5', 'L1', 'L2', 'L3', 'L4', 'L5']
    LAYER_COUNT = 5
    WIDTH = 32
    HEIGHT = 16

    def __init__(self, out_dir, format):
        self.c_layers = []
        self.l_layers = []
        self.size = (0, 0)
        self.__out_dir = out_dir
        self.format = format

    def __save_tile(self, input_img, x, y, name):
        img = Image.new(mode="RGBA", size=self.size)
        img.paste(input_img, (0, 0))
        img = img.crop((x, y, x + self.WIDTH, y + self.HEIGHT))
        img.save(os.path.join(self.__out_dir, name))
        print("SAVING", name)

    def export(self, psd_file):
        if not os.path.isfile(psd_file):
            return False
        psd = PSDImage.open(psd_file)
        for i in range(0, self.LAYER_COUNT):
            self.c_layers.append(Image.new(mode="RGBA", size=psd.size))
            self.l_layers.append(Image.new(mode="RGBA", size=psd.size))
        self.size = psd.size
        self.__export_fog_of_war(psd)
        # combine final layers and save tiles out
        for i in range(0, self.LAYER_COUNT):
            if i > 0:
                self.c_layers[i].paste(self.c_layers[i - 1], (0, 0), self.c_layers[i - 1])
            layer = self.c_layers[i]
            n = self.LAYER_COUNT - i
            self.__save_tile(layer, 1 * 32, 2 * 16, f"b corner topleft {n}.{self.format}")      ## fog_TL.png
            self.__save_tile(layer, 3 * 32, 2 * 16, f"b corner topright {n}.{self.format}")     ## fog_TR.png
            self.__save_tile(layer, 1 * 32, 5 * 16, f"b corner bottomleft {n}.{self.format}")   ## fog_BL.png
            self.__save_tile(layer, 3 * 32, 5 * 16, f"b corner bottomright {n}.{self.format}")  ## fog_BR.png
            self.__save_tile(layer, 2 * 32, 2 * 16, f"b top {n}.{self.format}")    ## fog_T.png
            self.__save_tile(layer, 2 * 32, 5 * 16, f"b bottom {n}.{self.format}") ## fog_B.png
            self.__save_tile(layer, 1 * 32, 3 * 16, f"b left {n}.{self.format}")   ## fog_L.png
            self.__save_tile(layer, 3 * 32, 3 * 16, f"b right {n}.{self.format}")  ## fog_R.png
            self.__save_tile(layer, 2 * 32, 3 * 16, f"b center {n}.{self.format}") ## fog_center.png
        for i in range(0, self.LAYER_COUNT):
            if i > 0:
                self.l_layers[i].paste(self.l_layers[i - 1], (0, 0), self.l_layers[i - 1])
            layer = self.l_layers[i]
            n = self.LAYER_COUNT - i
            self.__save_tile(layer, 1 * 32, 2 * 16, f"b L topleft {n}.{self.format}")     ## fog_L_TL.png
            self.__save_tile(layer, 1 * 32, 5 * 16, f"b L bottomleft {n}.{self.format}")  ## fog_L_BL.png
            self.__save_tile(layer, 3 * 32, 2 * 16, f"b L topright {n}.{self.format}")    ## fog_L_TR.png
            self.__save_tile(layer, 3 * 32, 5 * 16, f"b L bottomright {n}.{self.format}") ## fog_L_BR.png
            self.__save_tile(layer, 2 * 32, 4 * 16, f"b U {n}.{self.format}")  ## fog_U1.png
            self.__save_tile(layer, 2 * 32, 5 * 16, f"b U2 {n}.{self.format}") ## fog_U2.png
            self.__save_tile(layer, 2 * 32, 3 * 16, f"b U3 {n}.{self.format}") ## fog_U3.png
            self.__save_tile(layer, 2 * 32, 1 * 16, f"b U4 {n}.{self.format}") ## fog_U4.png
            self.__save_tile(layer, 2 * 32, 2 * 16, f"b U5 {n}.{self.format}") ## fog_U5.png
        # save transparent tile
        img = Image.new(mode="RGBA", size=(self.WIDTH, self.HEIGHT))
        name = f"fog_16.{self.format}"  ## fog_empty.png
        img.save(os.path.join(self.__out_dir, name))
        print("SAVING", name)

    def __export_fog_of_war(self, psd=None, i=0):
        """Exports the fog of war tiles"""
        for layer in psd:
            if layer.kind == "pixel":
                if layer.name in self.LAYERS:
                    layer_number = int(layer.name[1]) - 1
                    if layer.name[0] == "L":
                        self.l_layers[(self.LAYER_COUNT - 1) - layer_number].paste(layer.topil(), layer.offset)
                    elif layer.name[0] == "C":
                        self.c_layers[(self.LAYER_COUNT - 1) - layer_number].paste(layer.topil(), layer.offset)
            elif layer.kind == "group":
                if i + 1 < 2:
                    self.__export_fog_of_war(psd=layer.descendants(), i=i + 1)