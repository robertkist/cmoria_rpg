#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "gfx_viewport.h"
#include "helpers/h_memory.h"
#include "engine.h"
#include "engine_bitmap.h"
#include "game_tilemanager.h"
#include "game_mapmanager.h"
#include "gfx_tiledb.h"
#include "game.h"
#include "config.h"
#include "game_player.h"
#include "helpers/h_set.h"
#include "gfx_sprite.h"
#include "gfx_sightmap.h"

#ifndef DEBUG
#define getFogMap(x, y) fogMap[((y) * viewportSizeT.width) + (x)]
#define getFogMapOld(x, y) fogMapOld[((y) * viewportSizeT.width) + (x)]
#define setFogMap(x, y, v) fogMap[((y) * viewportSizeT.width) + (x)] = v
#define getBleedMap(x, y) bleedMap[((y) * viewportSizeT.width) + (x)]
#define getBleedMapOld(x, y) bleedMapOld[((y) * viewportSizeT.width) + (x)]
#define setBleedMap(x, y, v) bleedMap[((y) * viewportSizeT.width) + (x)] = v
#endif

#define INVISIBLE 0
#define VISIBLE 1
#define EMPTY 0

#define L_BL 22   // b L bottomleft 1.png
#define L_TL 23   // b L topleft 1.png
#define L_TR 24   // b L topright 1.png
#define L_BR 25   // b L bottomright 1.png
#define T_L 26    // b right 1.png
#define T_R 27    // b left 1.png
#define T_B 28    // b bottom 1.png
#define T_T 29    // b top 1.png
#define U_T 30    // b U 1.png
#define U_B 31    // b U2 1.png
#define C_BR 32   // b corner bottomright 1.png
#define C_TR 33   // b corner topright 1.png
#define C_BL 34   // b corner bottomleft 1.png
#define C_TL 35   // b corner topleft 1.png
#define C_TLTR 36 // b U3 1.png
#define C_X 37    // b U5 1.png
#define C_Y 38    // b U4 1.png
#define B_CTR 39  // b center 1.png

static GraphicsState graphicsState = RUNNING;
static bool redrawRequested = true;
static struct HCoordinate viewportOffsetP;
static struct HCoordinate viewportHalfSizeT;

// Map drawing
static struct HCoordinate mapDrawEnd = { .x=0, .y=0 };
static struct HCoordinate rightScrollAdjust = { .x=0, .y=0 };
static struct HCoordinate mapDrawStart = { .x=0, .y=0 };

// Scrolling
static struct HCoordinate scrollDirection = { .x=0, .y=0 };
static struct HCoordinate scrollOffset = { .x=0, .y=0 };
static int8_t scrollCount = 0;

// Sprite Advancement
static struct IntSet* animSprites_set = NULL;
static int* animSprites = NULL;
static size_t animSprites_size = 0;
static bool animSprites_recountFlag = true;

static struct EngineBitmap* b = NULL;

void AnimSprites_recount(void);
void drawTile(int tileId, int x, int y, int currentFrame);
bool advanceFrames(void);
void generateFogMap(struct HCoordinate pp);

static struct HCoordinate fogScrollAdjust = { .x=0, .y=0 }; // needed for scrolling old fog of war
int8_t* fogMap = NULL;
int8_t* fogMapOld = NULL;
int8_t* bleedMap = NULL;
int8_t* bleedMapOld = NULL;
int fogFrame = 0;

extern struct HCoordinate sightMapSize;
extern struct HCoordinate sightMapHalfSize;

#ifdef DEBUG
// viewportSizeT.height * 2 + 4) * (viewportSizeT.width + 2) * sizeof(int8_t)

int8_t getFogMap(int x, int y) {
    if (!(x >= 0 && y >= 0 && x < sightMapSize.width && y < (sightMapSize.height * 2))) {
        Engine_logError("getFogMap x: %i, y: %i\n", x, y); Engine_exit(-1); }
    return fogMap[((y) * (sightMapSize.width)) + (x)];
}
int8_t getFogMapOld(int x, int y) {
    if (!(x >= 0 && y >= 0 && x < sightMapSize.width && y < (sightMapSize.height * 2))) {
        Engine_logError("getFogMapOld x: %i, y: %i\n", x, y); Engine_exit(-1); }
    return fogMapOld[((y) * (sightMapSize.width)) + (x)];
}
void setFogMap(int x, int y, int8_t v) {
    if (!(x >= 0 && y >= 0 && x < sightMapSize.width && y < (sightMapSize.height * 2))) {
        Engine_logError("setFogMap x: %i, y: %i\n", x, y); Engine_exit(-1); }
    fogMap[((y) * (sightMapSize.width)) + (x)] = v;
}
int8_t getBleedMap(int x, int y) {
    if (!(x >= 0 && y >= 0 && x < sightMapSize.width && y < (sightMapSize.height * 2))) {
        Engine_logError("getBleedMap x: %i, y: %i\n", x, y); Engine_exit(-1); }
    return bleedMap[((y) * (sightMapSize.width)) + (x)];
}
int8_t getBleedMapOld(int x, int y) {
    if (!(x >= 0 && y >= 0 && x < sightMapSize.width && y < (sightMapSize.height * 2))) {
        Engine_logError("getBleedMapOld x: %i, y: %i\n", x, y); Engine_exit(-1); }
    return bleedMapOld[((y) * (sightMapSize.width)) + (x)];
}
void setBleedMap(int x, int y, int8_t v) {
    if (!(x >= 0 && y >= 0 && x < sightMapSize.width && y < (sightMapSize.height * 2))) {
        Engine_logError("setBleedMap x: %i, y: %i\n", x, y); Engine_exit(-1); }
    bleedMap[((y) * (sightMapSize.width)) + (x)] = v;
}
#endif

void MapDrawing_init(void) {
    SightMap_init();
    viewportOffsetP.width = (screenSizeP.width - (tileSizeP.width * viewportSizeT.width)) / 2;
    viewportOffsetP.height = (screenSizeP.height - (tileSizeP.height * viewportSizeT.height)) / 2;
    viewportHalfSizeT.width = (viewportSizeT.width - 1) / 2;
    viewportHalfSizeT.height = (viewportSizeT.height - 1) / 2;
    animSprites_set = IntSet_new(viewportSizeT.width * viewportSizeT.height * 3);
    animSprites = (int*) GMalloc(viewportSizeT.width * viewportSizeT.height * 3 * sizeof(int), "animSprites");
    b = EngineBitmap_new("tiles/player 00");  // TODO

    // TODO we should add a 1 tile border to this!
    fogMap = (int8_t*) GMalloc((sightMapSize.height * 2) * (sightMapSize.width) * sizeof(int8_t), "fogMap");
    fogMapOld = (int8_t*) GMalloc((sightMapSize.height * 2) * (sightMapSize.width) * sizeof(int8_t), "fogMapOld");
    bleedMap = (int8_t*) GMalloc((sightMapSize.height * 2) * (sightMapSize.width) * sizeof(int8_t), "bleedMap");
    bleedMapOld = (int8_t*) GMalloc((sightMapSize.height * 2) * (sightMapSize.width) * sizeof(int8_t), "bleedMapOld");

//    fogMap = (int8_t*) GMalloc(viewportSizeT.height * 2 * viewportSizeT.width * sizeof(int8_t), "fogMap");
//    fogMapOld = (int8_t*) GMalloc(viewportSizeT.height * 2 * viewportSizeT.width * sizeof(int8_t), "fogMapOld");
//    bleedMap = (int8_t*) GMalloc(viewportSizeT.height * 2 * viewportSizeT.width * sizeof(int8_t), "bleedMap");
//    bleedMapOld = (int8_t*) GMalloc(viewportSizeT.height * 2 * viewportSizeT.width * sizeof(int8_t), "bleedMapOld");

    calculateSight(Player_getPosition());
    generateFogMap(Player_getPosition());
}

void MapDrawing_delete(void) {
    SightMap_delete();
    IntSet_delete(animSprites_set);
    GFree(animSprites, "animSprites");
    EngineBitmap_delete(b);
    GFree(fogMap, "fogMap");
    GFree(fogMapOld, "fogMapOld");
    GFree(bleedMap, "bleedMap");
    GFree(bleedMapOld, "bleedMapOld");
}

GraphicsState getGraphicsState(void) {
    /* returns the current state of the graphics engine */
    return graphicsState;
}

bool updateRunning(void) {
    if (advanceFrames() || redrawRequested) {
        Engine_clearFrame();
        if (animSprites_recountFlag) {
            IntSet_clear(animSprites_set);
            Engine_log("ANIM SPRITES RECOUNT");
        }
        // draw map
//        struct HCoordinate pos = Player_getPosition();
        updateDrawFirstPass();

        Engine_drawBitmap(b,
                          viewportHalfSizeT.width * tileSizeP.width + viewportOffsetP.width,
                          viewportHalfSizeT.height * tileSizeP.height + viewportOffsetP.height);
        updateDrawSecondPass();
        updateDrawThirdPass();

        Engine_drawRect(0, 0, screenSizeP.width, screenSizeP.height); // sdl
        redrawRequested = false;
        AnimSprites_recount();
        return true; // flip page
    }
    return false;
}

/***************************************************************
 * SPRITE FRAME ADVANCEMENT
 ***************************************************************/

bool advanceFrames(void) {
    /* advances all visible frames by 1 */
    size_t advanceCount = 0;
    unsigned long time = Engine_getTime();
    for (size_t i = 0; i < animSprites_size; i++) {
        if(GfxTileDb_advanceFrame((int)animSprites[i], time)) {
            advanceCount++;
        }
    }
    return advanceCount > 0;
}

void AnimSprites_recount(void) {
    /* gathers all animated sprites from the sprite set. The sprite set collects all unique animated
     * sprites while the viewport is being drawn */
    if (animSprites_recountFlag) {
        size_t items = 0;
        for (size_t i = 0; i < animSprites_set->size && items < animSprites_set->items; i++) {
            if (animSprites_set->values[i] != -1) {
                animSprites[items] = animSprites_set->values[i];
                items++;
            }
        }
        animSprites_size = items;
        animSprites_recountFlag = false;
    }
}

void AnimSprites_addToSet(int tileId) {
    /* adds an animated sprite's tileID to the set */
    if (animSprites_recountFlag) {
        if (tileId != -1) {
            const struct GfxSprite* sprite = GfxTileDb_getSprite(tileId);
            if (sprite->frameCount > 1) {
                IntSet_insert(animSprites_set, tileId);
            }
        }
    }
}

/***************************************************************
 * SCROLLING
 ***************************************************************/

bool eventHandlerScrolling(EngineInputEvent event) {
    /* gets called while the engine is in the SCROLLING state */
    redrawRequested = true;
    return true;
}

void stopScrolling(void) {
    /* switches from SCROLLING to RUNNING state */
    graphicsState = RUNNING;
    animSprites_recountFlag = true;
    calculateMapDrawingVariables(Player_getPosition());
//    calculateSight(Player_getPosition()); // current (target sight)
}

bool updateScrolling(void) {
    /* performs the actual scrolling */
    scrollCount -= (int8_t)scrollIncrementP;
    scrollOffset.x += (scrollDirection.x * scrollIncrementP);
    scrollOffset.y += (scrollDirection.y * scrollIncrementP);
    redrawRequested = true;
    if (scrollCount < 0) {
        scrollCount = 0;
        scrollOffset.x = 0;
        scrollOffset.y = 0;
        stopScrolling();
    }
    updateRunning();
    return true;
}

void startScrolling(GameCommand command) {
    /* starts the scrolling; determines scroll direction based on player's movement */
    switch (command) {
        case GO_NORTH:
            scrollDirection.x=0;
            scrollDirection.y=1;
            scrollOffset.x = 0;
            scrollOffset.y = -(tileSizeP.height + 1);
            scrollCount = (int8_t)tileSizeP.height;
            break;
        case GO_SOUTH:
            scrollDirection.x=0;
            scrollDirection.y=-1;
            scrollOffset.x = 0;
            scrollOffset.y = tileSizeP.height + 1;
            scrollCount = (int8_t)tileSizeP.height;
            break;
        case GO_EAST:
            scrollDirection.x=-1;
            scrollDirection.y=0;
            scrollOffset.x = tileSizeP.width + 1;
            scrollOffset.y = 0;
            scrollCount = (int8_t)tileSizeP.width;
            break;
        case GO_WEST:
            scrollDirection.x=1;
            scrollDirection.y=0;
            scrollOffset.x = -(tileSizeP.width + 1);
            scrollOffset.y = 0;
            scrollCount = (int8_t)tileSizeP.width;
            break;
        case NO_COMMAND:
            break;
    }
    graphicsState = SCROLLING;
    calculateSight(Player_getPosition()); // current (target sight)
    generateFogMap(Player_getPosition());
    calculateMapDrawingVariables(Player_getPosition());
}

void generateFogMap(struct HCoordinate pp) {
    // TODO
    // memcpy fogMap to fogMapOld
    size_t size = sightMapSize.height * 2 * sightMapSize.width;
    memcpy(fogMapOld, fogMap, size * sizeof(int8_t));
    memcpy(bleedMapOld, bleedMap, size * sizeof(int8_t));
    for (size_t i = 0; i < size; i++) {
        fogMap[i] = VISIBLE; // visible
        bleedMap[i] = 0;
    }
    // generate fog of war tile map from game's sightmap
    int mapy = pp.y - sightMapHalfSize.height;
    int mapx = pp.x - sightMapHalfSize.width;
    for (int y = 0; y < sightMapSize.height; y++) {
        for (int x = 0; x < sightMapSize.width; x++) {
            if (getSightMap(x, y) == INVISIBLE) {
                int twoY = y * 2;
                // set sightmap tiles
                if (twoY  - 2 >= 0) {
                    setFogMap(x, twoY - 1, INVISIBLE);
                    setFogMap(x, twoY - 2, INVISIBLE);
                }
                // set "half-tiles"
                int tileId = GameMapManager_getTileId(mapx + x, mapy + y + 1, WALL_LAYER);
                if (tileId != -1) {
                    const struct GameTile *tile = GameTileManager_getTile(tileId);
                    if (!tile->blockSight)
                        setFogMap(x, twoY, INVISIBLE);
                }
            }
        }
    }
    // generate Bleed Tile Map
//    for (int y = 1; y < (viewportSizeT.height * 2) - 1; y++) {
//        for (int x = 1; x < viewportSizeT.width - 1; x++) {
//            if (getFogMap(x, y) == 1) {
//                int sumC = 0; // check cardinals
//                sumC += (1 - getFogMap(x, y - 1)) * 2;  // TODO can we remove the 1 - ?
//                sumC += (1 - getFogMap(x, y + 1)) * 34;
//                sumC += (1 - getFogMap(x - 1, y)) * 5;
//                sumC += (1 - getFogMap(x + 1, y)) * 13;
//                switch (sumC) {
//                    case 20: setFogMap(x, y, 0); break; // handle an edge case regarding bottom L bleed tiles
//                    case  7: setBleedMap(x, y, L_BL); break; // l shape - b L bottomleft 1.png
//                    case 39: setBleedMap(x, y, L_TL); break; // l shape - b L topleft 1.png
//                    case 47: setBleedMap(x, y, L_TR); break; // l shape - b L topright 1.png
//                    case 15: setBleedMap(x, y, L_BR); break; // l shape - b L bottomright 1.png
//                    case  5: setBleedMap(x, y, T_L); break; // left    - b right 1.png
//                    case 13: setBleedMap(x, y, T_R); break; // right   - b left 1.png
//                    case  2: setBleedMap(x, y, T_B); break; // top     - b bottom 1.png
//                    case 34: setBleedMap(x, y, T_T); break; // bottom  - b top 1.png
//                    case 18: setBleedMap(x, y, U_T); break; // - U shaped tile top
//                    case 52: setBleedMap(x, y, U_B); break; // - U shaped tile bottom
//                }
//                if (!(sumC == 0 || sumC == 5 || sumC == 13))
//                    continue;
//                int sumD = 0; // check diagonals & left-right
//                sumD += (1 - getFogMap(x - 1, y - 1)); // 1
//                sumD += (1 - getFogMap(x - 1, y + 1)) * 21;
//                sumD += (1 - getFogMap(x + 1, y - 1)) * 3;
//                sumD += (1 - getFogMap(x + 1, y + 1)) * 55;
//                sumD += (1 - getFogMap(x - 1, y)) * 5;
//                sumD += (1 - getFogMap(x + 1, y)) * 13;
//                switch (sumD) {
//                    case  1: setBleedMap(x, y, C_BR); break;  // corner - b corner bottomright 1.png
//                    case 21: setBleedMap(x, y, C_TR); break;  // corner - b corner topright 1.png
//                    case  3: setBleedMap(x, y, C_BL); break;  // corner - b corner bottomleft 1.png
//                    case 55: setBleedMap(x, y, C_TL); break;  // corner - b corner topleft 1.png
//                    case 76: setBleedMap(x, y, C_TLTR); break; // corner - b U3 1.png (topleft & topright)
//                    case 82: setBleedMap(x, y, C_X); break;   // sumC == 5
//                    case 92: setBleedMap(x, y, C_Y); break;   // sumC == 13
//                }
//            }
//        }
//    }
}

/***************************************************************
 * VIEWPORT DRAWING
 ***************************************************************/

void calculateMapDrawingVariables(struct HCoordinate pp) {
    /* pre-calculates some variables to speed up loops in the tile drawing passes */
    Engine_log("CALCULATE MAP DRAWING VARIABLES");
    mapDrawStart.x = pp.x - viewportHalfSizeT.width;
    mapDrawStart.y = pp.y - viewportHalfSizeT.height;
    mapDrawEnd.x = viewportSizeT.width + mapDrawStart.x;
    mapDrawEnd.y = viewportSizeT.height + mapDrawStart.y;
    rightScrollAdjust.width = viewportOffsetP.width;
    rightScrollAdjust.height = viewportOffsetP.height;
    fogScrollAdjust.width = 0;
    fogScrollAdjust.height = 0;
    if (graphicsState == SCROLLING) {
        if (scrollDirection.x > 0) {
            mapDrawEnd.x += 1;
            fogScrollAdjust.width = -1;
        }
        else if (scrollDirection.x < 0) {
            mapDrawStart.x -= 1;
            mapDrawEnd.x += 1;
            rightScrollAdjust.width -= 32;
            fogScrollAdjust.width = 1;
        }
        if (scrollDirection.y > 0) {
            mapDrawEnd.y += 1;
            fogScrollAdjust.height = -2;
        }
        else if (scrollDirection.y < 0) {
            mapDrawStart.y -= 1;
            mapDrawEnd.y += 1;
            rightScrollAdjust.height -= 32;
            fogScrollAdjust.height = 2;
        }
    }
}

void updateDrawFirstPass(void) {
    // TODO - skip drawing of invisible tiles (but not under tiles which are fading in or out - how can we determine these?)
    /* draw floor tiles and shadow */
    int tileId;
    for (int y = mapDrawStart.y, iy = rightScrollAdjust.height; y < mapDrawEnd.y; y++, iy += tileSizeP.height) {
        for (int x = mapDrawStart.x, ix = rightScrollAdjust.width; x < mapDrawEnd.x; x++, ix += tileSizeP.width) {
            // draw floor
            tileId = GameMapManager_getTileId(x, y, FLOOR_LAYER);
            drawTile(tileId, ix + scrollOffset.x, iy + scrollOffset.y, -1); //CURRENT_FRAME);
            AnimSprites_addToSet(tileId);
            // draw shadow
            tileId = GameMapManager_getTileId(x, y, SHADOW_LAYER);
            if (tileId != 0) {
                drawTile(tileId, ix + scrollOffset.x, iy + scrollOffset.y, -1); //CURRENT_FRAME);
                AnimSprites_addToSet(tileId);
            }
        }
    }
}

void updateDrawSecondPass(void) {
    // TODO - skip drawing of invisible tiles
    /* draws walls */
    int tileId;
    for (int y = mapDrawStart.y, iy = rightScrollAdjust.height; y < mapDrawEnd.y; y++, iy += tileSizeP.height) {
        for (int x = mapDrawStart.x, ix = rightScrollAdjust.width; x < mapDrawEnd.x; x++, ix += tileSizeP.width) {
            tileId = GameMapManager_getTileId(x, y, WALL_LAYER);
            if (tileId != 0) {
                drawTile(tileId, ix + scrollOffset.x, iy + scrollOffset.y, -1); //CURRENT_FRAME);
                AnimSprites_addToSet(tileId);
            }
        }
    }
}

void updateDrawThirdPass(void) {
    /* draw fog of war */
    // DRAW FOGMAP for next frame
    // TODO draw non-scrolling frames separately

    int frame = 0;
    int frameOut = 5; // TODO set this as constant = number of frames for fog tiles
    if (graphicsState == SCROLLING) {
        frame = (scrollCount * 5 / 32);
        frameOut = 5 - frame;
    }

    for (int mapy = 0, y = viewportOffsetP.height; mapy < viewportSizeT.height * 2 + 2; mapy++, y += 16) {
        for (int mapx = 0, x = viewportOffsetP.width; mapx < viewportSizeT.width + 2; mapx++, x += tileSizeP.width) {
//            int bleedTileOld = getBleedMapOld(mapx + fogScrollAdjust.width, mapy + fogScrollAdjust.height);
//            int bleedTile = getBleedMap(mapx, mapy);
            // fog
            int drawx = x + scrollOffset.x - 32; // 1 x 32 = border
            int drawy = y + scrollOffset.y - 32;
            int fogTileOld = 1; // TODO which value?!

            if (mapy + fogScrollAdjust.height < (sightMapSize.height * 2) &&
                    mapy + fogScrollAdjust.height > 0 &&
                    mapx + fogScrollAdjust.width < sightMapSize.width &&
                    mapx + fogScrollAdjust.width > 0)
                fogTileOld = getFogMapOld(mapx + fogScrollAdjust.width, mapy + fogScrollAdjust.height);
            int fogTile = getFogMap(mapx, mapy);


            if (fogTileOld == 0 && fogTile == 0) {  // no change
                drawTile(B_CTR, drawx, drawy, 0);
            }
            else if (fogTileOld == 1 && fogTile == 0) {
                drawTile(B_CTR, drawx, drawy, frame);
            }
            else if (fogTileOld == 0 && fogTile == 1) {
                if (frameOut < 5)
                    drawTile(B_CTR, drawx, drawy, frameOut);
            }





            // bleed
//            if (bleedTileOld == bleedTile && bleedTile != EMPTY) { // no change
//                drawTile(bleedTile, ix, iy, 0);
//            } else if (bleedTileOld == EMPTY && bleedTile != EMPTY) { // fade in
//                if (fogTileOld == INVISIBLE) // TODO
//                    drawTile(bleedTile, ix, iy, 0);
//                else
//                    drawTile(bleedTile, ix, iy, frame);
//            } else if (bleedTileOld != EMPTY && bleedTile == EMPTY) { // fade out
//                if (fogTile == INVISIBLE)
//                    drawTile(bleedTileOld, ix, iy, 0);
//                else if (frameOut < 5)
//                    drawTile(bleedTileOld, ix, iy, frameOut); // fr_out
//            } else if (bleedTileOld != EMPTY) { // cross-fade
//                if (bleedTile == C_BL || bleedTile == C_BR ||
//                    bleedTile == C_TL || bleedTile == C_TR) { // L-tile
//                    drawTile(bleedTile, ix, iy, 0);
//                    if (frameOut < 5)
//                        drawTile(bleedTileOld, ix, iy, frameOut);
//                } else if (bleedTileOld == C_BL || bleedTileOld == C_BR ||
//                           bleedTileOld == C_TL || bleedTileOld == C_TR) {
//                    drawTile(bleedTile, ix, iy, frame);
//                    drawTile(bleedTileOld, ix, iy, 0);
//                } else if (bleedTile == U_T || bleedTile == U_B) {
//                    drawTile(bleedTile, ix, iy, frame);
//                    drawTile(bleedTileOld, ix, iy, 0);
//                } else if (bleedTileOld == U_T || bleedTileOld == U_B) {
//                    drawTile(bleedTile, ix, iy, 0);
//                    if (frameOut < 5)
//                        drawTile(bleedTileOld, ix, iy, frameOut);
//                } else if (bleedTile == L_BL || bleedTile == L_TL ||
//                           bleedTile == L_TR || bleedTile == L_BR) { // corner tile
//                    drawTile(bleedTile, ix, iy, frame);
//                    drawTile(bleedTileOld, ix, iy, 0);
//                } else if (bleedTileOld == L_BL || bleedTileOld == L_TL ||
//                           bleedTileOld == L_TR || bleedTileOld == L_BR) {
//                    drawTile(bleedTile, ix, iy, 0);
//                    if (frameOut < 5)
//                        drawTile(bleedTileOld, ix, iy, frameOut);
//                }
//            }



//            else {
//                if (getFogMap(x, y) == INVISIBLE) // draw fog tile
//                    drawTile(B_CTR, ix, iy, 0);
//                if (bleedTile != EMPTY)   // draw bleed tile
//                    drawTile(bleedTile, ix, iy, 0);
//            }
        }
    }
}

void drawTile(int tileId, int x, int y, int currentFrame) {
    /* draws a single tile */
#ifdef DEBUG
    int outOfBounds = 0;
#endif
    int tileHeight = tileSizeP.height; // TODO
    if (tileId != -1 && x + tileSizeP.width >= 0 && y + tileSizeP.height >= 0 && x < screenSizeP.width && y < screenSizeP.height) {
        const struct EngineBitmap* bitmap;
        if (currentFrame == -1)
            bitmap = (struct EngineBitmap*) GfxSprite_getBitmap(GfxTileDb_getSprite(tileId));
        else
            bitmap = (struct EngineBitmap*)GfxSprite_getFrame(GfxTileDb_getSprite(tileId), currentFrame);
        int oy = bitmap->height - tileHeight;
        Engine_drawBitmap(bitmap, x, y - oy);
#ifdef DEBUG
        if (x >= screenSizeP.width || y >= screenSizeP.height || x+tileSizeP.width < 0 || y+tileSizeP.height < 0) {
            Engine_logError("OUT OF BOUNDS: %i,%i - %i,%i\n", x, y, x + 32, y + 32);
            outOfBounds++;
        }
#endif
    }
#ifdef DEBUG
    if (outOfBounds > 0)
        Engine_log("OUT OF BOUNDS TILES: %i\n", outOfBounds);
#endif
}
