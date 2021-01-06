#include "main.hxx"
#include "row.hxx"
#include "terminal.hxx"

#include <cstring>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unistd.h>

// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
// TODO: move vt constants to terminal.cxx
// TODO: use scrolling region DECSTBM 
// TODO: switch function arguments to ostream instead of ostringstream

// TODO: drawing optimizations
// - allow horziontal scrolling to jump by a certain amount of characters because it requires a redraw of the whole buffer
// - flag that triggers a full rerender, useful for horizontal scrolling and resizing
// - when scrolling, use the \x1bD and \x1bM to scroll up and down one line, then redraw status bar and the remaining one line
// - do scrolling first, then draw rows

// scroll to fit cursor in screen and keep file in bounds
void editorScroll(std::ostream &out) {
  // Prevent the cursor from going outside of the end of the line
  E.rx = 0;
  if (E.cy < E.row.size()) {
    E.rx = E.row[E.cy].cxToRx(E.cx);
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
    for (int i=E.rowoff + E.screenrows; i<E.cy - E.screenrows + 1; i++) {
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
