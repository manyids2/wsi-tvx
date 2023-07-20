#include "constants.h"
#include <openslide/openslide.h>
#include <pthread.h>

// ---  View  ---

// Remains constant upto resize, new slide
typedef struct World {
  int64_t ww, wh; // Slide at max zoom in pixels
  int vw, vh;     // Viewport in pixels
  int rows, cols; // Viewport dims in rows, cols
  int cw, ch;     // Character dims in pixels
  int ox, oy;     // offset in pixels
  int mi, mj;     // maximum tiles in view
  int mlevel;     // maximum level ( = level_count - 1 )
} World;

// Remains constant upto level change or move
typedef struct View {
  int level;        // Level of slide
  int left, top;    // Coords wrt slide
  int coi, coj;     // Cache offset to view
  float downsample; // downsample factor from max zoom
  int64_t sw, sh;   // Slide level dims
  int64_t sx, sy;   // Slide level position
  int64_t wx, wy;   // World level position
} View;

// Tile from slide ( params for load image )
typedef struct Tile {
  int si, sj, level;               // identity of tile
  int vi, vj;                      // position wrt view
  int32_t kitty_id;                // id used by kitty
  int thread_num, loaded, visible; // status
} Tile;

// Tile on screen ( params for draw/clear image )
typedef struct Position {
  int i, j;           // Position in view grid
  int col, row, X, Y; // Position in pixels
  int32_t kitty_id;
} Position;

// ---  Threads  ---
struct thread_info {
  pthread_t thread_id;
  int thread_num;
  int *l;          // level
  int *si;         // xth coord wrt slide
  int *sj;         // yth coord wrt slide
  uint32_t *buf;   // buffer for openslide
  uint32_t *buf64; // buffer for kitty
};
