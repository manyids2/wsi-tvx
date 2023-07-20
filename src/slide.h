#include "state.h"
#include <assert.h>
#include <openslide/openslide.h>
#include <stdlib.h>
#include <string.h>

// Constructor and destructor
void slide_init(slide_t *S, char *slide);
void slide_free(slide_t *S);

// Get (i, j)th tile from level (l), given tile size (ts)
void slide_get_tile(slide_t *S, uint32_t *buf, int l, int i, int j, int ts);
