import os.path
import struct
import sys
from typing import Tuple, Optional
from PIL import Image


"""
Converts images for DOS, Amiga and Mac68k platforms.

IMPORTANT: This tool is made specifically for converting images with a 4 color palette!
           It will not convert images with more colors!
"""

SUPPORTED_MODES = ["256", "1bw", "2bpl"]
CONVERT_8_TO_6_BIT = False  # interpolate RGB palette values

def hx(v):
    return hex(v)[2:].zfill(2)

def bn(v):
    return bin(v)[2:].zfill(8)


def interpolate(x1: float, x2: float, y1: float, y2: float, x: float) -> int:
    """Perform linear interpolation for x between (x1,y1) and (x2,y2) """
    return round(((y2 - y1) * x + x2 * y1 - x1 * y2) / (x2 - x1))


class Converter:

    def __create_palette(self, palette: bytes) -> bytes:
        length = int(len(palette) / 4)
        out = int.to_bytes(length, length=1, byteorder='little')  # palette length / number of color entries
        out += palette
        return out

    def __write_bmp(self, fname: str, buffer: bytes, width: int, height: int, big_endian: bool, transparency: bool) -> None:
        """
        Writes 16bit width, 16-bit height, followed by the bitmap data.
        https://docs.python.org/3/library/struct.html
        """
        pack_int = '>H' if big_endian else 'H'
        out = b''
        out += struct.pack(pack_int, width)
        out += struct.pack(pack_int, height)
        out += struct.pack('b', 1 if transparency else 0)
        out += buffer
        with open(fname, "wb") as fp:
            fp.write(out)

    def __write_amiga_bmp(self, fname: str, buffer: bytes, width: int, height: int, big_endian: bool, planes: int, transparency: bool) -> None:
        """
        Writes 16bit width, 16-bit height, followed by the bitmap data.
        https://docs.python.org/3/library/struct.html
        """
        pack_int = '>H' if big_endian else 'H'
        out = b''
        out += struct.pack(pack_int, width)
        out += struct.pack(pack_int, height)
        out += struct.pack('b', planes)  # write number of planes / images
        out += struct.pack('b', 1 if transparency else 0)
        out += buffer
        with open(fname, "wb") as fp:
            fp.write(out)

    def __write_mac_bmp(self, fname: str, buffer: bytes, width: int, height: int, big_endian: bool, transparency: bool) -> None:
        """
        Writes 16bit width, 16-bit height, followed by the bitmap data.
        https://docs.python.org/3/library/struct.html
        """
        pack_int = '>H' if big_endian else 'H'
        out = b''
        out += struct.pack(pack_int, width)
        out += struct.pack(pack_int, height)
        out += struct.pack('b', 1 if transparency else 0)
        out += buffer
        with open(fname, "wb") as fp:
            fp.write(out)

    def __write_palette(self, fname: str, buffer: bytes) -> None:
        """
        Writes the palette for MS-DOS 256 color images.
        """
        fname, _ = os.path.splitext(fname)
        fname += ".pal"
        with open(fname, "wb") as fp:
            fp.write(self.__create_palette(buffer))

    def __create_byte_data_indexed(self, fout: str, im: Image) -> Tuple[bytes, bool]:
        out = b''
        transparency = False
        for y in range(im.height):
            for x in range(im.width):
                r, g, b, a = im.getpixel((x, y))
                c = (r + g + b) / 3
                if a < 255:
                    index = 0
                    transparency = True
                elif c <= 76:
                    index = 1
                elif c <= 113:
                    index = 2
                elif c <= 153:
                    index = 3
                else:
                    index = 4
                out += int.to_bytes(index, length=1, byteorder='little')
        return out, transparency

    def __create_byte_data_1bw(self, fout: str, im: Image) -> Tuple[bytes, bool]:
        """
        MAC: Converts the image to 1 bit per pixel with width-padding.
        Input pixels brigher than >= 128 will be white (bit set to 1), other pixels will be back (bit set to 0).
        Width must be divisible by 16.
        Used for creating Mac68k black and white images.
        """
        transparency = False
        pos = 0

        col_byte = 0
        col_out = b''
        transp_out = b''
        transp_byte = 0
        for y in range(im.height):
            for x in range(im.width):
                r, g, b, a = im.getpixel((x, y))
                if a < 255:
                    transparency = True
                else:
                    transp_byte |= (1 << (7 - pos))
                avg = (r + g + b) / 3.0
                if avg < 128:
                    col_byte |= (1 << (7 - pos))
                pos += 1
                if pos == 8:  # byte is full
                    col_out += int.to_bytes(col_byte, length=1, byteorder='little')
                    col_byte = 0
                    transp_out += int.to_bytes(transp_byte, length=1, byteorder='little')
                    transp_byte = 0
                    pos = 0
            if pos != 0:
                col_out += int.to_bytes(col_byte, length=1, byteorder='little')
                col_byte = 0
                transp_out += int.to_bytes(transp_byte, length=1, byteorder='little')
                transp_byte = 0
                pos = 0
        print("TRANSPARENT", transparency)
        if transparency:
            col_out += transp_out
        return col_out, transparency

    def __create_byte_data_amiga(self, fout: str, im: Image) -> Tuple[bytes, bool]:
        """
        AMIGA
        """
        transparency = False
        planes = 2
        pos = 0
        col_byte = [0, 0, 0, 0]
        col_out = [b'', b'', b'', b'']
        transp_out = b''
        transp_byte = 0
        for y in range(im.height):
            for x in range(im.width):
                r, g, b, a = im.getpixel((x, y))
                c = (r + g + b) / 3.0
                if a < 255:
                    transparency = True
                else:
                    transp_byte |= (1 << (7 - pos))
                    if c <= 76:
                        col_byte[0] |= (1 << (7 - pos))
                    elif c <= 113:
                        col_byte[1] |= (1 << (7 - pos))
                    elif c <= 153:
                        pass
                    else:
                        col_byte[0] |= (1 << (7 - pos))  # 11 / palette index 3
                        col_byte[1] |= (1 << (7 - pos))
                pos += 1
                if pos == 8:  # byte is full
                    for i in range(0, planes):
                        col_out[i] += int.to_bytes(col_byte[i], length=1, byteorder='little')
                        col_byte[i] = 0
                    transp_out += int.to_bytes(transp_byte, length=1, byteorder='little')
                    transp_byte = 0
                    pos = 0
            if pos != 0:
                for i in range(0, planes):
                    col_out[i] += int.to_bytes(col_byte[i], length=1, byteorder='little')
                    col_byte[i] = 0
                transp_out += int.to_bytes(transp_byte, length=1, byteorder='little')
                transp_byte = 0
                pos = 0
        out = b''
        for i in range(0, planes):
            out += col_out[i]
        print("TRANSPARENT", transparency)
        if transparency:
            out += transp_out
        return out, transparency

    def convert_bmp(self, mode: str, fin: str, fout: str) -> None:
        """
        Main entrypoint
        """
        if mode not in SUPPORTED_MODES:
            print("ERROR: unsupported mode")
            sys.exit(-1)
        fin = os.path.normpath(os.path.abspath(fin))
        fout = os.path.normpath(os.path.abspath(fout))
        # gather image files from directory
        if os.path.isdir(fin):
            os.makedirs(fout, exist_ok=True)
            in_images = []
            out_images = []
            files = os.listdir(fin)
            for hfile in files:
                file = os.path.join(fin, hfile)
                if os.path.isfile(file):
                    basename, ext = os.path.splitext(hfile)
                    if ext == ".png":
                        in_images.append(file)
                        out_images.append(os.path.join(fout, basename + ".bmp"))
        else:
            in_images = [fin]
            out_images = [fout]
        # process all files in list
        for i in range(0, len(in_images)):
            fin = in_images[i]
            fout = out_images[i]
            print("in ", fin)
            print("out", fout)
            im = Image.open(fin)
            rgb_im = im.convert('RGBA')  # convert image to RGBA mode
            if mode == "256":  # MSDOS 4 color indexed
                out, transparency = self.__create_byte_data_indexed(fout, rgb_im)  # convert image to 8bit for DOS
                self.__write_bmp(fname=fout,
                                 buffer=out,
                                 width=im.width,
                                 height=im.height,
                                 big_endian=False,
                                 transparency=transparency)
            if mode == "1bw":  # MAC 1 bit with optional transparency mask
                out, transparency = self.__create_byte_data_1bw(fout, rgb_im)  # convert image to 1bit black-white for Mac
                self.__write_mac_bmp(fname=fout,
                                     buffer=out,
                                     width=im.width,
                                     height=im.height,
                                     big_endian=True,
                                     transparency=transparency)
            if mode == "2bpl":  # AMIGA
                out, transparency = self.__create_byte_data_amiga(fout, rgb_im)  # convert image for Amiga
                self.__write_amiga_bmp(fname=fout,
                                       buffer=out,
                                       width=im.width,
                                       height=im.height,
                                       big_endian=True,
                                       planes=2,
                                       transparency=transparency)
            im.close()


def get_args(required):
    """helper function for parsing command line arguments"""
    out = {}
    for a in sys.argv:
        if a.startswith("-") and "=" in a:
            key, value = a[1:].split("=")
            if key in required:
                out[key] = value
    if len(out) != len(required):
        return None
    return out


if __name__ == "__main__":
    args = get_args(["mode", "in", "out"])
    if args is None:
        print("ERROR: not enough arguments\n"
              "Usage:\n"
              "   python bm_converter.py -mode=<mode> -in=<input.png> -out=<output.bmp>\n\n"
              "Modes:\n"
              "   256   - DOS: 256 colors, 1 byte per pixel. Also creates a .pal file for the palette\n"
              "   1bw   - MacOS: black and white, 1 bit per pixel. 1 = black, 0 = white.\n"
              "   2bpl  - Amiga: 2 pit planes (4 colors)\n")
        # TODO 4 - Amiga 4 color palette in 2 1-bit planes
        sys.exit(-1)
    Converter().convert_bmp(mode=args["mode"], fin=args["in"], fout=args["out"])




