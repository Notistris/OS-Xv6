#include "kernel/stat.h"
#include "kernel/types.h"
#include "user/user.h"

int main(void) {
    int pid = getpid();
    printf("My PID is %d\n", pid);
    exit(0);
}