#include <unistd.h>
#include <stdio.h>

#include "terminal.hxx"

void editorProcessKeypress() {
  int c = Terminal::readKey();

  switch (c) {
  case '\r':
    editorInsertNewline();
    break;

  case CTRL_KEY('q'):
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    exit(0);
    break;

  case HOME_KEY:
    E.cx = 0;
    break;

  case END_KEY:
    if (E.cy < E.row.size())
      E.cx = E.row[E.cy].raw.length();
    break;

  // TODO: support delete key
  case BACKSPACE:
  case CTRL_KEY('h'):
    editorDelChar();
    break;

  case ARROW_UP:
    cursorUp();
    break;
  case ARROW_DOWN:
    cursorDown();
    break;
  case ARROW_LEFT:
    cursorLeft();
    break;
  case ARROW_RIGHT:
    cursorRight();
    break;

  case CTRL_KEY('l'):
  case '\x1b':
    break;

  default:
    editorInsertChar(c);
    break;
  }
}