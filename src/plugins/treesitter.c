#include "buffer/buffer.h"
#include "main.h"
#include "highlight.h"
#include "event.h"

#include <stdio.h>
#include <meraki/output.h>

#include <tree_sitter/treesitter.h>

// TODO: store one for each buffer using a map from buffer id to parser
TSParser *parser;
TSTree *tree;

static const char* treesitter_input(void *vbuffer, uint32_t byte_offset, TSPoint pos, uint32_t *bytes_read) {
  struct Buffer *buffer = vbuffer;

  if (pos.row >= buffer->lines.len) {
    *bytes_read = 0;
    return NULL;
  }

  // tree sitter needs a newline, so we use a temporary line buffer that stores
  // the line it asks for until the next invocation while reusing memory
  static vec_char line;
  vec_truncate(&line, 0);
  vec_append_vec(&line, &buffer->lines.data[pos.row]);
  vec_push(&line, '\n');

  *bytes_read = line.len - pos.column;
  return line.data + pos.column;
}

void treesitter_update(struct Event e) {
  switch (e.type) {
    case event_open:  
      parser = ts_parser_new();
      if (parser == NULL) exit(1);
      ts_parser_set_language(parser, tree_sitter_c());
      tree = ts_parser_parse(parser, NULL, (TSInput){(void*) e.open, treesitter_input, TSInputEncodingUTF8});
      break;
  }
}

bool treesitter_in_range(TSTreeCursor* cur, size_t line) {
  TSNode node = ts_tree_cursor_current_node(cur);
  TSPoint start = ts_node_start_point(node);
  TSPoint end = ts_node_end_point(node);
  return start.row <= line && end.row >= line;
}

bool treesitter_cursor_next(TSTreeCursor* cur, size_t line) {
  if (treesitter_in_range(cur, line) && ts_tree_cursor_goto_first_child(cur)) {
    return true;
  } else if (ts_tree_cursor_goto_next_sibling(cur)) {
    return true;
  }

  while (ts_tree_cursor_goto_parent(cur)) {
    if (ts_tree_cursor_goto_next_sibling(cur)) {
      return true;
    }
  }
  
  return false;
}

void treesitter_highlight(struct Buffer *buffer, size_t line, vec_style *highlight) {
  if (tree != NULL) {
    TSNode root = ts_tree_root_node(tree);
    TSNode top_node = ts_node_descendant_for_point_range(root, (TSPoint){line, 0}, (TSPoint){line, buffer->lines.data[line].len});
  
    TSTreeCursor cur = ts_tree_cursor_new(top_node);
    do {
      TSNode node = ts_tree_cursor_current_node(&cur);
      TSPoint start = ts_node_start_point(node);
      TSPoint end = ts_node_end_point(node);
      if (start.row == line && end.row == line && (
          // TODO: look up in hashmap?
          strcmp(ts_node_type(node), "primitive_type") == 0 ||
          strcmp(ts_node_type(node), "enumerator") == 0 ||
          strcmp(ts_node_type(node), "type_identifier") == 0)) {
        if (start.row == end.row) {
          struct MerakiStyle s = {{Meraki8Color, 35}, {Meraki8Color, -1}, MerakiBright};
          for (int i=start.column; i<end.column; i++) {
            highlight->data[i] = s;
          }
        } // TODO: handle else for multiline
      }
    } while (treesitter_cursor_next(&cur, line));
  }
}
