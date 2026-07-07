#ifndef RESTRICT_H
#define RESTRICT_H

void restrict_init(void);
int restrict_check(const char *cmd, char **argv, const char *redir_file);

#endif
