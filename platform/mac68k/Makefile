# path to RETRO68
BASILISK=/Applications/BasiliskII.app/Contents/MacOS/BasiliskII
RETRO68=/opt/Retro68-build/toolchain
DESTDRV=/Users/robert/Documents/Basilisk/exchange

PREFIX=$(RETRO68)/m68k-apple-macos
CC=$(RETRO68)/bin/m68k-apple-macos-gcc
CXX=$(RETRO68)/bin/m68k-apple-macos-g++
REZ=$(RETRO68)/bin/Rez

LDFLAGS=-lRetroConsole
RINCLUDES=$(PREFIX)/RIncludes
REZFLAGS=-I$(RINCLUDES)

APPNAME=cmoria
BUILDDIR=build/mac68k
DISTDIR=dist/mac68k
SRCDIR=src/game
DATADIR=content

PYTHON=python3

C_SRC=\
mac68k/main.c \
mac68k/config.c \
mac68k/engine.c \
mac68k/engine_macbitmap.c \
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
gfx_sightmap.c \
gfx_sprite.c \
gfx_viewport.c \
engine_bitmap.c

SRC=$(patsubst %, $(SRCDIR)/%, $(C_SRC))

define fn_mkdir
	@mkdir -p "$(1)"
endef

$(APPNAME).bin $(APPNAME).APPL $(APPNAME).dsk:
	@$(call fn_mkdir,$(BUILDDIR))
	. venv/bin/activate; $(PYTHON) src/tools/psd_exporter/main.py "$(BUILDDIR)/tiles_png" "png"
	. venv/bin/activate; $(PYTHON) src/tools/image_converter/converter.py -mode=1bw -in="$(BUILDDIR)/tiles_png" -out="$(DISTDIR)/tiles"
	$(CXX) $(SRC) -o $(BUILDDIR)/$(APPNAME).code.bin $(LDFLAGS)
	cd $(BUILDDIR); $(REZ) $(REZFLAGS) \
		--copy "$(APPNAME).code.bin" \
		"$(RINCLUDES)/Retro68APPL.r" \
		-t "APPL" -c "????" \
		-o $(APPNAME).bin --cc $(APPNAME).APPL
	cd $(BUILDDIR); dd if=/dev/zero of=$(APPNAME).dsk bs=1M count=64
	cd $(BUILDDIR); hformat -l Data $(APPNAME).dsk
	cd $(BUILDDIR); hcopy ./$(APPNAME).bin :
	@$(call fn_mkdir,$(DISTDIR))
	cp $(BUILDDIR)/*.APPL $(DISTDIR)
	cp $(DATADIR)/*.ini $(DISTDIR)

run:
	rm -R $(DESTDRV)/*
	cp -R $(DISTDIR)/* $(DESTDRV)
	$(BASILISK) --scale_integer true --scale_nearest true
