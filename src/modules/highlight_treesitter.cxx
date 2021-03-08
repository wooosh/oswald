#include "buffer/buffer.hxx"
#include "main.hxx"
#include "terminal/terminal.hxx"

#include <map>
#include <cstring>

extern "C" {
#include <tree_sitter/treesitter.h>
}

static std::map<std::list<Buffer>::iterator, TSParser*> parsers;
/*
const char* treeSitterInput(std::list<Buffer>::iterator *buffer, uint32_t byte_offset, TSPoint pos, uint32_t *bytes_read) {
  Buffer buf = **buffer;
  if (pos.row < buf.rows.length()) {
    *bytes_read = buf.rows[pos.row].length() - pos.column;
    return buf.rows[pos.row].c_str() + pos.column;
  }
  *bytes_read = 0;
  return NULL;
}
*/

void treeSitterOpenBuffer(std::list<Buffer>::iterator buffer) {


}

// returns false if end of file
bool cursorNext(TSTreeCursor* cur) {
  if (ts_tree_cursor_goto_first_child(cur)) {
    return true;
  } else if (ts_tree_cursor_goto_next_sibling(cur)) {
    return true;
  } else if (ts_tree_cursor_goto_parent(cur) && ts_tree_cursor_goto_next_sibling(cur)) {
    return true;
  }
  return false;
}

void highlightTreeSitter(BufferEditEvent ev) {
  if (ev.type == BufferEditEvent::Removed) {
    return;
  }
  
  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_c());

  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    ev.buffer->rows[ev.line].raw.c_str(),
    ev.buffer->rows[ev.line].raw.length()
  );

  TSTreeCursor cur = ts_tree_cursor_new(ts_tree_root_node(tree));

  for (size_t i = ev.line; i < ev.line + ev.length; i++) {
    Row *r = &ev.buffer->rows[i];
    r->hl.assign(r->raw.length(), HighlightType::Normal);
  }
  
  while (cursorNext(&cur)) {
    TSNode node = ts_tree_cursor_current_node(&cur);
    if (strcmp(ts_node_type(node), "primitive_type") == 0 ||
      strcmp(ts_node_type(node), "struct_specifier") == 0 ||
      strcmp(ts_node_type(node), "type_identifier") == 0
      ) {
      // we assume the highlight is only on one line currently
      uint32_t start = ts_node_start_point(node).column;
      uint32_t end = ts_node_end_point(node).column;

      for (uint32_t i = start; i<end; i++) {
        
        ev.buffer->rows[ev.line].hl[i] = HighlightType::Keyword;
      }

    }
  }

}
