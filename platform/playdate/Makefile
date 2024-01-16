PLAYDATE_SDK_PATH=$(HOME)/Developer/PlaydateSDK
PLAYDATE_SIMULATOR_PATH=$(PLAYDATE_SDK_PATH)/bin/Playdate\ Simulator.app/Contents/MacOS/Playdate\ Simulator
BUILDDIR=build/playdate
DISTDIR=dist/playdate
DATADIR=content

APPNAME=CMoria
CP=cp
MV=mv
PYTHON=python3
DELTREE=rm -R

define fn_mkdir
	@mkdir -p "$(1)"
endef

all:
	$(call fn_mkdir,$(BUILDDIR))
	. venv/bin/activate; $(PYTHON) src/tools/psd_exporter/main.py "$(BUILDDIR)/Source/tiles" "png"
	$(CP) $(DATADIR)/*.ini $(BUILDDIR)/Source
	$(CP) platform/playdate/pdxinfo $(BUILDDIR)/Source
	$(CP) platform/playdate/playdate.mk $(BUILDDIR)
	$(CP) -R src/game $(BUILDDIR)
	cd $(BUILDDIR) && make -f playdate.mk
	$(call fn_mkdir,$(DISTDIR))
	-@$(DELTREE) $(DISTDIR)/$(APPNAME).pdx
	$(CP) -R $(BUILDDIR)/$(APPNAME).pdx $(DISTDIR)

.PHONY: run
run:
	$(PLAYDATE_SIMULATOR_PATH) $(DISTDIR)/$(APPNAME).pdx