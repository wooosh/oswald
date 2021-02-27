#pragma once

#include <list>
#include <string>
#include <vector>

struct Row {
  // TODO: rename to contents
  std::string raw;

  /*
  struct HighlightRange {
    size_t start;
    size_t end;
    some_enum type;
  }*/
};

struct Buffer {
  // TODO: let buffers handle saving, closing, unsaved changes status
  // TODO: change filename to title

  std::string filename;
  std::vector<Row> rows;
  // TODO: method to create a mark
};

void openScratchPortion();
bool openFilePortion(std::string filename);
void saveFilePortion(Buffer p);
