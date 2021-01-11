#include "main.hxx"
#include "keypress.hxx"
#include "terminal.hxx"
#include "draw.hxx"

#include <fcntl.h>
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
// FEATURE: save screen buffer
// FEATURE: grep -r
// FEATURE: undo and redo
// FEATURE: unicode chars for special characters like control codes

// TODO: open files
// TODO: fix casing
// TODO: remove char*
// TODO: remove printfs
// TODO: change write(stdin_fileno)
// TODO: https://en.cppreference.com/w/cpp/language/range-for
// TODO: review comments
// TODO: clean up defines
struct Editor E;

// TODO: move to render.cxx and change code that calls this function to mark the row as dirty
#define TAB_STOP 8
void erow::updateRender() {
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

void initEditor() {
  Terminal::setup();
  
  auto p = E.portions.insert(E.portions.begin(), (portion){"default"});
  p->rows.push_back((erow){"hi", "hi", true});
  p->rows.push_back((erow){"hello", "hello", true});
  //E.row.insert(E.row.begin(), (erow){});
  E.cursor.p = p;
  E.cursor.x = 3;
  E.cursor.y = 1;
  

  if (Terminal::getWindowSize(&E.screenrows, &E.screencols) == -1) {
    Terminal::die("getWindowSize");
  }
}

int main(int argc, char *argv[]) {
  initEditor();

  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}
