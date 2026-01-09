#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_PATH 4096
#define MAX_FILES 10000

// Scan un répertoire (non récursif)
int scan_directory(const char *path, char files[][MAX_PATH], int max_files);

// Scan récursivement un répertoire
int scan_recursive(const char *path, char files[][MAX_PATH], int max_files, int current_count);

// Vérifie si un fichier doit être exclu
int should_exclude(const char *filename);

#endif
