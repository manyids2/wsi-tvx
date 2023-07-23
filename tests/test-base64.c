#include "../src/base64.h"
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <string.h>
#include <unistd.h>

/*  NUM_PIXELS : 256 * 256 * 32
 *  NUM_TILES  : 4096
 *
 * ❯ time ./tests/bin/test-base64 0
 * Executed in   11.13 secs    fish           external
 *    usr time   11.11 secs  801.00 micros   11.11 secs
 *    sys time    0.02 secs  239.00 micros    0.02 secs
 *
 * ❯ time ./tests/bin/test-base64 1
 * Executed in    3.47 secs    fish           external
 *    usr time   16.74 secs    0.00 millis   16.74 secs
 *    sys time    2.16 secs    1.08 millis    2.16 secs
 */

#define NUM_PIXELS (256 * 256 * 32)
#define NUM_ROWS 64
#define NUM_COLS 64

void test_serial() {
  printf("NUM_PIXELS: %d\n", NUM_PIXELS);
  printf("NUM_TILES : %d\n", NUM_ROWS * NUM_COLS);

  uint32_t *buf = malloc(NUM_PIXELS * sizeof(uint32_t));
  char *buf64 = malloc(NUM_PIXELS * sizeof(uint32_t) + 1);

  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      RGBAtoRGBbase64(NUM_PIXELS, buf, buf64);
    }
  }

  free(buf);
  free(buf64);
}

// -------------------------------------------
// From TLPI repo ( thread_multijoin.c )
// -------------------------------------------

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

static struct {      /* Info about each thread */
  pthread_t tid;     /* ID of this thread */
  enum tstate state; /* Thread state (TS_* constants above) */
  int level, si, sj; /* Number seconds to live before terminating */
} *thread;

#define die(s, msg)                                                            \
  if (s != 0) {                                                                \
    printf("Error: %d\n", s);                                                  \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  }

static void * /* Start function for thread */
threadFunc(void *arg) {

  int idx = (int)arg;
  int s;

  // do the work, extra allocations though
  uint32_t *buf = malloc(NUM_PIXELS * sizeof(uint32_t));
  char *buf64 = malloc(NUM_PIXELS * sizeof(uint32_t) + 1);

  RGBAtoRGBbase64(NUM_PIXELS, buf, buf64);

  // -- Change state of thread --
  s = pthread_mutex_lock(&threadMutex);
  die(s, "pthread_mutex_lock");

  numUnjoined++;
  thread[idx].state = TS_TERMINATED;

  s = pthread_mutex_unlock(&threadMutex);
  die(s, "pthread_mutex_unlock");

  free(buf);
  free(buf64);

  s = pthread_cond_signal(&threadDied);
  die(s, "pthread_cond_signal");

  return NULL;
}

void test_parallel() {
  printf("NUM_PIXELS: %d\n", NUM_PIXELS);
  printf("NUM_TILES : %d\n", NUM_ROWS * NUM_COLS);

  // Initialize one thread for each tile
  int s, idx;
  totThreads = NUM_COLS * NUM_ROWS;

  thread = calloc(totThreads, sizeof(*thread));
  if (thread == NULL)
    perror("calloc");

  /* Create all threads */

  for (int row = 0; row < NUM_ROWS; row++) {
    for (int col = 0; col < NUM_COLS; col++) {
      idx = row * NUM_COLS + col;
      thread[idx].level = 1;
      thread[idx].si = col;
      thread[idx].sj = row;
      thread[idx].state = TS_ALIVE;
      s = pthread_create(&thread[idx].tid, NULL, threadFunc, (void *)idx);
      die(s, "pthread_create");
    }
  }

  numLive = totThreads;

  /* Join with terminated threads */
  while (numLive > 0) {
    s = pthread_mutex_lock(&threadMutex);
    die(s, "pthread_mutex_lock");

    while (numUnjoined == 0) {
      s = pthread_cond_wait(&threadDied, &threadMutex);
      die(s, "pthread_cond_wait");
    }

    for (idx = 0; idx < totThreads; idx++) {
      if (thread[idx].state == TS_TERMINATED) {
        s = pthread_join(thread[idx].tid, NULL);
        die(s, "pthread_join");

        thread[idx].state = TS_JOINED;
        numLive--;
        numUnjoined--;
      }
    }

    s = pthread_mutex_unlock(&threadMutex);
    die(s, "pthread_mutex_unlock");
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: test-base64 [0/1] \n");
    return 1;
  }
  int parallel = atoi(argv[1]);

  if (parallel) {
    test_parallel();
  } else {
    test_serial();
  }

  exit(EXIT_SUCCESS);
}
