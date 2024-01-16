DOSBOX="/Applications/DOSBox Staging.app/Contents/MacOS/dosbox"
APPNAME=cmoria

BUILDDIR=build/msdos
DISTDIR=dist/msdos
SRCDIR=src/game
DATADIR=content

CC=/opt/djgpp/bin/i586-pc-msdosdjgpp-g++
CFLAGS=-O3 -fipa-pta -flto -fdevirtualize-at-ltrans

DELTREE=rm -R
PYTHON=python3

define fn_mkdir
	@mkdir -p "$(1)"
endef

SRC=\
msdos/main.c \
msdos/engine.c \
msdos/engine_bitmap_msdos.c \
msdos/vesa_api.c \
engine_bitmap.c \
game.c \
game_container.c \
game_map.c \
game_mapmanager.c \
game_player.c \
game_tile.c \
game_tilemanager.c \
gfx_tiledb.c \
gfx_sprite.c \
gfx_sightmap.c \
gfx_viewport.c \
helpers/h_hash.c \
helpers/h_set.c \
helpers/h_stringarray.c \
helpers/h_ini_file.c \
helpers/h_memory.c \
msdos/config.c

OBJ=$(patsubst %.c, $(BUILDDIR)/%.o, $(SRC))
OBJFILES=$(patsubst %.c, %.o, $(SRC))

all: $(APPNAME)

# copy final executable and data files
$(APPNAME): $(BUILDDIR)/$(APPNAME).exe
	# copy executable
	@$(call fn_mkdir,$(DISTDIR))
	@cp $(BUILDDIR)/$(APPNAME).exe $(DISTDIR)
	@cp platform/msdos/cwsdpmi.exe $(DISTDIR)
	# copy / create data files
	. venv/bin/activate; $(PYTHON) src/tools/psd_exporter/main.py "$(BUILDDIR)/tiles_png" "png"
	-@rm -R $(DISTDIR)/tiles
	. venv/bin/activate; $(PYTHON) src/tools/image_converter/converter.py -mode=256 -in="$(BUILDDIR)/tiles_png" -out="$(DISTDIR)/tiles"
	. venv/bin/activate; $(PYTHON) src/tools/msdos_renamer/main.py -dir=$(DISTDIR)/tiles -prefix=tiles/ -ext=bmp
	. venv/bin/activate; $(PYTHON) src/tools/msdos_renamer/main.py -dir=$(DISTDIR)/tiles -prefix=tiles/ -ext=pal
	@cp $(DATADIR)/maps.ini $(DISTDIR)/maps.ini
	@cp $(DATADIR)/tiles.ini $(DISTDIR)/tiles.ini

# link object files
$(BUILDDIR)/$(APPNAME).exe: $(OBJ)
	cd $(BUILDDIR); $(CC) $(CFLAGS) $(OBJFILES) -o $(APPNAME).exe $(LIBS)

# compile C sources
$(BUILDDIR)/%.o: $(addprefix $(SRCDIR)/, %.c)
	@$(call fn_mkdir,$(BUILDDIR)/msdos)
	@$(call fn_mkdir,$(BUILDDIR)/helpers)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run:
	-@rm $(DISTDIR)/LOG.TXT
	@cd $(DISTDIR); $(DOSBOX) -conf ../../platform/msdos/dosbox-staging.conf -noautoexec -c "mount c `pwd`" -c "c:" -c "$(APPNAME)"
