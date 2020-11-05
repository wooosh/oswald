#pragma once 
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

typedef struct erow {
  int size;
  int rsize;
  char *chars;
  char *render;

  int editorRowCxToRx(int cx);
  int editorRowRxToCx(int rx);
  void editorUpdateRow();
  void editorFreeRow();
  void editorRowInsertChar(int at, int c);
  void editorRowAppendString(char *s, size_t len);
  void editorRowDelChar(int at);
} erow;
void editorDelRow(int at);
void editorInsertRow(int at, char *s, size_t len);
