#include "buffer/buffer.hxx"
#include "main.hxx"

#include <regex>

void highlightRegex(BufferEditEvent ev) {
  if (ev.type == BufferEditEvent::Removed) {
    return;
  }

  for (size_t i = ev.line; i < ev.line + ev.length; i++) {
    Row *r = &ev.buffer->rows[i];
    std::string line = r->raw;

    r->hl.assign(line.length(), HighlightType::Normal);

    std::regex kw(" ?(if|else|for|while|do|switch|case|default|break|continue|"
                  "struct|enum|union|return|void)( |;)");
    std::sregex_iterator riter(line.begin(), line.end(), kw);
    std::sregex_iterator end;

    for (auto i = riter; i != end; ++i) {
      std::smatch s = *i;
      for (auto j = s.position(); j < s.position() + s.length(); j++) {
        r->hl[j] = HighlightType::Keyword;
      }
    }
  }
}
