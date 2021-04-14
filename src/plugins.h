#pragma once

typedef void (*PluginInit)();

void load_plugins(void);

void p_navigation(void);
void p_misc(void);

PluginInit plugins[] = {
  p_navigation,
  p_misc,
  NULL
};
