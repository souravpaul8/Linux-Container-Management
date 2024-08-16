#define _GNU_SOURCE
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    char ns_path[256];
    int ns_fd;

    if (argc < 2) {
        printf("Usage: %s <pid>\n", argv[0]);
        return 1;
    }

    // Get the PID from the command-line argument.
    pid = atoi(argv[1]);

    // Build the path to the namespace file descriptor.
    snprintf(ns_path, sizeof(ns_path), "/proc/%d/ns/uts", pid);

    // Open the namespace file descriptor.
    ns_fd = open(ns_path, O_RDONLY);
    if (ns_fd < 0) {
        perror("open");
        return 1;
    }

    // Join the namespace using setns().
    if (setns(ns_fd, CLONE_NEWUTS) < 0) {
        perror("setns");
        return 1;
    }

    // Run a command in the new namespace.
    char *new_argv[] = { "/bin/bash", NULL };
    execv(new_argv[0], new_argv);
    perror("execv");
    return 1;

    return 0;
}
