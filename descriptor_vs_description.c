/* vim: ts=4:sw=4:tw=79:noet */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define EXPECT_ZERO(x) do { if (x != 0) abort(); } while (0);

/**
 * This is a program to demonstrate the difference between file descriptors and
 * file descriptions.  Multiple file descriptors may refer to the same file
 * description.
 *
 * At the end of the program, the situation looks like this:
 *
 * +-----------+  +-----------+  +-----------+
 * |   fd      |  |   fd2     |  |   fd3     |
 * +-----------+  +-----------+  +-----------+
 *       \         /                  |
 *        \       /                   |
 *         V     V                    V
 *     +--------------+          +--------------+
 *     | description1 |          | description2 |
 *     | offset: 2    |          | offset: 0    |
 *     | perm: R/O    |          | perm: R/O    |
 *     |              |          |              |
 *     +--------------+          +--------------+
 *
 */
int main(void)
{
    int fd1, fd2, fd3;
    off_t off;

    EXPECT_ZERO(system("/bin/sh -c 'echo abcdef > /tmp/foo'"));
    fd1 = open("/tmp/foo", O_RDONLY);
    if (fd1 < 0) {
        perror("failed to open /tmp/foo: ");
        exit(EXIT_FAILURE);
    }
    fd2 = dup(fd1);
    if (fd2 < 0) {
        perror("failed to dup fd1: ");
        exit(EXIT_FAILURE);
    }
    fd3 = open("/tmp/foo", O_RDONLY);
    if (fd3 < 0) {
        perror("failed to open /tmp/foo (again): ");
        exit(EXIT_FAILURE);
    }
    if (lseek(fd1, 2, SEEK_SET) == -1) {
        perror("lseek failed: ");
        exit(EXIT_FAILURE);
    }
    off = lseek(fd2, 0, SEEK_CUR);
    if (off == -1) {
        perror("lseek failed: ");
        exit(EXIT_FAILURE);
    }
    if (off != 2) {
        fprintf(stderr, "expected fd1 and fd2 to share an offset.\n");
        exit(EXIT_FAILURE);
    }
    off = lseek(fd3, 0, SEEK_CUR);
    if (off == -1) {
        perror("lseek failed: ");
        exit(EXIT_FAILURE);
    }
    if (off != 0) {
        fprintf(stderr, "expected fd3 and fd1 to NOT share an offset.\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

