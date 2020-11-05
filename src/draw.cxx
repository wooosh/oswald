/*** includes ***/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <string>

#include "main.hxx"
#include "row.hxx"
#include "terminal.hxx"

#define KILO_VERSION "0.0.1"

/*** output ***/

void editorScroll() {
  E.rx = 0;
  if (E.cy < E.numrows) {
    E.rx = E.row[E.cy].editorRowCxToRx(E.cx);
  }

  if (E.cy < E.rowoff) {
    E.rowoff = E.cy;
  }
  if (E.cy >= E.rowoff + E.screenrows) {
    E.rowoff = E.cy - E.screenrows + 1;
  }
  if (E.rx < E.coloff) {
    E.coloff = E.rx;
  }
  if (E.rx >= E.coloff + E.screencols) {
    E.coloff = E.rx - E.screencols + 1;
  }
}

void editorDrawRows(std::string& buf) {
  int y;
  for (y = 0; y < E.screenrows; y++) {
    int filerow = y + E.rowoff;
    if (filerow >= E.numrows) {
      if (E.numrows == 0 && y == E.screenrows / 3) {
        char welcome[80];
        int welcomelen = snprintf(welcome, sizeof(welcome),
          "Kilo editor -- version %s", KILO_VERSION);
        if (welcomelen > E.screencols) welcomelen = E.screencols;
        int padding = (E.screencols - welcomelen) / 2;
        if (padding) {
          buf += "~";
          padding--;
        }
        while (padding--) buf += " ";
        buf += welcome;
      } else {
        buf += "~";
      }
    } else {
      int len = E.row[filerow].rsize - E.coloff;
      if (len < 0) len = 0;
      if (len > E.screencols) len = E.screencols;
      std::string sliceBuf(E.row[filerow].render);
      buf += sliceBuf.substr(E.coloff, len);
      //abAppend(ab, &E.row[filerow].render[E.coloff], len);
    }

    buf += "\x1b[K";
    buf += "\r\n";
  }
}

void editorDrawStatusBar(std::string& buf) {
  buf += "\x1b[7m";
  char status[80], rstatus[80];
  int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
    E.filename ? E.filename : "[No Name]", E.numrows,
    E.dirty ? "(modified)" : "");
  int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d",
    E.cy + 1, E.numrows);
  if (len > E.screencols) len = E.screencols;
  buf += status;
  while (len < E.screencols) {
    if (E.screencols - len == rlen) {
      buf +=  rstatus;
      break;
    } else {
      buf += " ";
      len++;
    }
  }
  buf += "\x1b[m";
  buf += "\r\n";
}

void editorDrawMessageBar(std::string& buf) {
  buf += "\x1b[K";
  int msglen = strlen(E.statusmsg);
  if (msglen > E.screencols) msglen = E.screencols;
  if (msglen && time(NULL) - E.statusmsg_time < 5)
    buf += E.statusmsg;
}

void editorRefreshScreen() {
  editorScroll();

  std::string buf;

  buf += "\x1b[?25l";
  buf += "\x1b[H";

  editorDrawRows(buf);
  editorDrawStatusBar(buf);
  editorDrawMessageBar(buf);

  char buf2[32];
  snprintf(buf2, sizeof(buf2), "\x1b[%d;%dH", (E.cy - E.rowoff) + 1,
                                            (E.rx - E.coloff) + 1);
  buf += buf2;
  buf += "\x1b[?25h";

  write(STDOUT_FILENO, buf.c_str(), buf.length());
}
