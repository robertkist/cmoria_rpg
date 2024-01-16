import os

"""
Analyzes the log of a debug build (set compiler flag -DDEBUG):
matches GMALLOC to the corresponding GFREE command.
Lists any unmatched GMALLOC or GFREE commands.
"""

FILE = os.path.join("..", "..", "..", "dist", "sdl", "memory_log.txt")
mallocs = {}

with open(FILE, "rt", encoding="utf8") as fp:
    data = fp.read()
data = data.split("\n")
for line in data:
    if line.startswith("GMALLOC"):
        line = line.split("GMALLOC:")[1].split(",")[0].strip()
        if line not in mallocs:
            mallocs[line] = 1
        else:
            mallocs[line] += 1
    elif line.startswith("GFREE"):
        line = line.split("GFREE:")[1].split(",")[0].strip()
        if line not in mallocs:
            mallocs[line] = -1
        else:
            mallocs[line] -= 1
for key in mallocs:
    if mallocs[key] != 0:
        print(key, mallocs[key])