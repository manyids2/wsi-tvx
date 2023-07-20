#include "slide.h"

void slide_init(slide_t *S, char *slide) {
  // Open ( exit if error )
  openslide_t *osr = openslide_open(slide);
  assert(osr != NULL && openslide_get_error(osr) == NULL);
  S->osr = osr;

  // Get count of levels in wsi pyramid
  S->level_count = openslide_get_level_count(osr);
  assert(S->level_count < MAX_LEVELS);

  // Store downsamples ( scale factors ), level_w, level_h
  for (int32_t level = 0; level < openslide_get_level_count(osr); level++) {
    S->downsamples[level] = openslide_get_level_downsample(osr, level);
    openslide_get_level_dimensions(osr, level, &S->level_w[level],
                                   &S->level_h[level]);
  }
}

void slide_load_thumbnail(slide_t *S) {
  // Get associated images
  const char *const *associated_image_names =
      openslide_get_associated_image_names(S->osr);

  S->has_thumbnail = 0;
  while (*associated_image_names) {
    int64_t w, h;
    const char *name = *associated_image_names;
    if (!strcmp(name, "thumbnail\0")) {
      // Get size of thumbnail
      openslide_get_associated_image_dimensions(S->osr, name, &w, &h);

      // Allocate
      S->has_thumbnail = 1;
      S->thumbnail_w = w;
      S->thumbnail_h = h;
      S->thumbnail = malloc(h * w * sizeof(uint32_t));

      // Read thumbnail
      openslide_read_associated_image(S->osr, name, S->thumbnail);
    }
    associated_image_names++;
  }

  // NOTE: Putting empty pic as thumbnail
  if (S->has_thumbnail == 0) {
    S->thumbnail_h = MIN_PIXELS;
    S->thumbnail_w = MIN_PIXELS;
    S->thumbnail = calloc(MIN_PIXELS * MIN_PIXELS, sizeof(uint32_t));
  }
}

void slide_get_tile(slide_t *S, uint32_t *buf, int l, int i, int j, int ts) {
  openslide_read_region(S->osr, buf, i * ts, j * ts, l, ts, ts);
  assert(!openslide_get_error(S->osr));
}

void slide_free(slide_t *S) {
  openslide_close(S->osr);

  // Free only if thumbnail was already allocated
  if (S->thumbnail_w > 0)
    free(S->thumbnail);
}
