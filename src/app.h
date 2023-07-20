#include "slide.h"
#include "state.h"
#include "term.h"

void setup_term(void);

void app_init(app_t *app, char *slidepath);
void app_free(app_t *app);

void setup_slide(slide_t *slide, char *slidepath);
void setup_world(app_t *app);
void setup_view(app_t *app, int level);
