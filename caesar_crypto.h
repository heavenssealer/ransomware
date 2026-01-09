#ifndef CAESAR_CRYPTO_H
#define CAESAR_CRYPTO_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Applique César à un caractère
char caesar_char(char c, int shift);

// Chiffre un fichier avec César
int caesar_encrypt_file(const char *input, const char *output, int shift);

// Déchiffre un fichier avec César
int caesar_decrypt_file(const char *input, const char *output, int shift);

#endif
