#pragma once

typedef void (*PluginInit)();

void load_plugins(void);

void p_open(void);
void p_navigation(void);
void p_snippets(void);
void p_misc(void);

PluginInit plugins[] = {
  p_open,
  p_navigation,
  p_snippets,
  p_misc,
  NULL
};
