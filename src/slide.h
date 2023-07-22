#include "base64.h"
#include "constants.h"
#include "kitty.h"
#include "state.h"
#include <assert.h>
#include <error.h>
#include <openslide/openslide.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constructor and destructor
void slide_init(slide_t *slide, char *slidepath);
void slide_free(slide_t *slide);

// Read associated images to load thumbnail
void slide_load_thumbnail(slide_t *slide);
void slide_provision_thumbnail(slide_t *slide);
void slide_toggle_thumbnail(slide_t *slide, world_t *world, int onoff);

// Get (i, j)th tile from level (l), given tile size (ts)
void slide_get_tile(slide_t *slide, uint32_t *buf, int l, int i, int j);
