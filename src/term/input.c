#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>

#include <term/mode.h>
#include <term/input.h>

// TODO: describe how the terminal input system works at the top

#define MAX_ESC_LEN 6
struct InputBuffer {
  // stores the available characters, null terminated
  char seq[MAX_ESC_LEN + 1];
  // how many characters are remaining in the buffer
  size_t available;
};

// TODO: document
void input_buffer_refill(struct InputBuffer *inbuf) {
  // if there is data available, do not wait for new data
  int timeout = -1;
  if (inbuf->available) {
    timeout = 0;
  }

  struct pollfd pfd = {STDIN_FILENO, POLLIN};
  int ready = poll(&pfd, 1, timeout);

  if (ready == -1) {
    // TODO: proper error handling
    term_die("poll");
  } else if (ready > 0) {
    if (pfd.revents & POLLIN) {
      // refill buffer
      size_t result = read(STDIN_FILENO, inbuf->seq + inbuf->available, MAX_ESC_LEN - inbuf->available);
      if (result < 1) {
        term_die("read");
      }
      inbuf->available += result;
      // insert null terminator
      inbuf->seq[inbuf->available] = '\0';
    } else { // POLLERR | POLLHUP
      term_die("poll");
    }
  }
}

// TODO: document
void input_buffer_consume(struct InputBuffer *inbuf, size_t used) {
  memmove(inbuf->seq, inbuf->seq + used, inbuf->available - used);
  inbuf->available -= used;
}

// TODO: should be moved to term/escapes.h
struct EscapeRule {
  enum KeyBase key;
  // null terminated
  char pattern[MAX_ESC_LEN + 1];

  // determines if the key supports modifiers
  bool mod;
};

struct EscapeRule escape_rules[] = {
  // TODO: remove "\x1b[" prefix
  {KeyDelete,   "\x1b[3~"},
  {KeyPageDown, "\x1b[5~"},
  {KeyPageUp,   "\x1b[6~"},
  
  {KeyUpArrow,    "\x1b[A", true},
  {KeyDownArrow,  "\x1b[B", true},
  {KeyRightArrow, "\x1b[C", true},
  {KeyLeftArrow,  "\x1b[D", true},
  {KeyHome,       "\x1b[H", true},
  {KeyEnd,        "\x1b[F", true},
  
  {KeyNone}
};

bool starts_with(char* str, char* prefix) {
  return strncmp(str, prefix, strlen(prefix)) == 0;
}

struct Key term_read_key() {
  static struct InputBuffer inbuf;

  input_buffer_refill(&inbuf);
  // TODO: will assert even work properly with custom terminal setup?
  assert(inbuf.available >= 1);

  if (starts_with(inbuf.seq, "\x1b[")) {
    for (int i=0; escape_rules[i].key != KeyNone; i++) {
      struct EscapeRule rule = escape_rules[i];
      if (starts_with(inbuf.seq, rule.pattern)) {
        input_buffer_consume(&inbuf, strlen(rule.pattern));
        return (struct Key){rule.key};
      } else if (rule.mod && inbuf.available >= 6) {
        /* Keys with modifiers held down use the following format:
         *    "\x1b[1;NX";
         *            ^ ^- key type      (index 5)
         *            |- modifier number (index 4)
         */
        if (!starts_with(inbuf.seq, "\x1b[1;")) continue;
        if (rule.pattern[2] != inbuf.seq[5]) continue;

        struct Key k = {rule.key};
        // convert ascii char into int and subtract one
        int mod = inbuf.seq[4] - 48 - 1;
        
        k.shift    = mod & (1 << 0);
        k.alt      = mod & (1 << 1);
        k.control  = mod & (1 << 2);

        input_buffer_consume(&inbuf, 6);

        return k;
      }
    } 
  }

  struct Key k = {
    inbuf.seq[0],
    .shift = isupper(inbuf.seq[0]),
    .control = iscntrl(inbuf.seq[0])
  };

  // TODO: explain
  if (k.control && k.base != '\r') {
    k.base += 0x60;
  }

  input_buffer_consume(&inbuf, 1);
  return k;
}

