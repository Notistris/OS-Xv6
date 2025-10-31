#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* fmtname(char *path) {
    char *p;

    //Tim ten file cuoi cung trong path
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    return p;
}


void find(char *path, char *target) {
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    //Mo file
    if((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    //Lay thong tin file
    if(fstat(fd, &st) < 0) {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type) {
        case T_DEVICE:
        case T_FILE:
            //So sanh ten file voi target
            if (strcmp(fmtname(path), target) == 0) {
                printf("%s\n", path);
            }
            break;
        
        case T_DIR:
            //Kiem tra do dai path
            if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
                printf("find: path too long\n");
                break;
            }
            strcpy(buf, path);
            p = buf + strlen(buf);
            *p++ = '/';

            while(read(fd, &de, sizeof(de)) == sizeof(de)) {
                if(de.inum == 0) //Bo qua thu muc rong
                    continue;
                
                //Bo qua thu muc . va thu muc ..
                if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                    continue;

                //Sao chep ten file vao buf 
                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;
                if(stat(buf, &st) < 0) {
                    printf("find: cannot stat %s\n", buf);
                    continue;
                }

                find(buf, target);
            }
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(2, "Usage: find <path> <file>\n");
        exit(1);
    }
    find(argv[1], argv[2]);

    exit(0);
}