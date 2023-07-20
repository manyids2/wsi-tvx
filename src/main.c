#include "app.h"
#include "keys.h"
#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Initialize app
slide_t slide;
world_t world = {0};
view_t view = {0};
app_t app = {.slide = &slide,
             .world = &world,
             .view = &view,
             .debug = 0,
             .last_pressed = INIT};

ev_io stdin_watcher;

static void stdin_cb(EV_P_ ev_io *w, int revents) {
  if (revents & EV_READ) {
    // NOTE: Expects at least one byte ( reads upto 3 )
    int c = parse_input();
    handle_keypress(EV_A_ w, &app, c);
    app_draw_statusline(&app);
    app_debug_world(&app);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: wsi-tvx path/to/slide \n");
    return 1;
  }

  // Full screen terminal
  setup_term();

  // Get path to slide
  char *slidepath = argv[1];
  printf("slidepath: %s\r\n", slidepath);

  // Initialize slide, world, view
  app_init(&app, slidepath);

  // Start the event loop
  struct ev_loop *loop = EV_DEFAULT;

  // Watch stdin
  ev_io_init(&stdin_watcher, stdin_cb, STDIN_FILENO, EV_READ);
  ev_io_start(loop, &stdin_watcher);

  // Now wait for events to arrive
  ev_run(loop, 0);

  // Free memory
  app_free(&app);

  // Exit successfully
  return EXIT_SUCCESS;
}
