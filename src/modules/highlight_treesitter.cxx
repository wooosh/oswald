#include "buffer/buffer.hxx"
#include "main.hxx"
#include "terminal/terminal.hxx"

#include <map>
#include <cstring>

extern "C" {
#include <tree_sitter/treesitter.h>
}

static std::map<std::list<Buffer>::iterator, TSParser*> parsers;

const char* treeSitterInput(void *buffer, uint32_t byte_offset, TSPoint pos, uint32_t *bytes_read) {
  Buffer *buf = (Buffer*) buffer;
  if (pos.row < buf->rows.size()) {
    *bytes_read = buf->rows[pos.row].raw.length() + 1 - pos.column;
    // TODO: free this
    static std::string row;
    row = buf->rows[pos.row].raw + "\n";
    return row.c_str() + pos.column;
  }
  *bytes_read = 0;
  return NULL;
}


void treeSitterOpenBuffer(std::list<Buffer>::iterator buffer) {


}

// returns false if end of file
bool cursorNext(TSTreeCursor* cur) {
  if (ts_tree_cursor_goto_first_child(cur)) {
    return true;
  } else if (ts_tree_cursor_goto_next_sibling(cur)) {
    return true;
  } else {
    while (ts_tree_cursor_goto_parent(cur)) {
      if (ts_tree_cursor_goto_next_sibling(cur)) {
        return true;
      }
    }
  }
  return false;
}

void highlightTreeSitter(BufferEditEvent ev) {
  if (ev.type == BufferEditEvent::Removed) {
    return;
  }
  
  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_c());

  TSTree *tree = ts_parser_parse(
    parser,
    NULL,
    {
      (void*) &(*ev.buffer),
      treeSitterInput,
      TSInputEncodingUTF8
    }
    //ev.buffer->rows[ev.line].raw.c_str(),
    //ev.buffer->rows[ev.line].raw.length()
  );


  TSTreeCursor cur = ts_tree_cursor_new(ts_tree_root_node(tree));

  for (size_t i = ev.line; i < ev.line + ev.length; i++) {
    Row *r = &ev.buffer->rows[i];
    r->hl.assign(r->raw.length(), HighlightType::Normal);
  }
  
  while (cursorNext(&cur)) {
    TSNode node = ts_tree_cursor_current_node(&cur);
    if (strcmp(ts_node_type(node), "primitive_type") == 0 ||
      strcmp(ts_node_type(node), "type_identifier") == 0
      ) {
      // we assume the highlight is only on one line currently
      TSPoint start = ts_node_start_point(node);
      TSPoint end = ts_node_end_point(node);

      for (uint32_t i = start.column; i<end.column; i++) {
        
        ev.buffer->rows[start.row].hl[i] = HighlightType::Keyword;
      }

    }
  }

}
