#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/socket.h>

void func1() {
    char *filename = getenv("FILENAME");
    FILE *input = fopen(filename, "r");
    
    int sock = socket(PF_INET, SOCK_STREAM, 0);

    int fd = fileno(input);
    struct stat sb;
    fstat(fd, &sb);
    void *shmem = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
}
