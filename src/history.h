/* history.h */
#ifndef HISTORY_H
#define HISTORY_H

#include <time.h>

#define HISTORY_MAX  1000

typedef struct HistoryEntry {
    int              index;   /* Global sequence number */
    char            *cmd;     /* Command string (heap-allocated) */
    time_t           ts;      /* Timestamp */
    struct HistoryEntry *prev;
    struct HistoryEntry *next;
} HistoryEntry;

typedef struct History {
    HistoryEntry *head;       /* Oldest entry */
    HistoryEntry *tail;       /* Newest entry */
    HistoryEntry *current;    /* Navigation pointer */
    int           count;      /* Total entries */
    int           max;        /* Max entries (HISTORY_MAX) */
} History;

/* API */
void  history_init(History *h, int max);
void  history_add(History *h, const char *cmd);
char *history_prev(History *h);
char *history_next(History *h);
void  history_save(History *h, const char *path);
void  history_load(History *h, const char *path);
void  history_search(History *h, const char *prefix);
void  history_free(History *h);

extern History shell_history;

#endif
