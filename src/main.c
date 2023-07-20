#include "keys.h"
#include "slide.h"
#include "state.h"
#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Initialize app
slide_t slide;
world_t world = {0};
view_t view = {0};
app_t app = {.slide = &slide, .world = &world, .view = &view, 0};

ev_io stdin_watcher;

static void stdin_cb(EV_P_ ev_io *w, int revents) {
  if (revents & EV_READ) {
    // NOTE: reads STDIN,
    // but also waits for input if nothing is there,
    // which is probably redundant
    int c = get_keypress();
    handle_keypress(EV_A_ w, &app, c);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: wsi-tvx path/to/slide \n");
    return 1;
  }

  // Usual terminal setup
  enable_raw_mode();
  hide_cursor();
  clear_screen();
  move_cursor(0, 0);

  // Get path to slide
  char *slidepath = argv[1];
  printf("slidepath: %s\r\n", slidepath);

  // Read slide info, get thumbnail
  slide_init(&slide, slidepath);

  // Store dims of slide at max zoom
  world.ww = slide.level_w[0];
  world.wh = slide.level_h[0];

  // Store dims of viewport in pixels
  get_window_size(&world.rows, &world.cols, &world.vw, &world.vh);
  get_window_size_kitty(&world.vw, &world.vh);

  // Compute character dims
  world.cw = world.vw / world.cols;
  world.ch = world.vh / world.rows;

  // IMPORTANT: Only after character dims are computed, impose view maxima
  world.vw = MIN(world.vw, MAX_WIDTH);
  world.vh = MIN(world.vh, MAX_HEIGHT);

  // Compute maximum level, cols and rows
  world.mlevel = slide.level_count - 1;
  world.vmi = world.vw / TILE_SIZE;
  world.vmj = world.vh / TILE_SIZE;

  // Choose starting position and level ( least zoom )
  view.level = world.mlevel;
  view.wx = world.ww / 2;
  view.wy = world.wh / 2;

  // Compute corresponding slide position at level
  view.zoom = slide.downsamples[view.level];
  view.sx = view.wx / view.zoom;
  view.sy = view.wy / view.zoom;

  // Start the event loop
  struct ev_loop *loop = EV_DEFAULT;

  // Watch stdin
  ev_io_init(&stdin_watcher, stdin_cb, STDIN_FILENO, EV_READ);
  ev_io_start(loop, &stdin_watcher);

  // Now wait for events to arrive
  ev_run(loop, 0);

  // Free memory
  slide_free(&slide);

  // Exit successfully
  return EXIT_SUCCESS;
}
