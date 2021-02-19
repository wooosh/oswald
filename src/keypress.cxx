#include "main.hxx"
#include "terminal.hxx"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

void editorProcessKeypress() {
  key k = Terminal::readKey();

  switch (k.base) {
  case CTRL_KEY('q'):
    exit(0);
    break;

  case key::Backspace:
  case CTRL_KEY('h'):
    E.cursor.deleteBackward();
    break;

  case key::UpArrow:
    E.cursor.moveUp();
    break;
  case key::DownArrow:
    E.cursor.moveDown();
    break;
  case key::LeftArrow:
    E.cursor.moveLeft();
    break;
  case key::RightArrow:
    E.cursor.moveRight();
    break;

  case key::PageUp:
    if (--E.cursor.p == E.portions.end()) {
      --E.cursor.p;
    }
    E.cursor.x = 0;
    E.cursor.y = 0;
    break;
  case key::PageDown:
    if (++E.cursor.p == E.portions.end()) {
      ++E.cursor.p;
    }
    E.cursor.x = 0;
    E.cursor.y = 0;
    break;
    /*
      case MOD_ARROW_RIGHT: {
        size_t pos = E.cursor.row()->raw.find_first_of(" ()\"':", E.cursor.x);

        if (pos != std::string::npos) {
          E.cursor.x = pos + 1;
        }

        break;
      }

      case MOD_ARROW_LEFT: {
        if (E.cursor.x <= 0) break;
        size_t pos = E.cursor.row()->raw.find_last_of(" ()\"':", E.cursor.x-1);

        if (pos != std::string::npos) {
          E.cursor.x = pos;
        }

        break;
      }
    */
  case CTRL_KEY('l'):
  case '\x1b':
    break;

  default:
    E.cursor.insertChar(k.base);
    break;
  }

  if (!k.shift) {
    E.anchor = E.cursor;
  }
}
