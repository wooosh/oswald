#include "terminal.hxx"
#include "main.hxx"

#include <ostream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

static struct termios origTermios;
static const char* errorMessage = NULL;
static int dieErrno;

namespace Terminal {

const std::string clearToRight = "\x1b[K";
const std::string homeCursor = "\x1b[H";
const std::string hideCursor = "\x1b[?25l";
const std::string showCursor = "\x1b[?25h";

void die(const char* msg) {
  dieErrno = errno;
  errorMessage = msg;
  exit(1);
}

void cleanup() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &origTermios) == -1)
    die("tcsetattr");

  // restore screen contents
  std::cout << "\x1b[?1049l";

  // switch back to block cursor
  std::cout << "\x1b[\x32 q";

  if (errorMessage) {
    errno = dieErrno;
    perror(errorMessage);
  }
}

void setup() {
  if (tcgetattr(STDIN_FILENO, &origTermios) == -1)
    die("tcgetattr");
  atexit(cleanup);

  struct termios raw = origTermios;
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");

  // switch to application buffer (preserves the contents of the terminal before the editor was started)
  std::cout << "\x1b[?1049h";

  // switch to line cursor
  std::cout << "\x1b[\x35 q";
}


void vtSingleChar(key* k, char c) {
  switch(c) {
  case 'A':
    k->base = key::UpArrow;
    return;
  case 'B':
    k->base = key::DownArrow;
    return;
  case 'C':
    k->base = key::RightArrow;
    return;
  case 'D':
    k->base = key::LeftArrow;
    return;
  case 'H':
    k->base = key::Home;
    return;
  case 'F':
    k->base = key::End;
    return;
  }
}

static char seq[6];
static size_t available = 0;
key readKey() {
  // TODO: error handling
  // refill buffer
  size_t result = read(STDIN_FILENO, seq+available, 6-available);
  if (result < 1) {
    Terminal::die("read error");
  }
  available += result;
  size_t used = 0;

  key k;
  k.control = false;
  k.alt = false;
  k.shift = false;

  if (seq[0] == '\x1b') {
    if (available >= 3 && seq[1] == '[') {
      if (available >= 4 && seq[2] >= '0' && seq[2] <= '9') {
        if (seq[3] == '~') {
          switch (seq[2]) {
          case '3':
            k.base = key::Delete;
            break;
          case '5':
            k.base = key::PageUp;
            break;
          case '6':
            k.base = key::PageDown;
            break;
          }
          used = 4;
        } else if (available >= 6 && seq[2] == '1' && seq[2] == ';') {
          // combines alt and control currently
          switch(seq[4]) {
          case '2':
            k.shift = true;
            break;
          case '3':
            k.alt = true;
            break;
          case '4':
            k.shift = true;
            k.alt = true;
            break;
          case '5':
            k.control = true;
            break;
          case '6':
            k.control = true;
            k.shift = true;
            break;
          case '7':
            k.control = true;
            k.alt = true;
            break;
          case '8':
            k.control = true;
            k.alt = true;
            k.shift = true;
            break;
          }
          vtSingleChar(&k, seq[5]);
          used = 6;
        }
      } else if (available >= 2) { 
        vtSingleChar(&k, seq[2]);
        used = 3;
      }
    } 
  } else {
    // TODO: handle control normalization
    k.base = (key::keyBase) seq[0];
    used = 1;
  }

  memmove(seq, seq+used, available - used);
  available -= used;
  return k;
}


void setCursorPosition(std::ostream &out, std::size_t row, size_t col) {
  out << "\x1b[" << row + 1 << ";" << col + 1 << "H";
}

int getCursorPosition(size_t *rows, size_t *cols) {
  char buf[32];
  unsigned int i = 0;

  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
    return -1;

  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1)
      break;
    if (buf[i] == 'R')
      break;
    i++;
  }
  buf[i] = '\0';

  if (buf[0] != '\x1b' || buf[1] != '[')
    return -1;
  if (sscanf(&buf[2], "%zu;%zu", rows, cols) != 2)
    return -1;

  return 0;
}

// @Todo: rewrite to store window size in editor struct on window resize
int getWindowSize(size_t *rows, size_t *cols) {
  struct winsize ws;

  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
      return -1;
    return getCursorPosition(rows, cols);
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}
} // namespace Terminal
