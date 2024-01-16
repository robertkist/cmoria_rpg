#ifndef CONFIG_H
#define CONFIG_H

#include "helpers/h_coordinate.h"

// Postfixes for measurement units:
// P .. pixel
// T .. tiles

extern struct HCoordinate tileSizeP;
extern struct HCoordinate screenSizeP;
extern struct HCoordinate viewportSizeT;
extern const int gfxScaleFactor;
extern const int gameFps;
extern const int scrollIncrementP;

#endif