#include <stddef.h>
#include <stdio.h>
#include "editor_container.h"
#include "../engine.h"
#include "../game_tilemanager.h"
#include "../game_mapmanager.h"
#include "../gfx_tiledb.h"
#include "../game.h"
#include "../game_player.h"
#include "../gfx_viewport.h"
#include "../helpers/h_ini_file.h"
#include "../helpers/h_memory.h"
#include "../game_map.h"
#include "dbmanager.h"

#define DEBUG 1

static GameCommand gameCommand = NO_COMMAND;

void eventHandlerRunning(EngineInputEvent event);

struct MapCollection* loadMaps() {
    struct MapCollection* maps = (struct MapCollection*) GMalloc(sizeof(struct MapCollection), "MapCollection");
    maps->mapCount = 1; // TODO
    maps->maps = (struct GameMap**)GMalloc(maps->mapCount * sizeof(struct GameMap*), "GameMaps");
    DbManager db = DbManager::getInstance();
    for (int i = 0; i < maps->mapCount; i++) {
        maps->maps[i] = db.loadMap();
    }
    return maps;
}








void GameContainer_init(void) {
    Engine_log("GameContainer_init()");
    // initialize sub-components
    GameTileManager_loadTiles("tiles.ini");
    GfxTileDb_loadTiles("tiles.ini");



    GameMapManager_loadMaps(loadMaps());


    Game_init(); // game logic initialization
    MapDrawing_init(); // game graphics viewport initialization
    // set initial game state
    GameMapManager_setCurrentMap(0);
    Player_setPosition(9, 8);  // TODO
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
