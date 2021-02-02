#include <fstream>
#include <iostream>
#include <string>

#include "main.hxx"

void openScratchPortion() {
  auto p = E.portions.insert(E.portions.begin(), (portion){"scratch"});
  p->rows.push_back((erow){"", "", true});
}

// returns true if successful
bool openFilePortion(std::string filename) {
  std::ifstream file(filename);
  // TODO: proper error handling
  if (!file.is_open()) return false;

  auto p = E.portions.insert(E.portions.begin(), (portion){filename});

  while (file.good()) {
    std::string line;
    std::getline(file, line);

    if (file.eof()) break;

    erow row = {line, "", true};
    row.updateRender();

    p->rows.push_back(row);
  }

  return true;
};
