#include <string.h>
#include <sys/mman.h>

#include "hello_for_elfhack.h"

int
main (void) {
  void *code = mmap(NULL, 4096, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  memcpy(code, hello_for_elfhack, hello_for_elfhack_len);
  void (*fn)(void) = (void (*) (void))code;
  fn();
}
