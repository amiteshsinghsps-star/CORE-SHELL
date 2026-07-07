/* readline_mod.c - Readline integration and prompt */
#include "readline_mod.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

char *readline_read(const char *prompt) {
    char *line = readline(prompt);
    if (line && *line) {
        add_history(line);
    }
    return line;
}

char *build_prompt(void) {
    static char prompt[512];
    const char *user = getenv("USER") ? getenv("USER") : "user";
    char host[64]; 
    if (gethostname(host, sizeof(host)) != 0) strcpy(host, "host");
    char cwd[1024];
    if (!getcwd(cwd, sizeof(cwd))) strcpy(cwd, "/");

    const char *home = getenv("HOME");
    char display_cwd[1024];
    if (home && strncmp(cwd, home, strlen(home)) == 0) {
        snprintf(display_cwd, sizeof(display_cwd), "~%s", cwd + strlen(home));
    } else {
        strcpy(display_cwd, cwd);
    }

    int last_exit_status = 0; // simplified
    const char *color = (last_exit_status == 0)
        ? "\033[1;32m"   /* green — last cmd succeeded */
        : "\033[1;31m";  /* red — last cmd failed */

    snprintf(prompt, sizeof(prompt),
        "%s[%s@%s:%s]\033[0m\033[1m$\033[0m ",
        color, user, host, display_cwd);
    return prompt;
}
