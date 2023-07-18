#include "slide.h"
#include "state.h"
#include "term.h"
#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

OpenSlide oslide;
World world = {0};
View view = {0};
ev_io stdin_watcher;

static void stdin_cb(EV_P_ ev_io *w, int revents) {
  ev_io_stop(EV_A_ w);         // Stop watching stdin
  ev_break(EV_A_ EVBREAK_ALL); // all nested ev_runs stop iterating
  clearText();

  int c = getKeypress();
  switch (c) {
  // Quit
  case 'q':
  case (CTRL_KEY('q')):
    ev_io_stop(EV_A_ w);         // Stop watching stdin
    ev_break(EV_A_ EVBREAK_ALL); // all nested ev_runs stop iterating
    break;
  default:
    break;
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: wsi-tvx path/to/slide \n");
    return 1;
  }

  // Usual terminal setup
  enableRawMode();
  hideCursor();
  clearScreen();
  moveCursor(0, 0);

  // Get path to slide
  char *slide = argv[1];
  printf("slidepath: %s\r\n", slide);

  // Read slide info, get thumbnail
  slideInit(&oslide, slide);

  // Store dims of slide at max zoom
  world.ww = oslide.level_w[0];
  world.wh = oslide.level_h[0];

  // Store dims of viewport in pixels
  getWindowSize(&world.rows, &world.cols, &world.vw, &world.vh);
  getWindowSizeKitty(&world.vw, &world.vh);

  // Compute character dims
  world.cw = world.vw / world.cols;
  world.ch = world.vh / world.rows;

  // Compute maximum level, cols and rows
  world.mlevel = oslide.level_count - 1;
  world.mi = world.vw / TILE_SIZE;
  world.mj = world.vh / TILE_SIZE;

  // Choose starting position and level ( least zoom )
  view.level = world.mlevel;
  view.wx = world.ww / 2;
  view.wy = world.wh / 2;

  // Compute corresponding slide position at level
  view.downsample = oslide.downsamples[view.level];
  view.sx = view.wx / view.downsample;
  view.sy = view.wy / view.downsample;

  // Start the event loop
  struct ev_loop *loop = EV_DEFAULT;

  // Watch stdin
  ev_io_init(&stdin_watcher, stdin_cb, STDIN_FILENO, EV_READ);
  ev_io_start(loop, &stdin_watcher);

  // Now wait for events to arrive
  ev_run(loop, 0);

  // Free memory
  slideFree(&oslide);

  // Exit successfully
  return EXIT_SUCCESS;
}
