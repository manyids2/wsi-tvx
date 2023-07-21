#include "constants.h"
#include "state.h"
#include <stdio.h>
#include <stdlib.h>

void tiles_init(tiles_t *tiles);
void tiles_free(tiles_t *tiles);

void tiles_load_tiles(tiles_t *tiles, int level, int left, int top, int right,
                      int bottom);
void tiles_set_tile(tiles_t *tiles, int index, int vi, int vj);
void tiles_set_visible(tiles_t *tiles);
