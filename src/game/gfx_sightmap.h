#ifndef GFX_SIGHTMAP_H
#define GFX_SIGHTMAP_H

#include "helpers/h_coordinate.h"

void SightMap_init(void);
void calculateSight(struct HCoordinate pp);
int getSightMap(int x, int y);
void SightMap_delete(void);

#endif