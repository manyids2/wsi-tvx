#include "../src/base64.h"
#include "../src/slide.h"
#include "../src/term.h"
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <string.h>
#include <unistd.h>

/*  NUM_PIXELS : 256 * 256 * 32
 *  NUM_TILES  : 4096
 *  NUM_THREADS: 4096
 *
 * ❯ test_serial
 * Executed in   22.78 secs    fish           external
 *   usr time   15.23 secs    0.00 millis   15.23 secs
 *   sys time    1.61 secs    1.03 millis    1.61 secs*
 *
 * ❯ test_parallel
 * Executed in   17.31 secs    fish           external
 *  usr time    7.40 secs   20.87 millis    7.38 secs
 *  sys time    2.68 secs    8.27 millis    2.67 secs
 *
 */

#define NUM_PIXELS (256 * 256)
#define NUM_ROWS 128
#define NUM_COLS 128

slide_t slide = {0};

static pthread_cond_t threadDied = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;
/* Protects all of the following global variables */

static int totThreads = 0;   /* Total number of threads created */
static int numLive = 0;      /* Total number of threads still alive or
                                terminated but not yet joined */
static int numUnjoined = 0;  /* Number of terminated threads that
                                have not yet been joined */
enum tstate {                /* Thread states */
              TS_ALIVE,      /* Thread is alive */
              TS_TERMINATED, /* Thread terminated, not yet joined */
              TS_JOINED      /* Thread terminated, and joined */
};

static struct {        /* Info about each thread */
  pthread_t tid;       /* ID of this thread */
  enum tstate state;   /* Thread state (TS_* constants above) */
  int level, col, row; /* Number seconds to live before terminating */
} *thread;

#define dieerr(s, msg)                                                         \
  if (s != 0) {                                                                \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  }

#define LOAD_TILE(buf, buf64)                                                  \
  double zoom = slide.downsamples[level];                                      \
  int64_t sx = (col * TILE_SIZE) * zoom;                                       \
  int64_t sy = (row * TILE_SIZE) * zoom;                                       \
  openslide_read_region(slide.osr, buf, sx, sy, level, TILE_SIZE, TILE_SIZE);  \
  assert(openslide_get_error(slide.osr) == NULL);                              \
  RGBAtoRGBbase64(TILE_SIZE *TILE_SIZE, buf, buf64);

#define PRINT_TILE(buf64)                                                      \
  kitty_provision(KITTY_ID_OFFSET, TILE_SIZE, TILE_SIZE, buf64);               \
  kitty_display(KITTY_ID_OFFSET, 5, 5, 0, 0, 1);

#define PRINT_INFO                                                             \
  printf("NUM_PIXELS: %d\r\n", NUM_PIXELS);                                    \
  printf("NUM_TILES : %d\r\n", NUM_ROWS *NUM_COLS);

#define SETUP_BUFFERS                                                          \
  uint32_t *buf = malloc(NUM_PIXELS * sizeof(uint32_t));                       \
  char *buf64 = malloc(NUM_PIXELS * sizeof(uint32_t) + 1);

#define SETUP_THREAD_BUFFERS                                                   \
  __thread uint32_t tbuf[NUM_PIXELS] = {0};                                    \
  __thread char tbuf64[NUM_PIXELS * 4 + 1] = {0};

#define FREE_BUFFERS                                                           \
  free(buf);                                                                   \
  free(buf64);

#define LOCK_MUTEX                                                             \
  s = pthread_mutex_lock(&threadMutex);                                        \
  dieerr(s, "pthread_mutex_lock");

#define UNLOCK_MUTEX                                                           \
  s = pthread_mutex_unlock(&threadMutex);                                      \
  dieerr(s, "pthread_mutex_unlock");

#define COND_SIGNAL_MUTEX                                                      \
  s = pthread_cond_signal(&threadDied);                                        \
  dieerr(s, "pthread_cond_signal");

#define COND_WAIT_MUTEX                                                        \
  s = pthread_cond_wait(&threadDied, &threadMutex);                            \
  dieerr(s, "pthread_cond_wait");

static void *threaded_alloc_each_time(void *arg) {
  // Setup
  int idx = (int)arg;
  int s, level, col, row;
  level = thread[idx].level;
  col = thread[idx].col;
  row = thread[idx].row;

  // allocate, load
  SETUP_BUFFERS
  LOAD_TILE(buf, buf64)

  // locked write to stdout
  LOCK_MUTEX
  PRINT_TILE(buf64)
  numUnjoined++;
  thread[idx].state = TS_TERMINATED;
  UNLOCK_MUTEX

  // free and join
  FREE_BUFFERS
  COND_SIGNAL_MUTEX

  return NULL;
}

SETUP_THREAD_BUFFERS

static void *threaded_alloc_once(void *arg) {
  // Setup
  int idx = (int)arg;
  int s, level, col, row;
  level = thread[idx].level;
  col = thread[idx].col;
  row = thread[idx].row;

  // allocate, load
  LOAD_TILE(tbuf, tbuf64)

  // locked write to stdout
  LOCK_MUTEX
  PRINT_TILE(tbuf64)
  numUnjoined++;
  thread[idx].state = TS_TERMINATED;
  UNLOCK_MUTEX

  // free and join
  COND_SIGNAL_MUTEX

  return NULL;
}

void test_serial(void) {
  int level = 0;
  PRINT_INFO
  SETUP_BUFFERS
  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      LOAD_TILE(buf, buf64)
      PRINT_TILE(buf64)
    }
  }
  FREE_BUFFERS
}

void test_parallel(void) {
  PRINT_INFO

  // Initialize one thread for each tile
  int s, idx;
  totThreads = NUM_COLS * NUM_ROWS;
  thread = calloc(totThreads, sizeof(*thread));
  if (thread == NULL)
    perror("calloc");

  // Create/launch all threads
  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      idx = row * NUM_COLS + col;
      thread[idx].level = 1;
      thread[idx].col = col;
      thread[idx].row = row;
      thread[idx].state = TS_ALIVE;
      s = pthread_create(&thread[idx].tid, NULL, threaded_alloc_once,
                         (void *)idx);
      dieerr(s, "pthread_create");
    }
  }

  // Join with terminated threads
  numLive = totThreads;
  while (numLive > 0) {
    LOCK_MUTEX
    while (numUnjoined == 0) {
      COND_WAIT_MUTEX
    }
    for (idx = 0; idx < totThreads; idx++) {
      if (thread[idx].state == TS_TERMINATED) {
        s = pthread_join(thread[idx].tid, NULL);
        dieerr(s, "pthread_join");
        thread[idx].state = TS_JOINED;
        numLive--;
        numUnjoined--;
      }
    }
    UNLOCK_MUTEX
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: test-base64 [0/1] \n");
    return 1;
  }
  int parallel = atoi(argv[1]);

  // Read slide
  char *slidepath = "/data/slides/aperio/CMU-1-JP2K-33005.svs";
  slide_init(&slide, slidepath);

  // Setup term
  enable_raw_mode();
  clear_screen();
  move_cursor(1, 1);
  hide_cursor();

  if (parallel) {
    test_parallel();
  } else {
    test_serial();
  }

  slide_free(&slide);

  exit(EXIT_SUCCESS);
}
