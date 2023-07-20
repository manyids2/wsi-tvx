#include "constants.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define CTRL_KEY(k) ((k)&0x1f)
enum keys_e { ARROW_LEFT = 1000, ARROW_RIGHT, ARROW_UP, ARROW_DOWN };

void disable_raw_mode(void);
void enable_raw_mode(void);

int get_keypress(void);
int get_window_size(int *rows, int *cols, int *vw, int *vh);
void get_window_size_kitty(int *vw, int *vh);

void clear_screen(void);
void clear_text(void);
void hide_cursor(void);
void show_cursor(void);
void move_cursor(int row, int col);
