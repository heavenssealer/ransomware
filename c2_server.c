#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 4444
#define BACKLOG 5

typedef struct {
    char command[32];
    char target[256];
    int mode;
} C2Command;

typedef struct {
    int status;
    char message[256];
} C2Response;

void* handle_client(void *arg) {
    int client_sock = *(int*)arg;
    free(arg);

    C2Command cmd;
    C2Response resp;
    ssize_t n;

    printf("[LOG] Nouveau client connecté.\n");

    while (1) {
        memset(&cmd, 0, sizeof(cmd));
        
        n = recv(client_sock, &cmd, sizeof(cmd), 0);
        
        if (n <= 0) break;
        if (n < sizeof(cmd) || strlen(cmd.command) == 0) {
            continue; 
        }

        memset(&resp, 0, sizeof(resp));
        printf("[LOG] Commande reçue : %s\n", cmd.command);

        if (strcmp(cmd.command, "STATUS") == 0) {
            resp.status = 1;
            strcpy(resp.message, "Server Online & Ready");
        }
        else if (strcmp(cmd.command, "ENCRYPT") == 0) {
            resp.status = 1;
            cmd.mode = 3; 
            snprintf(resp.message, sizeof(resp.message), "Mode: XOR (Key: 0xDEADBEEF)");
        }
        else {
            resp.status = 0;
            strcpy(resp.message, "Unknown command");
        }

        send(client_sock, &resp, sizeof(resp), 0);
    }

    printf("[LOG] Client déconnecté.\n");
    close(client_sock);
    return NULL;
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int opt = 1;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    if (listen(server_sock, BACKLOG) < 0) {
        perror("Listen failed");
        return 1;
    }

    printf(">>> C2 Server listening on port %d\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
        if (client_sock < 0) continue;

        pthread_t thread;
        int *sock_ptr = malloc(sizeof(int));
        *sock_ptr = client_sock;
        
        if (pthread_create(&thread, NULL, handle_client, sock_ptr) != 0) {
            perror("Thread creation failed");
            free(sock_ptr);
            close(client_sock);
        } else {
            pthread_detach(thread);
        }
    }

    close(server_sock);
    return 0;
}