#include "main.hxx"

// TODO: handle null marks

Row *Mark::row() { return &p->rows[y]; }

void Mark::moveLeft() {
  if (x != 0) {
    x--;
  } else if (y > 0) {
    y--;
    x = row()->raw.length();
  }
}

void Mark::moveRight() {
  if (x < row()->raw.length()) {
    x++;
  } else if (y + 1 < p->rows.size()) {
    y++;
    x = 0;
  }
}

void Mark::moveUp() {
  if (y != 0) {
    y--;
    x = std::min(x, row()->raw.length());
  }
}
void Mark::moveDown() {
  if (y + 1 < p->rows.size()) {
    y++;
    x = std::min(x, row()->raw.length());
  }
}

void Mark::deleteBackward() {
  if (y == p->rows.size())
    return;
  if (x == 0 && y == 0)
    return;

  // TODO: remove temporary
  Row *r = row();
  // find out if deleting a character requires merging two lines
  if (x > 0) {
    x--;
    r->raw.erase(r->raw.begin() + x);
  } else {
    Row *previousRow = &p->rows[y - 1];
    x = previousRow->raw.length();

    previousRow->raw += r->raw;

    p->rows.erase(p->rows.begin() + y);
    y--;
  }
}

void Mark::insertChar(char c) {
  // TODO: comment and refactor
  if (c != '\r') {
    Row *r = row();
    r->raw.insert(r->raw.begin() + x, c);
    x++;
    E.events.push_back({Event::BufferEdit, (BufferEditEvent){y, 1, p, BufferEditEvent::Changed}});
  } else {
    if (x == 0) {
      p->rows.insert(p->rows.begin() + y, (Row){});
    } else {
      // split the current row into two
      p->rows.insert(p->rows.begin() + y + 1, (Row){});
      Row *r = row();
      Row *nextRow = &p->rows[y + 1];
      nextRow->raw = r->raw.substr(x, r->raw.length() - x);

      // TODO: is resetting the row neccesary
      r = row();
      r->raw.erase(r->raw.begin() + x, r->raw.end());
    }
    y++;
    x = 0;
  }
}
