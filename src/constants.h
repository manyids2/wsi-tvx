// ---  Slide  ---
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
#define MARGIN_XY_CACHE 2
#define LAYERS_CACHE 3
#define MAX_TILE_CACHE 300

// --- Threads ---
#define NUM_THREADS 64

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

// ---  Write, if unsuccessful, exit  ---
#define write_or_die(s, len, msg)                                              \
  if (write(STDOUT_FILENO, s, len) != len) {                                   \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  }
