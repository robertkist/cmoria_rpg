QT += \
    core \
    gui \
    concurrent \
    svg
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# CONFIG += c++11
# QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
# QMAKE_CFLAGS += "-Wno-unused-parameter"

VPATH += ../../src/game

SOURCES += \
    qt/main.cpp \
    qt/mainwindow.cpp \
    qt/engine.cpp \
    engine_bitmap.c \
    game_container.c \
    helpers/h_memory.c \
    helpers/h_set.c \
    game_tilemanager.c \
    helpers/h_hash.c \
    helpers/h_ini_file.c \
    helpers/h_stringarray.c \
    gfx_tiledb.c \
    gfx_sprite.c \
    gfx_sightmap.c \
    gfx_viewport.c \
    game_mapmanager.c \
    game_tile.c \
    game_map.c \
    game.c \
    game_player.c \
    qt/config.c

HEADERS += \
    qt/mainwindow.h \
    engine.h \
    engine_bitmap.h

CONFIG += lrelease

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target