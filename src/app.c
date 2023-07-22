#include "app.h"

void setup_term(void) {
  enable_raw_mode();
  hide_cursor();
  clear_screen();
  move_cursor(0, 0);
}

void app_init(app_t *app, char *slidepath) {
  // Setup logger
  app->logfile = fopen(LOG_FILE, "a+");

  slide_t *slide = app->slide;
  setup_slide(slide, slidepath);
  setup_world(app);

  // Start at minimum zoom, centered
  int level = app->world->mlevel;
  int64_t wx = app->world->ww / 2 -
               (int)(app->world->vw * slide->downsamples[level] / 2);
  int64_t wy = app->world->wh / 2 -
               (int)(app->world->vh * slide->downsamples[level] / 2);
  setup_view(app, level, wx, wy);

  // Tiles, including loading them
  setup_tiles(app);

  // Draw statusline
  // app_draw_statusline(app);
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
  int *fvw = &app->world->fvw;
  int *fvh = &app->world->fvh;

  // Query using IOCTL first, then kitty
  get_window_size(rows, cols, fvw, fvh);

  // TODO: Not working
  // get_window_size_kitty(vw, vh);

  // Compute character dims
  world->cw = world->fvw / world->cols;
  world->ch = world->fvh / world->rows;

  // IMPORTANT: Only after character dims are computed, impose view maxima
  world->vw = MIN(world->fvw, MAX_WIDTH);
  world->vh = MIN(world->fvh, MAX_HEIGHT);

  // Compute maximum level, cols and rows
  world->mlevel = slide->level_count - 1;
  world->vmi = world->vw / TILE_SIZE;
  world->vmj = world->vh / TILE_SIZE;

  // Get world dims from slide
  world->ww = slide->level_w[0];
  world->wh = slide->level_h[0];

  // Compute positions on screen
  double x, y;
  int pad_x = (world->fvw - world->vw) / 2;
  int pad_y = (world->fvh - world->vh) / 2;
  for (int i = 0; i < world->vmi; i++) {
    for (int j = 0; j < world->vmj; j++) {
      pos_t *pos = &world->pos[i * world->vmj + j];
      x = (i * TILE_SIZE) + pad_x;
      y = (j * TILE_SIZE) + pad_y;
      pos->col = (int)floor(x / world->cw);
      pos->row = (int)floor(y / world->ch);
      pos->X = x - (pos->col * world->cw);
      pos->Y = y - (pos->row * world->ch);
    }
  }
}

void setup_view(app_t *app, int level, int64_t wx, int64_t wy) {
  view_t *view = app->view;
  slide_t *slide = app->slide;

  // Copy maximum tiles in view
  view->vmi = app->world->vmi;
  view->vmj = app->world->vmj;

  // Setup level dependent constants
  view_update_level(view, slide, level);

  // Setup view dependent constants given wx, wy
  view_set_wx_wy(view, wx, wy);
}

void setup_tiles(app_t *app) {
  view_t *view = app->view;
  world_t *world = app->world;
  tiles_t *tiles = app->tiles;
  // Keep reference to slide
  tiles->osr = app->slide->osr;

  // Initialize mutex
  tiles_init(tiles);

  // Load current level
  tiles_load_view(tiles, view, world);
}

void app_draw_statusline(app_t *app) {
  slide_t *slide = app->slide;
  world_t *world = app->world;
  view_t *view = app->view;
  char s[256];
  int len;

  len = snprintf(s, world->cols, "  %s", slide->slidepath);
  move_cursor(0, 0);
  write_or_die(s, len, "app_draw_statusline");

  len = snprintf(s, world->cols, "   %d / %d | %3d, %3d  ⇱ ", view->level,
                 world->mlevel, view->left, view->top);
  move_cursor(app->world->rows - 1, world->cols - len);
  write_or_die(s, len, "app_draw_statusline");
}

void app_draw_debug_state(app_t *app) {
  slide_t *slide = app->slide;
  world_t *world = app->world;
  view_t *view = app->view;
  char s[4096];
  int len =
      snprintf(s, sizeof(s),
               "Slide:            \r\n"
               "  slidepath    : %s       \r\n"
               "  level_count  : %d       \r\n"
               "\r\n"
               "  thumbnail:              \r\n"
               "     has: %d              \r\n"
               "    w, h: %ld, %ld        \r\n"
               "\r\n"
               "World:                  \r\n"
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
               "\r\n"
               "App:                    \r\n"
               "         debug: %d      \r\n"
               "         thumb: %d      \r\n"
               "  last_pressed: %d      \r\n"
               "\r\n",
               slide->slidepath, slide->level_count, slide->has_thumbnail,
               slide->thumbnail_w, slide->thumbnail_h, world->rows, world->cols,
               world->ww, world->wh, world->vw, world->vh, world->cw, world->ch,
               world->ox, world->oy, world->vmi, world->vmj, world->mlevel,
               view->level, view->zoom, view->left, view->top, view->smi,
               view->smj, view->sw, view->sh, view->sx, view->sy, view->wx,
               view->wy, app->debug, app->thumb, app->last_pressed);
  move_cursor(2, 0);
  write_or_die(s, len, "app_get_debug_world");
}

void app_draw_debug(app_t *app) {
  if (app->debug == DEBUG_STATE) {
    app_draw_debug_state(app);
  }
}

void app_free(app_t *app) {
  slide_t *slide = app->slide;
  tiles_t *tiles = app->tiles;
  tiles_free(tiles);
  slide_free(slide);

  fclose(app->logfile);
}
