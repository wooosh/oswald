#include "main.hxx"
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


*/
  case BACKSPACE:
  case CTRL_KEY('h'):
    E.cursor.deleteBackward();
    break;
  case ARROW_UP:
    E.cursor.moveUp();
    break;
  case ARROW_DOWN:
    E.cursor.moveDown();
    break;
  case ARROW_LEFT:
    E.cursor.moveLeft();
    break;
  case ARROW_RIGHT:
    E.cursor.moveRight();
    break;
  case PAGE_UP:
    if (--E.cursor.p == E.portions.end()) {
      --E.cursor.p;
    }
    E.cursor.x = 0;
    E.cursor.y = 0;
    break;
  case PAGE_DOWN:
    if (++E.cursor.p == E.portions.end()) {
      ++E.cursor.p;
    }
    E.cursor.x = 0;
    E.cursor.y = 0;
    break;
  case CTRL_KEY('l'):
  case '\x1b':
    break;
  
  default:
    E.cursor.insertChar(c);
    break;
  }
}
