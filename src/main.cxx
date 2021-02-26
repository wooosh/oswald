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

// FEATURE: unicode support
// FEATURE: portions
// FEATURE: config file
// FEATURE: delete key
// FEATURE: marks
// FEATURE: selections
// FEATURE: cursor column memory when moving cursor vertically
// FEATURE: non-printable characters
// FEATURE: update ui on resize
// FEATURE: keybind to build/debug using fifo
// FEATURE: handle resizing
// FEATURE: non LSP code formatting/on save commands
// FEATURE: line numbers
// FEATURE: LSP
// FEATURE: tests
// FEATURE: grep -r
// FEATURE: undo and redo
// FEATURE: unicode chars for special characters like control codes
// FEATURE: modal
// FEATURE: remote terminal control for build commands
// FEATURE: control down=normal mode, control up = insert mode

// TODO: open files
// TODO: fix casing
// TODO: remove char*
// TODO: remove printfs
// TODO: change write(stdin_fileno)
// TODO: https://en.cppreference.com/w/cpp/language/range-for
// TODO: review comments
// TODO: clean up defines
struct Editor E;

// TODO: move to render.cxx and change code that calls this function to mark the
// row as dirty
#define TAB_STOP 8
void Row::updateRender() {
  this->render.clear();
  this->dirty = true;

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
