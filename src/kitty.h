#include <bits/stdint-uintn.h>

void kitty_provision(uint32_t kitty_id, int w, int h, char *buf64);
void kitty_display(uint32_t kitty_id, int row, int col, int X, int Y, int Z);
void kitty_clear(uint32_t kitty_id);
void kitty_delete(uint32_t kitty_id);
