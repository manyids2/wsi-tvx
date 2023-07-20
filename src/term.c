#include "term.h"

struct termios orig_termios;

void clear_text(void) {
  write_or_die("\x1b[0;0H\x1b[0J\x1b[1J", 14, "clear_text");
}
void clear_screen(void) { write_or_die("\x1b[2J", 4, "clear_screen"); }
void hide_cursor(void) { write_or_die("\x1b[?25l", 6, "hide_cursor"); }
void show_cursor(void) { write_or_die("\x1b[?25h", 6, "show_cursor"); }
void move_cursor(int row, int col) {
  char s[32];
  int len = snprintf(s, sizeof(s), "\x1b[%d;%dH", row, col);
  write_or_die(s, len, "move_cursor");
}

void disable_raw_mode(void) {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");

  // reset and show cursor
  clear_screen();
  move_cursor(0, 0);
  show_cursor();
}

void enable_raw_mode(void) {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");
  atexit(disable_raw_mode); // Then is always called on exit
  struct termios raw = orig_termios;
  raw.c_iflag &= ~(ICRNL | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 1;
  raw.c_cc[VTIME] = 0; // NOTE: blocking, allows only single character input
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

void get_window_size(int *rows, int *cols, int *vw, int *vh) {
  struct winsize ws;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1)
    die("ioctl(fd, TIOCGWINSZ, &ws) failed.");

  if (ws.ws_col == 0)
    die("Unsupported terminal.");

  *cols = ws.ws_col;
  *rows = ws.ws_row;
  *vw = ws.ws_xpixel;
  *vh = ws.ws_ypixel;
}

void get_window_size_kitty(int *vw, int *vh) {
  // Read window size using kitty
  write_or_die("\x1b[14t", 5, "get_window_size_kitty");

  // Read response
  char str[16];
  int ch, n = 0;
  int p1 = -1;
  int p2 = -1;

  // Read bytes one by one and parse
  int nread;
  while ((nread = read(STDIN_FILENO, &ch, 1)) != 1) {
    // handle error
    if (nread == -1 && errno != EAGAIN)
      die("read");

    // save char to string
    str[n] = ch;

    // check break conditions
    if (ch == 't') {
      break;
    }
    if (ch == ';') {
      p1 == -1 ? (p1 = n) : (p2 = n);
    }
    n++;
  }

  // Parse height and width
  char str_w[10];
  char str_h[10];
  slice(str, str_h, p1 + 1, p2);
  slice(str, str_w, p2 + 1, n);
  *vw = atoi(str_w);
  *vh = atoi(str_h);
}
