#pragma once

// --- Terminal ---
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
  QUIT = 9,
};

// --- Basics ---
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

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
#define MAX_TILE_CACHE 300
#define LAYERS_CACHE 3
#define MARGIN_CACHE 2

// --- Threads ---
#define NUM_THREADS 64

// --- Debug ---
#define NUM_DEBUG 2

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
