#include "slide.h"
#include "state.h"
#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

OpenSlide S;
ev_io stdin_watcher;

static void stdin_cb(EV_P_ ev_io *w, int revents) {
  ev_io_stop(EV_A_ w);         // Stop watching stdin
  ev_break(EV_A_ EVBREAK_ALL); // all nested ev_runs stop iterating
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: wsi-tvx path/to/slide \n");
    return 1;
  }

  // Get path to slide
  char *slide = argv[1];
  printf("slidepath: %s\n", slide);

  slideInit(&S, slide);

  // Start the event loop
  struct ev_loop *loop = EV_DEFAULT;

  // Watch stdin
  ev_io_init(&stdin_watcher, stdin_cb, STDIN_FILENO, EV_READ);
  ev_io_start(loop, &stdin_watcher);

  // Now wait for events to arrive
  ev_run(loop, 0);

  // Free memory
  slideFree(&S);

  // Exit successfully
  return EXIT_SUCCESS;
}
