#pragma once

typedef void (*PluginInit)();


void p_open(void);
void p_navigation(void);
void p_misc(void);

PluginInit plugins[] = {
  p_open,
  p_navigation,
  p_misc,
  NULL
};
