#include <stddef.h>
#include <stdio.h>
#include "game_container.h"
#include "engine.h"
#include "game_tilemanager.h"
#include "game_mapmanager.h"
#include "gfx_tiledb.h"
#include "game.h"
#include "game_player.h"
#include "gfx_viewport.h"
#include "helpers/h_ini_file.h"
#include "helpers/h_memory.h"
#include "game_map.h"

#define DEBUG 1

static GameCommand gameCommand = NO_COMMAND;
void eventHandlerRunning(EngineInputEvent event);


struct GameMap* loadMap(int i, struct HIniFile* ini) {
    char section[11];
    snprintf(section, 11, "%i", i);
    struct HCoordinate size = {
            .x=HIniFile_getInt(ini, section, "width", 0),
            .y=HIniFile_getInt(ini, section, "height", 0)};
    struct HCoordinate playerStart = {
            .x=HIniFile_getInt(ini, section, "player_start_x", 0),
            .y=HIniFile_getInt(ini, section, "player_start_y", 0)};
    bool wrapAround = HIniFile_getBool(ini, section, "wraparound", false);
    char* name = HIniFile_get(ini, section, "name");
    char* floorLayerString = HIniFile_get(ini, section, "floor");
    int* floorLayer = (int*)GMalloc(sizeof(int) * size.x * size.y, "GameMapManager_floorLayer");
    csvStrToArray(floorLayer, floorLayerString, size.x * size.y);
    char* shadowLayerString = HIniFile_get(ini, section, "shadow");
    int* shadowLayer = (int*)GMalloc(sizeof(int) * size.x * size.y, "GameMapManager_shadowLayer");
    csvStrToArray(shadowLayer, shadowLayerString, size.x * size.y);
    char* wallLayerString = HIniFile_get(ini, section, "wall");
    int* wallLayer = (int*)GMalloc(sizeof(int) * size.x * size.y, "GameMapManager_wallLayer");
    csvStrToArray(wallLayer, wallLayerString, size.x * size.y);
    struct GameMap* map = GameMap_new(i, wrapAround, name, size, playerStart, floorLayer, shadowLayer, wallLayer);
    GFree(floorLayer, "GameMapManager_floorLayer");
    GFree(shadowLayer, "GameMapManager_shadowLayer");
    GFree(wallLayer, "GameMapManager_wallLayer");
    return map;
}

struct MapCollection* loadMaps(const char* fname) {
    struct MapCollection* maps = (struct MapCollection*) GMalloc(sizeof(struct MapCollection), "MapCollection");

    Engine_log("loading map from '%s'", fname);
    char* string = Engine_loadTextFile(fname);
    struct HIniFile* ini = HIniFile_new(string);
    GFree(string, "Engine_loadTextFile");
    maps->mapCount = HIniFile_getInt(ini, "general", "count", 0);
#ifdef DEBUG
    if (maps->mapCount == 0) {
        Engine_logError("no maps found in '%s'", fname);
        Engine_exit(-1);
    }
#endif
    maps->maps = (struct GameMap**)GMalloc(maps->mapCount * sizeof(struct GameMap*), "GameMaps");
    for (int i = 0; i < maps->mapCount; i++) {
        maps->maps[i] = loadMap(i, ini);
    }
    HIniFile_delete(ini);
    return maps;
}











void GameContainer_init(void) {
    Engine_log("GameContainer_init()");
    // initialize sub-components

    GameTileManager_loadTiles("tiles.ini");
    GfxTileDb_loadTiles("tiles.ini");
    GameMapManager_loadMaps(loadMaps("maps.ini"));

    Game_init(); // game logic initialization
    GameMapManager_setCurrentMap(0);
    Player_setPosition(9, 8);  // TODO
    MapDrawing_init(); // game graphics viewport initialization
    // set initial game state


    calculateMapDrawingVariables(Player_getPosition());
    Engine_log("===== GAME INIT FINISHED =====");
}

void GameContainer_delete(void) {
    Engine_log("===== GAME SHUTDOWN INITIATED =====");
    MapDrawing_delete();
    GameMapManager_delete();
    GfxTileDb_delete();
    GameTileManager_delete();
}

/***************************************************************
 * INPUT HANDLING
 ***************************************************************/

void GameContainer_handleInput(EngineInputEvent event) {
    switch (getGraphicsState()) {
        case STARTUP: break;
        case RUNNING: eventHandlerRunning(event); break;
        case SCROLLING:
            if (!eventHandlerScrolling(event))
                eventHandlerRunning(event);
            break;
    }
}

void eventHandlerRunning(EngineInputEvent event) {
    gameCommand = NO_COMMAND;
    switch(event) {
        case KEY_LEFT:
            gameCommand = GO_WEST;
            break;
        case KEY_RIGHT:
            gameCommand = GO_EAST;
            break;
        case KEY_UP:
            gameCommand = GO_NORTH;
            break;
        case KEY_DOWN:
            gameCommand = GO_SOUTH;
            break;
        default:
            break;
    }
    if (gameCommand != NO_COMMAND) {
        GameCommandResult result = Game_command(gameCommand);
        switch (result) {
            case MOVE_ONE_TILE:
                startScrolling(gameCommand);
                break;
            case NO_RESULT:
                break;
        }
    }
}

/***************************************************************
 * GRAPHICS UPDATE
 ***************************************************************/

bool GameContainer_update(EngineInputEvent event) {
    /* gets called regularly by the game loop */
    GameContainer_handleInput(event);
    switch (getGraphicsState()) {
        case RUNNING:
            return updateRunning();
        case SCROLLING:
            return updateScrolling();
        case STARTUP:
            break;
    }
    return false;
}
