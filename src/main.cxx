#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "keypress.hxx"
#include "main.hxx"
#include "row.hxx"
#include "terminal.hxx"

// FEATURE: config file
// FEATURE: unicode support
// FEATURE: portions
// FEATURE: cursor column memory when moving cursor vertically
// FEATURE: non-printable characters
// FEATURE: selections
// FEATURE: update ui on resize
// FEATURE: LSP
// FEATURE: keybind to build/debug using fifo
// FEATURE: handle resizing

// TODO: review comments
// TODO: clean up defines
#define KILO_VERSION "0.0.1"
#define KILO_TAB_STOP 8
#define KILO_QUIT_TIMES 3

struct editorConfig E;

// TODO: move status message into own file and namespace
void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen();

void editorSetStatusMessage(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
  va_end(ap);
  E.statusmsg_time = time(NULL);
}

void initEditor() {
  E.cx = 0;
  E.cy = 0;
  E.rx = 0;
  E.rowoff = 0;
  E.coloff = 0;
  E.dirty = 0;
  E.filename = NULL;
  E.statusmsg[0] = '\0';
  E.statusmsg_time = 0;
  E.row.insert(E.row.begin(), (erow){});
  if (Terminal::getWindowSize(&E.screenrows, &E.screencols) == -1)
    Terminal::die("getWindowSize");
  E.screenrows -= 2;
}

int main(int argc, char *argv[]) {
  Terminal::enableRawMode();
  initEditor();

  editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");

  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}
