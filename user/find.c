#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#include "user/find.h"

char* fmtname(char* path) {
    char* p;

    // Find first character after last slash.
    for (p = path + strlen(path); p >= path && *p != '/'; p--);
    p++;

    return p;
}

void find(char* path, char* fileName) {
    int fd;
    char buf[512], *p;
    struct stat st;
    struct dirent de;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type) {
        case T_DEVICE:
        case T_FILE:
            if (strcmp(fmtname(path), fileName) == 0)
                printf("%s\n", path);
            break;

        case T_DIR:
            if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                printf("ls: path too long\n");
                break;
            }

            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';

            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0)
                    continue;

                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;

                if (strcmp(fmtname(buf), ".") == 0 || strcmp(fmtname(buf), "..") == 0)
                    continue;

                if (stat(buf, &st) < 0) {
                    printf("ls: cannot stat %s\n", buf);
                    continue;
                }

                find(buf, fileName);
            }
            break;
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        write(1, "Usage: find <path> <file>\n", 26);
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}