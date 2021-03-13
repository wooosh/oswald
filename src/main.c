#include <term/escapes.h>
#include <term/mode.h>

#include <stdio.h>
#include <unistd.h>

int main() {
  term_setup();
  
  printf("%shello world", term_set_pos(15, 20));
  getc(stdin);
 
  term_die("test error"); 
  return 0;
}
