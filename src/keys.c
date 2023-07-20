#include "keys.h"

int get_keypress(void) {
  int nread;
  char c;
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
        return ARROW_UP;
      case 'B':
        return ARROW_DOWN;
      case 'C':
        return ARROW_RIGHT;
      case 'D':
        return ARROW_LEFT;
      }
    }

    // If nothing matches, just return esc
    return ESC_CHAR;

  } else {

    // Handle single char
    switch (c) {
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
  case 'q':
  case (CTRL_KEY('q')):
    ev_io_stop(EV_A_ w);         // Stop watching stdin
    ev_break(EV_A_ EVBREAK_ALL); // all nested ev_runs stop iterating
    break;

  // Up
  case ARROW_DOWN:
  case 'j':
    // viewMoveDown(A.V);
    break;

  // Down
  case ARROW_UP:
  case 'k':
    // viewMoveUp(A.V);
    break;

  // Left
  case ARROW_LEFT:
  case 'h':
    // viewMoveLeft(A.V);
    break;

  // Right
  case ARROW_RIGHT:
  case 'l':
    // viewMoveRight(A.V);
    break;

  // Zoom in
  case 'i':
    // viewZoomIn(A.V);
    break;

  // Zoom out
  case 'o':
    // viewZoomOut(A.V);
    break;

  // Toggle thumbnail
  case 't':
    break;

  // Debug info
  case 'd':
    app->debug = (app->debug + 1) % NUM_DEBUG; // loop through debug modes
    break;

  default:
    break;
  }
}
