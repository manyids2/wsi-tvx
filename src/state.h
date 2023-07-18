#include "constants.h"
#include <openslide/openslide.h>
#include <pthread.h>

// ---  Slide  ---

typedef struct Slide {
  openslide_t *osr;

  int level_count;
  int64_t level_w[MAX_LEVELS];
  int64_t level_h[MAX_LEVELS];
  float downsamples[MAX_LEVELS];

  int has_thumbnail;
  int64_t thumbnail_w, thumbnail_h;
} Slide;

// ---  View  ---

typedef struct Dimensions {
  int64_t w, h;
} Dimensions;

typedef struct DimensionsInt {
  int w, h;
} DimensionsInt;

typedef struct Position {
  int64_t x, y;
} Position;

typedef struct Coordinate {
  int i, j;
} Coordinate;

typedef struct World {
  Dimensions dims;
  Position pos;
  Position offset;
} World;

typedef struct View {
  Coordinate topleft;
  int level;

  Dimensions dims;
  Position pos;
  Coordinate max;
} View;

typedef struct Tile {
  Position world_pos;
  Position slide_pos;
  Coordinate slide_coord;
  Coordinate view_coord;
  int level, loaded, visible;
  int32_t kitty_id;
  int thread_num;
} Tile;

// ---  Threads  ---

struct thread_info {
  pthread_t thread_id;
  int thread_num;
  char *argv_string;
};
