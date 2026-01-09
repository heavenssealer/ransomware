#ifndef ROT13_CRYPTO_H
#define ROT13_CRYPTO_H

#include "caesar_crypto.h"

// ROT13 d'un caractère (utilise César avec shift=13)
char rot13_char(char c);

// Chiffre/Déchiffre un fichier avec ROT13
int rot13_file(const char *input, const char *output);

#endif
