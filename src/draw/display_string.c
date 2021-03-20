#include <vec/vec.h>
#include <x.h>

#include <ctype.h>
#include <string.h>
#include <wchar.h>

// TODO|CLEANUP: explain the difference between raw and display strings
// (probably in readme for draw/)

// TODO: move this feature comment to buffer, since we should store tab
// settings per buffer
// TODO|FEATURE: smart tab settings/non hardcoded tab length
//  - Automatically detect the files indentaion type by percentage
//  - Highlight inconsistent indentation (ie tabs in a file using spaces)
#define TAB_WIDTH 4

// converts a position in the raw string into a position in the displayed
// string, does not include characters that are would cross the length. useful
// for calculating where to cut lines.
//
// this function assumes that len is in bounds of the raw array
size_t ds_len_display_to_raw(vec_char_t raw, size_t len) {
  // TODO: tabs should be using modulo
  // TODO: Make custom wcwidth function that returns the length, so that we can
  // also have markToRenderX

  int last_char_width = 0;
  // TODO|CLEANUP: this loop is dumb
  size_t i = 0;
  for (; i < raw.length && 0 < len - last_char_width;
       i++, len -= last_char_width) {
    wchar_t ch;
    int ch_len = mbtowc(&ch, raw.data + i, raw.length - i);
    xassert(ch_len >= 1, "mbtowc should always return atleast one character");
    i += ch_len;

    if (ch == '\t') {
      len += TAB_WIDTH;
    } else if (iscntrl(ch)) {
      // TODO|FEATURE: display control characters as hex codes or names like
      // "ESC" with a background highlight

      // since control characters are represented as ^<letter> (ie ^M) they
      // have a length of two
      last_char_width = 2;
    } else {
      last_char_width = wcwidth(ch);
      if (last_char_width == -1) {
        last_char_width = 0;
      }
    }
  }
  return i;
}

// TODO: for horizontal scrolling use slicing technique:
// vec_char_t cutoff_line = line.content
// cutoff_line.data += max(cutoff_line.length - e.column_offset, 0);
// cutoff_line.length = max(cutoff_line.length - e.column_offset, 0);
// ds_row_to_display(line, new);

// void ds_raw_to_display(vec_char_t raw, vec_char_t *display);
