#include "state.h"
#include "term.h"
#include <ev.h>

void handle_keypress(struct ev_loop *loop, ev_io *w, app_t *app, int c);
