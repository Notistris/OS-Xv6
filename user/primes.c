#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "stdbool.h"

#define N 280
static bool arr[N + 2];  // khai báo mảng static để tiết kiệm bộ nhớ

void writeConsole(char* buf) {
    write(1, "prime ", 6);
    write(1, buf, sizeof(buf));
    write(1, "\n", 1);
}

__attribute__((noreturn)) void subProcess(
    int parentPipe[]) {  // Tham số truyền vào là fd (read, write) của tiến trình trước đó
                         // (parentPipe)
    char buf[4];         // Max là 280 -> 3 chữ số + '\0'

    if (read(parentPipe[0], buf, sizeof(buf)) ==
        0) {                   // Nếu tiến trình trước không gửi số nguyên tố nữa
        close(parentPipe[0]);  // <read(...) == 0>, có nghĩa là đã chạy đến N
        close(parentPipe[1]);  // Tiến hành đóng các fd đã mở
        exit(2);               // và exit kết thúc tiến trình
    }

    int n = atoi(buf);  // chuyển chuỗi sang số để xử lý

    // in ra số nguyên tố (n) mà tiến trình trước đó đã gửi qua
    writeConsole(buf);

    // Đánh đấu các bội của n
    for (int i = n * 2; i < N; i += n) arr[i] = 0;

    // Tìm số nguyên tố tiếp theo
    int nextPrime = -1;
    for (int j = n + 1; j < N; j++) {
        if (arr[j]) {
            nextPrime = j;
            break;
        }
    }

    // Nếu không có số nguyên tố kế tiếp thì kết thúc tiến trình -> không gửi cho tiến trình khác
    if (nextPrime == -1) {
        // Tắt các pipe của tiến trình cha (vì đọc, viết xong rồi không còn dùng đến nữa)
        close(parentPipe[0]);
        close(parentPipe[1]);
        exit(2);
    }

    // Khởi tạo pipe cho tiến trình hiện tại (current)
    int currentPipe[2];
    pipe(currentPipe);

    // Fork tiến trình để bắt đầu sàng số tiếp theo
    int pid = fork();

    close(parentPipe[0]);  // Đã đọc ra n nên không còn xài đến nữa

    // Xử lý cho tiến trình con
    if (pid == 0) {
        // Đóng các fd không còn dùng
        close(currentPipe[1]);  // Vì con chỉ đọc số nguyên tố từ cha nên đóng đầu ghi của cha lại
        subProcess(
            currentPipe);  // Đệ quy để xử lý tiến trình tiếp theo, truyền pipe hiện tại cho
                           // tiến trình con (currentPipe ở đây -> parrentPipe tiến trình tiếp theo)
    }

    // Xử lý cho tiến trình cha
    if (pid > 0) {
        close(currentPipe[0]);  // Đóng đầu đọc để gửi số nguyên tố cho tiến trình kế tiếp
        char* nextPrimePtr = itoa(nextPrime);  // Hàm tự tạo (chuỗi int sang char*) để gửi tin nhắn
        write(currentPipe[1], nextPrimePtr, sizeof(nextPrimePtr));
        close(currentPipe[1]);
        wait(0);  // Đợi cho tiến trình con (nếu có) kết thúc thì mới exit
    }

    exit(1);
}

void primes(void) {
    // Khởi tạo pipe cho tiến trình hiện tại
    int currentPipe[2];
    pipe(currentPipe);

    // Fork tiến trình để sàn số nguyên tố
    int pid = fork();

    // Xử lý tiến trình con
    if (pid == 0) {
        close(currentPipe[1]);  // Tiến trình con chỉ đọc từ cha nên đóng đầu ghi
        subProcess(currentPipe);
    }

    // Xử lý tiến trình cha
    if (pid > 0) {
        close(currentPipe[0]);  // Đóng đầu đọc để gửi message cho tiến trình con
        char* startPtr = itoa(2);
        write(currentPipe[1], startPtr, sizeof(startPtr));
        close(currentPipe[1]);
        wait(0);  // Đợi tất cả tiến trình con, cháu, ... kết thúc thì mới exit
    }

    exit(0);
}

void initArray() {  // Khởi tạo mảng tĩnh để tiết kiệm bộ nhớ
    memset(arr, 1, sizeof(arr));
    arr[0] = arr[1] = 0;  // Số 0, 1 không phải là số nguyên tố
}

int main(void) {
    initArray();
    primes();
    exit(0);
}