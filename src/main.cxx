#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>

#include "main.hxx"
#include "row.hxx"
#include "terminal.hxx"
#include "keypress.hxx"

/*** defines ***/

#define KILO_VERSION "0.0.1"
#define KILO_TAB_STOP 8
#define KILO_QUIT_TIMES 3

struct editorConfig E;

/*** prototypes ***/

void editorSetStatusMessage(const char *fmt, ...);
void editorRefreshScreen();

/*** editor operations ***/

void editorInsertChar(int c) {
  if (E.cy == E.row.size()) {
    editorInsertRow(E.row.size(), "");
  }
  E.row[E.cy].editorRowInsertChar(E.cx, c);
  E.cx++;
}

void editorInsertNewline() {
  if (E.cx == 0) {
    editorInsertRow(E.cy, "");
  } else {
    // we need to split the current row into two
    erow *row = &E.row[E.cy];
    editorInsertRow(E.cy + 1, row->raw.substr(E.cx, row->raw.length() - E.cx));
    row = &E.row[E.cy];
    row->raw.erase(row->raw.begin() + E.cx, row->raw.end());
    row->editorUpdateRow();
  }
  E.cy++;
  E.cx = 0;
}

void editorDelChar() {
  if (E.cy == E.row.size())
    return;
  if (E.cx == 0 && E.cy == 0)
    return;

  erow *row = &E.row[E.cy];
  if (E.cx > 0) {
    row->editorRowDelChar(E.cx - 1);
    E.cx--;
  } else {
    E.cx = E.row[E.cy - 1].raw.length();
    E.row[E.cy - 1].editorRowAppendString(row->raw);
    editorDelRow(E.cy);
    E.cy--;
  }
}

void editorSetStatusMessage(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(E.statusmsg, sizeof(E.statusmsg), fmt, ap);
  va_end(ap);
  E.statusmsg_time = time(NULL);
}

void clampCursorX() {
  // TODO: cleanup
  // set cursor x to not be outside the current row's text
  erow *row = (E.cy >= E.row.size()) ? NULL : &E.row[E.cy];
  int rowlen = row ? row->raw.length() : 0;
  if (E.cx > rowlen) {
    E.cx = rowlen;
  }
}

void cursorLeft() {
  if (E.cx != 0) {
      E.cx--;
    } else if (E.cy > 0) {
      E.cy--;
      E.cx = E.row[E.cy].raw.length();
    }
    // TODO: do we really need to clamp the cursor here
  clampCursorX();
}

void cursorRight() {
  erow *row = (E.cy >= E.row.size()) ? NULL : &E.row[E.cy];
  if (row && E.cx < row->raw.length()) {
      E.cx++;
    } else if (row && E.cx == row->raw.length()) {
      E.cy++;
      E.cx = 0;
    }
    // TODO: do we really need to clamp the cursor here
    clampCursorX();
}

void cursorUp() {
  if (E.cy != 0) {
      E.cy--;
    }
    clampCursorX();
}

void cursorDown() {
  if (E.cy < E.row.size()) {
      E.cy++;
    }
    clampCursorX();
}

/*** init ***/

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
