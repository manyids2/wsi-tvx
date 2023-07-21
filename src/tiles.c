#include "tiles.h"

void tiles_init(tiles_t *tiles) {

  // all cache calls are guarded by a mutex
  tiles->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  if (pthread_mutex_init(tiles->mutex, NULL))
    die("LRU Cache unable to initialise mutex");
}

void tiles_free(tiles_t *tiles) {
  // mutex
  if (tiles->mutex) {
    if (pthread_mutex_destroy(tiles->mutex)) {
      die("LRU Cache unable to destroy mutex");
    }
  }
}
