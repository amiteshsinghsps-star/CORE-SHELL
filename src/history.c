/* history.c - Doubly-Linked List History */
#include "history.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void history_init(History *h, int max) {
    h->head = NULL;
    h->tail = NULL;
    h->current = NULL;
    h->count = 0;
    h->max = max;
}

void history_add(History *h, const char *cmd) {
    if (!cmd || !*cmd) return;
    HistoryEntry *entry = calloc(1, sizeof(HistoryEntry));
    entry->cmd = strdup(cmd);
    entry->ts = time(NULL);
    
    if (h->tail) {
        h->tail->next = entry;
        entry->prev = h->tail;
        h->tail = entry;
    } else {
        h->head = h->tail = entry;
    }
    h->count++;
    
    if (h->count > h->max) {
        HistoryEntry *old = h->head;
        h->head = old->next;
        if (h->head) h->head->prev = NULL;
        free(old->cmd);
        free(old);
        h->count--;
    }
    h->current = NULL;
}

char *history_prev(History *h) {
    if (!h->current) h->current = h->tail;
    else if (h->current->prev) h->current = h->current->prev;
    return h->current ? h->current->cmd : NULL;
}

char *history_next(History *h) {
    if (h->current && h->current->next) h->current = h->current->next;
    else h->current = NULL;
    return h->current ? h->current->cmd : "";
}

void history_save(History *h, const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) return;
    for (HistoryEntry *e = h->head; e; e = e->next) {
        fprintf(f, "%s\n", e->cmd);
    }
    fclose(f);
}

void history_load(History *h, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        history_add(h, line);
    }
    fclose(f);
}

void history_search(History *h, const char *prefix) {
    (void)h;
    (void)prefix;
}

void history_free(History *h) {
    HistoryEntry *curr = h->head;
    while (curr) {
        HistoryEntry *next = curr->next;
        free(curr->cmd);
        free(curr);
        curr = next;
    }
    h->head = h->tail = h->current = NULL;
    h->count = 0;
}
