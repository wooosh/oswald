#pragma once

typedef void (*PluginInit)();

void load_plugins(void);

void p_navigation(void);

PluginInit plugins[] = {
  p_navigation,
  NULL
};
