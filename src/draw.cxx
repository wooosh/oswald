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


/*
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
    for (int i=E.rowoff + E.screenrows - 1; i<=E.cy; i++) {
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
*/

// TODO: rename
typedef struct renderIterator {
  // TODO: constructor
  std::list<portion>::iterator p;
  enum {
    Filename,
    Buffer,
    Empty
  } rowType;
  // -1 indicates we are on the filename label row
  ssize_t portionIndex;

  // TODO: add constructor for random y and start of portions
  void next() {
    if (p == E.portions.end()) return;

    portionIndex++;
    if (portionIndex > p->rows.size()) {
      portionIndex = -1;
      p++;
      if (p == E.portions.end()) {
        rowType = Empty;
        return;
      }
      rowType = Filename;
      return;
    }
    rowType = Buffer;
  }
} renderIterator;

renderIterator renderIteratorFromOffset(size_t y) {
  size_t rowsTraversed = 0;
  for (auto it=E.portions.begin(); it!=E.portions.end(); ++it) {
    // +1 includes filename row
    if (y <= rowsTraversed + it->rows.size() + 1) {
      renderIterator r;
      r.p = it;
      r.portionIndex = y - rowsTraversed - 1;
      if (r.portionIndex == -1) {
        r.rowType = renderIterator::Filename;
      } else {
        r.rowType = renderIterator::Buffer;
      }
      return r;
    }
    rowsTraversed += it->rows.size() + 1;
  }
  Terminal::die("offset outside of all rows");
}

void drawRows(std::ostream &out) {
  renderIterator r = renderIteratorFromOffset(E.rowoff);
  //r.p = E.portions.begin();
  //r.portionIndex = -1;
  //r.rowType = renderIterator::Filename;

  for (size_t y = 0; y < E.screenrows - 5; y++) {
    if (y > 0) {
      out << "\r\n";
    }

    switch (r.rowType) {
    case renderIterator::Filename:
      out << "\x1b[34ma" << r.p->filename << "\x1b[0m"  << Terminal::clearToRight;
      break;
    case renderIterator::Empty:
      out << "~" << Terminal::clearToRight;
      break;
    case renderIterator::Buffer:
      out << "a";
      break;
    }
    r.next();
  }

}

/*
renderCursor renderYToIterator(size_t y) {
  renderIterator r;
  r.p = E.portions.begin();
}*/

// TODO: store filename rows somewhere persistent
// TODO: remove editor prefix
/*
void editorDrawRows(std::ostream &out) {
  renderIterator r;
  r.p = E.portions.begin();

  for (size_t y = 0; y < E.screenrows; y++) {
    if (y != 0) {
      out << "\r\n";;
    }
    
    // linePos.moveDown(true - cross portion)
    // row drawRow = linePos.row()
    
    portionRowIndex++;
    if (portionRowIndex 

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
}*/

void editorRefreshScreen() {
  std::ostringstream out;
  //editorScroll(out);


  // hide cursor and clear screen
  out << Terminal::hideCursor << Terminal::homeCursor;

  drawRows(out);

  // set cursor pos
  //Terminal::setCursorPosition(out, E.cy - E.rowoff, E.rx - E.coloff);

  // show cursor
  out << Terminal::showCursor;

  std::cout << out.str();
  std::cout.flush();
}
