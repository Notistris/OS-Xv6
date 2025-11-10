#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  hello();  // Call our new system call
  exit(0);
}