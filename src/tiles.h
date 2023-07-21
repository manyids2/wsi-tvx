#include "base64.h"
#include "slide.h"
#include "state.h"
#include "term.h"
#include <stdio.h>
#include <stdlib.h>

void tiles_init(tiles_t *tiles);
void tiles_free(tiles_t *tiles);

void tiles_set_tile(tiles_t *tiles, int index, int vi, int vj);
void tiles_set_visible(tiles_t *tiles);

uint32_t tile_load(tiles_t *tiles, int level, int left, int top);
void tile_provision(uint32_t kitty_id, int w, int h, char *buf64);
void tile_display(uint32_t kitty_id, int row, int col, int X, int Y, int Z);
void tile_clear(uint32_t kitty_id);
void tile_delete(uint32_t kitty_id);
