#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv) {
    if(argc != 3) {
        printf("USAGE: %s [link-from] [link-to]\n", argv[0]);
        exit(1);
    }

    symlink(argv[1], argv[2]);
}
