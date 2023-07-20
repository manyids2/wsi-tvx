#include "app.h"
#include <stdio.h>

void setup_term(void) {
  enable_raw_mode();
  hide_cursor();
  clear_screen();
  move_cursor(0, 0);
}

void app_init(app_t *app, char *slidepath) {
  slide_t *slide = app->slide;
  setup_slide(slide, slidepath);
  setup_world(app);

  // Start at minimum zoom
  int level = app->world->mlevel;
  setup_view(app, level);

  app->last_pressed = INIT;
}

void setup_slide(slide_t *slide, char *slidepath) {
  slide_init(slide, slidepath);
  slide_load_thumbnail(slide);
}

void setup_world(app_t *app) {
  slide_t *slide = app->slide;
  world_t *world = app->world;

  // Values to retrieve
  int *rows = &app->world->rows;
  int *cols = &app->world->cols;
  int *vw = &app->world->vw;
  int *vh = &app->world->vh;

  // Query using IOCTL first, then kitty
  get_window_size(rows, cols, vw, vh);
  get_window_size_kitty(vw, vh);

  // Compute character dims
  world->cw = world->vw / world->cols;
  world->ch = world->vh / world->rows;

  // IMPORTANT: Only after character dims are computed, impose view maxima
  world->vw = MIN(world->vw, MAX_WIDTH);
  world->vh = MIN(world->vh, MAX_HEIGHT);

  // Compute maximum level, cols and rows
  world->mlevel = slide->level_count - 1;
  world->vmi = world->vw / TILE_SIZE;
  world->vmj = world->vh / TILE_SIZE;
}

void setup_view(app_t *app, int level) {
  slide_t *slide = app->slide;
  world_t *world = app->world;
  view_t *view = app->view;

  // Choose starting position at center of level
  view->level = level;
  view->wx = world->ww / 2;
  view->wy = world->wh / 2;

  // Compute corresponding slide position at level
  view->zoom = slide->downsamples[view->level];
  view->sx = view->wx / view->zoom;
  view->sy = view->wy / view->zoom;
}

void app_draw_statusline(app_t *app) {
  char s[256];
  int len = snprintf(s, sizeof(s), "Press 'h' for help  ( last_pressed:  %d )",
                     app->last_pressed);

  move_cursor(app->world->rows - 1, 1);
  write_or_die(s, len, "app_draw_statusline");
}

void app_free(app_t *app) {
  slide_t *slide = app->slide;

  // Free memory
  slide_free(slide);
}
