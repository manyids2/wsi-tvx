#include "constants.h"
#include <openslide/openslide.h>
#include <pthread.h>

// ---  View  ---

typedef struct Position {
  int64_t x, y, h, w;
  int i, j;
} Position;

typedef struct Bounds {
  int max_i, max_j;
} Bounds;

typedef struct World {
  Position pos;
  Position offset;
} World;

typedef struct View {
  int level;
  Position pos;
  Bounds max;
} View;

typedef struct Slide {
  int level;
  Position pos;
  Bounds max;
} Slide;

typedef struct Tile {
  int vi, vj, si, sj, level;
  int32_t kitty_id;
  int thread_num, loaded, visible;
} Tile;

typedef struct KittyTile {
  int vi, vj, col, row, X, Y;
} KittyPosition;

// ---  Threads  ---

struct thread_info {
  pthread_t thread_id;
  int thread_num;
  char *argv_string;
};
