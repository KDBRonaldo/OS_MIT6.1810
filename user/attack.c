#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

#define DATASIZE (8*4096)

int
main(int argc, char *argv[])
{
  // Your code here.
  char *p = sbrk(DATASIZE);

  int i;
  for(i = 0; i < DATASIZE - 16; i++){
    if(strcmp("This may help.", p + i) == 0){
      printf("%s\n", p + i + 16);
    }
  }

  exit(1);
}
