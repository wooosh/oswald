#pragma once
#include <string>

typedef struct erow {
  std::string raw;
  std::string render;

  size_t cxToRx(size_t cx);
  size_t rxToCx(size_t rx);
  void updateRender();
} erow;
