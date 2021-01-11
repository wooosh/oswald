#include "main.hxx"
#include "terminal.hxx"

#include <cstring>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unistd.h>

// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
// TODO: move vt constants to terminal.cxx
// TODO: document file
// TODO: statusbar
// - use scrolling region DECSTBM for statusbar 

#define TAB_STOP 8
size_t cxToRx(erow e, size_t cx) {
  size_t rx = 0;
  for (size_t j = 0; j < cx; j++) {
    if (e.raw[j] == '\t')
      rx += (TAB_STOP - 1) - (rx % TAB_STOP);
    rx++;
  }
  return rx;
}


// scroll to fit cursor in screen and keep file in bounds
void editorScroll(std::ostream &out) {
  // TODO: prevent the cursor from being on non-existent rows
  // TODO: move cxToRx stuff to the main rendering function
  // TODO: rename cxToRx to calculateDrawnCursorPos and make it take into account non existent rows
  // If the cursor is on a non-existent row, set it to zero, otherwise, calulcate the rx based on the actual cursor pos
  E.rx = 0;
  if (E.cy < E.row.size()) {
    E.rx = cxToRx(E.row[E.cy], E.cx);
  }

  // TODO: refactor the next 4 blocks to be one function

  // vertical scrolling
  // cursor before view
  if (E.cy < E.rowoff) {
    // move currently drawn rows down
    out << "\x1b[" << E.rowoff - E.cy << "T";
    
    // set the previously hidden rows to be rendered
    for (int i=E.cy; i<E.rowoff; i++) {
      E.row[i].dirty = true;
    }

    // move the view down
    E.rowoff = E.cy;
  }

  // cursor after view
  if (E.cy >= E.rowoff + E.screenrows) {
    // move currently drawn rows up
    out << "\x1b[" << E.cy - (E.rowoff + E.screenrows) << "S";

    // set the previously hidden rows to be rendered
    for (int i=E.rowoff + E.screenrows - 1; i<E.cy; i++) {
      E.row[i].dirty = true;
    }

    // Move the view up
    E.rowoff = E.cy - E.screenrows + 1;
  }

  // TODO: implement horizontal scrolling
  // horizontal scrolling
  if (E.rx < E.coloff) {
    E.coloff = E.rx;
  }
  if (E.rx >= E.coloff + E.screencols) {
    E.coloff = E.rx - E.screencols + 1;
  }
}

// TODO: remove editor prefix
void editorDrawRows(std::ostream &out) {
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
  std::ostringstream out;
  editorScroll(out);


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
