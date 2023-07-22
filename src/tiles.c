#include "tiles.h"
#include "constants.h"
#include "kitty.h"
#include "term.h"
#include <stdio.h>

void tiles_init(tiles_t *tiles) {
  tiles->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  if (pthread_mutex_init(tiles->mutex, NULL))
    die("LRU Cache unable to initialise mutex");

  tiles->current = 0;
  for (int i = 0; i < MAX_TILE_CACHE; i++) {
    tiles->tiles[i].kitty_id = 0;
  }
}

void tiles_free(tiles_t *tiles) {
  if (tiles->mutex) {
    if (pthread_mutex_destroy(tiles->mutex)) {
      die("LRU Cache unable to destroy mutex");
    }
  }
}

void tiles_clear(tiles_t *tiles) {
  for (int i = 0; i < MAX_TILE_CACHE; i++) {
    if (tiles->tiles[i].kitty_id >= KITTY_ID_OFFSET) {
      kitty_clear(tiles->tiles[i].kitty_id);
    }
  }
}

void tiles_log(tiles_t *tiles, view_t *view, world_t *world, FILE *logfile) {
  fprintf(logfile, "----view-----\n");

  int level = view->level;
  int left = view->left;
  int top = view->top;
  int vmi = world->vmi;
  int vmj = world->vmj;
  fprintf(logfile,
          "    level: %d\n"
          "left, top: %d, %d\n"
          " vmi, vmj: %d, %d\n"
          "",
          level, left, top, vmi, vmj);

  fprintf(logfile, "----visible-----\n");
  int index, si, sj;
  for (int i = 0; i < vmi; i++) {
    for (int j = 0; j < vmj; j++) {
      // Checks if loaded, else loads
      si = i + left;
      sj = j + top;
      index = tile_get(tiles, level, si, sj);
      if (index != -1) {
        uint32_t kitty_id = tiles->tiles[index].kitty_id;
        fprintf(logfile, "%3d, %3d: %6u : %4d, %3d, %3d \n", i, j, kitty_id,
                index, si, sj);
      }
    }
  }

  fprintf(logfile, "----cache-----\n");
  for (int i = 0; i < MAX_TILE_CACHE; i++) {
    tile_t t = tiles->tiles[i];
    if (t.kitty_id > 0)
      fprintf(logfile,
              "%3d: %6u : "
              "  %3d, %3d, %3d \n",
              i, t.kitty_id, t.level, t.si, t.sj);
  }
}

void tiles_load_view(tiles_t *tiles, slide_t *slide, view_t *view,
                     world_t *world) {
  // First clear all tiles
  tiles_clear(tiles);

  int level = view->level;
  int left = view->left;
  int top = view->top;
  int smi = view->smi;
  int smj = view->smj;
  int vmi = world->vmi;
  int vmj = world->vmj;
  int index, si, sj;

  // First display what is already there
  for (int i = 0; i < vmi; i++) {
    for (int j = 0; j < vmj; j++) {
      pos_t pos = world->pos[i * vmj + j];
      // Checks if loaded, else loads
      si = i + left;
      sj = j + top;
      index = tile_get(tiles, level, si, sj);
      if (index >= 0) {
        // Get kitty id and display in grid
        uint32_t kitty_id = tiles->tiles[index].kitty_id;
        kitty_display(kitty_id, pos.row, pos.col, pos.X, pos.Y, -2);
      }
    }
  }

  // Then handle requests
  for (int i = 0; i < vmi; i++) {
    for (int j = 0; j < vmj; j++) {
      pos_t pos = world->pos[i * vmj + j];
      // Checks if loaded, else loads
      si = i + left;
      sj = j + top;
      index = tile_get(tiles, level, si, sj);
      if (index == -1) {
        index = tile_load(tiles, slide->osr, view->zoom, level, si, sj);
        // Get kitty id and display in grid
        uint32_t kitty_id = tiles->tiles[index].kitty_id;
        kitty_display(kitty_id, pos.row, pos.col, pos.X, pos.Y, -2);
      }
    }
  }

  // Then handle cache - left and right
  for (int i = 0; i <= MARGIN_CACHE; i++) {
    for (int j = 0; j < vmj; j++) {
      sj = top + j;

      // left border
      si = MAX(left - i, 0);
      index = tile_get(tiles, level, si, sj);
      if (index == -1) {
        index = tile_load(tiles, slide->osr, view->zoom, level, si, sj);
      }

      // right border
      si = MIN(left + vmi + i, smi - 1);
      index = tile_get(tiles, level, si, sj);
      if (index == -1) {
        index = tile_load(tiles, slide->osr, view->zoom, level, si, sj);
      }
    }
  }

  // Then handle cache - top and bottom
  for (int i = 0; i < vmi; i++) {
    for (int j = 0; j <= MARGIN_CACHE; j++) {
      si = left + i;

      // top border
      sj = MAX(top - j, 0);
      index = tile_get(tiles, level, si, sj);
      if (index == -1) {
        index = tile_load(tiles, slide->osr, view->zoom, level, si, sj);
      }

      // bottom border
      sj = MIN(top + vmj + j, smj - 1);
      index = tile_get(tiles, level, si, sj);
      if (index == -1) {
        index = tile_load(tiles, slide->osr, view->zoom, level, si, sj);
      }
    }
  }
}

int tile_get(tiles_t *tiles, int level, int left, int top) {
  // Check all tiles
  tile_t tile;
  for (int index = 0; index < MAX_TILE_CACHE; index++) {
    tile = tiles->tiles[index];
    if ((tile.kitty_id >= KITTY_ID_OFFSET) && (tile.level == level) &&
        (tile.si == left) && (tile.sj == top)) {
      // Already loaded, so return index
      tile.freq += 1;
      return index;
    }
  }
  return -1;
}

int tile_load(tiles_t *tiles, openslide_t *osr, double zoom, int level,
              int left, int top) {
  // FIFO kind of cache
  int current = (tiles->current + 1) % MAX_TILE_CACHE;
  tiles->current = current;

  // // Check if it was already loaded, delete before reprovision
  // if (tile.kitty_id >= KITTY_ID_OFFSET) {
  //   kitty_delete(tile.kitty_id);
  // }

  // Not found, so start loading
  int64_t sx = (left * TILE_SIZE) * zoom;
  int64_t sy = (top * TILE_SIZE) * zoom;

  // Read from proper level and position
  openslide_read_region(osr, tiles->buf, sx, sy, level, TILE_SIZE, TILE_SIZE);
  assert(openslide_get_error(osr) == NULL);

  // Base64 encode it
  RGBAtoRGBbase64(TILE_SIZE * TILE_SIZE, tiles->buf, tiles->buf64);

  // Send to kitty
  uint32_t kitty_id = current + KITTY_ID_OFFSET;
  kitty_provision(kitty_id, TILE_SIZE, TILE_SIZE, tiles->buf64);

  // Register tile
  tile_t *tile = &tiles->tiles[current];
  tile->kitty_id = kitty_id;
  tile->level = level;
  tile->si = left;
  tile->sj = top;
  tile->vi = 0; // TODO: Use these
  tile->vj = 0;
  tile->freq = 1;
  return current;
}
