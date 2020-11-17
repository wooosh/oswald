#include <string>
#include <cstring>
#include <stdio.h>
#include <unistd.h>

#include "main.hxx"
#include "row.hxx"
#include "terminal.hxx"

#define KILO_VERSION "0.0.1"

/*** output ***/

void editorScroll() {
  E.rx = 0;
  if (E.cy < E.row.size()) {
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

void editorDrawRows(std::string &buf) {
  for (size_t y = 0; y < E.screenrows; y++) {
    size_t filerow = y + E.rowoff;
    if (filerow >= E.row.size()) {
      if (E.row.size() == 0 && y == E.screenrows / 3) {
        char welcome[80];
        size_t welcomelen = snprintf(welcome, sizeof(welcome),
                                  "Kilo editor -- version %s", KILO_VERSION);
        if (welcomelen > E.screencols)
          welcomelen = E.screencols;
        int padding = (E.screencols - welcomelen) / 2;
        if (padding) {
          buf += "~";
          padding--;
        }
        while (padding--)
          buf += " ";
        buf += welcome;
      } else {
        buf += "~";
      }
    } else {
      ssize_t len = E.row[filerow].render.length() - E.coloff;
      if (len > 0) {
        if (len > E.screencols)
          len = E.screencols;
        buf += E.row[filerow].render.substr(E.coloff, len);
      }
    }

    buf += "\x1b[K";
    buf += "\r\n";
  }
}

void editorDrawStatusBar(std::string &buf) {
  buf += "\x1b[7m";
  char status[80], rstatus[80];
  size_t len = snprintf(status, sizeof(status), "%.20s - %zu lines %s",
                     E.filename ? E.filename : "[No Name]", E.row.size(),
                     E.dirty ? "(modified)" : "");
  size_t rlen =
      snprintf(rstatus, sizeof(rstatus), "%zu/%zu", E.cy + 1, E.row.size());
  if (len > E.screencols)
    len = E.screencols;
  buf += status;
  while (len < E.screencols) {
    if (E.screencols - len == rlen) {
      buf += rstatus;
      break;
    } else {
      buf += " ";
      len++;
    }
  }
  buf += "\x1b[m";
  buf += "\r\n";
}

void editorDrawMessageBar(std::string &buf) {
  buf += "\x1b[K";
  size_t msglen = strlen(E.statusmsg);
  if (msglen > E.screencols)
    msglen = E.screencols;
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
  snprintf(buf2, sizeof(buf2), "\x1b[%zu;%zuH", (E.cy - E.rowoff) + 1,
           (E.rx - E.coloff) + 1);
  buf += buf2;
  buf += "\x1b[?25h";

  write(STDOUT_FILENO, buf.c_str(), buf.length());
}
