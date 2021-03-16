#pragma once

void term_setup();
void term_restore();
// TODO: make this try to save work
void term_die(const char *msg);
