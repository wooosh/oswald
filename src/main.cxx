#include "main.hxx"
#include "keypress.hxx"
#include "row.hxx"
#include "terminal.hxx"

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

// TODO: fix casing
// TODO: remove char*
// TODO: remove printfs
// TODO: change write(stdin_fileno)
// TODO: https://en.cppreference.com/w/cpp/language/range-for
// TODO: review comments
// TODO: clean up defines
struct Editor E;

// TODO: move status message into own file and namespace
void editorRefreshScreen();

void initEditor() {
  Terminal::setup();
  
  E.row.insert(E.row.begin(), (erow){});
  
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
