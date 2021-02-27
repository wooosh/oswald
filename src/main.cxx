#include "main.hxx"
#include "draw/draw.hxx"
#include "terminal/keypress.hxx"
#include "buffer/buffer.hxx"
#include "terminal/terminal.hxx"

#include <fcntl.h>
#include <iostream>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

struct Editor E;

// TODO: move to render.cxx and change code that calls this function to mark the
// row as dirty
// TODO: check if we can avoid precalculating this
#define TAB_STOP 8
void Row::updateRender() {
  this->dirty = false;
  this->render.clear();

  for (size_t j = 0; j < this->raw.length(); j++) {
    if (this->raw[j] == '\t') {
      this->render += ' ';
      while (this->render.length() % TAB_STOP != 0)
        this->render += ' ';
    } else {
      this->render += this->raw[j];
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    openScratchPortion();
  } else {
    for (int i = 1; i < argc; i++) {
      bool success = openFilePortion(argv[i]);

      if (!success) {
        std::cout << "Cannot open file: " << argv[i] << std::endl;
        return 1;
      }
    }
  }

  E.cursor.p = E.portions.begin();
  E.cursor.x = 0;
  E.cursor.y = 0;

  E.anchor = E.cursor;

  Terminal::setup();

  if (Terminal::getWindowSize(&E.screenrows, &E.screencols) == -1) {
    Terminal::die("getWindowSize");
  }

  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}
