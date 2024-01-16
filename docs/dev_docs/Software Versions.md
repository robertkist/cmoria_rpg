# Software Versions

## Introduction

This page lists versions of compilers, libraries, tools, etc. used to build the project.

## Limitations

This project is being developed primarily on a Mac. 

Here is a list for which target-platforms you can build the game and the editor on different build-platforms:
- Mac: Mac, Playdate, Amiga, MS-DOS, Mac68k
- Windows: Windows
- Linux: Linux

The code does not support building on Amiga OS, Classic Mac OS or MS-DOS.

It might be possible to adapt the build scripts fairly easily from MacOS to Linux, though.

All the platforms and softwares below have been tested to work. It is likely that everything works with newer
and even slightly older versions, or on adjacent systems (e.g. Windows 11, Arch Linux, etc.).

## Software List

MacOS
- Build-OS: Ventura 13.6 or newer
- Target-OS: Ventura 13.6 or newer
- Compiler: via [Homebrew](https://brew.sh), Homebrew GCC 11.4.0
- SDL2: via Homebrew, 2.28.5
- Qt: 6.6.1 [Open Source](https://www.qt.io/download-open-source)
- Python: via PyEnv (via Homebrew), Python 3.11.6
- make: GNU make 3.81

Playdate
- [Playdate SDK](https://play.date/dev/) 2.20
- Python: via PyEnv, Python 3.11.6
- make: GNU make 3.81

Amiga
- Target-OS: Amiga OS 3.1
- Compiler: [Amiga-GCC](https://github.com/bebbo/amiga-gcc), 6.5.0b
- Python: via PyEnv, Python 3.11.6
- Emulator for testing: [FS-UAE](https://fs-uae.net/) for MacOS, 3.1.66
- make: GNU make 3.81

Mac 68k
- Target-OS: Classic Mac OS 7.6.1
- Python: via PyEnv, Python 3.11.6
- Compiler: [Retro68](https://github.com/autc04/Retro68) for m68k, GCC 9.1.0
- Emulator for testing: [Basilisk II](https://basilisk.cebix.net/) on MacOS, version: Universal 20231229 ([download](https://www.emaculation.com/forum/viewtopic.php?f=6&t=7361))
- make: GNU make 3.81

MS-DOS
- Python: via PyEnv, Python 3.11.6
- Compiler: [DJGPP](https://www.delorie.com/djgpp/), GCC 12.2.0 ([download](https://github.com/andrewwutw/build-djgpp/releases))
- Emulator: [DOSBox Staging](https://dosbox-staging.github.io/) 0.80.1
- make: GNU make 3.81

Windows
- Target-OS: Windows 10, 11

Linux
- Target-OS: Ubuntu 22.04

Content Creation
- Adobe Photoshop CS5 or newer

Documentation
- yEd Graph Editor for .graphml files

For minimum versions of 3rd party modules used by Python, see `requirements.txt`.

## Amiga FS-UAE Settings (FS-UAE Launcher)

Tab 1: Main Configuration Options
- Amiga Model: A3000
- Accuracy: High

Tab 4: Hard Drives
- pick a folder for the first HDD and install Amiga OS 3.1

Tab 5: ROM and RAM
- Kickstart ROM: `Kickstart 3.1 (40.055) (A3000) (Commodore) (1993).rom`
- Chip RAM: 2 MB
- Motherboard RAM: 16 MB

Tab 8: Additional Configuration
- CPU: 68040

