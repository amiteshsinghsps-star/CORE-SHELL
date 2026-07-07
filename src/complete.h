#ifndef COMPLETE_H
#define COMPLETE_H

typedef struct Match {
    char *value;
    struct Match *next;
} Match;

Match *complete_path(const char *partial);

#endif
