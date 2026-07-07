/* main.c — CoreShell entry point */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/types.h>
#include "shell.h"
#include "signals.h"
#include "jobs.h"
#include "audit.h"
#include "restrict.h"

/* Global state */
pid_t           shell_pgid;
struct termios  shell_tmodes;
int             shell_terminal;
int             shell_is_interactive;

int main(int argc, char **argv) {
    int opt;
    int restricted_mode = 0;
    char *script_file   = NULL;

    /* Parse flags: -r (restricted), -c (command), file */
    while ((opt = getopt(argc, argv, "rc:")) != -1) {
        switch (opt) {
            case 'r': restricted_mode = 1; break;
            case 'c': return shell_run_command(optarg); break;
        }
    }
    if (optind < argc) script_file = argv[optind];

    /* Initialise all subsystems */
    shell_init(restricted_mode);
    audit_init(".coreshell_audit.log");

    if (script_file) {
        return shell_run_script(script_file);
    }

    /* Interactive REPL */
    shell_loop();

    /* Cleanup */
    audit_close();
    history_save(&shell_history, ".coreshell_history");
    return EXIT_SUCCESS;
}
