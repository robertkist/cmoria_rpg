## Mac68k 1-bit image format (1bw)
- iamge consists of 1 file
- image format:
    - int_16 width
    - int_16 height
  
## MS-DOS 256 color image format (256)
- images consist of 2 files: a palette (.pal) and the image data (.bmp)
- 18bit RGB color; 6 bit per color, 0 - 63
- BMP image format:
  - int_16 width
  - int_16 height
  - uint_8 [width * height] color data
- PAL image format:    

## MS-DOS black and white format (256bw)
- image format:
    - int_16 width
    - int_16 height
    - uint_8 [width * height] image data. 0xff = white, 0x00 = black

## Amiga image format (2bpl)
- bit planes are saves sequentially after each other
- image format:
    - int_16 width
    - int_16 height
    - int_8 number of bit-planes
    - uint_8 [width / 8 * height * number of bit-planes] bitplane data