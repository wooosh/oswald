#include "main.hxx"
#include "row.hxx"
#include "terminal.hxx"

#include <cstring>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

#define KILO_VERSION "0.0.1"

// scroll to fit cursor in screen and keep file in bounds
void editorScroll() {
  E.rx = 0;
  if (E.cy < E.row.size()) {
    E.rx = E.row[E.cy].cxToRx(E.cx);
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

// TODO: remove editor prefix
void editorDrawRows(std::ostringstream &out) {
  for (size_t y = 0; y < E.screenrows; y++) {
    size_t filerow = y + E.rowoff;
    if (filerow >= E.row.size()) {
        out << "~";
    } else {
      ssize_t len = E.row[filerow].render.length() - E.coloff;
      if (len > 0) {
        // TODO: fix signed unsigned comparison
        if (len > E.screencols)
          len = E.screencols;
        out << E.row[filerow].render.substr(E.coloff, len);
      }
    }

    out << "\x1b[K" << "\r\n";
  }
}

void editorDrawStatusBar(std::ostringstream &out) {
  out << "\x1b[7m";
  // TODO: remove char buffer
  /*char status[80], rstatus[80];
  size_t len = snprintf(status, sizeof(status), "%.20s - %zu lines %s",
                        E.filename ? E.filename : "[No Name]", E.row.size(),
                        E.dirty ? "(modified)" : "");
  size_t rlen =
      snprintf(rstatus, sizeof(rstatus), "%zu/%zu", E.cy + 1, E.row.size());
  */
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
  out << "\x1b[m" << "\r\n";
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
  // TODO: change to streams
  editorScroll();

  std::ostringstream out;

  // hide cursor and clear screen
  out << "\x1b[?25l" << "\x1b[H";

  editorDrawRows(out);
  //editorDrawStatusBar(buf);
  //editorDrawMessageBar(buf);

  // set cursor pos
  out << "\x1b[" << (E.cy-E.rowoff) + 1 << ";" << (E.rx - E.coloff) + 1 << "H";

  // show cursor
  out << "\x1b[?25h";

  std::cout << out.str();
  std::cout.flush();
}
