#include "../src/base64.h"
#include <stdio.h>
#include <stdlib.h>

#define TILE_SIZE 2

int main(void) {
  // RGBAtoRGBbase64(size_t num_pixels, const uint32_t *buf, char *buf64)
  int num_pixels = TILE_SIZE * TILE_SIZE;
  uint32_t *buf = malloc(num_pixels * sizeof(uint32_t));
  char *buf64 = malloc(num_pixels * sizeof(uint32_t) + 1);

  RGBAtoRGBbase64(num_pixels, buf, buf64);

  fprintf(stdout, "%s\n", buf64);

  free(buf);
  free(buf64);

  exit(EXIT_SUCCESS);
}
