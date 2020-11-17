#pragma once
#include <string>

// TODO: should be moved to main
typedef struct erow {
  std::string raw;
  std::string render;

  int editorRowCxToRx(int cx);
  int editorRowRxToCx(int rx);
  void updateRender();
} erow;
