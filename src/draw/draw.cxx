#include "main.hxx"
#include "terminal/terminal.hxx"

#include <regex>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>
#include <unistd.h>

// https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
// TODO: move vt constants to terminal.cxx
// TODO: document file
// TODO: move render buffer/iterator into own file

// TODO: rename
struct renderIterator {
  // TODO: constructor
  std::list<Buffer>::iterator p;
  enum { FilenameRow, BufferRow, EmptyRow } rowType;
  // TODO: rename portionindex
  // -1 indicates we are on the filename label row
  ssize_t portionIndex;

  // TODO: add constructor for random y and start of portions
  void next() {
    if (p == E.portions.end())
      return;

    portionIndex++;
    if (portionIndex >= p->rows.size()) {
      portionIndex = -1;
      p++;
      if (p == E.portions.end()) {
        rowType = EmptyRow;
        return;
      }
      rowType = FilenameRow;
      return;
    }
    rowType = BufferRow;
  }
};

renderIterator renderIteratorFromOffset(size_t y) {
  size_t rowsTraversed = 0;
  for (auto it = E.portions.begin(); it != E.portions.end(); ++it) {
    // +1 is for filename row
    if (y <= rowsTraversed + it->rows.size() + 1) {
      renderIterator r;
      r.p = it;
      r.portionIndex = y - rowsTraversed - 1;
      if (r.portionIndex == -1) {
        r.rowType = renderIterator::FilenameRow;
      } else {
        r.rowType = renderIterator::BufferRow;
      }
      return r;
    }
    rowsTraversed += it->rows.size() + 1;
  }
  Terminal::die("offset outside of all rows");
}

size_t markToRenderY(Mark m) {
  // +1 is for file labels
  size_t accumulator = m.y + 1;
  for (auto it = E.portions.begin(); it != m.p; ++it) {
    accumulator += it->rows.size() + 1;
  }
  return accumulator;
}

#define TAB_STOP 8

size_t markToRenderX(Mark m) {
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
  // vertical scrolling
  size_t cy = markToRenderY(E.cursor);

  // cursor before view
  if (cy <= E.rowoff) {
    // move the start of the view down (-1 to account for status bar)
    E.rowoff = cy - 1;
  }

  // cursor after view
  if (cy >= E.rowoff + E.screenrows) {
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

// TODO: add to buffer using syntax highlighting API
// takes a line and the line number and returns a highlighted version
std::string highlightSelection(std::string line, size_t lineNum) {
  // TODO: refactor and precalculate selstart + selend
  // TODO: make sure the selection is visible on screen, and account for
  // horizontal scrolling since the line is cropped to fit the screen at this
  // point

  // make sure there is actually a selection range
  if (E.cursor.y == E.anchor.y && E.cursor.x == E.anchor.x)
    return line;

  Mark selStart = E.cursor;
  Mark selEnd = E.anchor;

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

#define TAB_STOP 8
// TODO: rename function, assimilate with highlighting and trimmming to screen
std::string renderLine(std::string line) {
  std::string render;

  for (size_t j = 0; j < line.length(); j++) {
    if (line[j] == '\t') {
      render += ' ';
      while (render.length() % TAB_STOP != 0)
        render += ' ';
    } else {
      render += line[j];
    }
  }

  return render;
}

void drawRows(std::ostream &out) {
  // TODO: change renderIterator to renderIteratorView and have it take a length
  // this way we can do a range for loop over the iterator

  // + 1 because the status bar is technically the first line
  renderIterator r = renderIteratorFromOffset(E.rowoff + 1);

  // text view starts at first line on the screen because of the status bar
  for (size_t y = 1; y < E.screenrows; y++) {
    if (y > 0) {
      out << "\r\n";
    }

    switch (r.rowType) {
    case renderIterator::FilenameRow: {
      // TODO: move escape codes to terminal.cxx
      // TODO: get the length of the filename using the render length instead of
      // the byte length minus one for left padding
      // TODO: consistent casing
      size_t line_remaining = E.screencols - r.p->filename.length() - 1;

      out << "\x1b[90m─" << r.p->filename;

      // TODO: this is gross
      for (int i=0; i<line_remaining; i++) {
        out << "─";
      }

      out << "\x1b[0m";
      break;
    }
    case renderIterator::EmptyRow:
      out << "~" << Terminal::clearToRight;
      break;
    case renderIterator::BufferRow:
      // TODO: row vs line terminology
      // TODO: refactor renderIterator into loop that calls functions to row
      // types
      std::string line = r.p->rows[r.portionIndex].raw;
      std::vector<HighlightType> hl = r.p->rows[r.portionIndex].hl;

      // TODO: use references for modifying the line
      line = renderLine(line);
      
      // TODO: crop function that handles adjusting syntax highlighting data when horiztonally scrolling
      // cut the line to fit on screen
      if (line.length() > 0 && line.length() > E.screencols) {
        line = line.substr(0, E.screencols);
      }

      /*
      hl.assign(line.length(), HighlightType::Normal);

      std::regex kw(" ?(if|else|for|while|do|switch|case|default|break|continue|struct|enum|union|return)( |;)");
      std::sregex_iterator riter(line.begin(), line.end(), kw);
      std::sregex_iterator end;
      for (auto i = riter; i != end; ++i) {
        std::smatch s = *i;
        for (auto j = s.position(); j < s.position() + s.length(); j++) {
          hl[j] = HighlightType::Keyword;
        }
      }*/

      // could probably use find_if_not
      HighlightType hlType = HighlightType::Normal;
      for (int i=0; i<hl.size(); i++) {
        if (hl[i] != hlType) {
          hlType = hl[i];
          switch (hlType) {
          case HighlightType::Selection:
            out << "\x1b[7m";
            break;
          case HighlightType::Keyword:
            out << "\x1b[31m";
            break;
          default:
            out << "\x1b[m";
          }
        }

        out << line[i];
      }

      out << "\x1b[m" << Terminal::clearToRight;
      break;
    }
    r.next();
  }
}

void drawStatus(std::ostream &out) {
  // TODO: trim to screen width
  std::string status = "file: " + E.cursor.p->filename + " line: " + std::to_string(E.cursor.y);

  size_t remaining = E.screencols - status.length() - 1;
  out << "\x1b[7m " << status << std::string(remaining, ' ') << "\x1b[m";
}

void editorRefreshScreen() {
  std::ostringstream out;
  editorScroll(out);

  // hide cursor and clear screen
  out << Terminal::hideCursor << Terminal::homeCursor;

  drawStatus(out);
  drawRows(out);

  // set cursor pos
  Terminal::setCursorPosition(out, markToRenderY(E.cursor) - E.rowoff,
                              markToRenderX(E.cursor) - E.coloff);

  // show cursor
  out << Terminal::showCursor;

  std::cout << out.str();
  std::cout.flush();
}
