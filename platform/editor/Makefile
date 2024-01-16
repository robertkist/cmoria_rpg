WIN_MINGW_BIN_PATH=C:\Qt\Tools\mingw1120_64\bin
WIN_QT_BIN_PATH=C:\Qt\6.6.1\mingw_64\bin
MAC_QT_BIN_PATH=~/Qt/6.6.1/macos/bin
LINUX_QT_BIN_PATH=/opt/Qt/6.6.1/gcc_64/bin

APPNAME=editor

ifdef OS  # Windows:
define fn_mkdir
	if not exist "$(1)" mkdir "$(1)"
endef
	QMAKE=$(WIN_QT_BIN_PATH)\qmake
	QMAKE=SET PATH=$(WIN_MINGW_BIN_PATH);$(WIN_QT_BIN_PATH);%PATH% && qmake
	MAKE=SET PATH=$(WIN_MINGW_BIN_PATH);$(WIN_QT_BIN_PATH);%PATH% && make
	DEPLOYQT=SET PATH=$(WIN_MINGW_BIN_PATH);$(WIN_QT_BIN_PATH);%PATH% && windeployqt
	APPEXT=.exe
	PYTHON=python
	CP=copy
	MV=move
	DELTREE=rd /s /q
	VENVCMD=venv\Scripts\activate
	SEP=\\
	EXECUTABLE=release\$(APPNAME).exe
else
define fn_mkdir
	@mkdir -p "$(1)"
endef
	PYTHON=python3
	CP=cp
	MV=mv
	DELTREE=rm -R
	VENVCMD=. venv/bin/activate
	SEP=/
	MAKE=make
	ifeq ($(shell uname), Darwin)  # macOS
		QMAKE=$(MAC_QT_BIN_PATH)/qmake
		QTARCHS=QMAKE_APPLE_DEVICE_ARCHS="arm64"
		APPEXT=.app
		APPDIR=/$(APPNAME).app/Contents/MacOS
		DEPLOYQT=$(MAC_QT_BIN_PATH)/macdeployqt
		EXECUTABLE=$(APPNAME).app
	else  # other Unix / Linux
		QMAKE=$(LINUX_QT_BIN_PATH)/qmake
		DEPLOYQT=linuxdeployqt
		EXECUTABLE=$(APPNAME)
	endif
endif

QMAKEOPTS=-Wall -Wlogic
BUILDDIR=build$(SEP)editor
DISTDIR=dist$(SEP)editor
DATADIR=content

define fn_mkdir
	@mkdir -p "$(1)"
endef

all: build
	-@$(DELTREE) $(DISTDIR)
	@$(call fn_mkdir,$(DISTDIR))
	$(MV) $(BUILDDIR)$(SEP)$(EXECUTABLE) $(DISTDIR)
	$(CP) $(DATADIR)$(SEP)*.ini $(DISTDIR)$(APPDIR)
	$(VENVCMD) && $(PYTHON) src/tools/psd_exporter/main.py "$(DISTDIR)$(APPDIR)/tiles" "png"
	$(DEPLOYQT) $(DISTDIR)$(SEP)$(APPNAME)$(APPEXT)

.PHONY: build
build:
	@$(call fn_mkdir,$(BUILDDIR))
	$(QMAKE) $(QMAKEOPTS) $(QTARCHS) platform/editor/$(APPNAME).pro -o $(BUILDDIR)$(SEP)Makefile.qmake
	cd $(BUILDDIR) && $(MAKE) -f Makefile.qmake

.PHONY: run
run:
	cd $(DISTDIR)/$(APPDIR) && .$(SEP)$(APPNAME)