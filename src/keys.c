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
    default:
      return c;
    }
  }
}

void handle_keypress(app_t *app, int c) {
  switch (c) {
  // Quit
  case QUIT:
  case (CTRL_KEY('q')):
    app->last_pressed = QUIT;
    exit(EXIT_SUCCESS);
    break;

  // Up
  case MOVE_UP:
    move_up(app);
    break;

  // Down
  case MOVE_DOWN:
    move_down(app);
    break;

  // Left
  case MOVE_LEFT:
    move_left(app);
    break;

  // Right
  case MOVE_RIGHT:
    move_right(app);
    break;

  // Zoom in
  case ZOOM_IN:
    zoom_in(app);
    break;

  // Zoom out
  case ZOOM_OUT:
    zoom_out(app);
    break;

  // Toggle thumbnail
  case TOGGLE_THUMBNAIL:
    toggle_thumbnail(app);
    break;

  // Debug info
  case TOGGLE_DEBUG:
    toggle_debug(app);
    break;

  default:
    app->last_pressed = INIT;
    break;
  }
}

void move_left(app_t *app) {
  app->last_pressed = MOVE_LEFT;
  view_t *view = app->view;
  world_t *world = app->world;
  tiles_t *tiles = app->tiles;
  int left = MAX(0, view->left - 1);
  if (left == view->left)
    return;
  view_update_left_top(view, left, view->top);
  tiles_load_view(tiles, view, world);
}

void move_right(app_t *app) {
  app->last_pressed = MOVE_RIGHT;
  view_t *view = app->view;
  world_t *world = app->world;
  tiles_t *tiles = app->tiles;
  int left = MIN(view->smi - world->vmi, view->left + 1);
  if (left == view->left)
    return;
  view_update_left_top(view, left, view->top);
  tiles_load_view(tiles, view, world);
}

void move_up(app_t *app) {
  app->last_pressed = MOVE_UP;
  view_t *view = app->view;
  world_t *world = app->world;
  tiles_t *tiles = app->tiles;
  int top = MAX(0, view->top - 1);
  if (top == view->top)
    return;
  view_update_left_top(view, view->left, top);
  tiles_load_view(tiles, view, world);
}

void move_down(app_t *app) {
  app->last_pressed = MOVE_DOWN;
  view_t *view = app->view;
  world_t *world = app->world;
  tiles_t *tiles = app->tiles;
  int top = MIN(view->smj - world->vmj, view->top + 1);
  if (top == view->top)
    return;
  view_update_left_top(view, view->left, top);
  tiles_load_view(tiles, view, world);
}

void zoom_in(app_t *app) {
  app->last_pressed = ZOOM_IN;
  view_t *view = app->view;
  slide_t *slide = app->slide;
  world_t *world = app->world;
  tiles_t *tiles = app->tiles;
  int level = MAX(0, view->level - 1);
  if (level == view->level)
    return;
  view_update_level(view, slide, level);
  view_set_wx_wy(view, view->wx, view->wy);
  tiles_load_view(tiles, view, world);
}

void zoom_out(app_t *app) {
  app->last_pressed = ZOOM_OUT;
  view_t *view = app->view;
  slide_t *slide = app->slide;
  world_t *world = app->world;
  tiles_t *tiles = app->tiles;
  int level = MIN(world->mlevel, view->level + 1);
  if (level == view->level)
    return;
  view_update_level(view, slide, level);
  view_set_wx_wy(view, view->wx, view->wy);
  tiles_load_view(tiles, view, world);
}

void toggle_thumbnail(app_t *app) {
  app->thumb = !app->thumb;
  app->last_pressed = TOGGLE_THUMBNAIL;
}

void toggle_debug(app_t *app) {
  app->debug = (app->debug + 1) % NUM_DEBUG; // loop through debug modes
  app->last_pressed = TOGGLE_DEBUG;
}
