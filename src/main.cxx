#include "main.hxx"
#include "draw/draw.hxx"
#include "terminal/keypress.hxx"
#include "buffer/buffer.hxx"
#include "terminal/terminal.hxx"

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
      case Event::BufferEdit:
        if (ev.bufferEdit.type != BufferEditEvent::Removed) { 
          Row *r = &ev.bufferEdit.buffer->rows[ev.bufferEdit.line];
          std::string line = r->raw;
          auto hl = r->hl;

          hl.assign(line.length(), HighlightType::Normal);

          std::regex kw(" ?(if|else|for|while|do|switch|case|default|break|continue|struct|enum|union|return)( |;)");
          std::sregex_iterator riter(line.begin(), line.end(), kw);
          std::sregex_iterator end;

          for (auto i = riter; i != end; ++i) {
            std::smatch s = *i;
            for (auto j = s.position(); j < s.position() + s.length(); j++) {
              hl[j] = HighlightType::Keyword;
            }
          }

          r->hl = hl;
        }
        break;
      }
    }  
  }

  return 0;
}
