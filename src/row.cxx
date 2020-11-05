/*** includes ***/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "row.hxx"
#include "main.hxx"

#define KILO_TAB_STOP 8

int erow::editorRowCxToRx(int cx) {
  int rx = 0;
  int j;
  for (j = 0; j < cx; j++) {
    if (this->chars[j] == '\t')
      rx += (KILO_TAB_STOP - 1) - (rx % KILO_TAB_STOP);
    rx++;
  }
  return rx;
}

int erow::editorRowRxToCx(int rx) {
  int cur_rx = 0;
  int cx;
  for (cx = 0; cx < this->size; cx++) {
    if (this->chars[cx] == '\t')
      cur_rx += (KILO_TAB_STOP - 1) - (cur_rx % KILO_TAB_STOP);
    cur_rx++;

    if (cur_rx > rx) return cx;
  }
  return cx;
}

void erow::editorUpdateRow() {
  int tabs = 0;
  int j;
  for (j = 0; j < this->size; j++)
    if (this->chars[j] == '\t') tabs++;

  free(this->render);
  this->render = (char*) malloc(this->size + tabs*(KILO_TAB_STOP - 1) + 1);

  int idx = 0;
  for (j = 0; j < this->size; j++) {
    if (this->chars[j] == '\t') {
      this->render[idx++] = ' ';
      while (idx % KILO_TAB_STOP != 0) this->render[idx++] = ' ';
    } else {
      this->render[idx++] = this->chars[j];
    }
  }
  this->render[idx] = '\0';
  this->rsize = idx;
}

void editorInsertRow(int at, char *s, size_t len) {
  if (at < 0 || at > E.numrows) return;

  E.row = (erow*) realloc(E.row, sizeof(erow) * (E.numrows + 1));
  memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numrows - at));

  E.row[at].size = len;
  E.row[at].chars = (char*) malloc(len + 1);
  memcpy(E.row[at].chars, s, len);
  E.row[at].chars[len] = '\0';

  E.row[at].rsize = 0;
  E.row[at].render = NULL;
  E.row[at].editorUpdateRow();

  E.numrows++;
  E.dirty++;
}

void erow::editorFreeRow() {
  free(this->render);
  free(this->chars);
}

void editorDelRow(int at) {
  if (at < 0 || at >= E.numrows) return;
  E.row[at].editorFreeRow();
  memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1));
  E.numrows--;
  E.dirty++;
}

void erow::editorRowInsertChar(int at, int c) {
  if (at < 0 || at > this->size) at = this->size;
  this->chars = (char*) realloc(this->chars, this->size + 2);
  memmove(&this->chars[at + 1], &this->chars[at], this->size - at + 1);
  this->size++;
  this->chars[at] = c;
  this->editorUpdateRow();
  E.dirty++;
}

void erow::editorRowAppendString(char *s, size_t len) {
  this->chars = (char*) realloc(this->chars, this->size + len + 1);
  memcpy(&this->chars[this->size], s, len);
  this->size += len;
  this->chars[this->size] = '\0';
  this->editorUpdateRow();
  E.dirty++;
}

void erow::editorRowDelChar(int at) {
  if (at < 0 || at >= this->size) return;
  memmove(&this->chars[at], &this->chars[at + 1], this->size - at);
  this->size--;
  this->editorUpdateRow();
  E.dirty++;
}
