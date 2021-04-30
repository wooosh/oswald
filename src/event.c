#include <event.h>

#include <draw/draw.h>
#include <plugins/debug.h>
#include <plugins/treesitter.h>

typedef void (*EventHandler)(struct Event);
static EventHandler event_pipelines[][16] = {
    [event_open] = {treesitter_update, draw_event, NULL}, [event_edit] = {draw_event, NULL}, [event_mark_move] = {draw_event, NULL}, [event_render]= {draw_event, NULL}};

void dispatch_event(struct Event e) {
  EventHandler *pipeline = event_pipelines[e.type];

  // TODO|CLEANUP: size_t vs int
  for (int i = 0; pipeline[i] != NULL; i++) {
    pipeline[i](e);
  }

  // TODO: free event data??
}
