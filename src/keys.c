#include "keys.h"

int parse_input(void) {
  int nread;
  char c;
  // Read at least one char, else die
  if ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
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

void handle_keypress(struct ev_loop *loop, ev_io *w, app_t *app, int c) {
  switch (c) {
  // Quit
  case QUIT:
  case (CTRL_KEY('q')):
    ev_io_stop(EV_A_ w);         // Stop watching stdin
    ev_break(EV_A_ EVBREAK_ALL); // all nested ev_runs stop iterating
    app->last_pressed = QUIT;
    break;

  // Up
  case MOVE_DOWN:
    // viewMoveDown(A.V);
    app->last_pressed = MOVE_DOWN;
    break;

  // Down
  case MOVE_UP:
    // viewMoveUp(A.V);
    app->last_pressed = MOVE_UP;
    break;

  // Left
  case MOVE_LEFT:
    // viewMoveLeft(A.V);
    app->last_pressed = MOVE_LEFT;
    break;

  // Right
  case MOVE_RIGHT:
    // viewMoveRight(A.V);
    app->last_pressed = MOVE_RIGHT;
    break;

  // Zoom in
  case ZOOM_IN:
    // viewZoomIn(A.V);
    app->last_pressed = ZOOM_IN;
    break;

  // Zoom out
  case ZOOM_OUT:
    // viewZoomOut(A.V);
    app->last_pressed = ZOOM_OUT;
    break;

  // Toggle thumbnail
  case TOGGLE_THUMBNAIL:
    app->thumb = !app->thumb;
    app->last_pressed = TOGGLE_THUMBNAIL;
    break;

  // Debug info
  case TOGGLE_DEBUG:
    app->debug = (app->debug + 1) % NUM_DEBUG; // loop through debug modes
    app->last_pressed = TOGGLE_DEBUG;
    break;

  default:
    app->last_pressed = INIT;
    break;
  }
}
