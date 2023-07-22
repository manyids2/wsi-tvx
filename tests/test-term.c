#include "../src/term.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <uniwidth.h>
#include <wchar.h>

/* Should switch to full screen,
 * move cursor around,
 * print stuff,
 * then clear and exit
 */
int main(void) {
  enable_raw_mode();

  clear_screen();
  hide_cursor();

  char s[64];
  int len, width;
  len = snprintf(s, sizeof(s), "🐱");
  width = len > 2 ? 2 : 1;

  // Not accurate?
  // width = u8_strwidth((uint8_t *)s, "UTF-8");

  int left = 5;
  int top = 2;
  int right = 15;
  int bot = 5;

  // row and col are 1 indexed
  // need to leave space for each char
  for (int row = top; row <= bot; row++) {
    for (int col = left; col <= right; col++) {
      // move_cursor(row, col);
      move_cursor(row, (col - left) * width + left);
      write_or_die(s, len, "write");
    }
  }

  printf("\r\n");
  printf("len: %d, width:%d\r\n", len, width);
  // int i = 0;
  // while ((s[i] != L'\0') && (i < 64)) {
  //   // Not working
  //   width = wcwidth((char)s[i]);
  //   printf("%d: %d, %d\r\n", i, (char)s[i], width);
  //   i++;
  // }
  sleep(2);
  return EXIT_SUCCESS;
}
