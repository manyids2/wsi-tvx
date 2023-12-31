#include "state.h"
#include "term.h"

int parse_input(void);
void handle_keypress(app_t *app, int c);

void move_left(app_t *app);
void move_right(app_t *app);
void move_up(app_t *app);
void move_down(app_t *app);
void zoom_in(app_t *app);
void zoom_out(app_t *app);
void toggle_thumbnail(app_t *app);
void toggle_debug(app_t *app);
