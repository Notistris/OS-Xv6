#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Chuyển số nguyên sang chuỗi
void intToStr(int num, char *buf) {
    int i = 0;
    if (num == 0) {
        buf[i++] = '0';
    } else {
        int n = num;
        char temp[16];
        int j = 0;
        while (n > 0) {
            temp[j++] = '0' + n % 10;
            n /= 10;
        }
        while (j > 0)
            buf[i++] = temp[--j];
    }
    buf[i] = 0;
}

// Hàm in ra màn hình: pid: message
void writeConsole(int pid, char* message) {
    char pidStr[16];
    intToStr(pid, pidStr);

    write(1, pidStr, strlen(pidStr));
    write(1, ": ", 2);
    write(1, message, strlen(message));
    write(1, "\n", 1);
}

void pingpong(void) {
    int parentPipe[2], childPipe[2];
    pipe(parentPipe);
    pipe(childPipe);

    int pid = fork();
    if (pid == 0) { // con
        close(parentPipe[1]);  // con đọc từ cha
        close(childPipe[0]);   // con ghi cho cha

        char ping[1];
        if (read(parentPipe[0], ping, 1)) {
            writeConsole(getpid(), "ping");
            write(childPipe[1], "b", 1);
        }

        close(parentPipe[0]);
        close(childPipe[1]);
        exit(0);
    } else if (pid > 0) { // cha
        close(parentPipe[0]);  // cha ghi cho con
        close(childPipe[1]);   // cha đọc từ con

        write(parentPipe[1], "b", 1);

        char pong[1];
        if (read(childPipe[0], pong, 1)) {
            writeConsole(getpid(), "pong");
        }

        close(parentPipe[1]);
        close(childPipe[0]);

        wait(0);
        exit(0);
    } else {
        // lỗi fork
        exit(1);
    }
}

int main(void) {
    pingpong();
    exit(0);
}