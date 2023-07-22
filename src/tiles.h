#include "base64.h"
#include "slide.h"
#include "state.h"
#include "term.h"
#include <stdio.h>
#include <stdlib.h>

void tiles_init(tiles_t *tiles);
void tiles_free(tiles_t *tiles);

void tiles_clear(tiles_t *tiles);
void tiles_log(tiles_t *tiles, view_t *view, world_t *world, FILE *logfile);
void tiles_load_view(tiles_t *tiles, slide_t *slide, view_t *view,
                     world_t *world);

int tile_get(tiles_t *tiles, int level, int left, int top);
int tile_load(tiles_t *tiles, openslide_t *osr, double zoom, int level,
              int left, int top);
