#include "main.hxx"

namespace Cursor {

static void clampCursorX() {
  // TODO: cleanup
  // set cursor x to not be outside the current row's text
  erow *row = (E.cy >= E.row.size()) ? NULL : &E.row[E.cy];
  int rowlen = row ? row->raw.length() : 0;
  if (E.cx > rowlen) {
    E.cx = rowlen;
  }
}

void moveLeft() {
  if (E.cx != 0) {
      E.cx--;
    } else if (E.cy > 0) {
      E.cy--;
      E.cx = E.row[E.cy].raw.length();
    }
    // TODO: do we really need to clamp the cursor here
  clampCursorX();
}

void moveRight() {
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

void moveUp() {
  if (E.cy != 0) {
      E.cy--;
    }
    clampCursorX();
}

void moveDown() {
  if (E.cy < E.row.size()) {
      E.cy++;
    }
    clampCursorX();
}

void deleteBackward() {
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

void insertNewline() {
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

void insertChar(int c) {
  if (E.cy == E.row.size()) {
    editorInsertRow(E.row.size(), "");
  }
  E.row[E.cy].editorRowInsertChar(E.cx, c);
  E.cx++;
}
}