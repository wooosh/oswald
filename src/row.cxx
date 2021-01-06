#include "row.hxx"
#include "main.hxx"

#define KILO_TAB_STOP 8

size_t erow::cxToRx(size_t cx) {
  size_t rx = 0;
  for (size_t j = 0; j < cx; j++) {
    if (this->raw[j] == '\t')
      rx += (KILO_TAB_STOP - 1) - (rx % KILO_TAB_STOP);
    rx++;
  }
  return rx;
}

void erow::updateRender() {
  this->render.clear();
  this->dirty = true;

  for (size_t j = 0; j < this->raw.length(); j++) {
    if (this->raw[j] == '\t') {
      this->render += ' ';
      while (this->render.length() % KILO_TAB_STOP != 0)
        this->render += ' ';
    } else {
      this->render += this->raw[j];
    }
  }
}
