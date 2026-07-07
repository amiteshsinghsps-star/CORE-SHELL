#ifndef AUDIT_H
#define AUDIT_H

int audit_init(const char *logfile);
void audit_log(const char *cmd);
void audit_close(void);

#endif
