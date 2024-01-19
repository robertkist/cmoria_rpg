# CMoria RPG
A RPG game with retro graphics for Playdate console, PC, Mac, Linux and retro computers (Amiga, MacOS 7.6 m68k, MS-DOS 386/486 PC).

Our protagonist (yes, it's a cat) demoing the Ultima IV/V style fog-of-war (slowed down to show the fading in and out of the fog):

<img src="https://raw.githubusercontent.com/robertkist/robertkist/main/images/cmoria_01.gif.gif">

The goal is to make a game inspired by early rogue-likes and Richard Garriott's Ultima series (notably Ultima IV, V and a bit of VI). Rather than a straight up clone, this will be an independent game in its own right, drawing inspiration from those titles and adding its own lore, story and looks on top.

## WIP

This game is a work-in-progress. This repo will contain my efforts to port and combine various prototypes from Java and Python to ANSI C, which compiles on the various platforms I'd like to target.

## Tech

- ANSI C (C11) for the game code
- Qt / C++ for the editor code
- Make for building
- Python for the graphics export/conversion toolchain
- Win, Mac, Linux: SDL for graphics
- Playdate: Official Playdate C SDK
- Amiga: Amiga-GCC (cross)compiler, Amiga graphics/Intuition
- MS-DOS: DJGPP (cross)compiler, VESA graphics
- Mac 68k: Retro68 (cross)compiler, Quickdraw graphics
- Photoshop for the graphics authoring

## Why the CMoria Name?

Moria is one of the first rogue-likes with a town level. My aim is to write a game with mechanics similar to Ultima IV/V. Like Moria it has a top-down view, town levels, and I might even add some procedural generation. The C stands for C (the language I'm using) ... or it could stand for cats, which will feature prominently in the story and the lore.

## Where's the source code?

I just started this repo... It will be added soon(tm)

## Planned features

- Ultima IV/V style fog-of-war
- Ultima V style NPC dialogs and vendors
- A battle map
- Inventories, stats and talents
- Ultima VI style dungeons
- All kinds of melee and ranged weapons
- A magic and abilities system

## Building

- on MacOS: you can build for all platforms except Win, Linux
- Win: you can build for Win
- Linux: you can build for Linux

## Why target Retro-Computers?

Because I can :) The game isn't going to be computationally expensive. If it runs on the playdate, it might just as well run on some early 32-bit platforms. Also, being a collector of some vintage PCs, I thought it would be great to see my own game running on the vintage hardware I own.
