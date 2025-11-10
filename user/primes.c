#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "stdbool.h"

#define N 280
static bool arr[N + 2];

void intToStr(int num, char *buf) {
    int i = 0;
    if (num == 0) {
        buf[i++] = '0';
    } else {
        char temp[16];
        int j = 0;
        while (num > 0) {
            temp[j++] = '0' + num % 10;
            num /= 10;
        }
        while (j > 0)
            buf[i++] = temp[--j];
    }
    buf[i] = 0;
}

int strlen_custom(char *s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

void writeConsole(char* buf) {
    write(1, "prime ", 6);
    write(1, buf, strlen_custom(buf));
    write(1, "\n", 1);
}

__attribute__((noreturn)) void subProcess(int parentPipe[]) {
    char buf[6];

    if (read(parentPipe[0], buf, sizeof(buf)) <= 0) {
        close(parentPipe[0]);
        close(parentPipe[1]);
        exit(0);
    }

    int n = atoi(buf);
    writeConsole(buf);

    for (int i = n * 2; i <= N; i += n)
        arr[i] = 0;

    int nextPrime = -1;
    for (int j = n + 1; j <= N; j++) {
        if (arr[j]) {
            nextPrime = j;
            break;
        }
    }

    if (nextPrime == -1) {
        close(parentPipe[0]);
        close(parentPipe[1]);
        exit(0);
    }

    int currentPipe[2];
    pipe(currentPipe);

    int pid = fork();
    close(parentPipe[0]);

    if (pid == 0) {
        close(currentPipe[1]);
        subProcess(currentPipe);
    } else if (pid > 0) {
        close(currentPipe[0]);
        char nextBuf[6];
        intToStr(nextPrime, nextBuf);
        write(currentPipe[1], nextBuf, strlen_custom(nextBuf)+1);
        close(currentPipe[1]);
        wait(0);
    }

    exit(0);
}

void primes(void) {
    int currentPipe[2];
    pipe(currentPipe);

    int pid = fork();
    if (pid == 0) {
        close(currentPipe[1]);
        subProcess(currentPipe);
    } else if (pid > 0) {
        close(currentPipe[0]);
        char startBuf[6];
        intToStr(2, startBuf);
        write(currentPipe[1], startBuf, strlen_custom(startBuf)+1);
        close(currentPipe[1]);
        wait(0);
    }
    exit(0);
}

void initArray() {
    for (int i = 0; i <= N; i++)
        arr[i] = 1;
    arr[0] = arr[1] = 0;
}

int main(void) {
    initArray();
    primes();
    exit(0);
}