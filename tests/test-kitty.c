#include "../src/base64.h"
#include "../src/kitty.h"
#include "../src/term.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROW 5
#define COL 5
#define TEST_TILE_SIZE 64

int main(void) {
  int num_pixels = TEST_TILE_SIZE * TEST_TILE_SIZE;
  uint32_t *buf = calloc(num_pixels, sizeof(uint32_t));
  char *buf64 = malloc(num_pixels * sizeof(uint32_t) + 1);

  RGBAtoRGBbase64(num_pixels, buf, buf64);

  uint32_t uint32_id = 10;
  kitty_provision(uint32_id, TEST_TILE_SIZE, TEST_TILE_SIZE, buf64);
  kitty_display(uint32_id, ROW, COL, 0, 0, -1);
  sleep(2);
  kitty_clear(uint32_id);
  kitty_delete(uint32_id);

  int int_id = 15;
  kitty_provision(int_id, TEST_TILE_SIZE, TEST_TILE_SIZE, buf64);
  kitty_display(int_id, ROW, COL * 2, 0, 0, -1);
  sleep(2);
  kitty_clear(int_id);
  kitty_delete(int_id);

  free(buf);
  free(buf64);

  exit(EXIT_SUCCESS);
}
