#include "app.h"

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

  // Show stuff
  app_debug_world(app);
  app_draw_statusline(app);
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

  // TODO: Not working
  // get_window_size_kitty(vw, vh);

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

  // Get world dims from slide
  world->ww = slide->level_w[0];
  world->wh = slide->level_h[0];
}

void setup_view(app_t *app, int level) {
  slide_t *slide = app->slide;
  world_t *world = app->world;
  view_t *view = app->view;

  // Choose starting position at center of level
  view->level = level;
  view->zoom = slide->downsamples[view->level];

  // Get size of slide at level
  view->sw = slide->level_w[level];
  view->sh = slide->level_h[level];

  // Compute maximum tiles
  view->smi = (int)ceil((double)view->sw / TILE_SIZE);
  view->smj = (int)ceil((double)view->sh / TILE_SIZE);

  // Center on center of slide
  view->wx = world->ww / 2;
  view->wy = world->wh / 2;
  app_set_slide_from_worldxy(app);
}

void app_set_slide_from_worldxy(app_t *app) {
  view_t *view = app->view;
  world_t *world = app->world;

  // Compute corresponding slide position at level
  view->sx = view->wx / view->zoom;
  view->sy = view->wy / view->zoom;

  // Compute left top from center
  view->left = (int)(view->sx - (world->vw / 2)) / TILE_SIZE;
  view->top = (int)(view->sy - (world->vh / 2)) / TILE_SIZE;
}

void app_draw_statusline(app_t *app) {
  char s[256];
  int len = snprintf(s, sizeof(s), "Press 'h' for help  ( last_pressed:  %d )",
                     app->last_pressed);

  move_cursor(app->world->rows - 1, 1);
  write_or_die(s, len, "app_draw_statusline");
}

void app_debug_world(app_t *app) {
  world_t *world = app->world;
  view_t *view = app->view;
  char s[1024];
  int len = snprintf(
      s, sizeof(s),
      "World:          \r\n"
      " rows, cols: %6d, %6d   \r\n"
      "   ww,   wh: %6ld, %6ld \r\n"
      "   vw,   vh: %6d, %6d   \r\n"
      "   cw,   ch: %6d, %6d   \r\n"
      "   ox,   oy: %6d, %6d   \r\n"
      "  vmi,  vmj: %6d, %6d   \r\n"
      "     mlevel: %6d        \r\n"
      "\r\n"
      "View:                   \r\n"
      "      level: %d         \r\n"
      "       zoom: %f         \r\n"
      "  left, top: %6d, %6d   \r\n"
      "   smi, smj: %6d, %6d   \r\n"
      "    sw,  sh: %6ld, %6ld \r\n"
      "    sx,  sy: %6ld, %6ld \r\n"
      "    wx,  wy: %6ld, %6ld \r\n"
      " ol, oi, oj: %d, %d, %d \r\n"
      "\r\n"
      "App:                    \r\n"
      "         debug: %d      \r\n"
      "         thumb: %d      \r\n"
      "  last_pressed: %d      \r\n"
      "\r\n",
      world->rows, world->cols, world->ww, world->wh, world->vw, world->vh,
      world->cw, world->ch, world->ox, world->oy, world->vmi, world->vmj,
      world->mlevel, view->level, view->zoom, view->left, view->top, view->smi,
      view->smj, view->sw, view->sh, view->sx, view->sy, view->wx, view->wy,
      view->ol, view->oi, view->oj, app->debug, app->thumb, app->last_pressed);
  move_cursor(3, 0);
  write_or_die(s, len, "app_get_debug_world");
}

void app_free(app_t *app) {
  slide_t *slide = app->slide;

  // Free memory
  slide_free(slide);
}
