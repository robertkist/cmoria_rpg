# CMoria RPG
A RPG game with retro graphics for Playdate console, PC, Mac, Linux and retro computers (Amiga, MacOS 7.6 m68k, MS-DOS 386/486 PC).

Our protagonist (yes, it's a cat) demoing the Ultima IV/V style fog-of-war:

<img src="https://private-user-images.githubusercontent.com/9162068/295592756-a50e11c1-9ab3-4329-b40c-dca6705de9ee.gif?jwt=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3MDQ4OTYwNDEsIm5iZiI6MTcwNDg5NTc0MSwicGF0aCI6Ii85MTYyMDY4LzI5NTU5Mjc1Ni1hNTBlMTFjMS05YWIzLTQzMjktYjQwYy1kY2E2NzA1ZGU5ZWUuZ2lmP1gtQW16LUFsZ29yaXRobT1BV1M0LUhNQUMtU0hBMjU2JlgtQW16LUNyZWRlbnRpYWw9QUtJQVZDT0RZTFNBNTNQUUs0WkElMkYyMDI0MDExMCUyRnVzLWVhc3QtMSUyRnMzJTJGYXdzNF9yZXF1ZXN0JlgtQW16LURhdGU9MjAyNDAxMTBUMTQwOTAxWiZYLUFtei1FeHBpcmVzPTMwMCZYLUFtei1TaWduYXR1cmU9MDEyYzBjOTNiNzEwYmI4NzY2NmZmNGJhOTNjMDU3Y2QxNzkxY2ZlZmViZWQzNTZiMjg0ZmU2MjZhNTIyZjNiNCZYLUFtei1TaWduZWRIZWFkZXJzPWhvc3QmYWN0b3JfaWQ9MCZrZXlfaWQ9MCZyZXBvX2lkPTAifQ.YCxTkvBKIuAbSfQ2qGp6xX6OfyOPOwrdhMM1n1o2Wyw">

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

## Why make this same?

This project is a labor of love: I enjoy coding. I want to learn how to architect a simple non-general-purpose game engine.
I want to learn how to make different game systems (AI, dialogue, etc.), I want to tell a story, I want to learn how
to create pixel art, I want to say "I made my own game!", and I want to do it from A-Z.

## Why not use a game engine?

I don't want to tie myself to someone else's technological decisions, including possible deprecation, change of engine
license terms, upgrade cycles, or other changes.  

## Why the CMoria Name?

Moria is one of the first rogue-likes with a town level. My aim is to write a game with mechanics similar to Ultima IV/V. Like Moria it has a top-down view, town levels, and I might even add some procedural generation. The C stands for C (the language I'm using) ... or it could stand for cats, which will feature prominently in the story and the lore.

## Where's the source code?

I just started this repo... It will be added soon(tm), or check some of the development branches

## Planned features

- Ultima IV/V style fog-of-war
- Ultima V style NPC dialogs and vendors
- A battle map
- Inventories, stats and talents
- Ultima VI style dungeons
- All kinds of melee and ranged weapons
- A magic and abilities system

## Repo Contents

Included:

- Source code for the game and its toolchain (C, Python)
- Raw art sources (Photoshop)
- Developer documentation

Not included:

- Game design documents
- Prototypes
- Concept art and reference art

## Building

- on MacOS: you can build for all platforms except Win, Linux
- Win: you can build for Win
- Linux: you can build for Linux

## Why target Retro-Computers?

Because I can :) The game isn't going to be computationally expensive. If it runs on the playdate, it might just as well run on some early 32-bit platforms. Also, being a collector of some vintage PCs, I thought it would be great to see my own game running on the vintage hardware I own.

## License

This project is a labor of love and therefore I'd like as many fellow (game) developers and players to get value out of it.
The license covers code and artwork. The code license is very permissible, however, there are some restrictions on the
artwork.
