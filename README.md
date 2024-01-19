# CMoria RPG
A RPG game with retro graphics for Playdate console, PC, Mac, Linux and retro computers (Amiga, MacOS 7.6 m68k, MS-DOS 386/486 PC).

Our protagonist (yes, it's a cat) demoing the Ultima IV/V style fog-of-war (slowed down to show the fading in and out of the fog):

<img src="https://private-user-images.githubusercontent.com/9162068/297983924-d13e6ab7-efd2-4723-ab0f-24c3bb36331c.gif?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MDU2NDkzNDEsIm5iZiI6MTcwNTY0OTA0MSwicGF0aCI6Ii85MTYyMDY4LzI5Nzk4MzkyNC1kMTNlNmFiNy1lZmQyLTQ3MjMtYWIwZi0yNGMzYmIzNjMzMWMuZ2lmP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI0MDExOSUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNDAxMTlUMDcyNDAxWiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9M2Y1ODg3YTJmZTcyY2ZmNjQ3Y2ZhNTlmOGQ1ZjIxMGIxYjcwYzYzODVkY2VlY2E5NjEzN2UwMDgxZWI1MzQ2OSZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QmYWN0b3JfaWQ9MCZrZXlfaWQ9MCZyZXBvX2lkPTAifQ.ZYaNNMV-jbue7_iH4C5PluaGKPH_jHXQYNU_BvBpHN0">

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
