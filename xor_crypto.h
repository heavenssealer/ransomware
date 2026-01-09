#ifndef XOR_CRYPTO_H
#define XOR_CRYPTO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

// XOR un buffer avec une clé
void xor_buffer(char *buffer, size_t size, const char *key, size_t key_len);

// Chiffre un fichier avec XOR
int xor_encrypt_file(const char *input, const char *output, const char *key);

// Déchiffre un fichier avec XOR (identique à encrypt)
int xor_decrypt_file(const char *input, const char *output, const char *key);

#endif
