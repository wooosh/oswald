#pragma once
/*
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
*/
#include <string>

typedef struct erow {
  std::string raw;
  std::string render;

  int editorRowCxToRx(int cx);
  int editorRowRxToCx(int rx);
  void updateRender();
} erow;
void editorInsertRow(int at, std::string s);
