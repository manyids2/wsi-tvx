#include "keys.h"

void handle_keypress(struct ev_loop *loop, ev_io *w, char c) {
  switch (c) {
  // Quit
  case 'q':
  case (CTRL_KEY('q')):
    ev_io_stop(EV_A_ w);         // Stop watching stdin
    ev_break(EV_A_ EVBREAK_ALL); // all nested ev_runs stop iterating
    break;
  // Debug
  case 'd':
    break;
  default:
    break;
  }
}
