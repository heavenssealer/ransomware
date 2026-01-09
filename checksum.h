#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BUFFER_SIZE 4096

// Calcule le CRC32 d'un buffer
uint32_t crc32(const unsigned char *data, size_t length);

// Calcule le CRC32 d'un fichier
uint32_t calculate_crc32(const char *filepath);

// Vérifie l'intégrité d'un fichier
int verify_integrity(const char *filepath, uint32_t expected_crc);

#endif
