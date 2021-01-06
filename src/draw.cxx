#include "main.hxx"
#include "row.hxx"
#include "terminal.hxx"

#include <cstring>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unistd.h>

// TODO: drawing optimizations
// - only redraw lines that have been changed
//   - check for a dirty flag on each row, then print
//   - if a row doesn't have dirty flags, the next dirty row will have to set the cursor pos
// - allow horziontal scrolling to jump by a certain amount of characters because it requires a redraw of the whole buffer
// - when scrolling, use the \x1bD and \x1bM to scroll up and down one line, then redraw status bar and the remaining one line
// - do scrolling first, then draw rows

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
    if (y != 0) {
      out << "\r\n";;
    }
    size_t filerow = y + E.rowoff;
    if (filerow >= E.row.size()) {
      out << "~";
      out << Terminal::clearToRight;
    } else if (E.row[filerow].dirty) {
      ssize_t len = E.row[filerow].render.length() - E.coloff;
      if (len > 0) {
        // TODO: fix signed unsigned comparison
        if (len > E.screencols)
          len = E.screencols;
        out << E.row[filerow].render.substr(E.coloff, len);
      }
      out << Terminal::clearToRight;
      E.row[filerow].dirty = false;
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
