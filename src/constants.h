// ---  Slide  ---

#define MAX_LEVELS 32
#define MAX_ASSOCIATED_IMAGES 8
#define THUMBNAIL_ID 1

// ---  View  ---

#define MAX_COLS 6
#define MAX_ROWS 4
#define TILE_SIZE 256
#define MAX_WIDTH 1536
#define MAX_HEIGHT 1024

// --- Cache ---

#define MAX_TILE_CACHE 300
#define MAX_THREADS 8

// ---  Errors  ---

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
