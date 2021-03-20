#include <plugins/debug.h>
#include <stdio.h>

void report_event(struct Event e) {
  fprintf(stderr, "event_type: %d\n", e.type);
}
