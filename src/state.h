#pragma once

#include "constants.h"
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
} slide_t;

// ---  View  ---
// Remains constant upto resize, new slide
typedef struct world_t {
  int64_t ww, wh; // Slide at max zoom in pixels
  int vw, vh;     // Viewport in pixels
  int rows, cols; // Viewport dims in rows, cols
  int cw, ch;     // Character dims in pixels
  int ox, oy;     // Offset in pixels
  int vmi, vmj;   // Maximum tiles in view
  int mlevel;     // Maximum level ( = level_count - 1 )
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
  int ol, oi, oj; // Cache offset to level, view
} view_t;

// ---  Tile  ---
// Tile from slide ( params for load image )
typedef struct tile_t {
  int si, sj, level; // Identity of tile
  int vi, vj;        // Position wrt view ( can be negative )
  int32_t kitty_id;  // ID used by kitty
} tile_t;

// Position on screen ( params for draw/clear image )
typedef struct pos_t {
  int i, j;           // Position in view grid
  int col, row, X, Y; // Position in pixels to pass to kitty
} pos_t;

// ---  Threads  ---
typedef struct thread_info {
  pthread_t thread_id;
  int thread_num;
  tile_t *tile;
} thread_info;

// -- Wrap it all up ---
typedef struct app_t {
  // data
  world_t *world;
  slide_t *slide;
  view_t *view;
  // ui
  int debug;
  int thumb;
  enum keys_e last_pressed;
} app_t;
