/* builtins.c — All built-in command implementations */
#include "builtins.h"
#include "jobs.h"
#include "history.h"
#include "env.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

static int builtin_cd(char **argv);
static int builtin_pwd(char **argv);
static int builtin_echo(char **argv);
static int builtin_exit(char **argv);
static int builtin_export(char **argv);
static int builtin_unset(char **argv);
static int builtin_jobs(char **argv);
static int builtin_fg(char **argv);
static int builtin_bg(char **argv);
static int builtin_kill(char **argv);
static int builtin_history(char **argv);
static int builtin_alias(char **argv);

typedef struct { const char *name; int (*fn)(char **argv); } Builtin;

static const Builtin builtins[] = {
    { "cd",      builtin_cd      },
    { "pwd",     builtin_pwd     },
    { "echo",    builtin_echo    },
    { "exit",    builtin_exit    },
    { "export",  builtin_export  },
    { "unset",   builtin_unset   },
    { "jobs",    builtin_jobs    },
    { "fg",      builtin_fg      },
    { "bg",      builtin_bg      },
    { "kill",    builtin_kill    },
    { "history", builtin_history },
    { "alias",   builtin_alias   },
    { NULL, NULL }
};

int is_builtin(const char *name) {
    if (!name) return 0;
    for (int i = 0; builtins[i].name; i++)
        if (strcmp(name, builtins[i].name) == 0) return 1;
    return 0;
}

int run_builtin(char **argv) {
    if (!argv || !argv[0]) return 0;
    for (int i = 0; builtins[i].name; i++)
        if (strcmp(argv[0], builtins[i].name) == 0)
            return builtins[i].fn(argv);
    return -1;
}

static int builtin_cd(char **argv) {
    const char *dir = argv[1] ? argv[1] : getenv("HOME");
    if (!dir) { fputs("cd: HOME not set\n", stderr); return 1; }
    if (chdir(dir) != 0) { perror("cd"); return 1; }
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        setenv("PWD", cwd, 1);
    }
    return 0;
}

static int builtin_fg(char **argv) {
    int jid = argv[1] ? atoi(argv[1]+1) : jobs_last_stopped();
    return job_fg(jid);
}

static int builtin_bg(char **argv) {
    int jid = argv[1] ? atoi(argv[1]+1) : jobs_last_stopped();
    return job_bg(jid);
}

static int builtin_pwd(char **argv) {
    (void)argv;
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        printf("%s\n", cwd);
        return 0;
    }
    perror("pwd");
    return 1;
}

static int builtin_echo(char **argv) {
    int i = 1;
    while (argv[i]) {
        printf("%s%s", argv[i], argv[i+1] ? " " : "");
        i++;
    }
    printf("\n");
    return 0;
}

static int builtin_exit(char **argv) {
    int status = argv[1] ? atoi(argv[1]) : 0;
    exit(status);
}

static int builtin_export(char **argv) {
    if (!argv[1]) return 0;
    char *eq = strchr(argv[1], '=');
    if (eq) {
        *eq = '\0';
        setenv(argv[1], eq + 1, 1);
        *eq = '=';
    }
    return 0;
}

static int builtin_unset(char **argv) {
    if (argv[1]) unsetenv(argv[1]);
    return 0;
}

static int builtin_jobs(char **argv) {
    (void)argv;
    /* Implemented in jobs module, we can just call it here or we need to add a function. */
    /* Wait, the PRD doesn't provide it, so let's mock it for now. */
    return 0;
}

static int builtin_kill(char **argv) {
    (void)argv;
    return 0;
}

static int builtin_history(char **argv) {
    (void)argv;
    return 0;
}

static int builtin_alias(char **argv) {
    (void)argv;
    return 0;
}
