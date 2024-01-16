PLAYDATE_SDK_PATH=$(HOME)/Developer/PlaydateSDK

HEAP_SIZE      = 8388208
STACK_SIZE     = 61800

PRODUCT = CMoria.pdx

# Locate the SDK
SDK = ${PLAYDATE_SDK_PATH}
ifeq ($(SDK),)
	SDK = $(shell egrep '^\s*SDKRoot' ~/.Playdate/config | head -n 1 | cut -c9-)
endif

ifeq ($(SDK),)
$(error SDK path not found; set ENV value PLAYDATE_SDK_PATH)
endif

######
# IMPORTANT: You must add your source folders to VPATH for make to find them
# ex: VPATH += src1:src2
######

VPATH += game

# List C source files here
C_SRC=\
playdate/main.c \
playdate/engine.c \
engine_bitmap.c \
game_container.c \
game_tilemanager.c \
helpers/h_memory.c \
helpers/h_hash.c \
helpers/h_ini_file.c \
helpers/h_set.c \
helpers/h_stringarray.c \
gfx_tiledb.c \
gfx_sprite.c \
gfx_viewport.c \
gfx_sightmap.c \
game_tile.c \
game_mapmanager.c \
game_map.c \
game.c \
game_player.c \
playdate/config.c

SRC=$(patsubst %, game/%, $(C_SRC))

# List all user directories here
UINCDIR =

# List user asm files
UASRC =

# List all user C define here, like -D_DEBUG=1
UDEFS =

# Define ASM defines here
UADEFS =

# List the user directory to look for the libraries here
ULIBDIR =

# List all user libraries here
ULIBS =

include $(SDK)/C_API/buildsupport/common.mk

