#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
    // Tạo 2 pipe cho tiến trình cha và con chứa:
    // 0: đầu đọc (read)
    // 1: đầu ghi (write)
    int p[2];
    int c[2];
    pipe(p);  // pipe cha (cha ghi cho con đọc)
    pipe(c);  // pipe con (con ghi cho cha đọc)

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
        close(p[1]);              // đóng các đầu lại để tiến hành thực hiện
        close(c[0]);

        // Tiến hành đọc trong pipe, nếu có 1 byte từ đầu ghi (tiến trình cha gửi) thì
        // in ra màn hình để thông báo
        if (read(p[0], ping, 1)) {
            printf("%d: received ping\n", childPID);

            // Tiến hành gửi lại 1 byte cho tiến trình cha
            write(c[1], "b", 1);

            close(p[0]);
            close(c[1]);  // đóng các đầu sau khi đã thực hiện xong
        }

        exit(0);
    }

    // Tiến trình cha sẽ bắn một message đến tiến trình con thông qua pipe
    if (pid > 0) {
        char pong[1];
        int parentPID = getpid();  // lấy pid của tiến trình cha
        close(p[0]);               // đóng các đầu lại để tiến hành thực hiện
        close(c[1]);

        write(p[1], "b", 1);

        if (read(c[0], pong, 1)) {
            printf("%d: received pong\n", parentPID);

            close(p[1]);
            close(c[0]);  // đóng các đầu sau khi đã thực hiện xong
        }

        wait(0);  // đợi tiến trình con kết thúc để tắt tiến trình cha
        exit(0);
    }
}
