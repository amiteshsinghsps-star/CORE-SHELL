#ifndef SHELL_H
#define SHELL_H

#include <termios.h>
#include <sys/types.h>
#include "history.h"

extern pid_t shell_pgid;
extern struct termios shell_tmodes;
extern int shell_terminal;
extern int shell_is_interactive;

void shell_init(int restricted);
void shell_loop(void);
int shell_run_command(const char *cmd);
int shell_run_script(const char *script_file);

char *build_prompt(void);

#endif
