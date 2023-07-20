#include "keys.h"

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
