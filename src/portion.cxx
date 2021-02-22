#include <fstream>
#include <iostream>
#include <string>

#include "main.hxx"

void openScratchPortion() {
  auto p = E.portions.insert(E.portions.begin(), (Buffer){"scratch"});
  p->rows.push_back((Row){"", "", true});
}

// returns true if successful
bool openFilePortion(std::string filename) {
  std::ifstream file(filename);
  // TODO: proper error handling
  if (!file.is_open())
    return false;

  auto p = E.portions.insert(E.portions.begin(), (Buffer){filename});

  while (file.good()) {
    std::string line;
    std::getline(file, line);

    if (file.eof())
      break;

    Row row = {line, "", true};
    row.updateRender();

    p->rows.push_back(row);
  }

  file.close();

  return true;
};

// TODO: store a save function in the buffer itself since some buffer types
// (like scratch buffers) require special care.
void saveFilePortion(Buffer p) {
  std::ofstream file(p.filename);

  for (int i=0; i<p.rows.size(); i++) {
    file << p.rows[i].raw << '\n';
  }

  file.close();
}
