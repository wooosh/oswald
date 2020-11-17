#include <unistd.h>
#include <stdio.h>

#include "terminal.hxx"
#include "cursor.hxx"

void editorProcessKeypress() {
  int c = Terminal::readKey();

  switch (c) {
  case '\r':
    Cursor::insertNewline();
    break;

  case CTRL_KEY('q'):
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    exit(0);
    break;

  /*
  case HOME_KEY:
    E.cx = 0;
    break;

  case END_KEY:
    if (E.cy < E.row.size())
      E.cx = E.row[E.cy].raw.length();
    break;
  */

  // TODO: support delete key
  case BACKSPACE:
  case CTRL_KEY('h'):
    Cursor::deleteBackward();
    break;

  case ARROW_UP:
    Cursor::moveUp();
    break;
  case ARROW_DOWN:
    Cursor::moveDown();
    break;
  case ARROW_LEFT:
    Cursor::moveLeft();
    break;
  case ARROW_RIGHT:
    Cursor::moveRight();
    break;

  case CTRL_KEY('l'):
  case '\x1b':
    break;

  default:
    Cursor::insertChar(c);
    break;
  }
}