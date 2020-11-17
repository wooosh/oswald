#pragma once

#include "row.hxx"
#include <vector>
#include <termios.h>

extern struct editorConfig {
  size_t cx, cy;
  size_t rx;
  size_t rowoff;
  size_t coloff;
  size_t screenrows;
  size_t screencols;
  std::vector<erow> row;
  bool dirty;
  char *filename;
  char statusmsg[80];
  time_t statusmsg_time;
  struct termios orig_termios;
} E;

void editorInsertChar(int c);
void editorInsertNewline();
void editorDelChar();
void editorSetStatusMessage(const char *fmt, ...);

void cursorLeft();
void cursorRight();
void cursorUp();
void cursorDown();