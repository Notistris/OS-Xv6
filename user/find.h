#define DIRSIZ 14

struct dirent {
    ushort inum;
    char name[DIRSIZ];
};
