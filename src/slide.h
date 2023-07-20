#include "state.h"
#include <assert.h>
#include <openslide/openslide.h>
#include <stdlib.h>
#include <string.h>

// Constructor and destructor
void slide_init(slide_t *slide, char *slidepath);
void slide_free(slide_t *slide);

// Read associated images to load thumbnail
void slide_load_thumbnail(slide_t *slide);

// Get (i, j)th tile from level (l), given tile size (ts)
void slide_get_tile(slide_t *slide, uint32_t *buf, int l, int i, int j);
