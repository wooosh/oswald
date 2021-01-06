#include "main.hxx"
#include "row.hxx"
#include "terminal.hxx"

#include <cstring>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unistd.h>

#define KILO_VERSION "0.0.1"

// scroll to fit cursor in screen and keep file in bounds
void editorScroll() {
  E.rx = 0;
  if (E.cy < E.row.size()) {
    E.rx = E.row[E.cy].cxToRx(E.cx);
  }

  if (E.cy < E.rowoff) {
    E.rowoff = E.cy;
  }
  if (E.cy >= E.rowoff + E.screenrows) {
    E.rowoff = E.cy - E.screenrows + 1;
  }
  if (E.rx < E.coloff) {
    E.coloff = E.rx;
  }
  if (E.rx >= E.coloff + E.screencols) {
    E.coloff = E.rx - E.screencols + 1;
  }
}

// TODO: remove editor prefix
void editorDrawRows(std::ostringstream &out) {
  for (size_t y = 0; y < E.screenrows; y++) {
    out << "\r\n" << Terminal::clearLine;
    size_t filerow = y + E.rowoff;
    if (filerow >= E.row.size()) {
      out << "~";
    } else {
      ssize_t len = E.row[filerow].render.length() - E.coloff;
      if (len > 0) {
        // TODO: fix signed unsigned comparison
        if (len > E.screencols)
          len = E.screencols;
        out << E.row[filerow].render.substr(E.coloff, len);
      }
    }
  }
}

void editorRefreshScreen() {
  // TODO: change to streams
  editorScroll();

  std::ostringstream out;

  // hide cursor and clear screen
  out << Terminal::hideCursor << Terminal::homeCursor;

  editorDrawRows(out);

  // set cursor pos
  Terminal::setCursorPosition(out, E.cy - E.rowoff, E.rx - E.coloff);

  // show cursor
  out << Terminal::showCursor;

  std::cout << out.str();
  std::cout.flush();
}
