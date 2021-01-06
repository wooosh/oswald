#pragma once
#include <string>

typedef struct erow {
  std::string raw;
  std::string render;
  bool dirty;

  size_t cxToRx(size_t cx);
  void updateRender();
} erow;
