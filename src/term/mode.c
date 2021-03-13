#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <term/mode.h>
#include <term/escapes.h>

static struct termios old_termios;

void term_setup() {
  if (tcgetattr(STDIN_FILENO, &old_termios) == -1) {
    term_die("tcgettattr");
  }

  atexit(term_restore);

  // TODO: write descriptions for each flag
  struct termios new_termios = old_termios;
  new_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  new_termios.c_oflag &= ~(OPOST);
  new_termios.c_cflag |= (CS8);
  new_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  new_termios.c_cc[VMIN] = 0;
  new_termios.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios) == -1) {
    term_die("tcsetattr");
  }

  printf("%s%s", term_save_buffer, term_line_cursor);
}

static const char* error_message = NULL;
static int die_errno;
void term_die(const char *msg) {
  // preserve errno because term_restore can overwrite errno before we are 
  // able to print the error message
  die_errno = errno;
  error_message = msg;
  exit(1);
}

void term_restore() {
  // we don't check the return value here because there is no way to recover,
  // and calling die and exiting would result in undefined behavior becuase
  // we are in an atexit handler
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_termios);

  printf("%s%s", term_restore_buffer, term_block_cursor);

  if (error_message) {
    errno = die_errno;
    perror(error_message);
  }
}

