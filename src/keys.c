#include "keys.h"
#include "tiles.h"
#include "view.h"

int parse_input(void) {
  int nread;
  char c;
  // Read at least one char, else wait for input
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      die("read");
  }

  // Handle escape chars for arrows
  if (c == ESC_CHAR) {
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return ESC_CHAR;
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return ESC_CHAR;

    if (seq[0] == '[') {
      switch (seq[1]) {
      case 'A':
        return MOVE_UP;
      case 'B':
        return MOVE_DOWN;
      case 'C':
        return MOVE_RIGHT;
      case 'D':
        return MOVE_LEFT;
      }
    }

    // If nothing matches, just return esc
    return ESC_CHAR;

  } else {

    // Handle single char
    switch (c) {
    case 'q':
      return QUIT;
    case 'r':
      return RELOAD;
    case 'k':
      return MOVE_UP;
    case 'j':
      return MOVE_DOWN;
    case 'l':
      return MOVE_RIGHT;
    case 'h':
      return MOVE_LEFT;
    case 'i':
      return ZOOM_IN;
    case 'o':
      return ZOOM_OUT;
    case 't':
      return TOGGLE_THUMBNAIL;
    case 'd':
      return TOGGLE_DEBUG;
    case 'p':
      return LOG_TILES;
    default:
      return c;
    }
  }
}

void handle_keypress(app_t *app, int c) {
  switch (c) {
  case QUIT:
  case (CTRL_KEY('q')):
    app->last_pressed = QUIT;
    exit(EXIT_SUCCESS);
    break;

  case LOG_TILES: {
    WITH_WORLD_VIEW_TILES
    tiles_log(tiles, view, world, app->logfile);
  } break;

  case RELOAD: {
    WITH_WORLD_VIEW_SLIDE_TILES
    tiles_load_view(tiles, slide, view, world, 1);
  } break;

#define CASE_KEYPRESS(action, func)                                            \
  case action:                                                                 \
    func(app);                                                                 \
    break;

    CASE_KEYPRESS(MOVE_UP, move_up);
    CASE_KEYPRESS(MOVE_DOWN, move_down);
    CASE_KEYPRESS(MOVE_LEFT, move_left);
    CASE_KEYPRESS(MOVE_RIGHT, move_right);
    CASE_KEYPRESS(ZOOM_IN, zoom_in);
    CASE_KEYPRESS(ZOOM_OUT, zoom_out);
    CASE_KEYPRESS(TOGGLE_THUMBNAIL, toggle_thumbnail);
    CASE_KEYPRESS(TOGGLE_DEBUG, toggle_debug);

  default:
    app->last_pressed = INIT;
    break;
  }
}

#define MOVE_IF_CHANGED(key)                                                   \
  if (key == view->key)                                                        \
    return;                                                                    \
  view_update_left_top(view, left, top);                                       \
  tiles_load_view(tiles, slide, view, world, 0);

#define ZOOM_IF_CHANGED(key)                                                   \
  if (key == view->key)                                                        \
    return;                                                                    \
  view_update_level(view, slide, level);                                       \
  view_set_wx_wy(view, view->wx, view->wy);                                    \
  tiles_load_view(tiles, slide, view, world, 0);

void move_left(app_t *app) {
  WITH_WORLD_VIEW_SLIDE_TILES
  app->last_pressed = MOVE_LEFT;
  int left = MAX(0, view->left - 1);
  int top = view->top;
  MOVE_IF_CHANGED(left)
}

void move_right(app_t *app) {
  WITH_WORLD_VIEW_SLIDE_TILES
  app->last_pressed = MOVE_RIGHT;
  int left = MIN(view->smi - world->vmi, view->left + 1);
  int top = view->top;
  MOVE_IF_CHANGED(left)
}

void move_up(app_t *app) {
  WITH_WORLD_VIEW_SLIDE_TILES
  app->last_pressed = MOVE_UP;
  int top = MAX(0, view->top - 1);
  int left = view->left;
  MOVE_IF_CHANGED(top)
}

void move_down(app_t *app) {
  WITH_WORLD_VIEW_SLIDE_TILES
  app->last_pressed = MOVE_DOWN;
  int top = MIN(view->smj - world->vmj, view->top + 1);
  int left = view->left;
  MOVE_IF_CHANGED(top)
}

void zoom_in(app_t *app) {
  WITH_WORLD_VIEW_SLIDE_TILES
  app->last_pressed = ZOOM_IN;
  int level = MAX(0, view->level - 1);
  ZOOM_IF_CHANGED(level)
}

void zoom_out(app_t *app) {
  WITH_WORLD_VIEW_SLIDE_TILES
  app->last_pressed = ZOOM_OUT;
  int level = MIN(world->mlevel, view->level + 1);
  ZOOM_IF_CHANGED(level)
}

void toggle_thumbnail(app_t *app) {
  app->thumb = !app->thumb;
  slide_toggle_thumbnail(app->slide, app->world, app->thumb);
  app->last_pressed = TOGGLE_THUMBNAIL;
}

void toggle_debug(app_t *app) {
  app->debug = (app->debug + 1) % NUM_DEBUG; // loop through debug modes
  app->last_pressed = TOGGLE_DEBUG;
}
