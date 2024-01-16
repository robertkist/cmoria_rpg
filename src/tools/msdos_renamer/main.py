import sys
import os

"""
Garbles long file names to shorter MS-DOS compatible file names using a simple hash function.
TODO: there's a low chance of hash collisions, in which case files may overwrite each other.
      this hasn't happened so far, but if it does, the hash algorithm needs to be changed,
      or another solution to generate MS-DOS file names must be found.

Example call:
> python3 main.py -dir=dist/msdos/tiles -prefix=tiles/ -ext=bmp
"""

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


def createHash(str):
    v = 0
    i = 1
    for c in str:
        v += ord(c) * ord(c) * i
        i += 1
    v += len(str)
    return v

def main():
    args = get_args(["dir", "prefix", "ext"])
    if args is None:
        print("wrong parameters")
        return -1
    path = args["dir"]
    filterExt = "." + args["ext"]
    prefix = args["prefix"]

    renameDict = {}
    files = os.listdir(path)
    for hfile in files:
        basename, ext = os.path.splitext(hfile)
        if ext == filterExt:
            hash = createHash(prefix + basename)
            # print(hash, basename)
            if hash in renameDict:
                print("COLLISION")
                return -1
            renameDict[hash] = basename
    for key in renameDict:
        src = os.path.join(path, renameDict[key] + filterExt)
        dst = os.path.join(path, str(key) + filterExt)
        os.rename(src, dst)
    return 0


if __name__ == "__main__":
    sys.exit(main())
