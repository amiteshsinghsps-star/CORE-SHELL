/* restrict.c — Restricted shell mode */
#include "restrict.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int restricted = 0;

static const char *blacklist[] = {
    "su", "sudo", "chmod", "chown", "passwd",
    "mount", "umount", "fdisk", "mkfs",
    "iptables", "insmod", "rmmod", "modprobe",
    NULL
};

void restrict_init(void) {
    restricted = 1;
    setenv("PATH", "/usr/bin:/bin", 1);
    fprintf(stderr, "CoreShell: restricted mode active\n");
}

int restrict_check(const char *cmd, char **argv, const char *redir_file) {
    (void)argv;
    if (!restricted) return 0;
    if (cmd[0] == '/') {
        fprintf(stderr, "rbash: %s: restricted\n", cmd);
        return -1;
    }
    if (strcmp(cmd, "cd") == 0) {
        fputs("rbash: cd: restricted\n", stderr);
        return -1;
    }
    for (int i = 0; blacklist[i]; i++) {
        if (strcmp(cmd, blacklist[i]) == 0) {
            fprintf(stderr, "rbash: %s: restricted\n", cmd);
            return -1;
        }
    }
    if (redir_file && redir_file[0] == '/') {
        fputs("rbash: cannot redirect to absolute path\n", stderr);
        return -1;
    }
    return 0;
}
