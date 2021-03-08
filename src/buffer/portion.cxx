#include <fstream>
#include <iostream>
#include <string>

#include "main.hxx"

#include "modules/highlight_none.hxx"
#include "modules/highlight_regex.hxx"
#include "modules/highlight_treesitter.hxx"

void openScratchPortion() {
  auto p = E.portions.insert(E.portions.begin(),
                             (Buffer){"scratch", .highlight = highlightNone});
  E.events.push_back({Event::BufferOpen, .bufferOpen = {p}});
  p->rows.push_back((Row){""});
  E.events.push_back({Event::BufferEdit,
                      (BufferEditEvent){0, 1, p, BufferEditEvent::Inserted}});
}

// returns true if successful
bool openFilePortion(std::string filename) {
  std::ifstream file(filename);
  // TODO: proper error handling
  if (!file.is_open())
    return false;

  std::list<Buffer>::iterator p = E.portions.insert(
      E.portions.begin(), (Buffer){filename, .highlight = highlightTreeSitter});

  E.events.push_back({Event::BufferOpen, .bufferOpen = {p}});

  while (file.good()) {
    std::string line;
    std::getline(file, line);

    if (file.eof())
      break;

    Row row = {line};
    p->rows.push_back(row);
  }

  file.close();

  E.events.push_back(
      {Event::BufferEdit,
       .bufferEdit = {0, p->rows.size(), p, BufferEditEvent::Inserted}});

  return true;
};

// TODO: store a save function in the buffer itself since some buffer types
// (like scratch buffers) require special care.
void saveFilePortion(Buffer p) {
  std::ofstream file(p.filename);

  for (int i = 0; i < p.rows.size(); i++) {
    file << p.rows[i].raw << '\n';
  }

  file.close();
}
