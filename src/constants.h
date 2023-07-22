#pragma once

// ---  Slide  ---
#define MAX_PATH_LENGTH 1024
#define MAX_LEVELS 32
#define MAX_ASSOCIATED_IMAGES 8
#define THUMBNAIL_ID 1
#define MIN_PIXELS 32

// ---  View  ---
#define TILE_SIZE 256
#define MAX_COLS 6
#define MAX_ROWS 4
#define MAX_WIDTH 1536
#define MAX_HEIGHT 1024

// --- Cache ---
#define KITTY_ID_OFFSET 1000
#define MAX_TILE_LAYER_CACHE 100
#define MAX_TILE_CACHE 300
#define LAYERS_CACHE 3
#define MARGIN_CACHE 2
#define NUM_THREADS 64

typedef enum {
  TILE_NO_ERROR = 0,
  TILE_OPENSLIDE_ERROR,
  TILE_KITTY_ERROR,
  TILE_PTHREAD_ERROR,
} tile_error_e;

// lock helpers
#define lock_cache()                                                           \
  if (pthread_mutex_lock(cache->mutex)) {                                      \
    perror("LRU Cache unable to obtain mutex lock");                           \
    return LRUC_PTHREAD_ERROR;                                                 \
  }

#define unlock_cache()                                                         \
  if (pthread_mutex_unlock(cache->mutex)) {                                    \
    perror("LRU Cache unable to release mutex lock");                          \
    return LRUC_PTHREAD_ERROR;                                                 \
  }

// --- Terminal ---
#define CHUNK 4096
enum keys_e {
  INIT = 0,
  TOGGLE_THUMBNAIL = 1,
  MOVE_LEFT = 2,
  MOVE_RIGHT = 3,
  MOVE_UP = 4,
  MOVE_DOWN = 5,
  ZOOM_IN = 6,
  ZOOM_OUT = 7,
  TOGGLE_DEBUG = 8,
  LOG_TILES = 9,
  QUIT = 10,
};

// --- Debug ---
enum debug_e {
  DEBUG_NONE = 0,
  DEBUG_STATE = 1,
  DEBUG_TILES = 2,
};
#define NUM_DEBUG 3
#define LOG_FILE "debug.log"

// --- Basics ---
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// ---  Errors  ---
// DOUBT: why use do while?
#define handle_error_en(en, msg)                                               \
  do {                                                                         \
    errno = en;                                                                \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

#define die(msg)                                                               \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

// ---  Write, if unsuccessful, exit  ---
#define write_or_die(s, len, msg)                                              \
  if (write(STDOUT_FILENO, s, len) != len) {                                   \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  }

// --- string ---
#define slice(str, result, start, end)                                         \
  strncpy(result, str + start, end - start);
