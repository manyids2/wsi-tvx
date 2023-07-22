#include "kitty.h"
#include "constants.h"
#include "term.h"

void kittyProvisionImage(int index, int w, int h, char *buf64) {
  /*
   * Switch to RGB to get gains on base64 encoding
   *
   * <ESC>_Gf=24,s=<w>,v=<h>,m=1;<encoded pixel data first chunk><ESC>\
   * <ESC>_Gm=1;<encoded pixel data second chunk><ESC>\
   * <ESC>_Gm=0;<encoded pixel data last chunk><ESC>\
   */
  size_t sent_bytes = 0;
  size_t base64_size = w * h * sizeof(uint32_t);
  while (sent_bytes < base64_size) {
    size_t chunk_size =
        base64_size - sent_bytes < CHUNK ? base64_size - sent_bytes : CHUNK;
    int cont = !!(sent_bytes + chunk_size < base64_size);
    if (sent_bytes == 0) {
      fprintf(stdout, "\x1B_Gt=d,f=24,q=2,i=%u,s=%d,v=%d,m=%d%s;", index, w, h,
              cont, "");
    } else {
      fprintf(stdout, "\x1B_Gm=%d;", cont);
    }
    fwrite(buf64 + sent_bytes, chunk_size, 1, stdout);
    fprintf(stdout, "\x1B\\");
    sent_bytes += chunk_size;
  }
  fflush(stdout);
}

void kitty_provision(uint32_t kitty_id, int w, int h, char *buf64) {
  /*
   * Switch to RGB to get gains on base64 encoding
   *
   * <ESC>_Gf=24,s=<w>,v=<h>,m=1;<encoded pixel data first chunk><ESC>\
   * <ESC>_Gm=1;<encoded pixel data second chunk><ESC>\
   * <ESC>_Gm=0;<encoded pixel data last chunk><ESC>\
   */
  size_t sent_bytes = 0;
  size_t base64_size = w * h * sizeof(uint32_t);

  fprintf(stdout, "\x1B_Gt=d,f=24,q=2,i=%u,s=%d,v=%d,m=1;", kitty_id, w, h);
  while (sent_bytes < base64_size) {
    size_t chunk_size =
        base64_size - sent_bytes < CHUNK ? base64_size - sent_bytes : CHUNK;
    int cont = !!(sent_bytes + chunk_size < base64_size);
    if (sent_bytes > 0)
      fprintf(stdout, "\x1B_Gm=%d;", cont);
    fwrite(buf64 + sent_bytes, chunk_size, 1, stdout);
    fprintf(stdout, "\x1B\\");
    sent_bytes += chunk_size;
  }
  fflush(stdout);
}

void kitty_display(uint32_t kitty_id, int row, int col, int X, int Y, int Z) {
  char s[64];
  int len =
      snprintf(s, sizeof(s), "\x1b_Ga=p,i=%u,q=2,X=%d,Y=%d,C=1,z=%d;\x1b\\",
               kitty_id, X, Y, Z);
  move_cursor(row, col);
  write_or_die(s, len, "kitty_display");
}

void kitty_clear(uint32_t kitty_id) {
  char s[64];
  int len = snprintf(s, sizeof(s), "\x1b_Ga=d,d=i,i=%u;\x1b\\", kitty_id);
  write_or_die(s, len, "kitty_clear");
}

void kitty_delete(uint32_t kitty_id) {
  char s[64];
  int len = snprintf(s, sizeof(s), "\x1b_Ga=d,d=I,i=%u;\x1b\\", kitty_id);
  write_or_die(s, len, "kitty_delete");
}
