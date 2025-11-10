#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs command [args...]\n");
        exit(1);
    }

    char buf[512];
    char* xargv[MAXARG];
    int n = 0;  // số lượng ký tự đọc
    int i;

    // Copy phần command gốc vào xargv
    for (i = 1; i < argc; i++) {
        xargv[i - 1] = argv[i];
    }
    int base = i - 1;  // số lượng args gốc

    // Đọc stdin ký tự một
    while (read(0, &buf[n], 1) == 1) {
        if (buf[n] == '\n') {
            buf[n] = 0;         // kết thúc chuỗi
            xargv[base] = buf;  // nối thêm đối số mới
            xargv[base + 1] = 0;

            // fork + exec
            if (fork() == 0) {
                exec(argv[1], xargv);
                fprintf(2, "exec failed\n");
                exit(1);
            }
            wait(0);
            n = 0;  // reset buffer
        } else {
            n++;
        }
    }

    exit(0);
}