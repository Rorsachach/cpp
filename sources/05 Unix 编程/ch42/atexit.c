#include <stdio.h>
#include <stdlib.h>

void my_exit1(void) {
  printf("exit1 handler\n");
}

void my_exit2(void) {
  printf("exit2 handler\n");
}

int main() {
  if (atexit(my_exit2) != 0)
    perror("cannot register my_exit2");
  if (atexit(my_exit1) != 0)
    perror("cannot register my_exit1");
  if (atexit(my_exit1) != 0)
    perror("cannot register my_exit1");
  
  printf("main is done\n");
  return 0;
}