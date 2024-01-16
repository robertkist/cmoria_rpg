UAEDRIVE=/Users/robert/Documents/FS-UAE/Hard\ Drives/AmigaDev
FSUAE=/Applications/FS-UAE.app/Contents/MacOS/fs-uae
FSUAECONFIG=platform/amiga/A3000_dev.fs-uae

# executables
CC=/opt/amiga/bin/m68k-amigaos-gcc

# compiler flags
LIBS=-lamiga
CFLAGS=-O3

SRCDIR=src/game
BUILDDIR=build/amiga
DISTDIR=dist/amiga
DATADIR=content
APPNAME=cmoria

DELTREE=rm -R
PYTHON=python3

define fn_mkdir
	@mkdir -p "$(1)"
endef

SRC=\
amiga/main.c \
amiga/engine.c \
amiga/engine_amigabitmap.c \
amiga/engine_amigatime.c \
amiga/config.c \
helpers/h_memory.c \
helpers/h_hash.c \
helpers/h_set.c \
helpers/h_ini_file.c \
helpers/h_stringarray.c \
game.c \
game_container.c \
game_tilemanager.c \
game_map.c \
game_mapmanager.c \
game_player.c \
game_tile.c \
gfx_tiledb.c \
gfx_sprite.c \
gfx_sightmap.c \
gfx_viewport.c \
engine_bitmap.c

OBJ=$(patsubst %.c, $(BUILDDIR)/%.o, $(SRC))
OBJFILES=$(patsubst %.c, %.o, $(SRC))

all: $(APPNAME)

# copy final executable and data files
$(APPNAME): $(BUILDDIR)/$(APPNAME)
	$(call fn_mkdir,$(DISTDIR))
	@cp $(BUILDDIR)/$(APPNAME) $(DISTDIR)
	@cp $(DATADIR)/maps.ini $(DISTDIR)
	@cp $(DATADIR)/tiles.ini $(DISTDIR)
	. venv/bin/activate; $(PYTHON) src/tools/psd_exporter/main.py "$(DISTDIR)/tiles_png" "png"
	. venv/bin/activate; $(PYTHON) src/tools/image_converter/converter.py -mode=2bpl -in="$(DISTDIR)/tiles_png" -out="$(DISTDIR)/tiles"
	-@$(DELTREE) $(UAEDRIVE)/amiga
	@cp -R $(DISTDIR) $(UAEDRIVE)

# link object files
$(BUILDDIR)/$(APPNAME): $(OBJ)
	cd $(BUILDDIR); $(CC) $(CFLAGS) $(OBJFILES) -o $(APPNAME) $(LIBS)

# compile C sources
$(BUILDDIR)/%.o: $(addprefix $(SRCDIR)/, %.c)
	$(call fn_mkdir,$(BUILDDIR)/amiga)
	$(call fn_mkdir,$(BUILDDIR)/helpers)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run:
	$(FSUAE) $(FSUAECONFIG)
