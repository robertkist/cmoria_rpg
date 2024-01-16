MAC_SDL_PATH=/opt/homebrew/opt/sdl2
MAC_CC=/opt/homebrew/bin/gcc-11
WIN_SDL_PATH=
WIN_CC=
LINUX_SDL_PATH=/usr/include/SDL2
LINUX_CC=gcc

ifdef OS  # Windows:
else
	ifeq ($(shell uname), Darwin)  # macOS
		SDL2_PATH=$(MAC_SDL_PATH)
    	CC=$(MAC_CC)
	else  # other Unix / Linux
		SDL2_PATH=$(LINUX_SDL_PATH)
    	CC=$(LINUX_CC)
	endif
endif

APPNAME=cmoria
SRCDIR=src/game
BUILDDIR=build/sdl
DISTDIR=dist/sdl
DATADIR=content

# platform specific settings
APPEXT=
SEP=/
CP=cp
PYTHON=python3
DELTREE=rm -R
SDL2_DLL_PATH=$(SDL2_PATH)/lib
INCLUDES="-I$(SDL2_PATH)/include" "-I./include"
LIBS="-L$(SDL2_PATH)/lib" -lSDL2main -lSDL2 -lstdc++

CFLAGS=-std=c11 -Wall -Wextra -Wconversion -Wshadow -Wstrict-overflow -Wformat=2 \
       -Wno-unused-parameter -Wundef -fno-common -O2 -Os -Wpedantic -pedantic -Werror -Wno-sign-conversion \
       -Wno-strict-prototypes -D"LIB_VERSION=\"$(LIB_VERSION)\"" -O3 -fipa-pta -flto -fdevirtualize-at-ltrans \
       -DDEBUG

SRC=\
sdl/main.c \
sdl/sdl_core.c \
sdl/engine.c \
engine_bitmap.c \
game_container.c \
helpers/h_memory.c \
game_tilemanager.c \
helpers/h_ini_file.c \
helpers/h_set.c \
helpers/h_stringarray.c \
helpers/h_hash.c \
gfx_tiledb.c \
gfx_sprite.c \
gfx_viewport.c \
gfx_sightmap.c \
game_mapmanager.c \
game_tile.c \
game_map.c \
game.c \
game_player.c \
sdl/config.c

OBJ=$(patsubst %.c, $(BUILDDIR)/%.o, $(SRC))
OBJFILES=$(patsubst %.c, %.o, $(SRC))

define fn_mkdir
	@mkdir -p "$(1)"
endef

all: $(APPNAME)

# run executable
.PHONY: run
run:
	-@rm dist/sdl/*.txt
	@cd $(DISTDIR); ./$(APPNAME)$(APPEXT)

# copy final executable and data files
$(APPNAME): $(BUILDDIR)/$(APPNAME)$(APPEXT)
	$(call fn_mkdir,$(DISTDIR))
	@$(CP) $(BUILDDIR)$(SEP)$(APPNAME)$(APPEXT) $(DISTDIR)$(SEP)$(APPNAME)$(APPEXT)
	@$(CP) $(DATADIR)/*.ini $(DISTDIR)
	. venv/bin/activate; $(PYTHON) src/tools/psd_exporter/main.py "$(DISTDIR)/tiles" "bmp"

# link object files
$(BUILDDIR)/$(APPNAME)$(APPEXT): $(OBJ)
	cd $(BUILDDIR); $(CC) $(LDFLAGS) $(OBJFILES) -o $(APPNAME)$(APPEXT) $(LIBS)

# compile C sources
$(BUILDDIR)/%.o: $(addprefix $(SRCDIR)/, %.c)
	@$(call fn_mkdir,$(BUILDDIR)/sdl)
	@$(call fn_mkdir,$(BUILDDIR)/helpers)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

