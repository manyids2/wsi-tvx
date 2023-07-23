#include "../src/slide.h"
#include "../src/tiles.h"
#include <stdlib.h>

void test_load_tile(slide_t *slide, int level, int left, int top, uint32_t *buf,
                    char *buf64) {
  // Not found, so start loading
  double zoom = slide->downsamples[level];
  int64_t sx = (left * TILE_SIZE) * zoom;
  int64_t sy = (top * TILE_SIZE) * zoom;

  // Read from proper level and position
  openslide_read_region(slide->osr, buf, sx, sy, level, TILE_SIZE, TILE_SIZE);
  assert(openslide_get_error(slide->osr) == NULL);

  // Base64 encode it
  RGBAtoRGBbase64(TILE_SIZE * TILE_SIZE, buf, buf64);

  // BUG: Send to kitty -> what happens if it fails?
  kitty_provision(KITTY_ID_OFFSET, TILE_SIZE, TILE_SIZE, buf64);
  kitty_display(KITTY_ID_OFFSET, 5, 5, 0, 0, 1);

  move_cursor(2, 1);
  printf("%5d, %5d, %5d", level, left, top);

  // kitty_clear(KITTY_ID_OFFSET);
}

slide_t slide = {0};

int main(void) {
  // Read slide
  char *slidepath = "/data/slides/aperio/CMU-1-JP2K-33005.svs";
  slide_init(&slide, slidepath);

  // Setup term
  enable_raw_mode();
  clear_screen();
  hide_cursor();

  // Allocate buffers for tile and kitty
  uint32_t *buf = malloc(TILE_SIZE * TILE_SIZE * sizeof(uint32_t));
  char *buf64 = malloc(TILE_SIZE * TILE_SIZE * 4 * sizeof(char) + 1);

  // Get tiles to cycle over
  int level = slide.level_count - 2;
  int smi = slide.level_w[level] / TILE_SIZE;
  int smj = slide.level_h[level] / TILE_SIZE;
  printf("Tiles: %d\r\n", smi * smj);

  // Load, provision, dispay, clear
  for (int i = 0; i < smi; i++) {
    for (int j = 0; j < smj; j++) {
      test_load_tile(&slide, level, i, j, buf, buf64);
    }
  }

  // Free mem
  slide_free(&slide);
  free(buf);
  free(buf64);

  clear_screen();
  return EXIT_SUCCESS;
}

/* 1408_tiles => 12 / 1408 ~ 12 / 1200 = .01 sec per tile
 * ____________________________________________________
 * Executed in   13.80 secs    fish           external
 *    usr time   11.70 secs  590.00 micros   11.69 secs
 *    sys time   1.02 secs   170.00 micros   1.02 secs
 */
