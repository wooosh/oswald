#include "main.hxx"
#include "draw/draw.hxx"
#include "terminal/keypress.hxx"
#include "buffer/buffer.hxx"
#include "terminal/terminal.hxx"

#include "modules/highlight_treesitter.hxx"

#include <fcntl.h>
#include <iostream>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <regex>

struct Editor E;

int main(int argc, char *argv[]) {
  if (argc == 1) {
    openScratchPortion();
  } else {
    for (int i = 1; i < argc; i++) {
      bool success = openFilePortion(argv[i]);

      if (!success) {
        std::cout << "Cannot open file: " << argv[i] << std::endl;
        return 1;
      }
    }
  }

  E.cursor.p = E.portions.begin();
  E.cursor.x = 0;
  E.cursor.y = 0;

  E.anchor = E.cursor;

  Terminal::setup();

  if (Terminal::getWindowSize(&E.screenrows, &E.screencols) == -1) {
    Terminal::die("getWindowSize");
  }

  while (1) {
    // TODO: link renderer into event system
    // TODO: remove editor prefix    
    editorRefreshScreen();
    editorProcessKeypress();

    while (!E.events.empty()) {
      Event ev = E.events.front();
      E.events.pop_front();
      
      switch(ev.type) {
      case Event::BufferOpen:
        treeSitterOpenBuffer(ev.bufferOpen);
        break;
      case Event::BufferEdit:
        BufferEditEvent editEvent = ev.bufferEdit;
        //editEvent.buffer->highlight(editEvent);
        highlightTreeSitter(editEvent);
        break;
      }
    }  
  }

  return 0;
}
