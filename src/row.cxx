/*** includes ***/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "main.hxx"
#include "row.hxx"

#define KILO_TAB_STOP 8

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

void erow::editorUpdateRow() {
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

void editorInsertRow(int at, std::string s) {
  if (at < 0 || at > E.row.size())
    return;

  E.row.insert(E.row.begin() + at, (erow){});

  E.row[at].raw = s;
  E.row[at].editorUpdateRow();

  E.dirty++;
}

void editorDelRow(int at) {
  if (at < 0 || at >= E.row.size())
    return;
  E.row.erase(E.row.begin() + at);
  E.dirty++;
}

void erow::editorRowInsertChar(size_t at, char c) {
  if (at < 0 || at > this->raw.length())
    at = this->raw.length();
  this->raw.insert(this->raw.begin() + at, c);
  this->editorUpdateRow();
  E.dirty++;
}

void erow::editorRowAppendString(std::string s) {
  this->raw += s;
  this->editorUpdateRow();
  E.dirty++;
}

void erow::editorRowDelChar(int at) {
  if (at < 0 || at >= this->raw.length())
    return;
  this->raw.erase(at);
  this->editorUpdateRow();
  E.dirty++;
}
