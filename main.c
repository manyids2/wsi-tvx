#include "src/app.h"
#include "src/keys.h"
#include <ev.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Initialize app
slide_t slide = {0};
world_t world = {0};
view_t view = {0};
tiles_t tiles = {0};
app_t app = {.slide = &slide,
             .world = &world,
             .view = &view,
             .tiles = &tiles,
             .debug = DEBUG_NONE,
             .last_pressed = INIT};

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: wsi-tvx path/to/slide \n");
    return 1;
  }

  // Get path to slide
  char *slidepath = argv[1];
  // printf("slidepath: %s\r\n", slidepath);

  // Full screen terminal
  setup_term();

  // Initialize slide, world, view
  app_init(&app, slidepath);

  while (1) {
    // Get single keypress
    int c = parse_input();
    clear_text();

    // Handle it
    handle_keypress(&app, c);

    // Render info
    app_draw_statusline(&app);
    app_draw_debug(&app);
  }

  // Free memory
  app_free(&app);

  // Exit successfully
  return EXIT_SUCCESS;
}
