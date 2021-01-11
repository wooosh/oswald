#include "cursor.hxx"
#include "terminal.hxx"

#include <stdio.h>
#include <unistd.h>

void editorProcessKeypress() {
  int c = Terminal::readKey();

  switch (c) {
  case CTRL_KEY('q'):
    exit(0);
    break;
  /*
  case '\r':
    Cursor::insertNewline();
    break;


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
  */
  }
}
