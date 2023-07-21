#include "view.h"

void view_update_level(app_t *app, int level) {
  slide_t *slide = app->slide;
  view_t *view = app->view;

  // Set view according to level
  view->level = level;
  view->zoom = slide->downsamples[view->level];

  // Get size of slide at level
  view->sw = slide->level_w[level];
  view->sh = slide->level_h[level];

  // Compute maximum tiles
  view->smi = (int)ceil((double)view->sw / TILE_SIZE);
  view->smj = (int)ceil((double)view->sh / TILE_SIZE);
}

void view_update_worldxy(app_t *app, int wx, int wy) {
  view_t *view = app->view;
  world_t *world = app->world;
  view->wx = wx;
  view->wy = wy;

  // Compute corresponding slide position at level
  int sx, sy;
  sx = view->wx / view->zoom;
  sy = view->wy / view->zoom;

  // Compute left top from center
  int left, top;
  left = MAX(0, (int)((sx - (world->vw / 2)) / TILE_SIZE));
  top = MAX(0, (int)((sy - (world->vh / 2)) / TILE_SIZE));
  view_update_left_top(app, left, top);
}

void view_update_left_top(app_t *app, int left, int top) {
  view_t *view = app->view;
  world_t *world = app->world;
  view->left = left;
  view->top = top;

  // Affected: sx, sy, wx, wy
  view->sx = (left * TILE_SIZE) + (world->vmi * TILE_SIZE) / 2;
  view->sy = (top * TILE_SIZE) + (world->vmj * TILE_SIZE) / 2;
  view->wx = view->sx * view->zoom;
  view->wy = view->sy * view->zoom;
}
