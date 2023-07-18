#include "constants.h"
#include <assert.h>
#include <openslide/openslide.h>
#include <stdlib.h>
#include <string.h>

typedef struct OpenSlide {
  openslide_t *osr;

  int level_count;
  int64_t level_w[MAX_LEVELS];
  int64_t level_h[MAX_LEVELS];
  float downsamples[MAX_LEVELS];

  int has_thumbnail;
  uint32_t *thumbnail;
  int64_t thumbnail_w, thumbnail_h;
} OpenSlide;

// Constructor and destructor
void slideInit(OpenSlide *S, char *slide);
void slideFree(OpenSlide *S);

// Get (i, j)th tile from level (l), given tile size (ts)
void slideGetTile(OpenSlide *S, uint32_t *buf, int l, int i, int j, int ts);
