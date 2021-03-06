#pragma once

#include <list>
#include <string>
#include <vector>

struct BufferEditEvent;

enum HighlightType {
  // TODO: use semantic types
  Keyword,
  Selection,
  Normal
};

struct Row {
  // TODO: rename to contents
  std::string raw;
  std::vector<HighlightType> hl;
};

struct Buffer {
  // TODO: let buffers handle saving, closing, unsaved changes status
  // TODO: change filename to title
  // TODO: filetype 
  std::string filename;
  std::vector<Row> rows;

  void (*highlight)(BufferEditEvent ev);
  // TODO: method to create a mark
};

void openScratchPortion();
bool openFilePortion(std::string filename);
void saveFilePortion(Buffer p);
