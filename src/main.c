#include <term/escapes.h>
#include <term/mode.h>
#include <term/input.h>

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

int main() {
  //term_setup();
  
  while (true) {
    struct Key k = term_read_key();
    printf("read key\n");
    if (k.base == 'q' && k.control) break;
    printf("key: %d %d shift: %d alt: %d control: %d\n", 
      k.base,
      0,//k.base - 0x60,
      k.shift,
      k.alt,
      k.control);
  }
 
  term_die("test error"); 
  return 0;
}
