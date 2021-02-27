#pragma once

#include <list>
#include <string>
#include <vector>

struct Row {
  std::string raw;
  // TODO: decouple the buffer from the render and store the render string elsewhere
  std::string render;

  // marks if the contents of the string have changed
  bool dirty;
  // TODO: don't export this function, just redraw rows when they are set to
  // dirty and need to be drawn
  void updateRender();
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
