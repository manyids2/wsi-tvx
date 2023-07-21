#include "tiles.h"
#include "constants.h"

void tiles_init(tiles_t *tiles) {
  tiles->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  if (pthread_mutex_init(tiles->mutex, NULL))
    die("LRU Cache unable to initialise mutex");
}

void tiles_free(tiles_t *tiles) {
  if (tiles->mutex) {
    if (pthread_mutex_destroy(tiles->mutex)) {
      die("LRU Cache unable to destroy mutex");
    }
  }
}

uint32_t tile_load(tiles_t *tiles, int level, int left, int top) {
  // Check all tiles
  tile_t tile;
  for (int index = 0; index < MAX_TILE_CACHE; index++) {
    tile = tiles->tiles[index];
    if (tile.kitty_id > 0) {
      if ((tile.level == level) && (tile.si == left) && (tile.sj == top)) {
        // Already loaded, so return index
        return index;
      }
    }
  }

  // Not found, so start loading
  int64_t sx = left * TILE_SIZE;
  int64_t sy = top * TILE_SIZE;

  // Read from proper level and position
  openslide_read_region(tiles->osr, tiles->buf, sx, sy, level, TILE_SIZE,
                        TILE_SIZE);
  assert(openslide_get_error(tiles->osr) == NULL);

  // Base64 encode it
  RGBAtoRGBbase64(TILE_SIZE * TILE_SIZE, tiles->buf, tiles->buf64);

  // Send to kitty
  uint32_t kitty_id = 1000;
  tile_provision(kitty_id, TILE_SIZE, TILE_SIZE, tiles->buf64);
  return kitty_id;
}

void tile_provision(uint32_t kitty_id, int w, int h, char *buf64) {
  /*
   * Switch to RGB to get gains on base64 encoding
   *
   * <ESC>_Gf=24,s=<w>,v=<h>,m=1;<encoded pixel data first chunk><ESC>\
   * <ESC>_Gm=1;<encoded pixel data second chunk><ESC>\
   * <ESC>_Gm=0;<encoded pixel data last chunk><ESC>\
   */
  size_t sent_bytes = 0;
  size_t base64_size = w * h * sizeof(uint32_t);
  while (sent_bytes < base64_size) {
    size_t chunk_size =
        base64_size - sent_bytes < CHUNK ? base64_size - sent_bytes : CHUNK;
    int cont = !!(sent_bytes + chunk_size < base64_size);
    if (sent_bytes == 0) {
      fprintf(stdout, "\x1B_Gt=d,f=24,q=2,i=%u,s=%d,v=%d,m=%d%s;", kitty_id, w,
              h, cont, "");
    } else {
      fprintf(stdout, "\x1B_Gm=%d;", cont);
    }
    fwrite(buf64 + sent_bytes, chunk_size, 1, stdout);
    fprintf(stdout, "\x1B\\");
    sent_bytes += chunk_size;
  }
  fflush(stdout);
}

void tile_display(uint32_t kitty_id, int row, int col, int X, int Y, int Z) {
  char s[64];
  int len =
      snprintf(s, sizeof(s), "\x1b_Ga=p,i=%d,q=2,X=%d,Y=%d,C=1,z=%d;\x1b\\",
               kitty_id, X, Y, Z);
  move_cursor(row, col);
  write_or_die(s, len, "tile_display");
}

void tile_clear(uint32_t kitty_id) {
  char s[64];
  int len = snprintf(s, sizeof(s), "\x1b_Ga=d,d=i,i=%d;\x1b\\", kitty_id);
  write_or_die("\x1b_Ga=d,d=i,i=%d;\x1b\\", len, "tile_clear");
}

void tile_delete(uint32_t kitty_id) {
  char s[64];
  int len = snprintf(s, sizeof(s), "\x1b_Ga=d,d=I,i=%d;\x1b\\", kitty_id);
  write_or_die("\x1b_Ga=d,d=I,i=%d;\x1b\\", len, "tile_delete");
}
