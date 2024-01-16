QT += \
    core \
    gui \
    concurrent \
    svg \
    sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# CONFIG += c++11
# QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
# QMAKE_CFLAGS += "-Wno-unused-parameter"

VPATH += ../../src/game

SOURCES += \
    editor/main.cpp \
    editor/mainwindow.cpp \
    editor/engine.cpp \
    editor/config.cpp \
    editor/dbmanager.cpp \
    gfx_tiledb.c \
    editor/editor_container.cpp \
    game_mapmanager.c \
    game_tilemanager.c \
    engine_bitmap.c \
    helpers/h_memory.c \
    helpers/h_set.c \
    helpers/h_hash.c \
    helpers/h_ini_file.c \
    helpers/h_stringarray.c \
    gfx_sprite.c \
    gfx_viewport.c \
    gfx_sightmap.c \
    game_tile.c \
    game_map.c \
    game.c \
    game_player.c

HEADERS += \
    editor/mainwindow.h

FORMS += \
    editor/mainwindow.ui

CONFIG += lrelease

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target