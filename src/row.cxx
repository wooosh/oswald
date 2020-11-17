#include "main.hxx"
#include "row.hxx"

#define KILO_TAB_STOP 8

// TODO: remove editor prefix
// TODO: should be moved to render
int erow::editorRowCxToRx(int cx) {
  int rx = 0;
  int j;
  for (j = 0; j < cx; j++) {
    if (this->raw[j] == '\t')
      rx += (KILO_TAB_STOP - 1) - (rx % KILO_TAB_STOP);
    rx++;
  }
  return rx;
}

// TODO: should be moved to render
int erow::editorRowRxToCx(int rx) {
  int cur_rx = 0;
  int cx;
  for (cx = 0; cx < this->raw.length(); cx++) {
    if (this->raw[cx] == '\t')
      cur_rx += (KILO_TAB_STOP - 1) - (cur_rx % KILO_TAB_STOP);
    cur_rx++;

    if (cur_rx > rx)
      return cx;
  }
  return cx;
}

// TODO: should be moved to render
void erow::updateRender() {
  int tabs = 0;
  int j;
  for (j = 0; j < this->raw.length(); j++)
    if (this->raw[j] == '\t')
      tabs++;

  this->render.clear();

  for (j = 0; j < this->raw.length(); j++) {
    if (this->raw[j] == '\t') {
      this->render += ' ';
      while (this->render.length() % KILO_TAB_STOP != 0)
        this->render += ' ';
    } else {
      this->render += this->raw[j];
    }
  }
}