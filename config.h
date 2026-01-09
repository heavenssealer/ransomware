#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ITEMS 100
#define MAX_LINE 512

typedef struct {
    char whitelist[MAX_ITEMS][64];
    int whitelist_count;
    char blacklist[MAX_ITEMS][256];
    int blacklist_count;
} Config;

// Charge la configuration depuis un fichier
Config* load_config(const char *filepath);

// Vérifie si un fichier est autorisé
int is_allowed(const Config *cfg, const char *filepath);

// Libère la mémoire de la config
void free_config(Config *cfg);

#endif
