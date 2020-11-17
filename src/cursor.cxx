#include "main.hxx"

namespace Cursor {
// TODO: cursor column memory when moving up rows
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
  //erow *row = (E.cy >= E.row.size()) ? NULL : &E.row[E.cy];
  erow *row = &E.row[E.cy];
  if (E.cx < row->raw.length()) {
      E.cx++;
    } else if (E.cy + 1 < E.row.size()) {
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
  if (E.cy + 1 < E.row.size()) {
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
  // find out if deleting a character requires merging two lines
  if (E.cx > 0) {
    E.cx--;
    row->raw.erase(row->raw.begin()+E.cx);
    row->updateRender();
    E.dirty = true;
  } else {
    E.cx = E.row[E.cy - 1].raw.length();
    
    E.row[E.cy - 1].raw += row->raw;
    E.row[E.cy -1].updateRender();
    
    E.row.erase(E.row.begin() + E.cy);
    E.cy--;
    E.dirty = true;
  }
}

void insertNewline() {
  if (E.cx == 0) {
    E.row.insert(E.row.begin() + E.cy, (erow){});
    E.row[E.cy].updateRender();
  } else {
    // we need to split the current row into two
    E.row.insert(E.row.begin() + E.cy + 1, (erow){});
    erow *row = &E.row[E.cy];
    E.row[E.cy + 1].raw = row->raw.substr(E.cx, row->raw.length() - E.cx);
    E.row[E.cy + 1].updateRender();
    
    row = &E.row[E.cy]; 
    row->raw.erase(row->raw.begin() + E.cx, row->raw.end());
    row->updateRender();
  }
  E.cy++;
  E.cx = 0;
  E.dirty = true;
}

void insertChar(char c) {
  erow *row = &E.row[E.cy];
  row->raw.insert(row->raw.begin() + E.cx, c);
  row->updateRender();
  E.dirty = true;
  E.cx++;
}
}