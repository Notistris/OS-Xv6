#include "kernel/types.h"
#include "user/user.h"

// Hàm in ra màn hình
void writeConsole(int pid, char* message) {
    char* pidString = itoa(pid);  // Chuyển pid từ số sang chuỗi để in

    write(1, pidString, sizeof(pidString));
    write(1, ": received ", 11);
    write(1, message, sizeof(message));
    write(1, "\n", 1);
}

void pingpong(void) {
    // Tạo 2 pipe cho tiến trình cha và con chứa:
    // 0: đầu đọc (read)
    // 1: đầu ghi (write)
    int parentPipe[2];
    int childPipe[2];
    pipe(parentPipe);  // pipe cha (cha ghi cho con đọc)
    pipe(childPipe);   // pipe con (con ghi cho cha đọc)

    // Fork tiến trình để tạo tiến trình con
    int pid = fork();
    // Sau khi fork thì dưới câu lệnh fork đang có 2 tiến trình chạy song song với nhau
    // nếu pid = 0 thì tiến trình đang chạy là tiến trình con
    // nếu pid > 0 thì tiến trình đang chạy là tiến trình cha
    // nếu pid < 0 thì không tạo được tiến trình con (vẫn đang chạy ở tiến trình gốc)

    // Tiến trình con sẽ đợi nhận một message đến từ tiến trình cha thông qua pipe
    if (pid == 0) {
        char ping[1];
        int childPID = getpid();  // lấy pid của tiến trình con
        // Vì tiến trình con chỉ đọc của cha và chỉ ghi của con nên:
        close(parentPipe[1]);  // Đóng đầu ghi của cha
        close(childPipe[0]);   // Đóng đầu đọc của con

        // Đợi cha bắn 1 byte (ping) cho con
        if (read(parentPipe[0], ping, 1)) {
            writeConsole(childPID, "ping");

            // Tiến hành gửi lại 1 byte (pong) cho tiến trình cha
            write(childPipe[1], "b", 1);

            close(parentPipe[0]);
            close(childPipe[1]);  // Đóng các đầu còn lại sau khi đã thực hiện xong
        }

        exit(1);
    }

    // Tiến trình cha sẽ bắn một message đến tiến trình con thông qua pipe
    if (pid > 0) {
        char pong[1];
        int parentPID = getpid();  // lấy pid của tiến trình cha
        // Tiến trình cha chỉ đọc của con và chỉ ghi của cha nên:
        close(parentPipe[0]);  // Đóng đầu đọc của cha
        close(childPipe[1]);   // Đóng đầu ghi của con

        // Bắt đầu: gửi 1 byte (ping) cho con
        write(parentPipe[1], "b", 1);

        // Đợi con bắn lại 1 byte trả lời (pong)
        if (read(childPipe[0], pong, 1)) {
            writeConsole(parentPID, "pong");

            close(parentPipe[1]);
            close(childPipe[0]);  // Đóng các đầu còn lại sau khi đã thực hiện xong
        }

        wait(0);  // đợi tiến trình con kết thúc để tắt tiến trình cha
        exit(0);
    }
}

int main(void) {
    pingpong();
    exit(0);
}
