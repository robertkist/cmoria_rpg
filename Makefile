OBJDIR=build
DISTDIR=dist

ifdef OS  # Windows:
	DELTREE=rd /s /q
	PYTHON=python
	PIP=pip
	TOUCH=type nul>
	VENVCMD=venv\Scripts\activate
else  # Unix / Linux / Mac
	DELTREE=rm -R
	PYTHON=python3
	PIP=pip3
	TOUCH=touch
	VENVCMD=. venv/bin/activate
endif

.PHONY: all
all:
	@echo "TARGETS:"
	@echo "  editor        - builds the game's editor for MacOS, Win or Linux"
	@echo "  sdl           - builds for MacOS, Win or Linux using SDL backend"
	@echo "  qt            - builds for MacOS, Win or Linux using Qt backend"
	@echo "  playdate      - builds for Playdate"
	@echo "  msdos         - builds for MS-DOS"
	@echo "  mac68k        - builds for Classic MacOS System 7.6"
	@echo "  amiga         - builds for Amiga OS 3.1"
	@echo "  all_platforms - builds all the above targets"
	@echo "TIP: append _run to above targets to launch the game/editor!"
	@echo "  clean         - removes build, dist and venv directories"
	@echo "  dist_clean    - removes build and dist directories"
	@echo "  venv          - sets up the Python venv for the toolchain"

.PHONY: editor
editor: venv
	@make -f platform/editor/Makefile

.PHONY: editor_run
editor_run: editor
	@make -f platform/editor/Makefile run

.PHONY: sdl
sdl: venv
	@make -f platform/sdl/Makefile

.PHONY: sdl_run
sdl_run: sdl
	@make -f platform/sdl/Makefile run

.PHONY: qt
qt: venv
	@make -f platform/qt/Makefile

.PHONY: qt_run
qt_run: qt
	@make -f platform/qt/Makefile run

.PHONY: playdate
playdate: venv
	@make -f platform/playdate/Makefile

.PHONY: playdate_run
playdate_run: playdate
	@make -f platform/playdate/Makefile run

.PHONY: msdos
msdos: venv
	@make -f platform/msdos/Makefile

.PHONY: msdos_run
msdos_run: msdos
	@make -f platform/msdos/Makefile run

.PHONY: mac68k
mac68k: venv
	@make -f platform/mac68k/Makefile

.PHONY: mac68k_run
mac68k_run: mac68k
	@make -f platform/mac68k/Makefile run

.PHONY: amiga
amiga: venv
	@make -f platform/amiga/Makefile

.PHONY: amiga_run
amiga_run: amiga
	@make -f platform/amiga/Makefile run

.PHONY:
all_platforms: sdl qt editor playdate amiga mac68k msdos

venv: venv/touchfile

venv/touchfile: requirements.txt
	$(PYTHON) -m venv ./venv
	$(VENVCMD) && $(PIP) install -Ur requirements.txt
	$(TOUCH) venv/touchfile

.PHONY: dist_clean
dist_clean:
	-@$(DELTREE) $(DISTDIR)
	-@$(DELTREE) $(OBJDIR)

.PHONY: clean
clean: dist_clean
	-@$(DELTREE) venv








