#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <term/mode.h>

#include <term/escapes.h>
#include <main.h>
#include <x.h>

enum TermState { TermRaw, TermCooked };

static enum TermState term_state = TermCooked;
static struct termios old_termios;

void term_setup() {
  xassert_errno(tcgetattr(STDIN_FILENO, &old_termios) != -1, NULL);

  atexit(term_restore);

  // TODO|CLEANUP: write descriptions for each flag
  // https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
  struct termios new_termios = old_termios;
  new_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  new_termios.c_oflag &= ~(OPOST);
  new_termios.c_cflag |= (CS8);
  new_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  new_termios.c_cc[VMIN] = 0;
  new_termios.c_cc[VTIME] = 1;

  xassert_errno(tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios) != -1, NULL);

  printf("%s%s%s%s", term_save_buffer, term_line_cursor, term_set_pos(0, 0),
         term_clear_screen);
  fflush(stdout);

  term_state = TermRaw;
  
  // TODO|FEATURE: handle resizing
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  E.screen_width = w.ws_col;
  E.screen_height = w.ws_row;
}

void term_restore() {
  if (term_state == TermRaw) {
    // we don't check the return value here because there is no way to recover,
    // and calling die and exiting would result in undefined behavior becuase
    // we are in an atexit handler
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_termios);
    printf("%s%s%s", term_restore_buffer, term_block_cursor, term_show_cursor);
  }
}
