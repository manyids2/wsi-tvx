#include "slide.h"

void slide_init(slide_t *slide, char *slidepath) {
  // Save slidename
  strncpy(slide->slidepath, slidepath, sizeof(slide->slidepath));

  // Open ( exit if error )
  openslide_t *osr = openslide_open(slidepath);
  if (osr == NULL || openslide_get_error(osr) != NULL)
    die("No such file found\r\n");
  slide->osr = osr;

  // Get count of levels in wsi pyramid
  slide->level_count = openslide_get_level_count(osr);
  assert(slide->level_count < MAX_LEVELS);

  // Store downsamples ( zoom ), level_w, level_h
  for (int32_t level = 0; level < openslide_get_level_count(osr); level++) {
    slide->downsamples[level] = openslide_get_level_downsample(osr, level);
    openslide_get_level_dimensions(osr, level, &slide->level_w[level],
                                   &slide->level_h[level]);
  }
}

void slide_load_thumbnail(slide_t *slide) {
  const char *const *associated_image_names =
      openslide_get_associated_image_names(slide->osr);

  while (*associated_image_names) {
    int64_t w, h;
    const char *name = *associated_image_names;
    if (!strcmp(name, "thumbnail\0")) {
      // Get size of thumbnail
      openslide_get_associated_image_dimensions(slide->osr, name, &w, &h);

      // Allocate
      slide->has_thumbnail = 1;
      slide->thumbnail_w = w;
      slide->thumbnail_h = h;
      slide->thumbnail = malloc(h * w * sizeof(uint32_t));

      // Read thumbnail
      openslide_read_associated_image(slide->osr, name, slide->thumbnail);
    }
    associated_image_names++;
  }

  // NOTE: Putting empty pic as thumbnail
  if (slide->has_thumbnail == 0) {
    slide->thumbnail_h = MIN_PIXELS;
    slide->thumbnail_w = MIN_PIXELS;
    slide->thumbnail = calloc(MIN_PIXELS * MIN_PIXELS, sizeof(uint32_t));
  }
}

void slide_get_tile(slide_t *slide, uint32_t *buf, int l, int i, int j) {
  openslide_read_region(slide->osr, buf, i * TILE_SIZE, j * TILE_SIZE, l,
                        TILE_SIZE, TILE_SIZE);
  assert(!openslide_get_error(slide->osr));
}

void slide_free(slide_t *slide) {
  openslide_close(slide->osr);

  // Free only if thumbnail was already allocated
  if (slide->thumbnail_w > 0)
    free(slide->thumbnail);
}
