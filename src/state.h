#pragma once

#include "constants.h"
#include <bits/types/FILE.h>
#include <openslide/openslide.h>
#include <pthread.h>

// ---  Slide  ---
typedef struct slide_t {
  openslide_t *osr;

  int level_count;
  int64_t level_w[MAX_LEVELS];
  int64_t level_h[MAX_LEVELS];
  float downsamples[MAX_LEVELS];

  int has_thumbnail;
  uint32_t *thumbnail;
  int64_t thumbnail_w, thumbnail_h;

  char slidepath[1024];
} slide_t;

// ---  View  ---
// Position on screen ( params for draw image/text )
typedef struct pos_t {
  int col, row, X, Y; // Position in pixels to pass to kitty
} pos_t;

// Remains constant upto resize, new slide
typedef struct world_t {
  int64_t ww, wh; // Slide at max zoom in pixels
  int fvw, fvh;   // Full viewport in pixels
  int vw, vh;     // Viewport with tiles
  int rows, cols; // Viewport dims in rows, cols
  int cw, ch;     // Character dims in pixels
  int ox, oy;     // Offset in pixels
  int vmi, vmj;   // Maximum tiles in view
  int mlevel;     // Maximum level ( = level_count - 1 )
  pos_t pos[MAX_ROWS * MAX_COLS];
} world_t;

// Remains constant upto level change or move
typedef struct view_t {
  int level;      // Level of slide
  float zoom;     // Downsample factor from max zoom
  int left, top;  // Coords wrt slide
  int64_t sw, sh; // Slide level dims
  int64_t sx, sy; // Slide level position
  int64_t wx, wy; // World level position
  int smi, smj;   // Maximum tiles in slide at level
  int vmi, vmj;   // Maximum tiles in view, copied from world
} view_t;

// ---  Tile  ---
// Tile from slide ( params for load image )
typedef struct tile_t {
  int si, sj, level; // Identity of tile
  int vi, vj;        // Position wrt view ( can be negative )
  int32_t kitty_id;  // ID used by kitty, used to indicate if tile is loaded
  int32_t freq;      // Frequency with which it was requested
} tile_t;

typedef struct tiles_t {
  pthread_mutex_t *mutex;
  int current;
  tile_t tiles[MAX_TILE_CACHE];              // Loaded tiles and kitty_id
  uint32_t buf[TILE_SIZE * TILE_SIZE];       // Buffer for openslide
  char buf64[TILE_SIZE * TILE_SIZE * 4 + 1]; // Buffer for kitty
} tiles_t;

typedef struct thread_info {
  pthread_t thread_id;
  int thread_num;
  tile_t *tile;
} thread_info;

// -- Wrap it all up ---
typedef struct app_t {
  // world
  slide_t *slide;
  world_t *world;
  view_t *view;
  tiles_t *tiles;
  // ui
  int debug;
  int thumb;
  FILE *logfile;
  enum keys_e last_pressed;
} app_t;
