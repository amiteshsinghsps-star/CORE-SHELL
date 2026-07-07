/* complete.c — Tab-completion engine */
#include "complete.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

Match *complete_path(const char *partial) {
    Match *matches = NULL;
    char  dir_part[1024] = ".";
    char  file_part[256] = "";

    const char *slash = strrchr(partial, '/');
    if (slash) {
        strncpy(dir_part, partial, slash - partial);
        dir_part[slash - partial] = '\0';
        strncpy(file_part, slash+1, sizeof(file_part)-1);
    } else {
        strncpy(file_part, partial, sizeof(file_part)-1);
    }

    DIR *dir = opendir(dir_part);
    if (!dir) return NULL;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0]=='.' && file_part[0]!='.')
            continue;
        if (strncmp(entry->d_name, file_part, strlen(file_part)) == 0) {
            Match *m = malloc(sizeof(Match));
            char full[2048];
            snprintf(full, sizeof(full), "%s/%s", dir_part, entry->d_name);
            struct stat st;
            if (stat(full, &st) == 0 && S_ISDIR(st.st_mode)) {
                char with_slash[256];
                snprintf(with_slash, sizeof(with_slash), "%s/", entry->d_name);
                m->value = strdup(with_slash);
            } else {
                m->value = strdup(entry->d_name);
            }
            m->next = matches;
            matches = m;
        }
    }
    closedir(dir);
    return matches;
}
