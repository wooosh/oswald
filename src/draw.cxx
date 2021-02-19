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

// TODO: rename
typedef struct renderIterator {
  // TODO: constructor
  std::list<portion>::iterator p;
  enum {
    Filename,
    Buffer,
    Empty
  } rowType;
  // TODO: rename portionindex
  // -1 indicates we are on the filename label row
  ssize_t portionIndex;

  // TODO: add constructor for random y and start of portions
  void next() {
    if (p == E.portions.end()) return;

    portionIndex++;
    if (portionIndex >= p->rows.size()) {
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
    // +1 is for filename row
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

size_t markToRenderY(mark m) {
  // +1 is for file labels
  size_t accumulator = m.y + 1;
  for (auto it = E.portions.begin(); it != m.p; ++it) {
    accumulator += it->rows.size() + 1;
  }
  return accumulator;
}

#define TAB_STOP 8

size_t markToRenderX(mark m) {
  size_t rx = 0;
  for (size_t j = 0; j < m.x; j++) {
    if (m.p->rows[m.y].raw[j] == '\t')
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
  //E.rx = 0;
  // this should probably be an assert
  //if (E.cursor.y < E.row.size()) {
  //  E.rx = cxToRx(E.row[E.cy], E.cx);
  //}

  // TODO: refactor the next 4 blocks to be one function

  // vertical scrolling
  size_t cy = markToRenderY(E.cursor);

  // cursor before view
  if (cy < E.rowoff) {
    // move currently drawn rows down
    out << "\x1b[" << E.rowoff - cy << "T";
    
    renderIterator r = renderIteratorFromOffset(cy);
    // set the previously hidden rows to be rendered
    for (int i=cy; i<E.rowoff; i++) {
      if (r.rowType == renderIterator::Buffer) {
        r.p->rows[r.portionIndex].dirty = true;
      }
      r.next();
    }

    // move the view down
    E.rowoff = cy;
  }
  
  // cursor after view
  if (cy >= E.rowoff + E.screenrows) {
    // move currently drawn rows up
    out << "\x1b[" << cy - (E.rowoff + E.screenrows) << "S";

    // set the previously hidden rows to be rendered
    renderIterator r = renderIteratorFromOffset(E.rowoff + E.screenrows - 1);
    for (int i=E.rowoff + E.screenrows - 1; i<=cy; i++) {
      if (r.rowType == renderIterator::Buffer) {
        r.p->rows[r.portionIndex].dirty = true;
      }
      r.next();
    }

    // Move the view up
    E.rowoff = cy - E.screenrows + 1;
  }
/*
  // TODO: implement horizontal scrolling
  // horizontal scrolling
  if (E.rx < E.coloff) {
    E.coloff = E.rx;
  }
  if (E.rx >= E.coloff + E.screencols) {
    E.coloff = E.rx - E.screencols + 1;
  }
*/
}

// takes a line and the line number and returns a highlighted version
std::string highlightLine(std::string line, size_t lineNum) {
  // TODO: refactor and precalculate selstart + selend
  // TODO: make sure the selection is visible on screen, and account for
  // horizontal scrolling since the line is cropped to fit the screen at this
  // point
      
  // make sure there is actually a selection range
  if (E.cursor.y == E.anchor.y && E.cursor.x == E.anchor.x)
    return line;

  mark selStart = E.cursor;
  mark selEnd = E.anchor;

  if (selStart.y > selEnd.y) {
    selStart = E.anchor;
    selEnd = E.cursor;
  } else if (selStart.y == selEnd.y && selStart.x > selEnd.x) {
    selStart = E.anchor;
    selEnd = E.cursor;
  }
  
  // check if this line is in the selection bounds
  if (lineNum < selStart.y || lineNum > selEnd.y)
    return line;

  size_t highlightStart = 0;
  size_t highlightEnd = line.length();

  if (selStart.y == lineNum) {
    highlightStart = selStart.x;
  }

  if (selEnd.y == lineNum) {
    highlightEnd = selEnd.x;
  }

  // insert control characters to highlight segment
  // TODO: move control charcters into constants in terminal.cxx

  // we insert the ending character first so we don't have to worry about
  // the string positions changing, since the end position will always be
  // greater than the start position
  line.insert(highlightEnd, "\x1b[m");
  line.insert(highlightStart, "\x1b[7m");

  return line;
}

void drawRows(std::ostream &out) {
  // TODO: change renderIterator to renderIteratorView and have it take a length
  // this way we can do a range for loop over the iterator
  renderIterator r = renderIteratorFromOffset(E.rowoff);

  for (size_t y = 0; y < E.screenrows; y++) {
    if (y > 0) {
      out << "\r\n";
    }

    switch (r.rowType) {
    case renderIterator::Filename: {
      // TODO: move escape codes to terminal.cxx
      // TODO: get the length of the filename using the render length instead of the byte length
      // minus one for left padding
      size_t line_remaining = E.screencols - r.p->filename.length() - 1;
      out << "\x1b[7m " << r.p->filename << std::string(line_remaining, ' ') << "\x1b[0m";
      break;
    }
    case renderIterator::Empty:
      out << "~" << Terminal::clearToRight;
      break;
    case renderIterator::Buffer:
      // cut the line to fit on screen
      std::string line = r.p->rows[r.portionIndex].render;
      if (line.length() > 0 && line.length() > E.screencols) {
        line = line.substr(0, E.screencols);
      }
      
      line = highlightLine(line, r.portionIndex);

      out << line << Terminal::clearToRight;
      break;
    }
    r.next();
  }

}

void editorRefreshScreen() {
  std::ostringstream out;
  editorScroll(out);


  // hide cursor and clear screen
  out << Terminal::hideCursor << Terminal::homeCursor;

  drawRows(out);

  // set cursor pos
  Terminal::setCursorPosition(out, markToRenderY(E.cursor) - E.rowoff, markToRenderX(E.cursor) - E.coloff);

  // show cursor
  out << Terminal::showCursor;

  std::cout << out.str();
  std::cout.flush();
}
