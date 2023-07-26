#pragma once

// ---  Contexts  ---
#define WITH_WORLD_SLIDE                                                       \
  world_t *world = app->world;                                                 \
  slide_t *slide = app->slide;

#define WITH_VIEW_SLIDE                                                        \
  view_t *view = app->view;                                                    \
  slide_t *slide = app->slide;

#define WITH_VIEW_SLIDE_TILES                                                  \
  view_t *view = app->view;                                                    \
  slide_t *slide = app->slide;                                                 \
  tiles_t *tiles = app->tiles;

#define WITH_WORLD_VIEW_SLIDE_TILES                                            \
  world_t *world = app->world;                                                 \
  view_t *view = app->view;                                                    \
  slide_t *slide = app->slide;                                                 \
  tiles_t *tiles = app->tiles;

#define WITH_WORLD_VIEW_SLIDE                                                  \
  world_t *world = app->world;                                                 \
  view_t *view = app->view;                                                    \
  slide_t *slide = app->slide;

#define WITH_WORLD_VIEW_TILES                                                  \
  world_t *world = app->world;                                                 \
  view_t *view = app->view;                                                    \
  tiles_t *tiles = app->tiles;

#define WITH_SLIDE_TILES                                                       \
  slide_t *slide = app->slide;                                                 \
  tiles_t *tiles = app->tiles;

// ---  Slide  ---
#define MAX_PATH_LENGTH 1024
#define MAX_LEVELS 32
#define MAX_ASSOCIATED_IMAGES 8
#define THUMBNAIL_ID 1
#define KITTY_ID_THUMBNAIL 10
#define MIN_PIXELS 32

// ---  View  ---
#define TILE_SIZE 256
#define NUM_PIXELS (TILE_SIZE * TILE_SIZE)
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
  RELOAD = 11,
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

// --- Threads ---

#define dieerr(s, msg)                                                         \
  if (s != 0) {                                                                \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  }

#define LOAD_TILE_THREADED(buf, buf64)                                         \
  int64_t sx = (args->tile->si * TILE_SIZE) * args->zoom;                      \
  int64_t sy = (args->tile->sj * TILE_SIZE) * args->zoom;                      \
  openslide_read_region(args->osr, buf, sx, sy, args->tile->level, TILE_SIZE,  \
                        TILE_SIZE);                                            \
  assert(openslide_get_error(args->osr) == NULL);                              \
  RGBAtoRGBbase64(TILE_SIZE *TILE_SIZE, buf, buf64);

#define PROVISION_TILE_THREADED(kitty_id, buf64)                               \
  kitty_provision(kitty_id, TILE_SIZE, TILE_SIZE, buf64);

#define SETUP_THREAD_BUFFERS                                                   \
  __thread uint32_t tbuf[NUM_PIXELS] = {0};                                    \
  __thread char tbuf64[NUM_PIXELS * 4 + 1] = {0};

#define LOCK_MUTEX                                                             \
  s = pthread_mutex_lock(&tiles->threadMutex);                                 \
  dieerr(s, "pthread_mutex_lock");

#define UNLOCK_MUTEX                                                           \
  s = pthread_mutex_unlock(&tiles->threadMutex);                               \
  dieerr(s, "pthread_mutex_unlock");

#define COND_SIGNAL_MUTEX                                                      \
  s = pthread_cond_signal(&tiles->threadDied);                                 \
  dieerr(s, "pthread_cond_signal");

#define COND_WAIT_MUTEX                                                        \
  s = pthread_cond_wait(&tiles->threadDied, &tiles->threadMutex);              \
  dieerr(s, "pthread_cond_wait");
