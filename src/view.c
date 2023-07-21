#include "view.h"

void view_update_level(view_t *view, slide_t *slide, int level) {
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

void view_set_wx_wy(view_t *view, int wx, int wy) {
  // Compute corresponding slide position at level
  double sx, sy;
  sx = (double)wx / view->zoom;
  sy = (double)wy / view->zoom;

  // Compute left top from center
  int left, top;
  left = (int)ceil(sx / TILE_SIZE);
  top = (int)ceil(sy / TILE_SIZE);
  view_update_left_top(view, left, top);
}

void view_update_left_top(view_t *view, int left, int top) {
  view->left = left;
  view->top = top;

  // Affected: sx, sy, wx, wy
  view->sx = left * TILE_SIZE;
  view->sy = top * TILE_SIZE;
  view->wx = view->sx * view->zoom;
  view->wy = view->sy * view->zoom;
}
